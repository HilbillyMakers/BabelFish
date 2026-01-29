#ifndef ANALOG_HANDLERS_H
#define ANALOG_HANDLERS_H

#include "analogHandlers_Types.h"

uint8_t gpio_analog_commandStringHandler(uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

#endif