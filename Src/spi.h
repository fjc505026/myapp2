/*
 * spi.h
 *
 *  Created on: 17Jan.,2020
 *      Author: jingchen
 */

#ifndef SPI_H_
#define SPI_H_

#include "gpio.h"
#include "stm32l4xx_hal.h"
typedef struct
{
     Gpio_t Mosi;
     Gpio_t Miso;
     Gpio_t Sclk;
     Gpio_t Nss;
}Spi_t;

extern  Spi_t SPI_1;
extern  SPI_HandleTypeDef SPI_Handle;
void SPI_initialization(void);
void SPI_transmit_8bit(SPI_HandleTypeDef* hspi, uint8_t dat );
#endif /* SPI_H_ */
