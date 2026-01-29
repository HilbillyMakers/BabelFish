#include "uartHandlers.h"
#include "../devicePins/device_pins.h"
#include <string.h>
#include <pico/stdio.h>

void ep_uart_tx_handler(uint16_t len)
{
    
}

void ep_uart_rx_handler(uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize)
{

}

uint8_t com_uart_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");
    return responseSize;
}