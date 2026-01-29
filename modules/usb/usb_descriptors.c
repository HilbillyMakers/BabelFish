/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <tusb.h>
#include <bsp/board_api.h>
#include "usb_descriptors.h"

// set some example Vendor and Product ID
// the board will use to identify at the host
#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
#define CDC_EXAMPLE_VID     0xcafe
// use _PID_MAP to generate unique PID for each interface
#define CDC_EXAMPLE_PID     (0x4000 | _PID_MAP(CDC, 0))
// set USB 2.0
#define CDC_EXAMPLE_BCD     0x0200

// defines a descriptor that will be communicated to the host
tusb_desc_device_t const desc_device = 
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = CDC_EXAMPLE_BCD,

    .bDeviceClass       = TUSB_CLASS_VENDOR_SPECIFIC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,    // CDC uses IAD

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE, // 64 bytes

    .idVendor           = CDC_EXAMPLE_VID,
    .idProduct          = CDC_EXAMPLE_PID,
    .bcdDevice          = 0x0100, // Device release number

    .iManufacturer      = 0x01, // Index of manufacturer string
    .iProduct           = 0x02, // Index of product string
    .iSerialNumber      = 0x03, // Index of serial number string

    .bNumConfigurations = 0x01 // 1 configuration
};


// total length of configuration descriptor
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN + CFG_TUD_VENDOR * TUD_VENDOR_DESC_LEN)

// define endpoint numbers
#define EPNUM_UART_CDC_NOTIF    0x86 // notification endpoint for CDC 0
#define EPNUM_UART_CDC_OUT      0x01 // out endpoint for CDC 0
#define EPNUM_UART_CDC_IN       0x81 // in endpoint for CDC 0

#define EPNUM_CAN_OUT           0x02
#define EPNUM_CAN_IN            0x82

#define EPNUM_GP_OUT            0x03
#define EPNUM_GP_IN             0x83

#define EPNUM_I2C_OUT           0x04
#define EPNUM_I2C_IN            0x84

#define EPNUM_SPI_OUT           0x05
#define EPNUM_SPI_IN            0x85

// called when host requests to get configuration descriptor
uint8_t const * tud_descriptor_configuration_cb(uint8_t index);

// more device descriptor this time the qualifier
tusb_desc_device_qualifier_t const desc_device_qualifier = 
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = CDC_EXAMPLE_BCD,

    .bDeviceClass       = TUSB_CLASS_VENDOR_SPECIFIC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .bNumConfigurations = 0x01,
    .bReserved          = 0x00
};

enum 
{
    STRID_LANGID = 0,   // 0: supported language ID
    STRID_MANUFACTURER, // 1: Manufacturer
    STRID_PRODUCT,      // 2: Product
    STRID_SERIAL,       // 3: Serials
    STRID_CDC_UART,     // 4: CDC UART Interface
    STRID_CAN,          // 5: CAN Interface
    STRID_GP,           // 6: General Purpose Interface
    STRID_I2C,          // 7: I2C Interface
    STRID_SPI,          // 8: SPI Interface
    STRID_RESET         // 9: Reset Interface
};

// array of pointer to string descriptors
char const *string_desc_arr[] = 
{
    // switched because board is little endian
    (const char[]) { 0x09, 0x04 },  // 0: supported language is English (0x0409)
    "Raspberry Pi",                 // 1: Manufacturer
    "Pico (2)",                     // 2: Product
    NULL,                           // 3: Serials (null so it uses unique ID if available)
    "UART CDC Interface",           // 4: UART CDC Interface
    "CAN Interface",                // 5: CAN Interface,
    "General Purpose Interface",    // 6: General Purpose Interface
    "I2C Interface",                // 7: I2C Interface
    "SPI Interface",                // 8: SPI Interface 
    "RPiReset"                      // 9: Reset Interface
};

// configure descriptor (for 2 CDC interfaces)
uint8_t const desc_configuration[] = 
{
    // config descriptor | how much power in mA, count of interfaces, ...
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, STRID_LANGID, CONFIG_TOTAL_LEN, 0x80, 100),

    // CDC 0: Communication Interface - TODO: get 64 from tusb_config.h
    TUD_CDC_DESCRIPTOR   (ITF_NUM_UART_CDC, STRID_CDC_UART, EPNUM_UART_CDC_NOTIF, 8, EPNUM_UART_CDC_OUT,    EPNUM_UART_CDC_IN,  64),
    // CDC 0: Data Interface
    //TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_0_DATA, 4, 0x01, 0x02),

    // Vendor Interface: CAN
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_CAN,      STRID_CAN,                               EPNUM_CAN_OUT,         EPNUM_CAN_IN,       64),
    // Vendor Interface: General Purpose
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_GP,       STRID_GP,                                EPNUM_GP_OUT,          EPNUM_GP_IN,        64),
    // Vendor Interface: I2C
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_I2C,      STRID_I2C,                               EPNUM_I2C_OUT,         EPNUM_I2C_IN,       64),
    // Vendor Interface: SPI
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_SPI,      STRID_SPI,                               EPNUM_SPI_OUT,         EPNUM_SPI_IN,       64), 
};

// buffer to hold the string descriptor during the request | plus 1 for the null terminator
static uint16_t _desc_str[32 + 1];

// --------------------------------------------------------------------+
// IMPLEMENTATION
// --------------------------------------------------------------------+

uint8_t  const *tud_descriptor_device_cb            (void)
{
    return (uint8_t const *)&desc_device;
}

uint8_t  const *tud_descriptor_device_qualifier_cb  (void)
{
    return (uint8_t const *)&desc_device_qualifier;
}

uint8_t  const *tud_descriptor_configuration_cb     (uint8_t index)
{
    // avoid unused parameter warning and keep function signature consistent
    (void)index;

    return desc_configuration;
}

uint16_t const *tud_descriptor_string_cb            (uint8_t index, uint16_t langid)
{
    // TODO: check lang id
    (void) langid;
    size_t char_count;

    // Determine which string descriptor to return
    switch (index) {
        case STRID_LANGID:
            memcpy(&_desc_str[1], string_desc_arr[STRID_LANGID], 2);
            char_count = 1;
            break;

        case STRID_SERIAL:
            // try to read the serial from the board
            char_count = board_usb_get_serial(_desc_str + 1, 32);
            break;

        default:
            // COPYRIGHT NOTE: Based on TinyUSB example
            // Windows wants utf16le

            // Determine which string descriptor to return
            if ( !(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) {
                return NULL;
            }

            // Copy string descriptor into _desc_str
            const char *str = string_desc_arr[index];

            char_count = strlen(str);
            size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
            // Cap at max char
            if (char_count > max_count) {
                char_count = max_count;
            }

            // Convert ASCII string into UTF-16
            for (size_t i = 0; i < char_count; i++) {
                _desc_str[1 + i] = str[i];
            }
            break;
    }

    // First byte is the length (including header), second byte is string type
    _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (char_count * 2 + 2));

    return _desc_str;
}
