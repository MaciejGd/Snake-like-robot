/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <math.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//values to count servo positions
uint8_t PTS_NUM = 45;
uint8_t ZERO_SERVO = 150;
uint8_t MODULES_AMOUNT = 9;
uint8_t HOR_MAX_VAL = 180;
uint8_t VER_MAX_VAL = 166;
double HOR_PERIOD = 2 * M_PI;
double VER_PERIOD = 4 * M_PI;
double HOR_OFFSET = 0;
double VER_OFFSET = (-1.0/2.0) * M_PI;
//data buffers for communication
uint8_t RX_DATA[4];
uint8_t TX_DATA[4];
uint8_t BLU_BUFF;
uint8_t CALIBRATE_ID = 20;
//enumerators defining state and errors in robot
typedef enum {
	NO_ERROR = 0,
	CRC_ERROR = 1,
	TIMEOUT_ERROR = 2
} error_code;

typedef enum {
	STOPPED = 0,
	MOVING = 1,
	CALIBRATE = 2
} robot_state;

uint8_t ERROR_VAL = NO_ERROR;
uint8_t state = STOPPED;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void sendData(const uint8_t *data)
{
	HAL_GPIO_WritePin(TX_EN_GPIO_Port,TX_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1, data, 3, 1000);
	HAL_GPIO_WritePin(TX_EN_GPIO_Port,TX_EN_Pin, GPIO_PIN_RESET);
}

void sendBluetooth(const uint8_t *data)
{
	HAL_UART_Transmit(&huart2, data, 1, 1000);
}

void handle_bluetooth(const uint8_t *data)
{
	switch (*data)
	{
	case 2:
		sendBluetooth(data);
		state=MOVING;
		break;
	case 3:
		sendBluetooth(data);
		state=CALIBRATE;
		break;
	case 4:
		sendBluetooth(data);
		state=STOPPED;
		break;
	case 5:
		state=STOPPED;
	default:
		sendBluetooth(data);
		break;
	}
}

void check_crc(const uint8_t *rx_buff, const uint8_t *tx_buff)
{
	if (rx_buff[3] != tx_buff[3])
	{
		uint8_t err;
		ERROR_VAL = CRC_ERROR;
		err = ERROR_VAL * 10 + tx_buff[0];
		sendBluetooth(&err);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance==USART1)
	{
		check_crc(RX_DATA, TX_DATA);
		HAL_UARTEx_ReceiveToIdle_IT(huart, RX_DATA, 3);
	}
	else if (huart->Instance==USART2)
	{
		handle_bluetooth(&BLU_BUFF);
		HAL_UARTEx_ReceiveToIdle_IT(huart, &BLU_BUFF, 1);
	}
}



uint8_t crc8(uint8_t *buffer, uint8_t buff_len)
{
	uint8_t crc = 0xFF;
	while (buff_len--)
	{
		crc = crc ^ (*buffer++);
	}
	return crc;
}

//return 1 if there is no error in calc and 0 if there is

void preparePackets(uint8_t *buffer, const uint8_t slave_id, const uint8_t servo_hor, const uint8_t servo_ver)
{
	buffer[0] = slave_id;
	buffer[1] = servo_hor;
	buffer[2] = servo_ver;
	buffer[3] = crc8(buffer, 3);
}

uint8_t map(double sin_val, const uint8_t max_val)
{
	double multiplier = max_val - ZERO_SERVO;
	uint8_t mapped_val = sin_val * multiplier + ZERO_SERVO;
	return mapped_val;
}

void init_sin_values(uint8_t* sin_arr, const uint8_t max_val, const double period, const double offset)
{
	double alfa = period / (double)PTS_NUM;
	double curr_sin;
	for (int i = 0; i < PTS_NUM; i++)
	{
		curr_sin = sin(offset + i * alfa);
		*sin_arr++ = map(curr_sin, max_val);
	}
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
  HAL_Delay(100);
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, &BLU_BUFF, 1);
  uint8_t count_modules;
  uint8_t current_point = 0;
  uint8_t SinArrHor[PTS_NUM];
  uint8_t SinArrVer[PTS_NUM];
  init_sin_values(SinArrHor, HOR_MAX_VAL, HOR_PERIOD, HOR_OFFSET);
  init_sin_values(SinArrVer, VER_MAX_VAL, VER_PERIOD, VER_OFFSET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  for (count_modules = 1; count_modules < MODULES_AMOUNT && state==MOVING; count_modules++)
	  {
		  preparePackets(TX_DATA, count_modules, SinArrHor[current_point], SinArrVer[current_point]);
		  sendData(TX_DATA);
	  	  HAL_Delay(100);
	  }
	  if (state==MOVING)
	  {
		  current_point++;
		  current_point = current_point % PTS_NUM;
  	  }
	  else if (state==CALIBRATE)
	  {
		  preparePackets(TX_DATA, CALIBRATE_ID, 150, 150);
		  count_modules = 0;
		  current_point = 0;
		  state=STOPPED;
	  }
	  else if (state==STOPPED)
	  {
		  continue;
	  }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : TX_EN_Pin */
  GPIO_InitStruct.Pin = TX_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TX_EN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
