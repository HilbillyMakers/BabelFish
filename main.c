#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"

#include "modules/usb/usb_interface.h"

#define LED_DELAY_MS 1000u

void core_usb_loop()
{
    initUSB     ();
    runUSBLoop  ();
}

int main()

{
    timer_hw->dbgpause = 0;

    core_usb_loop();
    // /* Launch the usb loop on core1 */
    // multicore_launch_core1(core_usb_loop);

    // while(1)
    // {
    //     tight_loop_contents();
    // }


    return 0;
}