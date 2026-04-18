#include <stdlib.h>
#include <bsp/board_api.h>
#include <tusb.h>

#include <pico/stdio.h>

#include "usb_interface.h"
#include "usb_descriptors.h"

extern void ep_can_rx_handler   (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);
extern void ep_gp_rx_handler    (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);
extern void ep_i2c_rx_handler   (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);

extern uint8_t com_spi_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);
extern uint8_t com_i2c_commandStringHandler (const uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

// extern void ep_can_tx_handler   (uint16_t len);
// extern void ep_gp_tx_handler    (uint16_t len);
// extern void ep_i2c_tx_handler   (uint16_t len);
// extern void ep_spi_tx_handler   (uint16_t len);

enum
{
    ITF_IDX_CAN,
    ITF_IDX_GP,
    ITF_IDX_I2C,
    ITF_IDX_SPI,

    ITF_IDX_NUM
};
bool multi_packet = false;
uint8_t can_rx_buffer[512];
uint8_t can_rx_buffer_index;

int initUSB(void)
{
    // Initialize TinyUSB stack
    board_init();
    tusb_init();

    // TinyUSB board init callback after init
    board_init_after_tusb();

    stdio_init_all();
    printf("Device started\n");
    return 0;
}

int runUSBLoop(void)
{
    // main run loop
    while (1) 
    {
        // TinyUSB device task | must be called regurlarly
        tud_task();
        
        // // custom tasks
        // custom_cdc_task();
    }

    // indicate no error
    return 0;
}


void tud_vendor_rx_cb(uint8_t idx, const uint8_t *buffer, uint32_t bufsize)
{
    uint8_t response_size;
    uint8_t response[64];
    uint8_t error_buffer;

    switch (idx)
    {
        case ITF_IDX_CAN:
            tud_vendor_n_write      (idx, buffer, bufsize);

        break;
            
        case ITF_IDX_GP:
            tud_vendor_n_write      (idx, buffer, bufsize);
        break;
            
        case ITF_IDX_I2C:
            response_size = com_i2c_commandStringHandler (buffer, bufsize, response, &error_buffer);
            tud_vendor_n_write(idx, response, response_size);
        
        break;
        
        case ITF_IDX_SPI:
            response_size = com_spi_commandStringHandler (buffer, bufsize, response, &error_buffer);

        break;
        
        default:
        
        break;
    }
}

// callback when data is received on a CDC interface
void tud_cdc_rx_cb(uint8_t itf)
{
    // allocate buffer for the data in the stack
    uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

    printf("RX CDC %d\n", itf);

    // read the available data 
    // | IMPORTANT: also do this for CDC0 because otherwise
    // | you won't be able to print anymore to CDC0
    // | next time this function is called
    uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

    // check if the data was received on the second cdc interface
    if (itf == 0) 
    {
        // process the received data
        buf[count] = 0; // null-terminate the string
        // now echo data back to the console on CDC 0
        printf("Received on CDC 1: %s\n", buf);

        // and echo back OK on CDC 1
        tud_cdc_n_write(itf, (uint8_t const *) "OK\r\n", 4);
        tud_cdc_n_write_flush(itf);
    }
}

// void tud_vendor_tx_cb(uint8_t itf, uint32_t sent_bytes)
// {
//     return;
// }
