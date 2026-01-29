#include "endpoint.h"
#include "commandHandlers.h"

void ep_gp_rx_handler(uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t responseSize)
{
    printf("GP RX Handler called with message %s\n", buf);
}

void ep_gp_tx_handler(uint16_t len)
{

}