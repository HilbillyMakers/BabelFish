#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "pico/time.h"
#include "pico/multicore.h"

#include "modules/usb/usb_interface.h"

#define LED_DELAY_MS 1000u

extern uint8_t com_spi_init        (uint8_t* const configuration,  uint8_t* const responseBuffer, uint8_t* const errorBuffer);
extern uint8_t com_spi_deinit      (                               uint8_t* const responseBuffer, uint8_t* const errorBuffer);
extern uint8_t com_spi_transfer    (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer);

//CAN RESET using SPI
#define PROTOTYPE_SPI       spi0
#define SPI_SCK_PIN         18
#define SPI_TX_PIN          19
#define SPI_RX_PIN          20
#define SPI_CSN_PIN         21

#define SPI_BAUDRATE        1000*1002

#define SLAVE_SELECTED      0
#define SLAVE_UNSELECTED    1

#define RESET                0xC0u
uint8_t const read_status   = 0xA0u;
uint8_t const read          = 0x03u;

//can controller register maps
uint8_t const canstat        = 0x0Eu;
uint8_t const read_canstat[] = {read, canstat};

//can request to send for each TX buffer
uint8_t const RTSTXB0       = 0x81u;
uint8_t const RTSTXB1       = 0x82u;
uint8_t const RTSTXB2       = 0x84u;
//can TXBnSIDH memory map
uint8_t const TXB0SIDH      = 0x31u;
uint8_t const TXB1SIDH      = 0x41u;
uint8_t const TXB2SIDH      = 0x51u;

uint8_t const dummy_frame   = 0u;

// command request to write, start address, gibberish identifier, bytes to write (dlc), dummy frame data
uint8_t write_b0[]      = {RTSTXB0, TXB0SIDH, 0x1, 0x10, 0x10, 1, dummy_frame};
uint8_t write_b1[]      = {RTSTXB1, TXB1SIDH, 0x1, 0x10, 0x10, 1, dummy_frame};
uint8_t write_b2[]      = {RTSTXB2, TXB2SIDH, 0x1, 0x10, 0x10, 1, dummy_frame};

uint8_t read_CAN_status             () {

    uint8_t status_data;
    // Request to read CAN status
    gpio_put            (SPI_CSN_PIN,   SLAVE_SELECTED);
    spi_write_blocking  (PROTOTYPE_SPI, &read_status,       1);

    // Read CAN status
    spi_read_blocking   (PROTOTYPE_SPI, 0, &status_data,    1);
    gpio_put            (SPI_CSN_PIN,   SLAVE_UNSELECTED);

    return status_data;
}

void read_CAN_status_with_handlers  (uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t status_message[] = {1, 1, read_status};

    com_spi_transfer(status_message, responseBuffer, errorBuffer);
}


void write_message                  (uint8_t *buffer,       uint8_t buffer_size) {

    gpio_put            (SPI_CSN_PIN,   SLAVE_SELECTED);
    spi_write_blocking  (PROTOTYPE_SPI, buffer, buffer_size);
    gpio_put            (SPI_CSN_PIN,   SLAVE_UNSELECTED);
}


void write_CAN_frame                (uint8_t *write_buffer, uint8_t buffer_size) {

    write_message (write_buffer, buffer_size);
}

void write_CAN_frame_with_handlers  (uint8_t *write_buffer, uint8_t buffer_size, \
                                     uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t write_message[buffer_size + 2];

    write_message[0] =  buffer_size;
    write_message[1] =  0;
    memcpy( &write_message[2], write_buffer, buffer_size);

    com_spi_transfer (write_message, responseBuffer, errorBuffer);
}

void reset_CAN                      (){

    uint8_t reset_message[] = {(uint8_t)RESET};

    write_message(reset_message, 1);
    /*  Documentation:
        The values must be such that the device is held in
        Reset for a minimum of 2 µs after VDD reaches
        operating voltage
    */
    sleep_us(10);
}

void reset_CAN_with_handlers        (uint8_t* const responseBuffer, uint8_t* const errorBuffer) {

    uint8_t reset_message[] = {1, 0, (uint8_t)RESET};

    com_spi_transfer(reset_message, responseBuffer, errorBuffer);

    sleep_us(10);
}

