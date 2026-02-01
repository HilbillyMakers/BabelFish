#include "i2cHandlers.h"
#include "../devicePins/device_pins.h"
#include <string.h>
#include "hardware/i2c.h"

#define PROTOTYPE_I2C       i2c0

#define I2C_SDA_PIN         16
#define I2C_SCL_PIN         17
#define I2C_RET_ERROR_SIZE  -1

enum
{
    I2C_INIT,
    I2C_DEINIT,
    I2C_READ,
    I2C_WRITE,
};

typedef enum
{
    I2C_MASTER_MODE,
    I2C_SLAVE_MODE
} i2c_mode_e;

static inline int8_t com_i2c_init     (uint8_t* const configuration,  uint8_t* const responseBuffer, uint8_t* const errorBuffer);
static inline int8_t com_i2c_deinit   (                               uint8_t* const responseBuffer, uint8_t* const errorBuffer);
static inline int8_t com_i2c_write    (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer);
static inline int8_t com_i2c_read     (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer);

int8_t com_i2c_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t     command         =   commandString[0];
    uint8_t     responseSize    =   0u;

    switch(command)
    {
        case I2C_INIT:
            responseSize    = com_i2c_init      (&commandString[1],  responseBuffer, errorBuffer);
        break;

        case I2C_DEINIT:
            responseSize    = com_i2c_deinit    (                    responseBuffer, errorBuffer);
        break;

        case I2C_READ:
            responseSize    = com_i2c_read      (&commandString[1],  responseBuffer, errorBuffer);
        
        case I2C_WRITE:
            responseSize    = com_i2c_write     (&commandString[1],  responseBuffer, errorBuffer);
        break;
    }

    return responseSize;
}

int8_t com_i2c_init        (uint8_t* const configuration, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    i2c_mode_e  i2c_mode                = I2C_MASTER_MODE;
    uint32_t    baudRate                = 0u;
    uint32_t    physicalBaudRate        = 0u;
    uint8_t     i2c_address             = 0u;

    baudRate            = *((uint32_t *)&configuration[0]);
    i2c_mode            = configuration[4];
    i2c_address         = configuration[5];


    physicalBaudRate    = i2c_init(PROTOTYPE_I2C, baudRate);
    
    *((uint32_t *)responseBuffer)   =   physicalBaudRate;
    
    gpio_set_function   (I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function   (I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up        (I2C_SDA_PIN);
    gpio_pull_up        (I2C_SCL_PIN);
    
    i2c_set_slave_mode  (PROTOTYPE_I2C, i2c_mode, i2c_address);

    return sizeof(physicalBaudRate);
}

int8_t com_i2c_deinit      (uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    i2c_deinit(PROTOTYPE_I2C);

    return 0;
}

int8_t com_i2c_read        (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t     address         =  hostMessage[0];
    uint8_t     messageSize     =  hostMessage[1];
    int8_t      receivedBytes   = 0u;

    receivedBytes = i2c_read_blocking(PROTOTYPE_I2C, address, responseBuffer, messageSize, false);

    if(PICO_ERROR_GENERIC == receivedBytes)
    {
        receivedBytes   = I2C_RET_ERROR_SIZE;
       *errorBuffer     = E_I2C_NOT_RECEIVED;
    }
    else if(0 == receivedBytes)
    {
        receivedBytes   = I2C_RET_ERROR_SIZE;
        *errorBuffer    = E_I2C_EMPTY_MESSAGE;
    }

    return receivedBytes;
}

int8_t com_i2c_write        (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t     address     =  hostMessage[0];
    uint8_t     messageSize =  hostMessage[1];
    uint8_t*    message     = &hostMessage[2];
    int8_t      sentBytes   =  0u;

    if(!messageSize)
    {
        sentBytes       = I2C_RET_ERROR_SIZE;
        *errorBuffer    = E_I2C_NOTHING_TO_SEND;
    }

    sentBytes = i2c_write_blocking(PROTOTYPE_I2C, address, message, messageSize, false);

    if(!sentBytes)
    {
        sentBytes       = I2C_RET_ERROR_SIZE;
       *errorBuffer     = E_I2C_NOT_SENT;
    }

    return sentBytes;
}