/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

// Pico
#include "pico/stdlib.h"

// For memcpy
#include <string.h>

// Include descriptor struct definitions
#include "usb_common.h"
// USB register definitions from pico-sdk
#include "hardware/regs/usb.h"
// USB hardware struct definitions from pico-sdk
#include "hardware/structs/usb.h"
// For interrupt enable and numbers
#include "hardware/irq.h"
// For resetting the USB controller
#include "hardware/resets.h"

// Device descriptors
#include "usb_lowlevel.h"
#include "com/endpoint.h"
#include "gpio/endpoint.h"

#define usb_hw_set      ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hw_clear    ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

// Function prototypes for our device specific endpoint handlers defined
// later on
void ep0_in_handler (uint8_t *buf, uint16_t len);
void ep0_out_handler(uint8_t *buf, uint16_t len);

// Global device address
static      bool    should_set_address  = false;
static      uint8_t dev_addr            = 0;
volatile    bool    configured          = false;

// Global data buffer for EP0
static uint8_t ep0_buf[64];

// Struct defining the device configuration
static struct usb_device_configuration dev_config = 
{
    .device_descriptor      = &device_descriptor,
    .interface_descriptor   = &interface_descriptor,
    .config_descriptor      = &config_descriptor,
    .lang_descriptor        = lang_descriptor,
    .descriptor_strings     = descriptor_strings,
    .endpoints = 
   
    {
        {
            .descriptor         = &ep0_out,
            .handler            = &ep0_out_handler,
            .endpoint_control   = NULL, // NA for EP0
            .buffer_control     = &usb_dpram->ep_buf_ctrl[0].out,
            .data_buffer        = &usb_dpram->ep0_buf_a[EP0_IN_BUFF_OFFSET],        // EP0 in and out share a data buffer
        },
        {
            .descriptor         = &ep0_in,
            .handler            = &ep0_in_handler,
            .endpoint_control   = NULL, // NA for EP0,
            .buffer_control     = &usb_dpram->ep_buf_ctrl[0].in,
            .data_buffer        = &usb_dpram->ep0_buf_a[EP0_OUT_BUFF_OFFSET],       // EP0 in and out share a data buffer
        },
        {
            .descriptor         = &ep1_in,
            .handler            = &ep1_tx_handler,
            .endpoint_control   = &usb_dpram->ep_ctrl[0].in,                        /* Endpoint control register */
            .buffer_control     = &usb_dpram->ep_buf_ctrl[1].in,                    /* Buffer control register */
            .data_buffer        = &usb_dpram->epx_data[EP1_IN_BUFF_OFFSET],         /* Endpoint data buffer */
        },        
        {
            .descriptor         = &ep1_out,
            .handler            = &ep1_rx_handler,
            .endpoint_control   = &usb_dpram->ep_ctrl[0].out,                       // EP1 starts at offset 0 for endpoint control
            .buffer_control     = &usb_dpram->ep_buf_ctrl[1].out,                   /* Buffer control register */
            .data_buffer        = &usb_dpram->epx_data[EP1_OUT_BUFF_OFFSET],        // First free EPX buffer
        },
        {
            .descriptor         = &ep2_in,
            .handler            = &ep2_tx_handler,
            .endpoint_control   = &usb_dpram->ep_ctrl[1].in,                        /* Endpoint control register */
            .buffer_control     = &usb_dpram->ep_buf_ctrl[2].in,                    /* Buffer control register */
            .data_buffer        = &usb_dpram->epx_data[EP2_IN_BUFF_OFFSET],         /* Endpoint data buffer */
        },
        {
            .descriptor         = &ep2_out,
            .handler            = &ep2_rx_handler,
            .endpoint_control   = &usb_dpram->ep_ctrl[1].out,                       /* Endpoint control register */
            .buffer_control     = &usb_dpram->ep_buf_ctrl[2].out,                   /* Buffer control register */
            .data_buffer        = &usb_dpram->epx_data[EP2_OUT_BUFF_OFFSET],        /* Endpoint data buffer */
        }
    }
};



/**
 * @brief Given an endpoint address, return the usb_endpoint_configuration of that endpoint. Returns NULL
 * if an endpoint of that address is not found.
 *
 * @param addr
 * @return struct usb_endpoint_configuration*
 */
struct usb_endpoint_configuration *usb_get_endpoint_configuration(const uint8_t addr) 
{
    /* Bind the endpoint configs list */
    struct usb_endpoint_configuration *endpoints        = dev_config.endpoints;
    struct usb_endpoint_configuration *returnEndpoint   = NULL;