uint8_t read_canstat_register       () {

    uint8_t canstat_opmode  = 255u;
    // Request to read CANSTAT
    gpio_put            (SPI_CSN_PIN,   SLAVE_SELECTED);
    spi_write_blocking  (PROTOTYPE_SPI, read_canstat,       2);

    // Read CANSTAT
    spi_read_blocking   (PROTOTYPE_SPI, 0, &canstat_opmode, 1);
    gpio_put            (SPI_CSN_PIN,   SLAVE_UNSELECTED);

    return canstat_opmode;
}

void can_usage() {

    uint8_t status_data     = 255u;

    printf("SPI master example\n");
    //  Perform CAN functions using classic SPI blocking

    // Enable SPI 0 at 1 MHz and connect to GPIOs
    spi_init(PROTOTYPE_SPI, SPI_BAUDRATE);
    gpio_set_function(SPI_SCK_PIN,  GPIO_FUNC_SPI);
    gpio_set_function(SPI_RX_PIN,   GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX_PIN,   GPIO_FUNC_SPI);

    gpio_init           (SPI_CSN_PIN);
    gpio_set_dir        (SPI_CSN_PIN,   GPIO_OUT);
    gpio_put            (SPI_CSN_PIN,   1);

    // Make the SPI pins available to picotool
    //bi_decl(bi_4pins_with_func(SPI_RX_PIN, SPI_TX_PIN, SPI_CSN_PIN, SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Reset CAN
    reset_CAN();

    // Read CAN status
    status_data = read_CAN_status();

    //print to UART the CAN status received in spi_read
    printf("SPI master says: read status %d from the CAN module\n", status_data);


    printf("SPI master says: read canstat.opmode %d from the CAN module\n", read_canstat_register());

    printf("\n#################\nBegin writing\n");
    //WRITE 1
    write_CAN_frame( &(write_b0[0]), 7);

    status_data = read_CAN_status();
    printf("SPI master says: for first write, read status %d from the CAN module\n", status_data);
    // Test read CAN status to ensure same data is read
    status_data = read_CAN_status();
    printf("SPI master says (again): for first write, read status %d from the CAN module\n", status_data);

    //WRITE 2
    write_CAN_frame( &(write_b1[0]), 7);

    status_data = read_CAN_status();
    printf("SPI master says: for second write, read status %d from the CAN module\n", status_data);

    //WRITE 3
    write_CAN_frame( &(write_b2[0]), 7);

    status_data = read_CAN_status();
    printf("SPI master says: for third write, read status %d from the CAN module\n", status_data);

    spi_deinit(PROTOTYPE_SPI);

    ///TODO:
    /*
        Recreate above functionalities using spiHandler functions
    */

    uint8_t configuration[]  = {    SPI_BAUDRATE        & 0xFF, \
                                   (SPI_BAUDRATE >>  8) & 0xFF, \
                                   (SPI_BAUDRATE >> 16) & 0xFF, \
                                   (SPI_BAUDRATE >> 24) & 0xFF
                                };
    uint8_t *responseBuffer = NULL;
    uint8_t *errorBuffer    = NULL;


    printf("\n###############\nUsing com_spi functions\n");
    com_spi_init    (configuration, responseBuffer, errorBuffer);

    //reset CAN
    reset_CAN_with_handlers(responseBuffer, errorBuffer);

    //write CAN buffer 0
    write_CAN_frame_with_handlers( write_b0, 7, responseBuffer, errorBuffer);

    read_CAN_status_with_handlers( &status_data, errorBuffer);
    printf("SPI master says: for first write, read status %d from the CAN module\n", status_data);

    //write CAN buffer 1
    write_CAN_frame_with_handlers( write_b1, 7, responseBuffer, errorBuffer);

    read_CAN_status_with_handlers( &status_data, errorBuffer);
    printf("SPI master says: for second write, read status %d from the CAN module\n", status_data);

    //write CAN buffer 2
    write_CAN_frame_with_handlers( write_b2, 7, responseBuffer, errorBuffer);

    read_CAN_status_with_handlers( &status_data, errorBuffer);
    printf("SPI master says: for third write, read status %d from the CAN module\n", status_data);

    com_spi_deinit  (               responseBuffer, errorBuffer);
}

void core_usb_loop()
{
    initUSB     ();
    runUSBLoop  ();
}

int main()

{
    timer_hw->dbgpause = 0;

    /* Launch the usb loop on core1 */
    multicore_reset_core1();
    multicore_launch_core1(core_usb_loop);
    can_usage   ();

    /* Leave core 0 free for the CAN SM */
    while(1)
    {
        tight_loop_contents();
    }

    return 0;
}