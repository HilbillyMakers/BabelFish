#ifndef COMMAND_HANDLERS_H
#define COMMAND_HANDLERS_H

#include "canHandlers/canHandlers.h"
#include "i2cHandlers/i2cHandlers.h"
#include "spiHandlers/spiHandlers.h"
#include "uartHandlers/uartHandlers.h"

uint8_t com_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

#endif