    /* Iterate through endpoint configs */
    for (int i = 0; i < USB_NUM_ENDPOINTS; i++)
    {
        if (
            (NULL != endpoints[i].descriptor) &&                /* Check if the endpoint has a descriptor */
            (addr == endpoints[i].descriptor->bEndpointAddress) /* Check if the endpoint address matches the given address */
        )
        {
            returnEndpoint = &endpoints[i];
            break;
        }
    }

    return returnEndpoint;
}

/**
 * @brief Given a C string, fill the EP0 data buf with a USB string descriptor for that string.
 *
 * @param C string you would like to send to the USB host
 * @return the length of the string descriptor in EP0 buf
 */
uint8_t usb_prepare_string_descriptor(const unsigned char *str) 
{
    // 2 for bLength + bDescriptorType + strlen * 2 because string is unicode. i.e. other byte will be 0
    uint8_t bLength = 2 + (strlen((const char *)str) * 2);
    static const uint8_t bDescriptorType = 0x03;

    volatile uint8_t *buf   = &ep0_buf[0];
                     *buf++ = bLength;
                     *buf++ = bDescriptorType;

    uint8_t c;

    do
    {
        c = *str++;
        *buf++ = c;
        *buf++ = 0;
    }while (c != '\0');

    return bLength;
}

/**
 * @brief Take a buffer pointer located in the USB RAM and return as an offset of the RAM.
 *
 * @param buf
 * @return uint32_t
 */
static inline uint32_t usb_buffer_offset(volatile uint8_t *buf) 
{
    return (uint32_t) buf ^ (uint32_t) usb_dpram;
}

/**
 * @brief Set up the endpoint control register for an endpoint (if applicable. Not valid for EP0).
 *
 * @param ep
 */
void usb_setup_endpoint(const struct usb_endpoint_configuration *ep)
{
    printf("Set up endpoint 0x%x with buffer address 0x%p\n", ep->descriptor->bEndpointAddress, ep->data_buffer);

    // EP0 doesn't have one so return if that is the case
    if (!ep->endpoint_control)
    {
        return;
    }

    // Get the data buffer as an offset of the USB controller's DPRAM
    uint32_t dpram_offset   = usb_buffer_offset(ep->data_buffer);
    uint32_t reg            = EP_CTRL_ENABLE_BITS
                            | EP_CTRL_INTERRUPT_PER_BUFFER
                            | (ep->descriptor->bmAttributes << EP_CTRL_BUFFER_TYPE_LSB)
                            | dpram_offset;
    *ep->endpoint_control   = reg;
}

/**
 * @brief Set up the endpoint control register for each endpoint.
 *
 */
void usb_setup_endpoints() 
{
    const struct usb_endpoint_configuration *endpoints = dev_config.endpoints;
    for (int i = 0; i < USB_NUM_ENDPOINTS; i++)
    {
        if (endpoints[i].descriptor && endpoints[i].handler)
        {
            usb_setup_endpoint(&endpoints[i]);
        }
    }
}

/**
 * @brief Set up the USB controller in device mode, clearing any previous state.
 *
 */
void usb_device_init()
{
    // Reset usb controller
    reset_unreset_block_num_wait_blocking(RESET_USBCTRL);

    // Clear any previous state in dpram just in case
    memset(usb_dpram, 0, sizeof(*usb_dpram)); // <1>

    // Enable USB interrupt at processor
    irq_set_enabled(USBCTRL_IRQ, true);

    // Mux the controller to the onboard usb phy
    usb_hw->muxing          = USB_USB_MUXING_TO_PHY_BITS
                            | USB_USB_MUXING_SOFTCON_BITS;

    // Force VBUS detect so the device thinks it is plugged into a host
    usb_hw->pwr             = USB_USB_PWR_VBUS_DETECT_BITS 
                            | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS;

    // Enable the USB controller in device mode.
    usb_hw->main_ctrl       = USB_MAIN_CTRL_CONTROLLER_EN_BITS;

    // Enable an interrupt per EP0 transaction
    usb_hw->sie_ctrl        = USB_SIE_CTRL_EP0_INT_1BUF_BITS; // <2>

    // Enable interrupts for when a buffer is done, when the bus is reset,
    // and when a setup packet is received
    usb_hw->inte            = USB_INTS_BUFF_STATUS_BITS 
                            | USB_INTS_BUS_RESET_BITS 
                            | USB_INTS_SETUP_REQ_BITS;

    // Set up endpoints (endpoint control registers)
    // described by device configuration
    usb_setup_endpoints();

    // Present full speed device by enabling pull up on DP
    usb_hw_set->sie_ctrl    = USB_SIE_CTRL_PULLUP_EN_BITS;
}

