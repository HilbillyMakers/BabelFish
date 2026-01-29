#include "commandHandlers.h"
#include <string.h>
#include "../devicePins/device_pins.h"

#include "analogHandlers/analogHandlers.h"
#include "digitalHandlers/digitalHandlers.h"
#include "pwmHandlers/pwmHandlers.h"

void ep_gp_tx_handler(uint16_t len)
{
    
}

int8_t gpio_rx_commandStringHandler         (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t commandType     = 0u;
    int8_t  response_size   = -1;
    
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