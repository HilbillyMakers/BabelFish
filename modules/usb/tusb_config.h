/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUD_ENABLED         (1)

// Legacy RHPORT configuration
#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT        (0)
#endif
// end legacy RHPORT

//------------------------
// DEVICE CONFIGURATION //
//------------------------

// Enable 1 CDC class
#define CFG_TUD_CDC             (1)
// Set CDC FIFO buffer sizes
#define CFG_TUD_CDC_RX_BUFSIZE  (64)
#define CFG_TUD_CDC_TX_BUFSIZE  (64)
#define CFG_TUD_CDC_EP_BUFSIZE  (64)

// Enable 4 vendor classes
#define CFG_TUD_VENDOR          (4)
// Set vendor FIFO buffer sizes
#define CFG_TUD_VENDOR_RX_BUFSIZE  (64)
#define CFG_TUD_VENDOR_TX_BUFSIZE  (64)
#define CFG_TUD_VENDOR_EP_BUFSIZE  (64)

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE  (64)
#endif

#endif /* _TUSB_CONFIG_H_ */
