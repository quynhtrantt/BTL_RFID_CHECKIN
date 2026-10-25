#include "bsp_buzzer.h"
#include "board.h"
#include "driver_gpio.h"

void BSP_Buzzer_Set(bool state) {
    Driver_GPIO_Write(BUZZER_PORT, BUZZER_PIN, state);
}
