#ifndef GPIO_COMMAND_HANDLERS_H
#define GPIO_COMMAND_HANDLERS_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "stdbool.h"

enum gpio_CommandType
{
    BASIC_IO    = 1,
    COM_IO      = 2,
};

enum gpio_ErrorType
{
    E_GPIO_OK,

    E_GPIO_UNKNOWN,
};

uint8_t gpio_commandStringHandler        (uint8_t *commandString,  uint8_t  commandStringLength,
                                          uint8_t *responseBuffer, uint8_t *errorBuffer);
#endif