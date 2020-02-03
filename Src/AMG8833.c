/*
 * AMG8833.c
 *
 *  Created on: 22Jan.,2020
 *      Author: jingchen
 */



#include "AMG8833.h"
#include "i2c.h"
#include <stdio.h>

uint16_t DevAddress = 0x69<<1;
uint8_t  MemAddress = 0x00;

uint8_t  Txbuff[2] = {0x00};
uint16_t thermistor;

uint16_t quarter_deg = 0x004;  //0.25 Celsius degree



static float int12ToFloat(uint16_t val);
static void Error_Handler(void);
static void Read(uint8_t *buf);

void  AMG8833_Init(void)
{
	I2C3_initaliztion();
	MemAddress = AMG88xx_PCTL;    //PCTL
	Txbuff[0]=0x00;               //cmd to send
	if (HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress),Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY)) //AMG88xx_NORMAL_MODE;
	{
		Error_Handler();
	}
	MemAddress = AMG88xx_RST;    //RST
	Txbuff[0]=0x3F;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY); //AMG88xx_INITIAL_RESET;

	MemAddress = AMG88xx_INTC;    //INTC
	Txbuff[0]=AMG88xx_INT_DISABLED;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY); //disable INTC;

	MemAddress = AMG88xx_FPSC;    //FRR
	Txbuff[0]=AMG88xx_FPS_10;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress),Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY); //AMG88xx_FPS_10;
	HAL_Delay(100);

}



void AMG8833_Read_pixdels(float *buf)
{
	uint16_t recast;
	float converted;
	const uint8_t bytesToRead = 128;
	uint8_t rawArray[bytesToRead];
	Read(rawArray);

	while((rawArray[0]==0)&&(rawArray[1]==0)&&(rawArray[2]==0)&&(rawArray[3]==0))
	{
	  Read(rawArray);
	}

	//puts("\n rawArry data is \n ");
	for(int i=0; i<64; i++){
		uint8_t pos = i << 1;
		//printf("L(%d)C(%d)-%02X %02X\t",i/8,i%8,rawArray[pos + 1],rawArray[pos]);
		recast = ((uint16_t)rawArray[pos + 1] << 8) | ((uint16_t)rawArray[pos]);
		converted = int12ToFloat(recast) * 0.25;
		buf[(8*((63-i)/8))+(i%8)] = converted;
	}
	//puts("\n");
}


uint16_t AMG8833_Read_thermistor(void )
{
	 MemAddress = 0x0E;    //AMG88xx_TTHL
	 thermistor=0;
	 HAL_I2C_Mem_Read(&hi2c3, DevAddress, MemAddress,  sizeof(MemAddress) , (uint8_t*)&thermistor, sizeof(thermistor), HAL_MAX_DELAY);
	 return thermistor;
}



void Set_interrupt(float upper, float  lower)
{

	volatile int upperlimit = (upper * 4);//*quarter_deg;
	volatile int lowerlimit = (lower * 4);//*quarter_deg;
	volatile int hys_level  = ((upper*0.95)* 4);

	Txbuff[0]=upperlimit&0xFF;
	Txbuff[1]=((upperlimit&0x0F00) >> 8);

    MemAddress = AMG88xx_INTHL;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), &Txbuff[0], 1, HAL_MAX_DELAY);
	MemAddress = AMG88xx_INTHH;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), &Txbuff[1], 1, HAL_MAX_DELAY);

	Txbuff[0]=lowerlimit&0xFF;
	Txbuff[1]=((lowerlimit&0x0F00) >> 8);
	MemAddress = AMG88xx_INTLL;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), &Txbuff[0], 1, HAL_MAX_DELAY);
	MemAddress = AMG88xx_INTLH;
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), &Txbuff[1], 1, HAL_MAX_DELAY);

	Txbuff[0]=hys_level&0xFF;
	Txbuff[1]=((hys_level&0x0F00) >> 8);
	MemAddress = AMG88xx_IHYSL;

	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), &Txbuff[0], 1, HAL_MAX_DELAY);
	MemAddress = AMG88xx_IHYSH;
	//HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), &Txbuff[1], 1, HAL_MAX_DELAY);

}

void Enable_interrupt(void)
{
	MemAddress = AMG88xx_INTC;
	Txbuff[0]=0x03;//AMG88xx_ABSOLUTE_VALUE<<1;  // enable INT output and Absolute INT mode
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY);
	//MemAddress = AMG88xx_INTC;
	//Txbuff[0]==0x03;//AMG88xx_INT_ENABLED;  // enable INT output and Absolute INT mode
	//HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY);
}

void Clear_interrupt(void)
{

	MemAddress = AMG88xx_RST;
    Txbuff[0]  =  AMG88xx_FLAG_RESET ;
    //MemAddress =AMG88xx_SCLR;
	//Txbuff[0]=  0x02;
    HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY);
   // MemAddress = AMG88xx_INTC;
   // Txbuff[0] =(AMG88xx_INT_DISABLED)|(AMG88xx_ABSOLUTE_VALUE<<1);  // enable INT output and Absolute INT mode
   // HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY);
}


void Disable_interrupt(void)

{
	MemAddress = AMG88xx_INTC;
	Txbuff[0]=AMG88xx_INT_DISABLED;  // enable INT output and Absolute INT mode
	HAL_I2C_Mem_Write(&hi2c3,DevAddress,MemAddress, sizeof(MemAddress), Txbuff, sizeof(Txbuff[0]) , HAL_MAX_DELAY);
}




void getInterrupt(uint8_t *buf)
{
	uint8_t bytesToRead = 8;
	MemAddress=AMG88xx_INT_OFFSET;
	HAL_I2C_Mem_Read(&hi2c3, DevAddress, MemAddress,  sizeof(MemAddress) , buf,  bytesToRead , HAL_MAX_DELAY);//sizeof(pixdata)
}



/**************************************************************************/
/*!
    @brief  convert a 12-bit integer two's complement value to a floating point number
    @param  val the 12-bit integer  two's complement value to be converted
    @returns the converted floating point value
*/
/**************************************************************************/

static float int12ToFloat(uint16_t val)
{
	int16_t sVal = (val << 4); //shift to left so that sign bit of 12 bit integer number is placed on sign bit of 16 bit signed integer number
	return sVal >> 4;          //shift back the signed number, return converts to float
}



static void Read(uint8_t *buf)
{
	MemAddress = 0x80;    //AMG88xx_T01L
	HAL_I2C_Mem_Read(&hi2c3, DevAddress, MemAddress,  sizeof(MemAddress) , (uint8_t*)buf,  128 , HAL_MAX_DELAY);//sizeof(pixdata)
	//printf("The raw data in AMG8833 BUFFER are: \n ");
	//for(int i=0;i<128;i++)
	//{
	//	printf("%02X ", buf[i]);
	//}
}



static void Error_Handler(void)
{

	volatile int i = 0;
	for(;;) i+=1;
}








