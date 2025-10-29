#ifndef I2C_HANDLERS_H
#define I2C_HANDLERS_H

#include "pico/stdlib.h"

uint8_t com_i2c_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

#endif