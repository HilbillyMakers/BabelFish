#ifndef I2C_ENDPOINT_H
#define I2C_ENDPOINT_H

#include "stdint.h"
#include <stdio.h>

void ep_i2c_tx_handler(uint16_t len);
void ep_i2c_rx_handler(uint8_t const *buf, uint16_t len, uint8_t const *responseBuffer, uint16_t *responseSize);

#endif