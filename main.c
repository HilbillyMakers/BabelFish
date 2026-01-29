#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "modules/usb/usb_interface.h"

#define LED_DELAY_MS 1000u

int main()

{
    timer_hw->dbgpause = 0;
    stdio_init_all();

    initUSB();

    runUSBLoop();

    return 0;
}