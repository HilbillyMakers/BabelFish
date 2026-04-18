#include "../devicePins/device_pins.h"
#include <string.h>
#include <pico/stdlib.h>

uint8_t com_can_rx_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");

    responseSize = E_OK;

    return responseSize;
}

uint8_t com_can_tx_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{

}