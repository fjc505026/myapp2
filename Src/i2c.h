/*
 * i2c.h
 *
 *  Created on: 22Jan.,2020
 *      Author: jingchen
 */

#ifndef I2C_H_
#define I2C_H_
#include <string.h>
#include "gpio.h"


typedef struct
{

   Gpio_t SCLK;
   Gpio_t SDA;

}i2c_t;


extern I2C_HandleTypeDef hi2c3;
extern i2c_t I2C_3;

void I2C3_initaliztion(void);


#endif /* I2C_H_ */
