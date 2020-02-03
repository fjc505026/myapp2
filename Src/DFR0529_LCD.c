/*
 * DFR0529_LCD.c
 *
 *  Created on: 20Jan.,2020
 *      Author: jingchen
 */


#include "DFR0529_LCD.h"
#include "gpio.h"


static void writeDat(uint8_t dat);
static void writeRepeatPixel(uint16_t color, uint16_t count, uint16_t repeatCount);

void LCD_Init(void)
{

	  HAL_GPIO_WritePin(LCD_LCK.port,   LCD_LCK.pin,  GPIO_PIN_RESET);     //LCK->0
	  HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_SET);      //CS->1
	  HAL_GPIO_WritePin(LCD_RS.port,    LCD_RS.pin,    GPIO_PIN_SET);      //RS->1
	  HAL_GPIO_WritePin(LCD_WR.port,    LCD_WR.pin,    GPIO_PIN_SET);      //WR->1
	  HAL_GPIO_WritePin(LCD_LCK.port,   LCD_LCK.pin,   GPIO_PIN_SET);      //CLK->1

	  HAL_Delay(50);
	  writeCmd(0xd7);                 //AutoLoadSet
	  writeDat(0x9f);                 //D4->1: Disable EEPROM auto read,
	  writeCmd(0xE0);                 //control EEPROM
	  writeDat(0x00);                 //the Read enable of EEPROM will be opened
	  HAL_Delay(5);
	  writeCmd(0xFA);                 //EEPROM Function Selection
	  writeDat(0x01);                 //0 : EEPROM erase disable,0 : EEPROM write disable
	  HAL_Delay(5);
	  writeCmd(0xE3);                 //EEPRD: Read from EEPROM
	  HAL_Delay(5);
	  writeCmd(0xE1);                 //EEPROM control out
	  writeCmd(0x28);                 //Display Off
	  writeCmd(0x11);                 //Sleep Out
	  HAL_Delay(5);
	  writeCmd(0xc0);                 //Vop set
	  writeDat(0x17);                 //ctrL=0x1b 080416 5PCS 0X1E; 8PCS 0X2A
	  writeDat(0x01);
	  writeCmd(0x25);                 //Write Contrast WRCNTR       fine tuning the contrast of the display
	  writeDat(0x1E);
	  writeCmd(0xC3);                 //Bias Selection
	  writeDat(0x03);                 //1/9
	  writeCmd(0xC4);                 //Booster Setting
	  writeDat(0x07);                 //x8 boosting circuit
	  writeCmd(0xC5);
	  writeDat(0x01);
	  writeCmd(0xCB);                //Vg source control
	  writeDat(0x01);
	  writeCmd(0xB7);                //Scan Direction for glass layout
	  writeDat(0x00);                //Keep MX,Keep BGR
	  writeCmd(0xD0);                //Analog circuit setting
	  writeDat(0x1d);
	  writeCmd(0xB5);                //N-Line control
	  writeDat(0x89);
	  writeCmd(0xBD);                 //Display Compensation Step
	  writeDat(0x02);
	  writeCmd(0xF0);                 //Frame Freq. in Temperature range
	  writeDat(0x07);
	  writeDat(0x0C);
	  writeDat(0x0C);
	  writeDat(0x12);
	  writeCmd(0xF4);                 //Temperature Gradient Compensation Coefficient Set
	  writeDat(0x33);
	  writeDat(0x33);
	  writeDat(0x33);
	  writeDat(0x00);
	  writeDat(0x33);
	  writeDat(0x66);
	  writeDat(0x66);
	  writeDat(0x66);
	  writeCmd(0x20);                 //Display Inversion Off
	  writeCmd(0x2A);                 //Column Address Set
	  writeDat(0x00);
	  writeDat(0x7F);
	  writeCmd(0x2B);                 //Row Address Set
	  writeDat(0x00);
	  writeDat(0x7f);
	  writeCmd(0x3A);                 //Interface Pixel Format
	  writeDat(0x05);
	  writeCmd(0x36);                 //Memory Data Access Control
	  writeDat(0x80);                 //RGB->0, MX,MV,ML->0,MY->1
	  writeCmd(0xB0);                 //Display Duty setting
	  writeDat(0x7F);
	  writeCmd(0x29);                 //Display On
	  writeCmd(0xF9);                  //Frame PWM Set
	  writeDat(0x00);
	  writeDat(0x02);
	  writeDat(0x04);
	  writeDat(0x06);
	  writeDat(0x08);
	  writeDat(0x0a);
	  writeDat(0x0c);
	  writeDat(0x0e);
	  writeDat(0x10);
	  writeDat(0x12);
	  writeDat(0x14);
	  writeDat(0x16);
	  writeDat(0x18);
	  writeDat(0x1A);
	  writeDat(0x1C);
	  writeDat(0x1E);
	  writeCmd(0x29);                    //Display On
}



