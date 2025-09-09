#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


#define CS_PIN 5
#define SCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19
#define RST_PIN 21

static spi_device_handle_t spi;

// RC522 Register Map - langsung dari datasheet
#define REG_COMMAND     0x01    // Command register
#define REG_COM_IRQ     0x04    // Interrupt request bits
#define REG_ERROR       0x06    // Error bits showing the error status
#define REG_FIFO_DATA   0x09    // Input/output of 64 byte FIFO buffer
#define REG_FIFO_LEVEL  0x0A    // Number of bytes stored in FIFO buffer
#define REG_CONTROL     0x0C    // Miscellaneous control bits
#define REG_BIT_FRAME   0x0D    // Adjustments for bit-oriented frames
#define REG_TX_CONTROL  0x14    // Controls antenna driver pins TX1 and TX2
#define REG_TX_ASK      0x15    // Controls the modulation of antenna driver
#define REG_MODE        0x11    // Defines general modes for transmit/receive
#define REG_T_MODE      0x2A    // Timer settings
#define REG_T_PRESCALER 0x2B    // Timer prescaler
#define REG_T_RELOAD_L  0x2D    // Timer reload value low byte
#define REG_T_RELOAD_H  0x2C    // Timer reload value high byte
#define REG_VERSION     0x37    // Shows the software version

// RC522 Command Set - dari datasheet
#define CMD_IDLE        0x00    // No action, cancels current command execution
#define CMD_TRANSCEIVE  0x0C    // Transmits data from FIFO buffer and activates receiver
#define CMD_SOFT_RESET  0x0F    // Resets the RC522

// ISO14443 Type A Commands - untuk komunikasi dengan RFID card
#define CARD_REQUEST    0x26    // Request command (REQA)
#define CARD_WAKE       0x52    // Wake-up command (WUPA) 
#define CARD_SELECT     0x93    // Anti-collision/Select command

void write_reg(uint8_t reg, uint8_t data) {
    uint8_t tx[2]={(reg << 1) & 0x7E, data};
    spi_transaction_t t = {.length = 16, .tx_buffer = tx};
    spi_device_polling_transmit(spi, &t);
};

uint8_t read_reg(uint8_t reg) {
    uint8_t tx[2] = {((reg << 1) &0x7E) | 0x80, 0x00};
    uint8_t rx[2];
    spi_transaction_t t = {.length = 16, .tx_buffer = tx, .rx_buffer = rx};
    spi_device_polling_transmit(spi, &t);
    return rx[1];
}

void set_bits(uint8_t reg, uint8_t mask) {
    write_reg(reg, read_reg(reg) | mask);
}

void clear_bits(uint8_t reg, uint8_t mask) {
    write_reg(reg, read_reg(reg) & (~mask));
}

