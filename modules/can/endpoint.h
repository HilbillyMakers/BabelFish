#ifndef CAN_ENDPOINT_H
#define CAN_ENDPOINT_H

#include "stdint.h"
#include <stdio.h>

void ep_can_tx_handler(uint32_t len);
void ep_can_rx_handler(uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t responseSize);

#endif