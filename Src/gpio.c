/*
 * gpio.c
 *
 *  Created on: 17Jan.,2020
 *      Author: jingchen
 */


#include "gpio.h"

// define GPIOs for nursing button
Gpio_t Cancel_BTN=
{
  .pin  =  GPIO_PIN_5,
  .port =  GPIOC,
  .Mode =  GPIO_MODE_IT_FALLING,
  .Pull =  GPIO_PULLUP
};

Gpio_t Call_BTN=
{
  .pin  =  GPIO_PIN_6,
  .port =  GPIOC,
  .Mode =  GPIO_MODE_IT_FALLING,
  .Pull =  GPIO_PULLUP
};

Gpio_t Cancel_LED=
{
  .pin  =  GPIO_PIN_8,
  .port =  GPIOC,
  .Mode =  GPIO_MODE_OUTPUT_PP,
  .Pull =  GPIO_NOPULL,
  .Speed=GPIO_SPEED_FREQ_LOW
};


// define GPIOs for LCD_control
Gpio_t LCD_RS=
{
  .pin  =  GPIO_PIN_8,
  .port =  GPIOA,
  .Mode =  GPIO_MODE_OUTPUT_PP,
  .Pull =  GPIO_NOPULL
};

Gpio_t LCD_WR=
{
  .pin  =  GPIO_PIN_9,
  .port =  GPIOA,
  .Mode =  GPIO_MODE_OUTPUT_PP,
  .Pull =  GPIO_NOPULL
};


Gpio_t LCD_LCK=
{
  .pin  =  GPIO_PIN_10,
  .port =  GPIOA,
  .Mode =  GPIO_MODE_OUTPUT_PP,
  .Pull =  GPIO_NOPULL
};

//the other SPI_LCD pins defined in spi.c

//define AMG8833_INT for EXTI
Gpio_t AMG8833_INT=
{
  .pin  =  GPIO_PIN_7,
  .port =  GPIOC,
  .Mode =  GPIO_MODE_IT_FALLING,
  .Pull =  GPIO_PULLUP
};


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void ALL_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins for nursing button */
  Init_GPIO_PIN(&Cancel_BTN);
  Init_GPIO_PIN(&Call_BTN);
  Init_GPIO_PIN(&Cancel_LED);

  /*Configure GPIO pins for nursing button */
  Init_GPIO_PIN(&LCD_RS);
  Init_GPIO_PIN(&LCD_WR);
  Init_GPIO_PIN(&LCD_LCK);

  /*Configure GPIO pins for EXTI from AMG8833 */
  Init_GPIO_PIN(&AMG8833_INT);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Cancel_LED.port, Cancel_LED.pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Cancel_BTN.port, Cancel_BTN.pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(Call_BTN.port,   Call_BTN.pin,   GPIO_PIN_SET);

  HAL_GPIO_WritePin(LCD_RS.port,  LCD_RS.pin,  GPIO_PIN_SET);                 //RS->1
  HAL_GPIO_WritePin(LCD_WR.port,  LCD_WR.pin,  GPIO_PIN_SET);                 //WR->1
  HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_SET);                 //LCK->1


  HAL_GPIO_WritePin(AMG8833_INT.port, AMG8833_INT.pin, GPIO_PIN_SET);
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}



void Init_GPIO_PIN(Gpio_t * Gpio_obj)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin       =  Gpio_obj->pin;
	GPIO_InitStruct.Mode      =  Gpio_obj->Mode;
	GPIO_InitStruct.Pull      =  Gpio_obj->Pull;
	GPIO_InitStruct.Speed     =  Gpio_obj->Speed;
	GPIO_InitStruct.Alternate =  Gpio_obj->Alternate;
	HAL_GPIO_Init(Gpio_obj->port, &GPIO_InitStruct);
}



