#!/usr/bin/env python3

import BabelFish_API as BF
import BabelFish_Data as BFD

BF.initBabelFish()
BF.send_data(BF.INTERFACE_CAN, "test")