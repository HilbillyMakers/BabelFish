#!/usr/bin/env python3

import BabelFish_API as BF
import BabelFish_Data as BFD

BF.initBabelFish()
BF.send_data(BF.INTERFACE_CAN, "test")

BF.info_I2C()

# SPI function test
spi_init_response = BF.init_SPI     (115200)

# write to a first CAN buffer
write_ans  = BF.transfer_SPI_data   ([0x81, 0x31, 0x1, 0x10, 0x10, 0x1, 0],  1)
status_ans = BF.transfer_SPI_data   ( [0xa0],                                1)
print("Write to first CAN buffer ", write_ans, " and status: ", status_ans)

# write to a second CAN buffer
write_ans  = BF.transfer_SPI_data   ([0x82, 0x41, 0x1, 0x10, 0x10, 0x1, 0],  1)
status_ans = BF.transfer_SPI_data   ([0xa0],                                 1)
print("Write to second CAN buffer ", write_ans, " and status: ", status_ans)

# write to a third CAN buffer
write_ans  = BF.transfer_SPI_data   ([0x84, 0x51, 0x1, 0x10, 0x10, 0x1, 0],  1)
status_ans = BF.transfer_SPI_data   ([0xa0],                                 1)
print("Write to third CAN buffer ", write_ans, " and status: ", status_ans)

BF.deinit_SPI                       ()