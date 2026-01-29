#ifndef CAN_HANDLERS_H
#define CAN_HANDLERS_H

#include "pico/stdlib.h"

uint8_t com_can_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

#endif