void reset_rc522(void) {
    gpio_set_level(RST_PIN, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(RST_PIN, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    while (read_reg(REG_COMMAND) & 0x10) {
        vTaskDelay(10 /portTICK_PERIOD_MS);
    }
}

void configure_rc522(void) {
    write_reg(REG_T_MODE, 0x8D);// timer autostart setelah TX
    write_reg(REG_T_PRESCALER, 0x3E);// timer prescaler value
    write_reg(REG_T_RELOAD_L, 30);//  timer reload low byte
    write_reg(REG_T_RELOAD_H, 0);//  timer reload high byte

    write_reg(REG_TX_ASK, 0x40);// 100% ASK modulation
    write_reg(REG_MODE, 0x3D);// CRC init value 0x6363

    // menghidupkan RF field / antenna
    // TX1 dan TX2 pin kontrol antena driver
    if (!(read_reg(REG_TX_CONTROL) & 0x03)) {
        set_bits(REG_TX_CONTROL, 0x03); //  ON TX1 dan TX2
    }
}
// card comunication protocol
uint8_t communicate_with_card(uint8_t command, uint8_t *send_data, uint8_t send_len, uint8_t *recv_data, uint16_t *recv_len) {
    uint8_t wait_irq = 0x30; // tunggu untuk RxIRq (0x20) dan IdleIRq (0x10)
    uint8_t irq_status;
    //1. hapus semua interupt request bits
    write_reg(REG_COM_IRQ, 0x7F);
    //2. flush FIFO buffer
    set_bits(REG_FIFO_LEVEL, 0x80);//flush buffer =1
    //3. set command ke idle state
    write_reg(REG_COMMAND, CMD_IDLE);
    //4. tulis data ke FIFO buffer
    for (int i = 0; i < send_len; i++) {
        write_reg(REG_FIFO_DATA, send_data[i]);
    }
    //5. jalankan command
    write_reg(REG_COMMAND, command);
    //6. mulai tranmisi
    if (command == CMD_TRANSCEIVE) {
        set_bits(REG_BIT_FRAME, 0x80);//startsend=1
    }
    //7. tunggu untuk selelsai command
    uint16_t timeout = 2000;
    do {
        irq_status = read_reg(REG_COM_IRQ);
        timeout--;
    }while (timeout && !(irq_status & 0x01) && !(irq_status & wait_irq));
    //8. transmisi berhento
    clear_bits(REG_BIT_FRAME, 0x80);//startsend=0
    //9. cek timeot
    if (timeout == 0) {
        return 1;//timeot err
    }
    //10. cek untuk komunikasi error 
    if (read_reg(REG_ERROR) & 0x1B) { // BufferOvfl, CollErr, CRCErr, ParityErr
        return 2;//komunikasi error
    }
    //11. baca data yang diterima dari FIFO
    if (command == CMD_TRANSCEIVE) {
        uint8_t fifo_level = read_reg(REG_FIFO_LEVEL);
        if (fifo_level > 64) fifo_level = 64;//limit buffer
        //baca data dari FIFO
        for (int i = 0; i < fifo_level; i++) {
            recv_data[i] = read_reg(REG_FIFO_DATA);
        }
        //hitung panjang bit yang diterima
        uint8_t control_bits = read_reg(REG_CONTROL) & 0x07;
        if (control_bits) {
            *recv_len = (fifo_level - 1) * 8 + control_bits;
        } else {
            *recv_len = fifo_level * 8;
        }
    }
    return 0;//sukses
}

// rfid card detection algoritma ISO14443 type a protocol
uint8_t request_card(uint8_t request_mode, uint8_t *card_type) {
    //set bit framing untuk short frame 7 bits for REQA/WUPA
    write_reg(REG_BIT_FRAME, 0x07);

    card_type[0] = request_mode;
    uint16_t recv_len;
    uint8_t status = communicate_with_card(CMD_TRANSCEIVE, card_type, 1, card_type, &recv_len);
    //respon yang diharapkan: 16 bits - ATQA(Answer to Request)
    if (status == 0 && recv_len == 16) {
        return 0; //card detected
    }
    return 1;//no card
}

uint8_t anticollision_get_uid(uint8_t *uid) {
    // set bit framing untuk normal 8-bit frame
    write_reg(REG_BIT_FRAME, 0x00);
    //Anti-collision command: SELECT + NVB (Number of Valid Bits = 0x20)
    uint8_t cmd_buffer[2] = {CARD_SELECT, 0x20};
    uint8_t response[10]; 
    uint16_t recv_len;

    uint8_t status = communicate_with_card(CMD_TRANSCEIVE, cmd_buffer, 2, response, &recv_len);

    if (status == 0 && recv_len >=40) {
        //verifikasi BCC(Block Chech Character) - xor checksum
        uint8_t bcc_check = 0;
        for (int i = 0; i < 4; i++) {
            bcc_check ^= response[i];
        }
        //jika ceksum valid copy UID
        if (bcc_check == response[4]) {
            memcpy(uid, response, 4);
            return 0; //sukses
        }
    }
    return 1;//gagal
}

uint8_t detect_card(uint8_t *uid) {
    uint8_t card_type[2];
    //1. coba wake up/rekuest kartu
    //REQA(0x26) - Request type A
    if (request_card(CARD_REQUEST, card_type) != 0) {
        //jika REQA gagal, coba WUPA (0x52) - Wake up type a
        if (request_card(CARD_WAKE, card_type) != 0) {
            return 1;//tidak terdeteksi
        }
    }
    //2. gunakan Anti-collision dan dapatkan UID
    if (anticollision_get_uid(uid) !=0) {
        return 2;//Anti-collision gagal
    }
    return 0;//sukses kartu terdeteksi dan baca UID
}

void init_hardware(void) {
    gpio_config_t rst_cfg = {
        .pin_bit_mask = (1ULL << RST_PIN),
        .mode = GPIO_MODE_OUTPUT 
    };
    gpio_config(&rst_cfg);

    spi_bus_config_t bus_cfg = {
        .miso_io_num = MISO_PIN,
        .mosi_io_num = MOSI_PIN,
        .sclk_io_num = SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 5000000,
        .mode = 0,
        .spics_io_num = CS_PIN,
        .queue_size = 7 
    };
    spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi);
}

void app_main(void) {
    printf("RC522 PAMBACA KARTU / TAG RFID\n");

    init_hardware();
    reset_rc522();
    configure_rc522();

    uint8_t ver = read_reg(REG_VERSION);
    printf("RC522 Version : 0x%02X\n", ver);

    if (ver == 0x00 || ver == 0xFF) {
        printf("ERROR: RC522 HENTEU NGARESPON ! CEK KABEL SIA. \n");
        return;
    }
    printf("RC522 SIYAP! Deukeutken kartu dinu antena...\n");

    uint8_t current_uid[4];
    uint8_t previous_uid[4] = {0, 0, 0, 0};
    bool card_present = false;

    while (1) {
        uint8_t result = detect_card(current_uid);

        if (result == 0) {
            bool is_new_card = !card_present;

            if (card_present) {
                for (int i = 0; i < 4; i++) {
                    if (current_uid[i] != previous_uid[i]) {
                        is_new_card = true;
                        break;
                    }
                }
            }
            if (is_new_card) {
                printf("KARTU KADETEKSI!\n");
                printf("UID: %02X:%02X:%02X:%02X\n", current_uid[0], current_uid[1], current_uid[2], current_uid[3]);
                printf("---------------------\n");
                memcpy(previous_uid, current_uid, 4);
                card_present = true;
            }
        } else {
            if (card_present) {
                printf("Kartu dilepasken\n");
                printf("---------------------\n");
                card_present = false;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}




