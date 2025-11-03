#include "usb_device_pins.h"

pinConfig pinConfigs[PIN_NUMBER] = 
{
    {
        .pinID           = 0u,
        .pinFunction     = PIN_FUNCTION_UART,
        .pinSubfunction  = UART_SUB_RX, 
        .pinInitialized  = 1u, /* Pin is initialized by std_init */
    },
    {
        .pinID           = 1u,
        .pinFunction     = PIN_FUNCTION_UART,
        .pinSubfunction  = UART_SUB_TX, 
        .pinInitialized  = 1u, /* Pin is initialized by std_init */
    },
    {
        .pinID           = 2u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 3u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 4u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 5u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 6u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 7u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 8u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 9u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 10u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 11u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 12u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 13u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 14u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 15u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 16u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 17u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 18u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 19u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 20u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 21u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 22u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 23u, /* PS pin */
        .pinFunction     = PIN_FUNCTION_RESERVED,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 1u,
    },
    {
        .pinID           = 24u, /* VBUS sense */
        .pinFunction     = PIN_FUNCTION_RESERVED,
        .pinSubfunction  = 0u, 
        .pinInitialized  = 1u,
    },
    {
        .pinID           = 25u, /* LED_Pin */
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u,
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 26u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u,
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 27u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u,
        .pinInitialized  = 0u,
    },
    {
        .pinID           = 28u,
        .pinFunction     = PIN_FUNCTION_NONE,
        .pinSubfunction  = 0u,
        .pinInitialized  = 0u,
    },
};