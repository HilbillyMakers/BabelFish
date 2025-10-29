import usb

""" Command Types """
COMMAND_TYPE_BASIC_IO       = chr(1)
COMMAND_TYPE_COM_IO         = chr(2)

""" Functions """
FUNCTION_NONE           = chr(0)
FUNCTION_GPIO           = chr(1)
FUNCTION_ANALOG         = chr(2)
FUNCTION_PWM            = chr(3)
FUNCTION_SPI            = chr(4)
FUNCTION_UART           = chr(5)
FUNCTION_I2C            = chr(6)
FUNCTION_RESERVED       = chr(7)    
""" Basic IO Command """

DIGITAL_INIT_PIN            = chr(0)
DIGITAL_DEINIT_PIN          = chr(1)
DIGITAL_READ_PIN            = chr(2)
DIGITAL_WRITE_PIN           = chr(3)

DIGITAL_DIRECTION_IN        = chr(0)
DIGITAL_DIRECTION_OUT       = chr(1)

""" Digital Errors """

DIGITAL_E_OK                = chr(0)

DIGITAL_E_UNINITIALIZED     = chr(1)
DIGITAL_E_INIT_FAILED       = chr(2)
DIGITAL_E_DEINIT_FAILED     = chr(3)
DIGITAL_E_UNKNOWN_COMMAND   = chr(4)

DIGITAL_E_UNKNOWN           = chr(5)

output_endpoint = None
input_endpoint  = None

def initBabelFish():
    global output_endpoint
    global input_endpoint 

    # find our device
    dev = usb.core.find(idVendor=0x0000, idProduct=0x0001)
    
    # was it found?
    if dev is None:
        raise ValueError('Device not found')
    
    # get an endpoint instance
    cfg = dev.get_active_configuration()
    intf = cfg[(0, 0)]

    output_endpoint = usb.util.find_descriptor(intf,
    # match the first OUT endpoint
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)

    input_endpoint = usb.util.find_descriptor(intf,
    # match the first IN endpoint
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)

    assert input_endpoint   is not None
    assert output_endpoint  is not None

def init_DigitalPin(pinID):
    global output_endpoint
    global input_endpoint
    
    command_string = FUNCTION_GPIO + DIGITAL_INIT_PIN + chr(pinID) + DIGITAL_DIRECTION_OUT
    
    output_endpoint.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = input_endpoint.read(64)

    print("Init_DigitalPin response: {}".format(''.join([chr(x) for x in response])))

    return response

def set_DigitalPin(pinID, pinState):
    global output_endpoint
    global input_endpoint

    command_string = FUNCTION_GPIO + DIGITAL_WRITE_PIN + chr(pinID) + chr(pinState)
    
    output_endpoint.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = input_endpoint.read(64)

    print("Set_DigitalPin response: {}".format(''.join([chr(x) for x in response])))

    return response