""" Board Identification """
BOARD_ID_BABELFISH            = 0xcafe


""" Endpoint IDs"""
ENDPOINT_UART_CDC_NOTIF       = 0x81
ENDPOINT_UART_CDC_OUT         = 0x02
ENDPOINT_UART_CDC_IN          = 0x82
ENDPOINT_CAN_OUT              = 0x03
ENDPOINT_CAN_IN               = 0x83
ENDPOINT_GP_OUT               = 0x04
ENDPOINT_GP_IN                = 0x84
ENDPOINT_I2C_OUT              = 0x05
ENDPOINT_I2C_IN               = 0x85
ENDPOINT_SPI_OUT              = 0x06
ENDPOINT_SPI_IN               = 0x86

""" Interface IDs """
INTERFACE_ID_UART_CDC_NOTIF   = 0
INTERFACE_ID_UART_CDC_DATA    = 1
INTERFACE_ID_CAN              = 2
INTERFACE_ID_GP               = 3
INTERFACE_ID_I2C              = 4
INTERFACE_ID_SPI              = 5

""" Functions """
GP_FUNCTION_DIGITAL           = chr(0)
GP_FUNCTION_ANALOG            = chr(1)
GP_FUNCTION_PWM               = chr(2)

""" Digital IO Command """

DIGITAL_INIT_PIN              = chr(0)
DIGITAL_DEINIT_PIN            = chr(1)
DIGITAL_READ_PIN              = chr(2)
DIGITAL_WRITE_PIN             = chr(3)

DIGITAL_DIRECTION_IN          = chr(0)
DIGITAL_DIRECTION_OUT         = chr(1)

""" PWM Commands """
PWM_INIT_PIN                  = chr(0)
PWM_DEINIT_PIN                = chr(1)
PWM_GET_WRAP_SIZE             = chr(2)
PWM_SET_WRAP_SIZE             = chr(3)
PWM_GET_DUTY_CYCLE            = chr(4)
PWM_SET_DUTY_CYCLE            = chr(5)
PWM_GET_FREQUENCY             = chr(6)
PWM_SET_FREQUENCY             = chr(7)

""" PWM Direction """
PWM_DIRECTION_OUT             = chr(0)
PWM_DIRECTION_IN              = chr(1)

""" SPI Commands """
SPI_INIT_BUS                  = chr(0)
SPI_DEINIT_BUS                = chr(1)
SPI_SET_BAUDRATE              = chr(2)
SPI_GET_BAUDRATE              = chr(3)
SPI_WRITE_DATA                = chr(4)
SPI_READ_DATA                 = chr(5)

""" I2C Commands """
I2C_INIT_BUS                  = chr(0)
I2C_DEINIT_BUS                = chr(1)
I2C_SET_BAUDRATE              = chr(2)
I2C_GET_BAUDRATE              = chr(3)
I2C_WRITE_DATA                = chr(4)
I2C_READ_DATA                 = chr(5)
I2C_SCAN_BUS                  = chr(6)

""" CAN Commands """
CAN_INIT_BUS                  = chr(0)
CAN_DEINIT_BUS                = chr(1)
CAN_SET_BAUDRATE              = chr(2)
CAN_GET_BAUDRATE              = chr(3)
CAN_DETECT_BAUDRATE           = chr(4)
CAN_WRITE_FRAME               = chr(5)
CAN_READ_FRAME                = chr(6)
CAN_SCAN_BUS                  = chr(7)

""" Digital Errors """

DIGITAL_E_OK                  = chr(0)

DIGITAL_E_UNINITIALIZED       = chr(1)
DIGITAL_E_INIT_FAILED         = chr(2)
DIGITAL_E_DEINIT_FAILED       = chr(3)
DIGITAL_E_UNKNOWN_COMMAND     = chr(4)

DIGITAL_E_UNKNOWN             = chr(5)