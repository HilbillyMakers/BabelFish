#ifndef PWM_HANDLERS_H
#define PWM_HANDLERS_H

#include "pico/stdlib.h"

uint8_t gpio_pwm_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer);

#endif