#include "commandHandlers.h"

uint8_t com_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");
    return responseSize;
}