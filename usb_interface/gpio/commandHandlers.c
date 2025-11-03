#include "commandHandlers.h"
#include <string.h>
#include "../usb_device_pins.h"

#include "analogHandlers/analogHandlers.h"
#include "digitalHandlers/digitalHandlers.h"
#include "pwmHandlers/pwmHandlers.h"

uint8_t gpio_commandStringHandler         (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t commandType     = 0u;
    uint8_t response_size   = 0u;
    
    *errorBuffer    = E_UNKNOWN;
    commandType     = commandString[0];

    switch(commandType)
    {
        case PIN_FUNCTION_GPIO:
            response_size = gpio_digital_commandStringHandler   (commandString, commandStringLength, responseBuffer, errorBuffer);
        break;

        case PIN_FUNCTION_ANALOG:
            response_size = gpio_analog_commandStringHandler    (commandString, commandStringLength, responseBuffer, errorBuffer);
        break;

        case PIN_FUNCTION_PWM:
            response_size = gpio_pwm_commandStringHandler       (commandString, commandStringLength, responseBuffer, errorBuffer);
        break;

        default:
    }

    return response_size;
}