#ifndef CAN_ENDPOINT_H
#define CAN_ENDPOINT_H

#include "stdint.h"
#include <stdio.h>

void ep2_tx_handler (uint8_t *buf, uint16_t len);
void ep2_rx_handler(uint8_t *buf, uint16_t len);

#endif