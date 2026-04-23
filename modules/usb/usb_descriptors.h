#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

enum 
{
    ITF_NUM_UART_CDC = 0,
    ITF_NUM_UART_CDC_DATA,
    ITF_NUM_CAN,
    ITF_NUM_GP,
    ITF_NUM_I2C,
    ITF_NUM_SPI,
    ITF_NUM_TOTAL
};

/***TODO - temporary solution - must remove vendors ***/
enum
{
    VENDOR_CAN = 0,
    VENDOR_GP,
    VENDOR_I2C,
    VENDOR_SPI
};

#endif