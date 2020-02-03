/*
 * i2c.c
 *
 *  Created on: 22Jan.,2020
 *      Author: jingchen
 */

#include "i2c.h"
/**I2C3 GPIO Configuration
 PC0     ------> I2C3_SCL
 PC1     ------> I2C3_SDA
 */

i2c_t I2C_3={

	.SCLK={
			.pin  =  GPIO_PIN_0,
			.port =  GPIOC,
			.Mode =  GPIO_MODE_AF_OD,
			.Pull =  GPIO_NOPULL,
			.Speed=  GPIO_SPEED_FREQ_HIGH,
			.Alternate = GPIO_AF4_I2C3
		  },

	.SDA={
		   .pin  =  GPIO_PIN_1,
		   .port =  GPIOC,
		   .Mode =  GPIO_MODE_AF_OD,
		   .Pull =  GPIO_NOPULL,
		   .Speed=  GPIO_SPEED_FREQ_HIGH,
		   .Alternate = GPIO_AF4_I2C3
		 }
};


I2C_HandleTypeDef hi2c3;

static void I2C3_GPIO_Init(i2c_t*);
static void I2C3_Init(void);
static void Error_Handler(void);

void I2C3_initaliztion(void)
{

   I2C3_GPIO_Init(&I2C_3);
   I2C3_Init();

}

static void I2C3_Init(void)
{

   hi2c3.Instance = I2C3;
   hi2c3.Init.Timing = 0x00702991;                        //PRESC[3:0], SCLDEL[3:0] and SDADEL[3:0]
   hi2c3.Init.OwnAddress1 = 0;
   hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
   hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
   hi2c3.Init.OwnAddress2 = 0;
   hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
   hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
   hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
   if (HAL_I2C_Init(&hi2c3) != HAL_OK)
   {
     Error_Handler();
   }

   if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
   {
     Error_Handler();
   }

   if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
   {
     Error_Handler();
   }

}


static void I2C3_GPIO_Init(i2c_t* I2Cx)
{
  /**I2C3 GPIO Configuration
	PC0     ------> I2C3_SCL
	PC1     ------> I2C3_SDA
	*/
  __HAL_RCC_GPIOC_CLK_ENABLE();
  Init_GPIO_PIN(&I2Cx->SCLK);
  Init_GPIO_PIN(&I2Cx->SDA);
  __HAL_RCC_I2C3_CLK_ENABLE();
}




static void Error_Handler(void)
{

	volatile int i = 0;
	for(;;) i+=1;
}


