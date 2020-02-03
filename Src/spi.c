/*
 * spi.c
 *
 *  Created on: 17Jan.,2020
 *      Author: jingchen
 */


#include "spi.h"


Spi_t SPI_1={

	.Mosi= {.pin  =  GPIO_PIN_7,
		    .port =  GPIOA,
		    .Mode =  GPIO_MODE_AF_PP,
		    .Pull =  GPIO_PULLUP,
		    .Speed=  GPIO_SPEED_FREQ_HIGH,
	        .Alternate = GPIO_AF5_SPI1 },

	.Sclk={.pin  =  GPIO_PIN_5,
		   .port =  GPIOA,
		   .Mode =  GPIO_MODE_AF_PP,
		   .Pull =  GPIO_PULLUP,
		   .Speed=  GPIO_SPEED_FREQ_HIGH,
		   .Alternate = GPIO_AF5_SPI1},

	.Nss={.pin  =  GPIO_PIN_4,
		  .port =  GPIOA,
		  .Mode =  GPIO_MODE_OUTPUT_PP,
		  .Pull =  GPIO_PULLUP,
		  .Speed=  GPIO_SPEED_FREQ_HIGH,
		  }
};


SPI_HandleTypeDef SPI_Handle;
static void Init_GPIO_SPI(Spi_t * SPIx);


static void Init_GPIO_SPI(Spi_t * SPIx)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	Init_GPIO_PIN(&SPIx->Mosi);
	Init_GPIO_PIN(&SPIx->Sclk);
	Init_GPIO_PIN(&SPIx->Nss);
	HAL_GPIO_WritePin(SPIx->Nss.port, SPIx->Nss.pin, GPIO_PIN_SET); //CS->1

}


void Init_SPI_1(void)
{

	SPI_Handle.Instance = SPI1;
	SPI_Handle.Init.Mode = SPI_MODE_MASTER;
	SPI_Handle.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI_Handle.Init.NSS = SPI_NSS_HARD_OUTPUT;
	SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;//2
	SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_Handle.Init.CRCPolynomial = 7;
	SPI_Handle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	SPI_Handle.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&SPI_Handle) != HAL_OK)
	{
	}

}


void SPI_initialization(void)
{
	__HAL_RCC_SPI1_CLK_ENABLE();
	Init_GPIO_SPI(&SPI_1);
	Init_SPI_1();
	__HAL_SPI_ENABLE(&SPI_Handle);
}


void SPI_transmit_8bit(SPI_HandleTypeDef* hspi, uint8_t dat )
{

	hspi->Instance->CR1|=SPI_CR1_SPE;
	while((hspi->Instance->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
	{
      (void)hspi->Instance->DR;
	}
	while((hspi->Instance->SR & SPI_SR_TXE) != SPI_SR_TXE)
	{
	}
	*((__IO uint8_t *)&(hspi->Instance->DR))= dat;
	while((hspi->Instance->SR & SPI_SR_RXNE) != SPI_SR_RXNE)
	{
	}
	(void)hspi->Instance->DR;
	while((hspi->Instance->SR & SPI_SR_BSY) == SPI_SR_BSY)
	{
	}
}





