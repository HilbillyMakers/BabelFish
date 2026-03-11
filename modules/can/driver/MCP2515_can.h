#ifndef MCP2515_H
#define MCP2515_H

#include "canDefinitions.h"
#include "icDefinitions.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

typedef enum {
    MCP2515_E_OK              ,
    MCP2515_E_DMA_UNAVAILABLE ,
    MCP2515_E_FAIL            ,
    MCP2515_E_ALLTXBUSY       ,
    MCP2515_E_FAILINIT        ,
    MCP2515_E_FAILTX          ,
    MCP2515_E_NOMSG           ,
    MCP2515_E_MSGTOOBIG       ,
    MCP2515_E_TOOBIG          ,
    MCP2515_E_MALFORMED_INPUT ,

    MCP2515_E_UNKNOWN
} MCP2515_Error;

typedef struct  {
    uint8_t         CS_PIN;
    uint8_t         TX_PIN;
    uint8_t         RX_PIN;
    uint8_t         SCK_PIN;
    uint8_t         INT_PIN;
    uint32_t        SPI_CLOCK;
    spi_inst_t     *SPI_INSTANCE;

    uint8_t         rxStatus;
    uint8_t         icStatus;
    MCP2515_Error   error;
}MCP2515_instance;

/**
 * @brief Initialize the MCP2515 SPI peripheral and pins
 * 
 * @param instance MCP2515 structure pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_init              (MCP2515_instance *instance);

/**
 * @brief Reset the MCP2515 filters, masks and state
 * 
 * @param instance MCP2515 structure pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_reset             (MCP2515_instance *instance);

MCP2515_Error MCP2515_setConfigMode     (MCP2515_instance *instance);
MCP2515_Error MCP2515_setListenOnlyMode (MCP2515_instance *instance);
MCP2515_Error MCP2515_setSleepMode      (MCP2515_instance *instance);
MCP2515_Error MCP2515_setLoopbackMode   (MCP2515_instance *instance);

/**
 * @brief Read and acknowledge masked and filtered messages
 * 
 * @param instance MCP2515 structure pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_setNormalMode     (MCP2515_instance *instance);

MCP2515_Error MCP2515_setClkOut         (MCP2515_instance *instance, const CAN_CLKOUT divisor);

/**
 * @brief Set the clock and bitrate
 * 
 * @param instance MCP2515 structure pointer
 * @param canSpeed Communication speed bps enum
 * @param canClock The hardware oscilator value enum
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_setBitrate        (MCP2515_instance *instance, const CAN_SPEED  canSpeed,  const CAN_CLOCK canClock                       );


MCP2515_Error MCP2515_setFilterMask     (MCP2515_instance *instance, const MASK       num,       const bool ext,         const uint32_t ulData  );
MCP2515_Error MCP2515_setFilter         (MCP2515_instance *instance, const RXF        num,       const bool ext,         const uint32_t ulData  );

/**
 * @brief Send CAN frame using a specific TX buffer
 * 
 * @param instance  MCP2515 structure pointer
 * @param rxbn      TX buffer index
 * @param frame     Sent CAN frame buffer pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_sendMessage_Buff  (MCP2515_instance *instance, const TXBn       txbn,      const can_frame *frame                         );

/**
 * @brief Send CAN frame using any available TX buffer
 * 
 * @param instance  MCP2515 structure pointer
 * @param frame     Sent CAN frame buffer pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_sendMessage       (MCP2515_instance *instance,                             const can_frame *frame                         );

/**
 * @brief Read the contents of a specific RX buffer
 * 
 * @param instance  MCP2515 structure pointer
 * @param rxbn      RX buffer index
 * @param frame     Received frame buffer pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_readMessage_Buff  (MCP2515_instance *instance, const RXBn       rxbn,            can_frame *frame                         );

/**
 * @brief Read the contents of any available RX buffer
 * 
 * @param instance  MCP2515 structure pointer
 * @param frame     Received frame buffer pointer
 * 
 * @return MCP2515_Error 
 */
MCP2515_Error MCP2515_readMessage       (MCP2515_instance *instance,                                   can_frame *frame                         );

void          MCP2515_getRxStatus       (MCP2515_instance *instance, uint8_t *status);
void          MCP2515_getStatus         (MCP2515_instance *instance, uint8_t *status);

#endif