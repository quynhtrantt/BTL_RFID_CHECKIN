#include "bsp_rfid.h"
#include "stm32f1xx_hal.h"

#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_rfid.h"

#define RFID_CS_PORT  GPIOA
#define RFID_CS_PIN   GPIO_PIN_4
#define RFID_RST_PORT GPIOB
#define RFID_RST_PIN  GPIO_PIN_0
#define RFID_IRQ_PIN  GPIO_PIN_1

extern SPI_HandleTypeDef hspi1;
extern volatile bool flag_RFID_Scanned;

static void BSP_RFID_CS_Write(bool state) {
    Driver_GPIO_WritePin(RFID_CS_PORT, RFID_CS_PIN, state);
}
static uint8_t BSP_RFID_SPI_Transfer(uint8_t data) {
    return Driver_SPI_TxRx(&hspi1, data);
}

void BSP_RFID_Setup(void) {
    Driver_GPIO_WritePin(RFID_RST_PORT, RFID_RST_PIN, false);
    Driver_Delay(10);
    Driver_GPIO_WritePin(RFID_RST_PORT, RFID_RST_PIN, true);
    Driver_Delay(50);

    Driver_RFID_Assign(BSP_RFID_CS_Write, BSP_RFID_SPI_Transfer);
    Driver_RFID_Init();

    Driver_RFID_ArmInterrupt();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == RFID_IRQ_PIN) {
        flag_RFID_Scanned = true;
    }
}
