#include "canHandlers.h"
#include "../devicePins/device_pins.h"
#include <string.h>

void ep_can_tx_handler(uint32_t len)
{
    
}

int8_t com_can_rx_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    int8_t responseSize = -1;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");

    responseSize = E_OK;

    return responseSize;
}