void fillScreen(uint16_t color)
{
  setCursorAddr(0, 0, 128, 128);
  writeCmd(0x2c); //writeToRam();
  writeRepeatPixel(color, 128, 128);
}



void writeCmd(uint8_t cmd)

{
	HAL_GPIO_WritePin(LCD_RS.port,    LCD_RS.pin,    GPIO_PIN_RESET);   //RS->0
	HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_RESET);   //CS->0
	SPI_transmit_8bit(&SPI_Handle, cmd );
	//HAL_SPI_Transmit(&SPI_Handle,&cmd,sizeof(cmd),10);  //1byte
	HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_SET);        //LCK->1
	HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_RESET);      //LCK->0
	HAL_GPIO_WritePin(LCD_WR.port, LCD_WR.pin, GPIO_PIN_RESET);        //WR->0
	HAL_GPIO_WritePin(LCD_WR.port, LCD_WR.pin, GPIO_PIN_SET);          //WR->1
	HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_SET);    //CS->1
}



void showText(char* text)
{
	setTextSize(1);
	setTextColor(0xF800);       //set text color to white
	setTextBackground(0X0000);  //set text background to black
	Display(128,128);
	setCursor(35, 100);
	print1(text);
}




/*transfer data */
static void writeDat(uint8_t dat)
{

	writeDatBytes(&dat,sizeof(dat));
	/*
    HAL_GPIO_WritePin(LCD_RS.port, LCD_RS.pin, GPIO_PIN_SET);           //RS->1
	HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_RESET);   //CS->0
	SPI_transmit_8bit(&SPI_Handle, dat);
	//HAL_SPI_Transmit(&SPI_Handle, &dat, sizeof(dat), 10);
	HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_SET);        //LCK->1
	HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_RESET);      //LCK->0
	HAL_GPIO_WritePin(LCD_WR.port, LCD_WR.pin, GPIO_PIN_RESET);        //WR->0
	HAL_GPIO_WritePin(LCD_WR.port, LCD_WR.pin, GPIO_PIN_SET);          //WR->1
	HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_SET);    //CS->1*/
}


void writeDatBytes(uint8_t* pDat, uint16_t count)
{
	HAL_GPIO_WritePin(LCD_RS.port, LCD_RS.pin, GPIO_PIN_SET);         //RS->1
	HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_RESET);   //CS->0
	SPI_Handle.Instance->CR1|=SPI_CR1_SPE;
	while((SPI_Handle.Instance->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
	{
		(void)SPI_Handle.Instance->DR;
	}
	while(count --)
	{
		while((SPI_Handle.Instance->SR & SPI_SR_TXE) != SPI_SR_TXE)
		{
		}
		*((__IO uint8_t *)&SPI_Handle.Instance->DR)=*pDat;
		while((SPI_Handle.Instance->SR & SPI_SR_RXNE) != SPI_SR_RXNE)
		{
		}
		(void)SPI_Handle.Instance->DR;
		while((SPI_Handle.Instance->SR & SPI_SR_BSY) == SPI_SR_BSY)
		{
		}
		//HAL_SPI_Transmit(&SPI_Handle, pDat, sizeof(*pDat), 10);
		HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_SET);        //LCK->1
		HAL_GPIO_WritePin(LCD_LCK.port, LCD_LCK.pin, GPIO_PIN_RESET);      //LCK->0
		HAL_GPIO_WritePin(LCD_WR.port,  LCD_WR.pin,  GPIO_PIN_RESET);      //WR->0
		HAL_GPIO_WritePin(LCD_WR.port,  LCD_WR.pin,  GPIO_PIN_SET);        //WR->1
		pDat ++;
	}
	HAL_GPIO_WritePin(SPI_1.Nss.port, SPI_1.Nss.pin, GPIO_PIN_SET);    //CS->1
}


void setCursorAddr(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	uint8_t addrBuf[2] = {(uint16_t)x0 , (uint16_t)x1};
	writeCmd(0x2a);                                                          //column address set
	writeDatBytes(addrBuf, 2);
	addrBuf[0] = (uint16_t)y0;
	addrBuf[1] = (uint16_t)y1;
	writeCmd(0x2b);                                                          //row address set
	writeDatBytes(addrBuf, 2);
}



static void writeRepeatPixel(uint16_t color, uint16_t count, uint16_t repeatCount)
{
	uint8_t    colorBuf[2] = {color , color>> 8};
	uint32_t   i = 0;
	for(i = 0; i < repeatCount * count; i ++)
	{
	writeDatBytes(colorBuf, 2);
	}
}











