#ifndef MESSAGES_H
#define MESSAGES_H

#include "pico/types.h"

enum RxCommand {
  RX_INIT,
  RX_GET_DEVICE_STATUS,
  RX_SET_DEVICE_STATUS,
  RX_SET_PIN_FUNCTION,
  RX_GET_PIN_FUNCTION,
  RX_SET_PIN_STATE,
  RX_GET_PIN_STATE,
};

#endif