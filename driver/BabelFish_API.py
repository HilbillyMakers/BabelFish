import usb
import BabelFish_Data as BFD

""" Interface containers """
INTERFACE_UART_CDC_NOTIFICATION = None
INTERFACE_UART_CDC_DATA         = None
INTERFACE_GP                    = None
INTERFACE_CAN                   = None
INTERFACE_I2C                   = None
INTERFACE_SPI                   = None

""" Endpoint containers """
ENDPOINT_UART_CDC_NOTIFICATION = None
ENDPOINT_UART_CDC_OUT          = None
ENDPOINT_UART_CDC_IN           = None

ENDPOINT_GP_OUT                = None
ENDPOINT_GP_IN                 = None

ENDPOINT_CAN_OUT               = None
ENDPOINT_CAN_IN                = None

ENDPOINT_I2C_OUT               = None
ENDPOINT_I2C_IN                = None

ENDPOINT_SPI_OUT               = None
ENDPOINT_SPI_IN                = None


def printDeviceInfo():
    dev = usb.core.find(idVendor=BFD.BOARD_ID_BABELFISH)
    
    # was it found?
    if dev is None:
        raise ValueError('Device not found')
    
    # get an endpoint instance
    cfg = dev.get_active_configuration()

    print(cfg)

def initBabelFish():
    global INTERFACE_UART_CDC_NOTIFICATION
    global INTERFACE_UART_CDC_DATA        
    global INTERFACE_GP                   
    global INTERFACE_CAN                  
    global INTERFACE_I2C                  
    global INTERFACE_SPI                  
    global ENDPOINT_UART_CDC_NOTIFICATION 
    global ENDPOINT_UART_CDC_OUT          
    global ENDPOINT_UART_CDC_IN           
    global ENDPOINT_GP_OUT                
    global ENDPOINT_GP_IN                 
    global ENDPOINT_CAN_OUT               
    global ENDPOINT_CAN_IN                
    global ENDPOINT_I2C_OUT               
    global ENDPOINT_I2C_IN                
    global ENDPOINT_SPI_OUT               
    global ENDPOINT_SPI_IN                

    # find our device
    dev = usb.core.find(idVendor=BFD.BOARD_ID_BABELFISH)
    
    # was it found?
    if dev is None:
        raise ValueError('Device not found')
    
    # get an endpoint instance
    cfg = dev.get_active_configuration()

    intf = cfg[(BFD.INTERFACE_ID_GP, 0)]
    INTERFACE_UART_CDC_NOTIFICATION = cfg[(BFD.INTERFACE_ID_UART_CDC_NOTIF, 0)]
    INTERFACE_UART_CDC_DATA         = cfg[(BFD.INTERFACE_ID_UART_CDC_DATA,  0)]
    INTERFACE_GP                    = cfg[(BFD.INTERFACE_ID_GP,             0)]
    INTERFACE_CAN                   = cfg[(BFD.INTERFACE_ID_CAN,            0)]
    INTERFACE_I2C                   = cfg[(BFD.INTERFACE_ID_I2C,            0)]
    INTERFACE_SPI                   = cfg[(BFD.INTERFACE_ID_SPI,            0)]

    ENDPOINT_UART_CDC_NOTIFICATION = usb.util.find_descriptor(INTERFACE_UART_CDC_NOTIFICATION,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    ENDPOINT_UART_CDC_OUT          = usb.util.find_descriptor(INTERFACE_UART_CDC_DATA,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ENDPOINT_UART_CDC_IN           = usb.util.find_descriptor(INTERFACE_UART_CDC_DATA,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    ENDPOINT_CAN_OUT               = usb.util.find_descriptor(INTERFACE_CAN,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ENDPOINT_CAN_IN                = usb.util.find_descriptor(INTERFACE_CAN,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    ENDPOINT_GP_OUT                = usb.util.find_descriptor(INTERFACE_GP,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ENDPOINT_GP_IN                 = usb.util.find_descriptor(INTERFACE_GP,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    ENDPOINT_I2C_OUT               = usb.util.find_descriptor(INTERFACE_I2C,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ENDPOINT_I2C_IN                = usb.util.find_descriptor(INTERFACE_I2C,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    ENDPOINT_SPI_OUT               = usb.util.find_descriptor(INTERFACE_SPI,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ENDPOINT_SPI_IN                = usb.util.find_descriptor(INTERFACE_SPI,
        custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)      

    assert INTERFACE_UART_CDC_NOTIFICATION is not None
    assert INTERFACE_UART_CDC_DATA         is not None
    assert INTERFACE_GP                    is not None
    assert INTERFACE_CAN                   is not None
    assert INTERFACE_I2C                   is not None
    assert INTERFACE_SPI                   is not None
    assert ENDPOINT_UART_CDC_NOTIFICATION  is not None
    assert ENDPOINT_UART_CDC_OUT           is not None
    assert ENDPOINT_UART_CDC_IN            is not None
    assert ENDPOINT_GP_OUT                 is not None
    assert ENDPOINT_GP_IN                  is not None
    assert ENDPOINT_CAN_OUT                is not None
    assert ENDPOINT_CAN_IN                 is not None
    assert ENDPOINT_I2C_OUT                is not None
    assert ENDPOINT_I2C_IN                 is not None
    assert ENDPOINT_SPI_OUT                is not None
    assert ENDPOINT_SPI_IN                 is not None

""" Digital Functions """
def init_DigitalPin_Output(pinID):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN
    
    command_string = BFD.GP_FUNCTION_DIGITAL + BFD.DIGITAL_INIT_PIN + chr(pinID) + BFD.DIGITAL_DIRECTION_OUT
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Init_DigitalPin response: {}".format(''.join([chr(x) for x in response])))

    return response

def set_DigitalPin(pinID, pinState):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN

    command_string = BFD.GP_FUNCTION_DIGITAL + BFD.DIGITAL_WRITE_PIN + chr(pinID) + chr(pinState)
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Set_DigitalPin response: {}".format(''.join([chr(x) for x in response])))

    return response

""" PWM Functions """
def init_PwmPin(pinID, direction):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN

    command_string = BFD.GP_FUNCTION_PWM + BFD.PWM_INIT_PIN + chr(pinID) + direction
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Init_PwmPin response: {}".format(''.join([chr(x) for x in response])))

    return response

def deinit_PwmPin(pinID):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN

    command_string = BFD.GP_FUNCTION_PWM + BFD.PWM_DEINIT_PIN + chr(pinID)
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Deinit_PwmPin response: {}".format(''.join([chr(x) for x in response])))

    return response

def set_PwmDutyCycle(pinID, dutyCycle):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN

    dutyCycleLower = int(dutyCycle / 256)
    dutyCycleUpper = int(dutyCycle % 255)

    command_string = BFD.GP_FUNCTION_PWM + BFD.PWM_SET_DUTY_CYCLE + chr(pinID) + chr(dutyCycleUpper) + chr(dutyCycleLower)
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Set_PwmDutyCycle response: {}".format(''.join([chr(x) for x in response])))

    return response

""" ANALOG Functions """
def init_AnalogPin(pinID):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN

    command_string = BFD.GP_FUNCTION_ANALOG + BFD.ANALOG_INIT_PIN + chr(pinID)
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Init_AnalogPin response: {}".format(''.join([chr(x) for x in response])))

    return response

def read_AnalogPin(pinID):
    global INTERFACE_GP
    global ENDPOINT_GP_OUT
    global ENDPOINT_GP_IN

    command_string = BFD.GP_FUNCTION_ANALOG + BFD.ANALOG_READ_PIN + chr(pinID)
    
    ENDPOINT_GP_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_GP_IN.read(64)

    print("Read_AnalogPin response: {}".format(''.join([chr(x) for x in response])))

    return response

""" I2C Functions """
def init_I2C(frequency):
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    frequencyLower = int(frequency / 256)
    frequencyUpper = int(frequency % 255)

    command_string = BFD.I2C_INIT_BUS  + chr(frequencyUpper) + chr(frequencyLower)
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)

    print("Init_I2C response: {}".format(''.join([chr(x) for x in response])))

    return response

def deinit_I2C():
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    command_string = BFD.I2C_DEINIT_BUS
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)

    print("Deinit_I2C response: {}".format(''.join([chr(x) for x in response])))

    return response

def set_I2C_baudRate(baudRate):
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    baudRateLower = int(baudRate / 256)
    baudRateUpper = int(baudRate % 255)

    command_string = BFD.I2C_SET_BAUDRATE + chr(baudRateUpper) + chr(baudRateLower)
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)

    print("Set_I2C_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def get_I2C_baudRate():
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    command_string = BFD.I2C_GET_BAUDRATE
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)

    print("Get_I2C_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def write_I2C(deviceAddress, dataArray):
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    command_string = BFD.I2C_WRITE_DATA + chr(deviceAddress) + chr(dataArray)
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)

    print("Write_I2C response: {}".format(''.join([chr(x) for x in response])))

    return response

def read_I2C(deviceAddress):
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    command_string = BFD.I2C_READ_DATA + chr(deviceAddress)
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)

    print("Read_I2C response: {}".format(''.join([chr(x) for x in response])))

    return response

def scan_I2C():
    global INTERFACE_I2C
    global ENDPOINT_I2C_OUT
    global ENDPOINT_I2C_IN

    command_string = BFD.I2C_SCAN_BUS
    
    ENDPOINT_I2C_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_I2C_IN.read(64)
    response = ENDPOINT_I2C_IN.read

    print("Scan_I2C response: {}".format(''.join([chr(x) for x in response])))

    return response

""" SPI Functions """
def init_SPI(frequency, mode):
    global INTERFACE_SPI
    global ENDPOINT_SPI_OUT
    global ENDPOINT_SPI_IN

    frequencyLower = int(frequency / 256)
    frequencyUpper = int(frequency % 255)

    command_string = BFD.SPI_INIT_BUS+ chr(frequencyUpper) + chr(frequencyLower) + chr(mode)
    
    ENDPOINT_SPI_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_SPI_IN.read(64)

    print("Init_SPI response: {}".format(''.join([chr(x) for x in response])))

    return response

def deinit_SPI():
    global INTERFACE_SPI
    global ENDPOINT_SPI_OUT
    global ENDPOINT_SPI_IN

    command_string = BFD.SPI_DEINIT_BUS
    
    ENDPOINT_SPI_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_SPI_IN.read(64)

    print("Deinit_SPI response: {}".format(''.join([chr(x) for x in response])))

    return response

def set_SPI_baudRate(baudRate):
    global INTERFACE_SPI
    global ENDPOINT_SPI_OUT
    global ENDPOINT_SPI_IN

    baudRateLower = int(baudRate / 256)
    baudRateUpper = int(baudRate % 255)

    command_string = BFD.SPI_SET_BAUDRATE + chr(baudRateUpper) + chr(baudRateLower)
    
    ENDPOINT_SPI_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_SPI_IN.read(64)

    print("Set_SPI_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def get_SPI_baudRate():
    global INTERFACE_SPI
    global ENDPOINT_SPI_OUT
    global ENDPOINT_SPI_IN

    command_string = BFD.SPI_GET_BAUDRATE
    
    ENDPOINT_SPI_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_SPI_IN.read(64)

    print("Get_SPI_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def write_SPI(dataArray):
    global INTERFACE_SPI
    global ENDPOINT_SPI_OUT
    global ENDPOINT_SPI_IN

    command_string = BFD.SPI_WRITE_DATA + chr(dataArray)
    
    ENDPOINT_SPI_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_SPI_IN.read(64)

    print("Write_SPI response: {}".format(''.join([chr(x) for x in response])))

    return response

def read_SPI():
    global INTERFACE_SPI
    global ENDPOINT_SPI_OUT
    global ENDPOINT_SPI_IN

    command_string = BFD.SPI_READ_DATA
    
    ENDPOINT_SPI_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_SPI_IN.read(64)

    print("Read_SPI response: {}".format(''.join([chr(x) for x in response])))

    return response

""" CAN Functions """
def init_CAN(baudRate):
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    baudRateLower = int(baudRate / 256)
    baudRateUpper = int(baudRate % 255)

    command_string = BFD.CAN_INIT_BUS + chr(baudRateUpper) + chr(baudRateLower)
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Init_CAN response: {}".format(''.join([chr(x) for x in response])))

    return response

def deinit_CAN():
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_DEINIT_BUS
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Deinit_CAN response: {}".format(''.join([chr(x) for x in response])))

    return response

def set_CAN_baudRate(baudRate):
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    baudRateLower = int(baudRate / 256)
    baudRateUpper = int(baudRate % 255)

    command_string = BFD.CAN_SET_BAUDRATE + chr(baudRateUpper) + chr(baudRateLower)
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Set_CAN_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def get_CAN_baudRate():
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_GET_BAUDRATE
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Get_CAN_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def detect_CAN_baudRate():
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_DETECT_BAUDRATE
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Detect_CAN_baudRate response: {}".format(''.join([chr(x) for x in response])))

    return response

def write_CAN_frame(messageID, dataArray):
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_WRITE_MESSAGE + chr(messageID) + chr(dataArray)
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Write_CAN response: {}".format(''.join([chr(x) for x in response])))

    return response

def write_CAN_frame_cyclic(messageID, dataArray, periodMs):
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_WRITE_MESSAGE_CYCLIC + chr(messageID) + chr(dataArray) + chr(periodMs)
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Write_CAN_cyclic response: {}".format(''.join([chr(x) for x in response])))

    return response

def read_CAN_frame(messageID):
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_READ_MESSAGE + chr(messageID)
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Read_CAN response: {}".format(''.join([chr(x) for x in response])))

    return response

def scan_CAN_bus():
    global INTERFACE_CAN
    global ENDPOINT_CAN_OUT
    global ENDPOINT_CAN_IN

    command_string = BFD.CAN_SCAN_BUS
    
    ENDPOINT_CAN_OUT.write(command_string)
    print("Message " + command_string + " sent successfully")
    
    response = ENDPOINT_CAN_IN.read(64)

    print("Scan_CAN response: {}".format(''.join([chr(x) for x in response])))

    return response