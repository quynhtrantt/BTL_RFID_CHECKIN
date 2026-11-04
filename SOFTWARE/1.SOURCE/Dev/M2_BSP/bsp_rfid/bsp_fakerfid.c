/*
 * bsp_fakerfid.c
 *
 *  Created on: Nov 4, 2026
 *      Author: Quynh
 */

#include "bsp_rfid.h"
#include "driver_rfid.h"
#include "app_lcd.h"
#include "board.h"
#include "stm32f1xx_hal.h"

#define BUTTON_PORT GPIOB
#define BUTTON_PIN  GPIO_PIN_13

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} id_fake;

static const id_fake dipswitch[8]={
		{GPIOB, GPIO_PIN_0},
		{GPIOB, GPIO_PIN_2},
		{GPIOB, GPIO_PIN_10},
		{GPIOB, GPIO_PIN_11},
		{GPIOA, GPIO_PIN_4},
		{GPIOA, GPIO_PIN_5},
		{GPIOA, GPIO_PIN_6},
		{GPIOA, GPIO_PIN_7},
	};
 void BSP_RFID_Init (void)
 {
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	     __HAL_RCC_GPIOB_CLK_ENABLE();

	     GPIO_InitTypeDef GPIO_InitStruct = {0};
	     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	     GPIO_InitStruct.Pull = GPIO_PULLUP;

	     for (uint8_t i = 0; i < 8; i++)
	     {
	         GPIO_InitStruct.Pin = dipswitch[i].pin;
	         HAL_GPIO_Init(dipswitch[i].port, &GPIO_InitStruct);
	     }

	     GPIO_InitStruct.Pin = BUTTON_PIN;
	     HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
 }
 bool BSP_RFID_ReadCard(RFID_UID_t *out_card)
 {
	 uint8_t raw_dip = 0;
	 for (uint8_t i = 0; i < 8; i++)
	     {
	         if (HAL_GPIO_ReadPin(dipswitch[i].port, dipswitch[i].pin) == GPIO_PIN_RESET)
	         {
	             raw_dip |= (1 << i);
	         }
	     }

	     out_card->size = 5;

	     out_card->uid[0] = 0xAA;
	     out_card->uid[1] = 0xBB;
	     out_card->uid[2] = 0xCC;
	     out_card->uid[3] = 0xDD;
	     out_card->uid[4] = raw_dip;
	     return (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_RESET);
 }
 void BSP_RFID_ClearInterrupt(void) {}
 void BSP_RFID_ArmInterrupt(void) {}
