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
GP_FUNCTION_DIGITAL           = 0
GP_FUNCTION_ANALOG            = 1
GP_FUNCTION_PWM               = 2

""" Digital IO Command """

DIGITAL_INIT_PIN              = 0
DIGITAL_DEINIT_PIN            = 1
DIGITAL_READ_PIN              = 2
DIGITAL_WRITE_PIN             = 3

DIGITAL_DIRECTION_IN          = 0
DIGITAL_DIRECTION_OUT         = 1

""" PWM Commands """
PWM_INIT_PIN                  = 0
PWM_DEINIT_PIN                = 1
PWM_GET_WRAP_SIZE             = 2
PWM_SET_WRAP_SIZE             = 3
PWM_GET_DUTY_CYCLE            = 4
PWM_SET_DUTY_CYCLE            = 5
PWM_GET_FREQUENCY             = 6
PWM_SET_FREQUENCY             = 7

""" PWM Direction """
PWM_DIRECTION_OUT             = 0
PWM_DIRECTION_IN              = 1

""" SPI Commands """
SPI_INIT_BUS                  = 0
SPI_DEINIT_BUS                = 1
SPI_TRANSFER_DATA             = 2

""" I2C Commands """
I2C_INIT_BUS                  = 0
I2C_DEINIT_BUS                = 1
I2C_SET_BAUDRATE              = 2
I2C_GET_BAUDRATE              = 3
I2C_WRITE_DATA                = 4
I2C_READ_DATA                 = 5
I2C_SCAN_BUS                  = 6
I2C_INFO                      = 7

""" CAN Commands """
CAN_INIT_BUS                  = 0
CAN_DEINIT_BUS                = 1
CAN_SET_BAUDRATE              = 2
CAN_GET_BAUDRATE              = 3
CAN_DETECT_BAUDRATE           = 4
CAN_WRITE_FRAME               = 5
CAN_READ_FRAME                = 6
CAN_SCAN_BUS                  = 7

""" Digital Errors """

DIGITAL_E_OK                  = 0

DIGITAL_E_UNINITIALIZED       = 1
DIGITAL_E_INIT_FAILED         = 2
DIGITAL_E_DEINIT_FAILED       = 3
DIGITAL_E_UNKNOWN_COMMAND     = 4

DIGITAL_E_UNKNOWN             = 5
