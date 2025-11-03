#!/usr/bin/env python3

#
# Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# sudo pip3 install pyusb

# import usb.core
# import usb.util


import BabelFish_API as BF


#test_string = "D" + chr(25) + chr(1)
# test_string = chr(68) + chr(25) + chr(0)
# output_endpoint.write(test_string)
# print(r"Message "+ test_string +" sent successfully")
# from_device = input_endpoint.read(len(test_string))

# print("Device Says: {}".format(''.join([chr(x) for x in from_device])))
BF.initBabelFish()
BF.deinit_PwmPin    (25)
BF.init_PwmPin      (25, BF.PWM_DIRECTION_OUT)
BF.set_PwmDutyCycle (25, 0)