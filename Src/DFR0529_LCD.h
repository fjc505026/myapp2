/*
 * DFR0529_LCD.h
 *
 *  Created on: 20Jan.,2020
 *      Author: jingchen
 */

#ifndef DFR0529_LCD_H_
#define DFR0529_LCD_H_


#include "spi.h"
#include "LCD_display.h"

void LCD_Init(void);
void fillScreen(uint16_t color);
void writeDatBytes(uint8_t* pDat, uint16_t count);
void setCursorAddr(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void writeCmd(uint8_t cmd);
void showText(char* text);
#endif /* DFR0529_LCD_H_ */




