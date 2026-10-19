#include "driver_rfid.h"
#include <stddef.h>

static void (*CS_Write)(bool) = NULL;
static uint8_t (*SPI_TxRx)(uint8_t) = NULL;

void Driver_RFID_Assign(void (*cs_fn)(bool), uint8_t (*spi_fn)(uint8_t)) {
    CS_Write = cs_fn; SPI_TxRx = spi_fn;
}

static void WriteReg(uint8_t reg, uint8_t val) {
    CS_Write(false); SPI_TxRx((reg << 1) & 0x7E); SPI_TxRx(val); CS_Write(true);
}
static uint8_t ReadReg(uint8_t reg) {
    CS_Write(false); SPI_TxRx(((reg << 1) & 0x7E) | 0x80);
    uint8_t val = SPI_TxRx(0); CS_Write(true);
    return val;
}

void Driver_RFID_Init(void) {
    WriteReg(0x01, 0x0F);
    uint8_t tx = ReadReg(0x14);
    if ((tx & 0x03) != 0x03) WriteReg(0x14, tx | 0x03);
}

void Driver_RFID_ArmInterrupt(void) {
    WriteReg(0x04, 0x7F);
    WriteReg(0x02, 0x20);
    WriteReg(0x09, 0x26);
    WriteReg(0x01, 0x0C);
}

void Driver_RFID_ClearInterrupt(void) {
    WriteReg(0x04, 0x7F);
}

bool Driver_RFID_ReadCard(RFID_UID_t *uid) {
    WriteReg(0x01, 0x00);
    WriteReg(0x04, 0x7F);
    WriteReg(0x0A, 0x80);
    WriteReg(0x09, 0x93); WriteReg(0x09, 0x20);
    WriteReg(0x01, 0x0C);

    uint8_t irq = ReadReg(0x04);
    if (irq & 0x20) {
        uid->size = ReadReg(0x0A);
        if(uid->size > 5) uid->size = 5;
        for (int i = 0; i < uid->size; i++) {
            uid->uid[i] = ReadReg(0x09);
        }
        return true;
    }
    return false;
}
