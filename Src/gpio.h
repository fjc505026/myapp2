/*
 * gpio.h
 *
 *  Created on: 17Jan.,2020
 *      Author: jingchen
 */

#ifndef GPIO_H_
#define GPIO_H_


#include <stdint.h>
#include "stm32l4xx_hal.h"

typedef void( GpioIrqHandler )( void* context );

typedef struct
{
	uint16_t        pin;
	GPIO_TypeDef*   port;
    uint32_t	    Mode;
    uint32_t	    Pull;
    uint32_t        Speed;
    uint32_t        Alternate;
    GpioIrqHandler *IrqHandler;
} Gpio_t;

extern Gpio_t Cancel_BTN, Call_BTN, Cancel_LED, LCD_RS, LCD_WR, LCD_LCK, AMG8833_INT;

void ALL_GPIO_Init(void);
void Init_GPIO_PIN(Gpio_t * Gpio_obj);

#endif