/**
 * @brief Starts a transfer on a given endpoint.
 *
 * @param ep, the endpoint configuration.
 * @param buf, the data buffer to send. Only applicable if the endpoint is TX
 * @param len, the length of the data in buf (this example limits max len to one packet - 64 bytes)
 */
void usb_start_transfer(struct usb_endpoint_configuration *ep, uint8_t *buf, uint16_t len) 
{
    // We are asserting that the length is <= 64 bytes for simplicity of the example.
    // For multi packet transfers see the tinyusb port.
    assert(len <= 64);

    printf("Start transfer of len %d on ep addr 0x%x\n", len, ep->descriptor->bEndpointAddress);

    // Prepare buffer control register value
    uint32_t val = len | USB_BUF_CTRL_AVAIL;

    /* Check if the endpoint is transmitting data to the host (i.e. is an IN endpoint) */
    if (ep->descriptor->bEndpointAddress & USB_DIR_IN)
    {
        /* Copy the data from the user buffer to the usb memory */
        memcpy((void *) ep->data_buffer, (void *) buf, len);
        /* Mark tx data buffer as full */
        val |= USB_BUF_CTRL_FULL;
    }

    // Set pid and flip for next transfer
    val |= ep->next_pid ? USB_BUF_CTRL_DATA1_PID : USB_BUF_CTRL_DATA0_PID;
    ep->next_pid ^= 1u;
    
    /* Write commands to control buffer */
    *ep->buffer_control = val;
}

/**
 * @brief Send device descriptor to host
 *
 */
void usb_handle_device_descriptor(volatile struct usb_setup_packet *pkt) 
{
    const struct usb_device_descriptor *d = dev_config.device_descriptor;
    // EP0 in
    struct usb_endpoint_configuration *ep = usb_get_endpoint_configuration(EP0_IN_ADDR);
    // Always respond with pid 1
    ep->next_pid = 1;
    usb_start_transfer(ep, (uint8_t *) d, MIN(sizeof(struct usb_device_descriptor), pkt->wLength));
}

/**
 * @brief Send the configuration descriptor (and potentially the configuration and endpoint descriptors) to the host.
 *
 * @param pkt, the setup packet received from the host.
 */
void usb_handle_config_descriptor(volatile struct usb_setup_packet *pkt) 
{
    uint8_t *buf = &ep0_buf[0];

    // First request will want just the config descriptor
    const struct usb_configuration_descriptor *d = dev_config.config_descriptor;
    memcpy((void *) buf, d, sizeof(struct usb_configuration_descriptor));
    buf += sizeof(struct usb_configuration_descriptor);

    // If we more than just the config descriptor copy it all
    if (pkt->wLength >= d->wTotalLength) 
    {
        memcpy((void *) buf, dev_config.interface_descriptor, sizeof(struct usb_interface_descriptor));
        buf += sizeof(struct usb_interface_descriptor);
        const struct usb_endpoint_configuration *ep = dev_config.endpoints;

        // Copy all the endpoint descriptors starting from EP1
        for (uint i = 2; i < USB_NUM_ENDPOINTS; i++) 
        {
            if (ep[i].descriptor) 
            {
                memcpy((void *) buf, ep[i].descriptor, sizeof(struct usb_endpoint_descriptor));
                buf += sizeof(struct usb_endpoint_descriptor);
            }
        }
    }

    // Send data
    // Get len by working out end of buffer subtract start of buffer
    uint32_t len = (uint32_t) buf - (uint32_t) &ep0_buf[0];
    usb_start_transfer(usb_get_endpoint_configuration(EP0_IN_ADDR), &ep0_buf[0], MIN(len, pkt->wLength));
}

/**
 * @brief Handle a BUS RESET from the host by setting the device address back to 0.
 *
 */
void usb_bus_reset(void) 
{
    // Set address back to 0
    dev_addr                = 0;
    should_set_address      = false;
    usb_hw->dev_addr_ctrl   = 0;
    configured              = false;
}

/**
 * @brief Send the requested string descriptor to the host.
 *
 * @param pkt, the setup packet from the host.
 */
void usb_handle_string_descriptor(volatile struct usb_setup_packet *pkt) 
{
    uint8_t i = pkt->wValue & 0xff;
    uint8_t len = 0;

    if (i == 0) 
    {
        len = 4;
        memcpy(&ep0_buf[0], dev_config.lang_descriptor, len);
    } 
    else 
    {
        // Prepare fills in ep0_buf
        len = usb_prepare_string_descriptor(dev_config.descriptor_strings[i - 1]);
    }

    usb_start_transfer(usb_get_endpoint_configuration(EP0_IN_ADDR), &ep0_buf[0], MIN(len, pkt->wLength));
}

