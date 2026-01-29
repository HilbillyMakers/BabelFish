#include <stdlib.h>
#include <bsp/board_api.h>
#include <tusb.h>

#include <pico/stdio.h>

#include "usb_interface.h"
#include "usb_descriptors.h"

extern void ep_can_rx_handler   (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);
extern void ep_gp_rx_handler    (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);
extern void ep_i2c_rx_handler   (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);
extern void ep_spi_rx_handler   (uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);

extern void ep_can_tx_handler   (uint16_t len);
extern void ep_gp_tx_handler    (uint16_t len);
extern void ep_i2c_tx_handler   (uint16_t len);
extern void ep_spi_tx_handler   (uint16_t len);

int initUSB(void)
{
    // Initialize TinyUSB stack
    board_init();
    tusb_init();

    // TinyUSB board init callback after init
    if (board_init_after_tusb) 
    {
        board_init_after_tusb();
    }

    stdio_init_all();

    return 0;
}

int runUSBLoop(void)
{
    // let pico sdk use the first cdc interface for std io
    stdio_init_all();

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

void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize)
{
    uint8_t     responseBuffer[512];
    uint16_t    responseSize    = 0;
    
    printf("RX Vendor on interface %d\n", itf);
    printf("Message: %s\n", buffer);

    if(false == tud_vendor_n_available(itf))
    {
        // no data available
        
    }
    else
    {
        switch(itf)
        {
            case ITF_NUM_CAN:
                // process CAN data
                ep_can_rx_handler(buffer, bufsize, responseBuffer, &responseSize);
    
            break;
    
            case ITF_NUM_GP:
                // process General Purpose data
                ep_gp_rx_handler (buffer, bufsize, responseBuffer, &responseSize);
    
            break;
    
            case ITF_NUM_I2C:
                // process I2C data
                ep_i2c_rx_handler(buffer, bufsize, responseBuffer, &responseSize);
    
            break;
    
            case ITF_NUM_SPI:
                // process SPI data
                ep_spi_rx_handler(buffer, bufsize, responseBuffer, &responseSize);
    
            break;
    
            default:
                // unknown interface
            break;
        }
    }
    


    
}

void tud_vendor_tx_cb(uint8_t itf, uint32_t sent_bytes)
{
    printf("TX Vendor on interface %d\n", itf);

    switch(itf)
    {
        case ITF_NUM_CAN:
            // process CAN data
            ep_can_tx_handler(sent_bytes);

        break;

        case ITF_NUM_GP:
            // process General Purpose data
            ep_gp_tx_handler(sent_bytes);

        break;

        case ITF_NUM_I2C:
            // process I2C data
            ep_i2c_tx_handler(sent_bytes);

        break;

        case ITF_NUM_SPI:
            // process SPI data
            ep_spi_tx_handler(sent_bytes);

        break;

        default:
            // unknown interface
        break;
    }
}
