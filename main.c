#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "usb_interface/usb_lowlevel.h"
#include "usb_protocol/usb_protocol.h"

#define LED_DELAY_MS 1000u


int main()

{
    timer_hw->dbgpause = 0;
    stdio_init_all();
    printf("USB Device Low-Level hardware example\n");
    usb_device_init();

    // Wait until configured
    while (!configured) {
        tight_loop_contents();
    }
    
    // Get ready to rx from host
    usb_start_transfer(usb_get_endpoint_configuration(EP1_OUT_ADDR), NULL, 64);

    // Everything is interrupt driven so just loop here
    while (1) {
        tight_loop_contents();
    }
}