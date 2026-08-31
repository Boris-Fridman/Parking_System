/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CommonData.h"
#include "ParkingsData.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WORKING_I2C hi2c1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile bool GenerationEmabled = true;
volatile uint32_t LastI2CAccessTick = 0;
volatile bool I2CAccessing = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void DoCordsRegeneration();
void DoI2CWatchDog();


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*----------------------------------------------------------------------------------------------------------------------*/
/*  Is used for sending characters to the required UART port (huart3) while calling the standard output functions       */
/*  as "printf()", "putch()" or "puts()".                                                                               */
int _write(int file, char *ptr, int len)
 {
  HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len - 1, HAL_MAX_DELAY);
  if (ptr[len - 1] == '\n')
   {
    HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
   }
  else
   {
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr + len - 1, 1, HAL_MAX_DELAY);
   }
return len;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  srand(HAL_GetTick());
  printf("Starting GPS Manager...\n\r");
  HAL_I2C_EnableListen_IT(&WORKING_I2C);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
   DoCordsRegeneration();
   DoI2CWatchDog();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   HAL_Delay(1);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//void HAL_I2C_RxCpltCallback(I2C_HandleTypeDef *hi2c)
// {
//  if( hi2c == &WORKING_I2C )
//   {
//    static int ParkPlace;
//    static ParkingData_s Parking;
//    GetRandParking(&ParkPlace, &Parking);
//    HAL_I2C_DisableListen_IT(hi2c);
//    HAL_I2C_Slave_Transmit_IT(&WORKING_I2C, (uint8_t *)&Parking, sizeof(Parking));
//    HAL_I2C_EnableListen_IT(hi2c);
//   }
// }



void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
 {
  uint8_t TransferDirection;
  HAL_I2C_EnableListen_IT(hi2c);
  TransferDirection = I2C_GET_DIR(hi2c);
  if(TransferDirection == I2C_DIRECTION_RECEIVE)
   {
    /* After transmitting data. */
    HAL_I2C_SlaveTxCpltCallback(hi2c);
   }
  else
   {
    /* After receiving data. */
    HAL_I2C_SlaveRxCpltCallback(hi2c);
   }
 }

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
 {

  if( hi2c == &WORKING_I2C )
   {
    LastI2CAccessTick = HAL_GetTick();
    I2CAccessing = true;
    if(TransferDirection == I2C_DIRECTION_RECEIVE)  /* BeagleBone ▬▬▬▶ STM32 */
     {
      static int ParkPlace;
      static ParkingData_s Parking;
      if(GenerationEmabled)
       {
        GenerationEmabled = false;
        GetRandParking(&ParkPlace, &Parking);
       }
      //HAL_I2C_DisableListen_IT(hi2c);
      __disable_irq();
      HAL_I2C_Slave_Seq_Transmit_IT(&WORKING_I2C, (uint8_t *)&Parking, sizeof(Parking), I2C_NEXT_FRAME);
      __enable_irq();
      //HAL_I2C_EnableListen_IT(hi2c);

      //HAL_I2C_Slave_Seq_Transmit_IT(hi2c, buf, BUF_SIZE, I2C_NEXT_FRAME);
     }
    else  /* Transmit  */  /* BeagleBone ◀▬▬▬ STM32 */
     {
      uint8_t buf[100];
      __disable_irq();
      HAL_I2C_Slave_Seq_Receive_IT(hi2c, buf, sizeof(buf), I2C_NEXT_FRAME);
      __enable_irq();
      UNUSED(buf);

      //HAL_I2C_Slave_Seq_Receive_IT(hi2c, buf, BUF_SIZE, I2C_NEXT_FRAME);
     }

   }
  //I2CAccessing = false;
 }

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
 {
  GenerationEmabled = true;
  LastI2CAccessTick = HAL_GetTick();
  I2CAccessing = false;
 }

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
 {
  LastI2CAccessTick = HAL_GetTick();
  I2CAccessing = false;
 }

void DoCordsRegeneration()
 {
  int ParkPlace;
  ParkingData_s Parking;

  static uint32_t LastTick = 0;
  uint32_t CurrentTick = HAL_GetTick();
  if(CurrentTick - LastTick > 1000)
   {
    if(GenerationEmabled)
     {
      char buf[200];
      GenRandParkNumber();
      GetRandParking(&ParkPlace, &Parking);
      CreateCordsFormatted(buf, sizeof(buf), Parking.ParkingCords, true);
      printf("%5d: %s\n\r",ParkPlace, buf);
 //     printf("%5d: ",ParkPlace);
 //     PrintGPSCords(Parking.ParkingCords);
 //     printf("\n\r");
     }
    LastTick = CurrentTick;
   }
 }

void DoI2CWatchDog()
 {
  static uint32_t LastTick = 0;
  uint32_t CurrentTick = HAL_GetTick();
  if(CurrentTick - LastTick > 1)
   {
    GPIO_PinState SCL_PinsState = GPIO_PIN_SET;
    //GPIO_PinState SDL_PinsState = GPIO_PIN_RESET;
    SCL_PinsState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
    //SDL_PinsState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);

    if(SCL_PinsState == GPIO_PIN_SET)
     LastI2CAccessTick = CurrentTick;

    if(!I2CAccessing)
     {
      if(CurrentTick - LastI2CAccessTick > 50)
       {
        if((uint32_t)WORKING_I2C.Instance == (uint32_t)hi2c1.Instance)
         {
          if((SCL_PinsState == GPIO_PIN_RESET))  /* Restarting I2C Bus. */
           {
            HAL_I2C_DeInit(&WORKING_I2C);
            HAL_Delay(2);
            MX_I2C1_Init();
            HAL_I2C_EnableListen_IT(&WORKING_I2C);
           }
         }
        LastI2CAccessTick = CurrentTick;
       }
     }
    else
     {
    	LastI2CAccessTick = CurrentTick;
     }
    LastTick = CurrentTick;
   }

 }


/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
