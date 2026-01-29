#ifndef DIGITAL_HANDLERS_H
#define DIGITAL_HANDLERS_H

#include "pico/stdlib.h"

uint8_t gpio_digital_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

#endif