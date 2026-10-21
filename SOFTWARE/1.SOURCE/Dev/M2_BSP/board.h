#ifndef M2_BSP_BOARD_H_
#define M2_BSP_BOARD_H_

#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;

#define BUTTON_1_PORT   GPIOB
#define BUTTON_1_PIN    GPIO_PIN_12

#define LED_RED_PORT    GPIOB
#define LED_RED_PIN     GPIO_PIN_13
#define LED_GREEN_PORT  GPIOB
#define LED_GREEN_PIN   GPIO_PIN_14

#define BUZZER_PORT     GPIOB
#define BUZZER_PIN      GPIO_PIN_8

#define RFID_CS_PORT    GPIOA
#define RFID_CS_PIN     GPIO_PIN_4
#define RFID_RST_PORT   GPIOB
#define RFID_RST_PIN    GPIO_PIN_0
#define RFID_IRQ_PORT   GPIOB
#define RFID_IRQ_PIN    GPIO_PIN_1

#define KEYPAD_ROW_PORT GPIOA
#define KEYPAD_R0_PIN   GPIO_PIN_0
#define KEYPAD_R1_PIN   GPIO_PIN_1
#define KEYPAD_R2_PIN   GPIO_PIN_2
#define KEYPAD_R3_PIN   GPIO_PIN_3

#define KEYPAD_COL_PORT GPIOA
#define KEYPAD_C0_PIN   GPIO_PIN_4
#define KEYPAD_C1_PIN   GPIO_PIN_5
#define KEYPAD_C2_PIN   GPIO_PIN_6
#define KEYPAD_C3_PIN   GPIO_PIN_7

#define EEPROM_WP_PORT  GPIOB
#define EEPROM_WP_PIN   GPIO_PIN_5

#endif /* M2_BSP_BOARD_H_ */
