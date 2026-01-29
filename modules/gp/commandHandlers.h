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

#endif