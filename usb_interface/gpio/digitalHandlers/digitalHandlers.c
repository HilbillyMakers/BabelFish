#include "digitalHandlers.h"
#include "../../usb_device_pins.h"
#include <string.h>

enum 
{
    DIGITAL_INIT_PIN            = 0u,
    DIGITAL_DEINIT_PIN          = 1u,
    DIGITAL_READ_PIN            = 2u,
    DIGITAL_WRITE_PIN           = 3u,
};

enum
{
    DIGITAL_E_OK                = 0u,

    DIGITAL_E_UNINITIALIZED     = 1u,
    DIGITAL_E_INIT_FAILED       = 2u,
    DIGITAL_E_DEINIT_FAILED     = 3u,
    DIGITAL_E_UNKNOWN_COMMAND   = 4u,

    DIGITAL_E_UNKNOWN
};

enum
{
    DIGITAL_DIRECTION_IN,
    DIGITAL_DIRECTION_OUT
};

/* Local Helper functions */

uint8_t gpio_digital_initPin    (uint8_t pinID, bool outputPin, uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_digital_deinitPin  (uint8_t pinID,                 uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_digital_read       (uint8_t pinID,                 uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_digital_write      (uint8_t pinID, bool pinState,  uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_digital_unknown    (                               uint8_t *responseBuffer, uint8_t *errorBuffer);

uint8_t gpio_digital_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t command         = commandString[1];
    uint8_t pinID           = commandString[2];
    uint8_t argument        = 0u;
    uint8_t responseSize    = 0u;

    *errorBuffer = DIGITAL_E_UNKNOWN;

    switch(command)
    {
        case DIGITAL_INIT_PIN:
            /* Setup digital pin as input or output */
            argument        = commandString[3];
            responseSize    = gpio_digital_initPin    (pinID, argument,   responseBuffer, errorBuffer);
        break;
        
        case DIGITAL_DEINIT_PIN:
            /* Setup digital pin as input or output */
            responseSize    = gpio_digital_deinitPin  (pinID,             responseBuffer, errorBuffer);
        break;

        case DIGITAL_READ_PIN:
            argument        = commandString[3];
            responseSize    = gpio_digital_read       (pinID,             responseBuffer, errorBuffer);
        break;

        case DIGITAL_WRITE_PIN:
            argument        = commandString[3];
            responseSize    = gpio_digital_write      (pinID, argument,   responseBuffer, errorBuffer);
        break;

        default:
            responseSize    = gpio_digital_unknown    (                   responseBuffer, errorBuffer);
    }

    return responseSize;
}

uint8_t gpio_digital_initPin(uint8_t pinID, bool outputPin, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    if(PIN_FUNCTION_NONE == pinConfigs[pinID].pinInitialized)
    {
        /*  */
        gpio_init   (pinID);
        gpio_set_dir(pinID, outputPin ? DIGITAL_DIRECTION_OUT : DIGITAL_DIRECTION_IN);

        pinConfigs[pinID].pinFunction       = PIN_FUNCTION_GPIO;
        pinConfigs[pinID].pinSubfunction    = outputPin ? GPIO_SUB_OUT : GPIO_SUB_IN;
        pinConfigs[pinID].pinInitialized    = true;

        memcpy(responseBuffer, "Pin Initialized", sizeof("Pin Initialized"));
        responseSize = sizeof("Pin Initialized");
        *errorBuffer = DIGITAL_E_OK;
    }
    else
    {
        /* Pin is already initialized */
        memcpy(responseBuffer, "Pin already initialized", sizeof("Pin already initialized"));
        responseSize = sizeof("Pin already initialized");
        *errorBuffer = DIGITAL_E_INIT_FAILED;
    }

    return responseSize;
}

uint8_t gpio_digital_deinitPin(uint8_t pinID, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    if(
          (true                 == pinConfigs[pinID].pinInitialized ) &&
          (PIN_FUNCTION_GPIO    == pinConfigs[pinID].pinFunction    )
    )
    {
        /*  */
        gpio_deinit (pinID);

        pinConfigs[pinID].pinInitialized    = false;
        pinConfigs[pinID].pinFunction       = PIN_FUNCTION_NONE;
        pinConfigs[pinID].pinSubfunction    = 0u;

        memcpy(responseBuffer, "Pin Deinitialized", sizeof("Pin Deinitialized"));
        responseSize = sizeof("Pin Deinitialized");
        *errorBuffer = DIGITAL_E_OK;
    }
    else
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize = sizeof("Pin not initialized");
        *errorBuffer = DIGITAL_E_DEINIT_FAILED;
    }

    return responseSize;
}

uint8_t gpio_digital_read(uint8_t pinID, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    if(
          (PIN_FUNCTION_GPIO  == pinConfigs[pinID].pinFunction   ) &&
          (GPIO_SUB_IN        == pinConfigs[pinID].pinSubfunction) &&
          (true               == pinConfigs[pinID].pinInitialized)
    )
    {
        /*  */
        *responseBuffer  = gpio_get(pinID);

        memcpy(responseBuffer, "Pin Read", sizeof("Pin Read"));
        responseSize = sizeof("Pin Read");
        *errorBuffer = DIGITAL_E_OK;
    }
    else
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize = sizeof("Pin not initialized");
        *errorBuffer = DIGITAL_E_UNINITIALIZED;
    }

    return responseSize;
}

uint8_t gpio_digital_write(uint8_t pinID, bool pinState, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    if(
        (PIN_FUNCTION_GPIO  == pinConfigs[pinID].pinFunction   ) &&
        (GPIO_SUB_OUT       == pinConfigs[pinID].pinSubfunction) &&
        (true               == pinConfigs[pinID].pinInitialized)
    )
    {
        /*  */
        gpio_put(pinID, pinState);
        
        memcpy(responseBuffer, "Pin Written", sizeof("Pin Written"));
        responseSize = sizeof("Pin Written");
        *errorBuffer = DIGITAL_E_OK;
    }
    else
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize = sizeof("Pin not initialized");
        *errorBuffer = DIGITAL_E_UNINITIALIZED;
    }

    return responseSize;
}

uint8_t gpio_digital_unknown(uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    memcpy(responseBuffer, "Unknown Digital Command", sizeof("Unknown Digital Command"));
        
    *errorBuffer = DIGITAL_E_UNKNOWN_COMMAND;
    responseSize = 0u;
    return responseSize;
}