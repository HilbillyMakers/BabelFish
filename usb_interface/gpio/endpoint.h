#ifndef GPIO_ENDPOINT_H
#define GPIO_ENDPOINT_H

#include "stdint.h"
#include <stdio.h>

void ep1_tx_handler (uint8_t *buf, uint16_t len);
void ep1_rx_handler (uint8_t *buf, uint16_t len);

#endif