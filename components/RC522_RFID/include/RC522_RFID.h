#pragma once

#include <stdint.h>
#include "driver/spi_master.h"

// RC522 Register Map - langsung dari datasheet
#define REG_COMMAND       0x01    // Command register
#define REG_COMI_EN       0x02    // Interrupt enable bits
#define REG_DIVI_EN       0x03    // Div. interrupt enable bits
#define REG_COM_IRQ       0x04    // Interrupt request bits
#define REG_DIV_IRQ       0x05    // Div. interrupt request bits
#define REG_ERROR         0x06    // Error bits showing the error status
#define REG_STATUS1       0x07    // Communication status
#define REG_STATUS2       0x08    // Receiver and transmitter status
#define REG_FIFO_DATA     0x09    // Input/output of 64 byte FIFO buffer
#define REG_FIFO_LEVEL    0x0A    // Number of bytes stored in FIFO buffer
#define REG_WATER_LEVEL   0x0B    // Water level for FIFO
#define REG_CONTROL       0x0C    // Miscellaneous control bits
#define REG_BIT_FRAME     0x0D    // Adjustments for bit-oriented frames
#define REG_COLL          0x0E    // Collisions detected

// Mode and TX/RX
#define REG_MODE          0x11    // Defines general modes for transmit/receive
#define REG_TX_MODE       0x12    // Defines transmit data rate and framing
#define REG_RX_MODE       0x13    // Defines receive data rate and framing
#define REG_TX_CONTROL    0x14    // Controls antenna driver pins TX1 and TX2
#define REG_TX_ASK        0x15    // Controls the modulation of antenna driver
#define REG_TX_SEL        0x16
#define REG_RX_SEL        0x17
#define REG_RX_THRESHOLD  0x18
#define REG_DEMOD         0x19
#define REG_MIFARE        0x1C
#define REG_SERIAL_SPEED  0x1D
#define REG_CRC_RESULT_H  0x21    // CRC result MSB
#define REG_CRC_RESULT_L  0x22    // CRC result LSB

// Timer
#define REG_T_MODE        0x2A    // Timer settings
#define REG_T_PRESCALER   0x2B    // Timer prescaler
#define REG_T_RELOAD_H    0x2C    // Timer reload high byte
#define REG_T_RELOAD_L    0x2D    // Timer reload low byte

// Misc
#define REG_VERSION       0x37    // Shows the software version
#define REG_TEST_SEL1     0x38
#define REG_TEST_SEL2     0x39
#define REG_TEST_PIN      0x3A
#define REG_TEST_ADC      0x3B
#define REG_TEST_DAC      0x3C
#define REG_TEST_ADC1     0x3D
#define REG_TEST_ADC2     0x3E

// RC522 Command Set - dari datasheet
#define CMD_IDLE        0x00    // No action, cancels current command execution
#define CMD_TRANSCEIVE	0x0C    // Transmits data from FIFO buffer and activates receiver
#define CMD_SOFT_RESET	0x0F    // Resets the RC522
#define CMD_MEM			0x01
#define CMD_GENERATE_RANDOM_ID 0x02
#define CMD_CALC_CRC   0x03   // <<< ini untuk CRC
#define CMD_TRANSMIT   0x04
#define CMD_NO_CMD_CHANGE 0x07
#define CMD_RECEIVE    0x08
#define CMD_MFAUTHENT  0x0E


// ISO14443 Type A Commands - untuk komunikasi dengan RFID card
#define CARD_REQUEST    0x26    // Request command (REQA)
#define CARD_WAKE       0x52    // Wake-up command (WUPA) 
#define CARD_SELECT     0x93

typedef struct {
    uint8_t cs_pin;
    uint8_t sck_pin;
    uint8_t mosi_pin;
    uint8_t miso_pin;
    uint8_t rst_pin;
    int clock_speed_hz;
} rc522_conf_t;

void rc522_init(rc522_conf_t *conf);
uint8_t detect_card(uint8_t *uid, uint8_t *uid_len);



