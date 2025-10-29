#include "endpoint.h"
#include "../usb_lowlevel.h"
#include "commandHandlers.h"

///TODO: implement GPIO command handler

void ep1_rx_handler(uint8_t *buf, uint16_t len)
{
    uint8_t responseBuffer[64]  = {0};
    uint8_t responseSize        = 0u;
    uint8_t errorBuffer         = 0u;

    responseSize = gpio_commandStringHandler(buf, len, &(responseBuffer[0]), &errorBuffer);

    /* Send data back to host */
    struct usb_endpoint_configuration *ep = usb_get_endpoint_configuration(EP1_IN_ADDR);
    usb_start_transfer(ep, &(responseBuffer[0]), responseSize);
}

void ep1_tx_handler(uint8_t *buf, uint16_t len)
{
    printf("Sent %d bytes to host\n", len);
    // Get ready to rx again from host
    usb_start_transfer(usb_get_endpoint_configuration(EP1_OUT_ADDR), NULL, 64);
}