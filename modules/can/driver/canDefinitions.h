#ifndef SPI_CAN_DEFS_H
#define SPI_CAN_DEFS_H

#include <stdint.h>

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0b10000000000000000000000000000000UL /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0b01000000000000000000000000000000UL /* remote transmission request */
#define CAN_ERR_FLAG 0b00100000000000000000000000000000UL /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0b00000000000000000000011111111111UL /* standard frame format (SFF) mask*/
#define CAN_EFF_MASK 0b00011111111111111111111111111111UL /* extended frame format (EFF) mask*/
#define CAN_ERR_MASK 0b00011111111111111111111111111111UL /* omit EFF, RTR, ERR flags */

/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28 : CAN identifier (11/29 bit)
 * bit 29   : error message frame flag (0 = data frame, 1 = error message)
 * bit 30   : remote transmission request flag (1 = rtr frame)
 * bit 31   : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef uint32_t canid_t;

#define CAN_SFF_ID_BITS 0b00000000000000000000000000001011UL
#define CAN_EFF_ID_BITS 0b00000000000000000000000000011101UL

/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC     8U

#define CANCTRL_REQOP   0xE0
#define CANCTRL_ABAT    0x10
#define CANCTRL_OSM     0x08
#define CANCTRL_CLKEN   0x04
#define CANCTRL_CLKPRE  0x03
    
#define CANSTAT_OPMOD   0xE0
#define CANSTAT_ICOD    0x0E
    
#define CNF3_SOF        0x80

#define TXB_EXIDE_MASK  0b00000000000000000000000000001000UL
#define DLC_MASK        0b00000000000000000000000000001111UL
#define RTR_MASK        0b00000000000000000000000001001111UL

/* @brief SPI functioning frequency */
typedef enum  
{
    MCP_20MHZ,
    MCP_16MHZ,
    MCP_8MHZ
} CAN_CLOCK;

/* @brief Communication Bit Transfer rate */
typedef enum  
{
    CAN_5KBPS,
    CAN_10KBPS,
    CAN_20KBPS,
    CAN_31K25BPS,
    CAN_33KBPS,
    CAN_40KBPS,
    CAN_50KBPS,
    CAN_80KBPS,
    CAN_83K3BPS,
    CAN_95KBPS,
    CAN_100KBPS,
    CAN_125KBPS,
    CAN_200KBPS,
    CAN_250KBPS,
    CAN_500KBPS,
    CAN_1000KBPS
} CAN_SPEED;

/* Clock Output divisor */
typedef enum
{
    CLKOUT_DISABLE              = ( -1),
    CLKOUT_DIV1                 = (0x0),
    CLKOUT_DIV2                 = (0x1),
    CLKOUT_DIV4                 = (0x2),
    CLKOUT_DIV8                 = (0x3),
} CAN_CLKOUT;

/* @brief Controller Event Interrupt Flags */
typedef enum  
{
    CANINTF_RX0IF               = (0x01),   /* Receive Buffer 0 Full Interrupt Flag bit */
    CANINTF_RX1IF               = (0x02),   /* Receive Buffer 1 Full Interrupt Flag bit */
    CANINTF_TX0IF               = (0x04),   /* Transmit Buffer 0 Empty Interrupt Flag bit */
    CANINTF_TX1IF               = (0x08),   /* Transmit Buffer 1 Empty Interrupt Flag bit */
    CANINTF_TX2IF               = (0x10),   /* Transmit Buffer 2 Empty Interrupt Flag bit */
    CANINTF_ERRIF               = (0x20),   /* Error Interrupt Flag bit (multiple sources in EFLG register) */
    CANINTF_WAKIF               = (0x40),   /* Wake-up Interrupt Flag bit */
    CANINTF_MERRF               = (0x80)    /* Message Error Interrupt Flag bit */
} CANINTF;

/* @brief Error flag bits */
typedef enum  
{
    EFLG_RX1OVR                 = (1<<7),   /* Receive Buffer 1 Overflow Flag bit  */
    EFLG_RX0OVR                 = (1<<6),   /* Receive Buffer 0 Overflow Flag bit */
    EFLG_TXBO                   = (1<<5),   /* Bus-Off Error Flag bit */
    EFLG_TXEP                   = (1<<4),   /* Transmit Error-Passive Flag bit */
    EFLG_RXEP                   = (1<<3),   /* Receive Error-Passive Flag bit */
    EFLG_TXWAR                  = (1<<2),   /* Transmit Error Warning Flag bit */
    EFLG_RXWAR                  = (1<<1),   /* Receive Error Warning Flag bit */
    EFLG_EWARN                  = (1<<0)    /* Error Warning Flag bit */
} EFLG;

/* @brief Operational Mode Request Bits */
typedef enum 
{
    CANCTRL_REQOP_NORMAL        = (0x00),  /* Normal Operation Mode Request Bit */
    CANCTRL_REQOP_SLEEP         = (0x20),  /* Sleep Mode Request  Bit */
    CANCTRL_REQOP_LOOPBACK      = (0x40),  /* Loopback Operation Mode Request Bit */
    CANCTRL_REQOP_LISTENONLY    = (0x60),  /* Listen-Only Operation Mode Request Bit */
    CANCTRL_REQOP_CONFIG        = (0x80),  /* Configuration Mode Request Bit */
    CANCTRL_REQOP_POWERUP       = (0xE0)   /* Power up Initialization Mode Request Bit */
} CANCTRL_REQOP_MODE;

/* @brief generic CAN frame structure */
typedef struct {
    canid_t can_id;             /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t can_dlc;            /* frame payload length in byte (0 .. CAN_MAX_DLC) */
    uint8_t data[CAN_MAX_DLC];  /* Frame data bytes */
} can_frame;


/* @brief generic CAN frame structure */
typedef struct {
    canid_t     can_id;         /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t     can_dlc;        /* frame payload length in byte (0 .. CAN_MAX_DLC) */
    uint64_t    data;           /* Frame data bytes */
} can_frame64;

#endif /* CAN_H_ */