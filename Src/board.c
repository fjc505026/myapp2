/*
 * board.c
 *
 *  Created on: 17Jan.,2020
 *      Author: jingchen
 */
#include <stdbool.h>
#include "board.h"
#include "spi.h"
#include "AMG8833.h"


static void SystemClock_Config(void);
static int Error_Handler( void );


extern volatile bool intReceived;

void InitBoard(void)
{
	HAL_Init();
	SystemClock_Config();
	ALL_GPIO_Init();
	SPI_initialization();
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);
  if(HAL_GPIO_ReadPin(Call_BTN.port, Call_BTN.pin) == GPIO_PIN_RESET)             //CALL button pushed
  {
	 HAL_GPIO_WritePin(Cancel_LED.port, Cancel_LED.pin, GPIO_PIN_SET);            // CANCEL led light up
	 HAL_GPIO_WritePin(Call_BTN.port, Call_BTN.pin, GPIO_PIN_SET);
  }

  if(HAL_GPIO_ReadPin(Cancel_BTN.port, Cancel_BTN.pin) == GPIO_PIN_RESET)         //CANCEL button pushed
  {
	 HAL_GPIO_WritePin(Cancel_LED.port, Cancel_LED.pin, GPIO_PIN_RESET);         // CANCEL led turn off
	 HAL_GPIO_WritePin(Cancel_BTN.port, Cancel_BTN.pin, GPIO_PIN_SET);
  }

  if(HAL_GPIO_ReadPin(AMG8833_INT.port, AMG8833_INT.pin) == GPIO_PIN_RESET)         //Detect the AMG8833_INT is low, ????
    {
	  intReceived = true;
    }

}


static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;                     //Division factor for PLL VCO input clock
  RCC_OscInitStruct.PLL.PLLN = 10;                    //Multiplication factor for PLL VCO output clock
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;         //SAI clock
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;         //SDMMC1, RNG and USB
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;         // main system clock
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks*/
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage*/
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}


static int Error_Handler( void )
{
       return 0;
}


