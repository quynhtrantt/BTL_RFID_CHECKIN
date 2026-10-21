#include "bsp_rfid.h"
#include "board.h"
#include "driver_gpio.h"
#include "driver_rfid.h"

extern volatile bool flag_RFID_Scanned;

static void bsp_rfid_cs_write(bool state) {
    Driver_GPIO_Write(RFID_CS_PORT, RFID_CS_PIN, state);
}

void BSP_RFID_Init(void) {
    Driver_GPIO_Write(RFID_RST_PORT, RFID_RST_PIN, false);
    HAL_Delay(10);
    Driver_GPIO_Write(RFID_RST_PORT, RFID_RST_PIN, true);
    HAL_Delay(50);
    Driver_RFID_AssignCS(bsp_rfid_cs_write);
    Driver_RFID_Init();
    Driver_RFID_ArmInterrupt();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    if (GPIO_Pin == RFID_IRQ_PIN) {
        flag_RFID_Scanned = true;
    }
}
