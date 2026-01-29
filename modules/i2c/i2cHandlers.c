#include "i2cHandlers.h"
#include "../devicePins/device_pins.h"
#include <string.h>
#include "pico/stdlib.h"

void ep_i2c_tx_handler(uint16_t len)
{
    
}

uint8_t com_i2c_rx_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");
    return responseSize;
}