/**
 * @brief Sends a zero length status packet back to the host.
 */
void usb_acknowledge_out_request(void) 
{
    usb_start_transfer(usb_get_endpoint_configuration(EP0_IN_ADDR), NULL, 0);
}

/**
 * @brief Handles a SET_ADDR request from the host. The actual setting of the device address in
 * hardware is done in ep0_in_handler. This is because we have to acknowledge the request first
 * as a device with address zero.
 *
 * @param pkt, the setup packet from the host.
 */
void usb_set_device_address(volatile struct usb_setup_packet *pkt) 
{
    // Set address is a bit of a strange case because we have to send a 0 length status packet first with
    // address 0
    dev_addr = (pkt->wValue & 0xff);
    printf("Set address %d\r\n", dev_addr);
    // Will set address in the callback phase
    should_set_address = true;
    usb_acknowledge_out_request();
}

/**
 * @brief Handles a SET_CONFIGRUATION request from the host. Assumes one configuration so simply
 * sends a zero length status packet back to the host.
 *
 * @param pkt, the setup packet from the host.
 */
void usb_set_device_configuration(__unused volatile struct usb_setup_packet *pkt)
{
    
    // Only one configuration so just acknowledge the request
    printf("Device Enumerated\r\n");
    usb_acknowledge_out_request();
    configured = true;
}

/**
 * @brief Respond to a setup packet from the host.
 *
 */
void usb_handle_setup_packet(void)
{
    volatile struct 
    usb_setup_packet   *pkt;
    uint8_t             req_direction;
    uint8_t             req;

    pkt             = (volatile struct usb_setup_packet *) &usb_dpram->setup_packet;
    req_direction   = pkt->bmRequestType;
    req             = pkt->bRequest;

    // Reset PID to 1 for EP0 IN
    usb_get_endpoint_configuration(EP0_IN_ADDR)->next_pid = 1u;

    if (USB_DIR_OUT == req_direction)
    {
        if (USB_REQUEST_SET_ADDRESS == req)
        {
            usb_set_device_address(pkt);
        } 
        else 
        if (USB_REQUEST_SET_CONFIGURATION == req)
        {
            usb_set_device_configuration(pkt);
        } 
        else
        {
            usb_acknowledge_out_request();
            printf("Other OUT request (0x%x)\r\n", pkt->bRequest);
        }
    } 
    else if (USB_DIR_IN == req_direction)
    {
        if (USB_REQUEST_GET_DESCRIPTOR == req)
        {
            uint16_t descriptor_type = pkt->wValue >> 8;

            switch (descriptor_type)
            {
                case USB_DT_DEVICE:
                    usb_handle_device_descriptor(pkt);
                    printf("GET DEVICE DESCRIPTOR\r\n");
                    break;

                case USB_DT_CONFIG:
                    usb_handle_config_descriptor(pkt);
                    printf("GET CONFIG DESCRIPTOR\r\n");
                    break;

                case USB_DT_STRING:
                    usb_handle_string_descriptor(pkt);
                    printf("GET STRING DESCRIPTOR\r\n");
                    break;

                default:
                    printf("Unhandled GET_DESCRIPTOR type 0x%x\r\n", descriptor_type);
            }
        } 
        else
        {
            printf("Other IN request (0x%x)\r\n", pkt->bRequest);
        }
    }
}

/**
 * @brief Notify an endpoint that a transfer has completed.
 *
 * @param ep, the endpoint to notify.
 */
static void usb_handle_ep_buff_done(struct usb_endpoint_configuration *ep)
{
    uint32_t buffer_control     = *ep->buffer_control;
    // Get the transfer length for this endpoint
    uint16_t len                = buffer_control & USB_BUF_CTRL_LEN_MASK;

    // Call that endpoints buffer done handler
    ep->handler((uint8_t *) ep->data_buffer, len);
}

/**
 * @brief Find the endpoint configuration for a specified endpoint number and
 * direction and notify it that a transfer has completed.
 *
 * @param ep_num
 * @param in
 */
static void usb_handle_buff_done(uint ep_num, bool in)
{
    /* Add the direction flag to the endpoint number for the address */
    uint8_t ep_addr = ep_num | (in ? USB_DIR_IN : 0);

    printf("EP %d (in = %d) done\n", ep_num, in);

    ///TODO: create a continuous ep association to avoid unneccesary for loops
    for (uint i = 0; i < USB_NUM_ENDPOINTS; i++)
    {
        struct usb_endpoint_configuration *ep = &dev_config.endpoints[i];
        
        if(
            (NULL       != ep->descriptor   ) && 
            (NULL       != ep->handler      ) &&
            (ep_addr    == ep->descriptor->bEndpointAddress)
        )
        {
            usb_handle_ep_buff_done(ep);
            return;
        }
    }
}

