#ifndef DEVICE_PINS_H
#define DEVICE_PINS_H

#define PIN_NUMBER 29

enum PinFunction
{
    PIN_FUNCTION_NONE       = 0u,
    PIN_FUNCTION_GPIO       = 1u,
    PIN_FUNCTION_ANALOG     = 2u,
    PIN_FUNCTION_PWM        = 3u,
    PIN_FUNCTION_SPI        = 4u,
    PIN_FUNCTION_UART       = 5u,
    PIN_FUNCTION_I2C        = 6u,
    PIN_FUNCTION_CAN        = 7u,
    
    PIN_FUNCTION_RESERVED   ,
};

enum GpioSubFunction
{
    GPIO_SUB_IN,
    GPIO_SUB_OUT,
};

enum AnalogSubFunction
{
    ANALOG_SUB_NONE
};

enum PwmSubFunction
{
    PWM_SUB_OUT,
    PWM_SUB_IN,
};

enum SpiSubFunction
{
    SPI_SUB_CS,
    SPI_SUB_MOSI,
    SPI_SUB_MISO,
    SPI_SUB_CLK,
};

enum UartSubFunction
{
    UART_SUB_RX,
    UART_SUB_TX,
};

enum I2cSubFunction
{
    I2C_SUB_SDA,
    I2C_SUB_SDL,
};

typedef struct 
{
  unsigned short pinID;
  unsigned short pinFunction;
  unsigned short pinSubfunction;
  unsigned short pinInitialized;
} pinConfig;

enum
{
    E_I2C_NOT_SENT,
    E_I2C_NOTHING_TO_SEND,
    E_I2C_NOT_RECEIVED,
    E_I2C_EMPTY_MESSAGE,
    E_I2C_GENERIC,
};



extern pinConfig pinConfigs[PIN_NUMBER];

#endif