/**
 * @brief Handle a "buffer status" irq. This means that one or more
 * buffers have been sent / received. Notify each endpoint where this
 * is the case.
 */
static void usb_handle_buff_status()
{
    uint32_t remaining_buffers  = usb_hw->buf_status;
    uint32_t bit                = 1u;

    for (uint i = 0; ((remaining_buffers > 0) && (i < USB_NUM_ENDPOINTS * 2)); i++)
    {
        if (remaining_buffers & bit)
        {
            // clear this in advance
            usb_hw_clear->buf_status = bit;

            // IN transfer for even i, OUT transfer for odd i
            usb_handle_buff_done(i >> 1u, !(i & 1u));
            
            /* Reset the handled bit in the remaining buffer variable */
            remaining_buffers &= ~bit;
        }
        
        /* Shift the mask bit to check the buffer flag */
        bit <<= 1u;
    }
}

/**
 * @brief USB interrupt handler
 * @details "isr_usbctrl" is defined inside intctrl file, allowing the creation of a callback function locally
 */
void isr_usbctrl(void)
{
    // USB interrupt handler
    uint32_t status     = usb_hw->ints;
    uint32_t handled    = 0;

    // Setup packet received
    if (status & USB_INTS_SETUP_REQ_BITS)
    {
        /* Save Setup_Req bit as handled */
        handled |= USB_INTS_SETUP_REQ_BITS;

        /* Set controller status as Device: Setup packet received */
        usb_hw_clear->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;

        usb_handle_setup_packet();
    }

    // Buffer status, one or more buffers have completed
    if (status & USB_INTS_BUFF_STATUS_BITS) /* Raised when any bit in BUFF_STATUS is set */
    {
        /* Save Buffer_Status bit as handled */
        handled |= USB_INTS_BUFF_STATUS_BITS;

        usb_handle_buff_status();
    }
    
    // BUS_RESET            (0) Source: SIE_STATUS
    if (status & USB_INTS_BUS_RESET_BITS)
    {
        printf("BUS RESET\n");

        /* Save Bus_Reset bit as handled */
        handled |= USB_INTS_BUS_RESET_BITS;

        /* Set controller status as Device: bus reset received */
        usb_hw_clear->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;

        usb_bus_reset();
    }


    // VBUS_DETECT          - Source: SIE_STATUS
    // STALL                - Source: SIE_STATUS
    // ERROR_CRC            - Source: SIE_STATUS
    // ERROR_BIT_STUFF      - Source: SIE_STATUS
    // ERROR_RX_OVERFLOW    - Source: SIE_STATUS
    // ERROR_RX_TIMEOUT     - Source: SIE_STATUS
    // ERROR_DATA_SEQ       - Source: SIE_STATUS
    // TRANS_COMPLETE       - Raised every time SIE_STATUS

    // EP_STALL_NAK         - Raised when any bit in EP_STATUS_STALL_NAK is set
    // ABORT_DONE           - Raised when any bit in ABORT_DONE is set
    // DEV_SOF              - Set every time the device receives a SOF (Start of Frame) packet
    // SETUP_REQ            - Device
    // DEV_RESUME_FROM_HOST - Set when the device receives a resume from the host
    // DEV_SUSPEND          - Set when the device suspend state changes
    // DEV_CONN_DIS         - Set when the device connection state changes

    // Check if there's any unhandled IRQs remaining
    if (status ^ handled)
    {
        panic("Unhandled IRQ 0x%x\n", (uint) (status ^ handled));
    }

}

/**
 * @brief EP0 in transfer complete. Either finish the SET_ADDRESS process, or receive a zero
 * length status packet from the host.
 *
 * @param buf the data that was sent
 * @param len the length that was sent
 */
void ep0_in_handler (__unused uint8_t *buf, __unused uint16_t len)
{
    if (should_set_address)
    {
        // Set actual device address in hardware
        usb_hw->dev_addr_ctrl   = dev_addr;
        should_set_address      = false;
    } 
    else
    {
        // Receive a zero length status packet from the host on EP0 OUT
        struct usb_endpoint_configuration *ep = usb_get_endpoint_configuration(EP0_OUT_ADDR);
        usb_start_transfer(ep, NULL, 0);
    }
}

void ep0_out_handler(__unused uint8_t *buf, __unused uint16_t len)
{

}



