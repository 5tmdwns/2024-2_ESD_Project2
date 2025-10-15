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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "i2c_lcd.h"
#include "stuffManage.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

enum cuisine {
	SmallBites = 31,
	AlmondWithCavior, //서브1
	SnowCrabAndPickledChrysanthemum, //서브1 대체1
	AblaloneTaco, //서브1 대체2
	HearthOvenGrilledHanwoo, //메인
	EmberToastedAcornNoodle, //메인 대체1
	BlackSesameSeaurchinNasturtium, //메인 대체2
	BurdockTarteTatinWithSkinJuice, //서브2
	TilefishMustardBrassica, //서브2 대체1
	fattyTuna, //서브2 대체2
	SmallSweets
} cuisine;

typedef struct {
	uint8_t courseData [5][2];
	uint8_t changeCourse [3];
} user_data;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
TaskHandle_t copyHandler;
TaskHandle_t cookHandler;
TaskHandle_t lcdHandler;
TaskHandle_t buzzerHandler;


SemaphoreHandle_t spiSem;
SemaphoreHandle_t cookSem;
SemaphoreHandle_t lcdSem;

TimerHandle_t userTimerHandler;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
void StartdefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
void dataInit(void);
void formatTime(uint8_t, char*);
uint8_t findMaxIndex(uint8_t arr[], uint8_t);
void updateUserTimer(TimerHandle_t);
void copyTask (void *argument);
void cookTask (void *argument);
void lcdTask (void *argument);
void buzzerTask (void *argument);
void UART_Print(const char *message);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
user_data userData[4];
uint8_t rxBuffer[5];
volatile bool startSig = false;
uint8_t whatCourse = 0;
char timeStr1[6];
char timeStr2[6];
char timeStr3[6];
char timeStr4[6];
uint8_t arrCourseTime[4];
uint8_t arrCourseTime1[4];
uint8_t arrCourseTime2[4];
uint8_t arrCourseTime3[4];
uint8_t maxIndex;
uint8_t maxIndex1;
uint8_t maxIndex2;
uint8_t maxIndex3;
uint8_t remainingStuff[30];
volatile bool buzzerSig = false;
uint8_t resetCnt = 0;
volatile bool lcdState = false;
uint8_t txData = 0x30;
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();
  dataInit();
  HAL_SPI_Receive_IT(&hspi1, rxBuffer, sizeof(rxBuffer));
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  spiSem = xSemaphoreCreateBinary();
  cookSem = xSemaphoreCreateBinary();
  lcdSem = xSemaphoreCreateBinary();
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  userTimerHandler = xTimerCreate("userTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL, updateUserTimer);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of deaultTask */

  /* USER CODE BEGIN RTOS_THREADS */
  xTaskCreate(copyTask, "spiTask", 128, NULL, 3, &copyHandler);
  xTaskCreate(cookTask, "cookTask", 128, NULL, 2, &cookHandler);
  xTaskCreate(lcdTask, "lcdTask", 128, NULL, 1, &lcdHandler);
  xTaskCreate(buzzerTask, "buzzerTask", 128, NULL, 1, &buzzerHandler);

  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Buzzer_Pin|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SS_Pin */
  GPIO_InitStruct.Pin = SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Buzzer_Pin LED_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void UART_Print(const char *message)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

void dataInit()
{
	for (int i = 0; i < 4; i ++)
	{
		userData[i].courseData[0][0] = SmallBites;
		userData[i].courseData[0][1] = 5;
		userData[i].courseData[1][0] = AlmondWithCavior;
		userData[i].courseData[1][1] = 10;
		userData[i].courseData[2][0] = HearthOvenGrilledHanwoo;
		userData[i].courseData[2][1] = 15;
		userData[i].courseData[3][0] = BurdockTarteTatinWithSkinJuice;
		userData[i].courseData[3][1] = 10;
		userData[i].courseData[4][0] = SmallSweets;
		userData[i].courseData[4][1] = 15;
	}
	memset(remainingStuff, 50, sizeof(remainingStuff));
}

void updateUserTimer (TimerHandle_t xTimer)
{
	//UART_Print("Timer Start!\r\n");
	uint8_t temp = 0;
	lcd_clear();
	switch (whatCourse)
	{
	case 1 :
		//UART_Print("SUB1 Course Timer Start!\r\n");
		switch (maxIndex1)
		{
		case 0 :
			if (arrCourseTime1[maxIndex1] > 0)
			{
				temp = arrCourseTime1[maxIndex1];
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime1[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime1[maxIndex1] --;
			}
			if (temp <= arrCourseTime1[1] && arrCourseTime1[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime1[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime1[1] --;
			}
			if (temp <= arrCourseTime1[2] && arrCourseTime1[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime1[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime1[2] --;
			}
			if (temp <= arrCourseTime1[3] && arrCourseTime1[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime1[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime1[3] --;
			}
			break;
		case 1 :
			if (arrCourseTime1[maxIndex1] > 0)
			{
				temp = arrCourseTime1[maxIndex1];
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime1[maxIndex1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime1[maxIndex1] --;
			}
			if (temp <= arrCourseTime1[0] && arrCourseTime1[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime1[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime1[0] --;
			}
			if (temp <= arrCourseTime1[2] && arrCourseTime1[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime1[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime1[2] --;
			}
			if (temp <= arrCourseTime1[3] && arrCourseTime1[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime1[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime1[3] --;
			}
			break;
		case 2 :
			if (arrCourseTime1[maxIndex1] > 0)
			{
				temp = arrCourseTime1[maxIndex1];
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime1[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime1[maxIndex1] --;
			}
			if (temp <= arrCourseTime1[0] && arrCourseTime1[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime1[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime1[0] --;
			}
			if (temp <= arrCourseTime1[1] && arrCourseTime1[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime1[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime1[1] --;
			}
			if (temp <= arrCourseTime1[3] && arrCourseTime1[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime1[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime1[3] --;
			}
			break;
		case 3 :
			if (arrCourseTime1[maxIndex1] > 0)
			{
				temp = arrCourseTime1[maxIndex1];
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime1[maxIndex1], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime1[maxIndex1] --;
			}
			if (temp <= arrCourseTime1[0] && arrCourseTime1[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime1[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime1[0] --;
			}
			if (temp <= arrCourseTime1[1] && arrCourseTime1[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime1[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime1[1] --;
			}
			if (temp <= arrCourseTime1[2] && arrCourseTime1[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime1[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime1[2] --;
			}
			break;
		}
		if (arrCourseTime1[0] == 0 && arrCourseTime1[1] == 0 && arrCourseTime1[2] == 0 && arrCourseTime1[3] == 0)
		{
			lcd_clear();
			lcd_send_string("Course Finish!");
			buzzerSig = true;
			xSemaphoreGive(lcdSem);
			//UART_Print("S/W Timer lcdSem Give!\r\n");
			xTimerStop(userTimerHandler, 0);
		}
		break;
	case 2 :
		//UART_Print("Main Course Timer Start!\r\n");
		switch (maxIndex2)
		{
		case 0 :
			if (arrCourseTime2[maxIndex2] > 0)
			{
				temp = arrCourseTime2[maxIndex2];
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime2[maxIndex2], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime2[maxIndex2] --;
			}
			if (temp <= arrCourseTime2[1] && arrCourseTime2[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime2[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime2[1] --;
			}
			if (temp <= arrCourseTime2[2] && arrCourseTime2[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime2[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime2[2] --;
			}
			if (temp <= arrCourseTime2[3] && arrCourseTime2[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime2[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime2[3] --;
			}
			break;
		case 1 :
			if (arrCourseTime2[maxIndex2] > 0)
			{
				temp = arrCourseTime2[maxIndex2];
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime2[maxIndex2], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime2[maxIndex2] --;
			}
			if (temp <= arrCourseTime2[0] && arrCourseTime2[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime2[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime2[0] --;
			}
			if (temp <= arrCourseTime2[2] && arrCourseTime2[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime2[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime2[2] --;
			}
			if (temp <= arrCourseTime2[3] && arrCourseTime2[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime2[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime2[3] --;
			}
			break;
		case 2 :
			if (arrCourseTime2[maxIndex2] > 0)
			{
				temp = arrCourseTime2[maxIndex2];
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime2[maxIndex2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime2[maxIndex2] --;
			}
			if (temp <= arrCourseTime2[0] && arrCourseTime2[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime2[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime2[0] --;
			}
			if (temp <= arrCourseTime2[1] && arrCourseTime2[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime2[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime2[1] --;
			}
			if (temp <= arrCourseTime2[3] && arrCourseTime2[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime2[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime2[3] --;
			}
			break;
		case 3 :
			if (arrCourseTime2[maxIndex2] > 0)
			{
				temp = arrCourseTime2[maxIndex2];
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime2[maxIndex2], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime2[maxIndex2] --;
			}
			if (temp <= arrCourseTime2[0] && arrCourseTime2[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime2[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime2[0] --;
			}
			if (temp <= arrCourseTime2[1] && arrCourseTime2[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime2[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime2[1] --;
			}
			if (temp <= arrCourseTime2[2] && arrCourseTime2[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime2[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime2[2] --;
			}
			break;
		}
		if (arrCourseTime2[0] == 0 && arrCourseTime2[1] == 0 && arrCourseTime2[2] == 0 && arrCourseTime2[3] == 0)
		{
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Course Finish!");
			buzzerSig = true;
			xSemaphoreGive(lcdSem);
			//UART_Print("S/W Timer lcdSem Give!\r\n");
			xTimerStop(userTimerHandler, 0);
		}
		break;
	case 3 :
		//UART_Print("SUB2 Course Timer Start!");
		switch (maxIndex3)
		{
		case 0 :
			if (arrCourseTime3[maxIndex3] > 0)
			{
				temp = arrCourseTime3[maxIndex3];
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime3[maxIndex3], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime3[maxIndex3] --;
			}
			if (temp <= arrCourseTime3[1] && arrCourseTime3[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime3[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime3[1] --;
			}
			if (temp <= arrCourseTime3[2] && arrCourseTime3[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime3[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime3[2] --;
			}
			if (temp <= arrCourseTime3[3] && arrCourseTime3[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime3[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime3[3] --;
			}
			break;
		case 1 :
			if (arrCourseTime3[maxIndex3] > 0)
			{
				temp = arrCourseTime3[maxIndex3];
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime3[maxIndex3], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime3[maxIndex3] --;
			}
			if (temp <= arrCourseTime3[0] && arrCourseTime3[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime3[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime3[0] --;
			}
			if (temp <= arrCourseTime3[2] && arrCourseTime3[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime3[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime3[2] --;
			}
			if (temp <= arrCourseTime3[3] && arrCourseTime3[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime3[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime3[3] --;
			}
			break;
		case 2 :
			if (arrCourseTime3[maxIndex3] > 0)
			{
				temp = arrCourseTime3[maxIndex3];
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime3[maxIndex3], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime3[maxIndex3] --;
			}
			if (temp <= arrCourseTime3[0] && arrCourseTime3[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime3[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime3[0] --;
			}
			if (temp <= arrCourseTime3[1] && arrCourseTime3[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime3[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime3[1] --;
			}
			if (temp <= arrCourseTime3[3] && arrCourseTime3[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime3[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime3[3] --;
			}
			break;
		case 3 :
			if (arrCourseTime3[maxIndex3] > 0)
			{
				temp = arrCourseTime3[maxIndex3];
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime3[maxIndex3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime3[maxIndex3] --;
			}
			if (temp <= arrCourseTime3[0] && arrCourseTime3[0] > 0)
			{
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime3[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime3[0] --;
			}
			if (temp <= arrCourseTime3[1] && arrCourseTime3[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime3[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime3[1] --;
			}
			if (temp <= arrCourseTime3[2] && arrCourseTime3[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime3[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime3[2] --;
			}
			break;
		}
		if (arrCourseTime3[0] == 0 && arrCourseTime3[1] == 0 && arrCourseTime3[2] == 0 && arrCourseTime3[3] == 0)
		{
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Course Finish!");
			buzzerSig = true;
			xSemaphoreGive(lcdSem);
			UART_Print("S/W Timer lcdSem Give!\r\n");
			xTimerStop(userTimerHandler, 0);
		}
		break;
	default :
		//UART_Print("Appetizer or Desert Course Timer Start!\r\n");
		lcd_clear();
		lcd_put_cur(0, 0);
		formatTime(arrCourseTime[0], timeStr1);
		lcd_send_string(timeStr1);
		lcd_put_cur(0, 8);
		formatTime(arrCourseTime[1], timeStr2);
		lcd_send_string(timeStr2);
		lcd_put_cur(1, 0);
		formatTime(arrCourseTime[2], timeStr3);
		lcd_send_string(timeStr3);
		lcd_put_cur(1, 8);
		formatTime(arrCourseTime[3], timeStr4);
		lcd_send_string(timeStr4);
		for (int i = 0; i < 4; i ++)
			arrCourseTime[i]--;
		if (arrCourseTime[0] == 0 && arrCourseTime[1] == 0 && arrCourseTime[2] == 0 && arrCourseTime[3] == 0)
		{
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Course Finish!");
			buzzerSig = true;
			xSemaphoreGive(lcdSem);
			UART_Print("S/W Timer lcdSem Give!\r\n");
			xTimerStop(userTimerHandler, 0);
		}
		break;
	}
}

void formatTime(uint8_t time, char* str)
{
	uint8_t minutes = (time / 60);
	uint8_t seconds = (time / 1);
	sprintf(str, "%02d:%02d", minutes, seconds);
}

uint8_t findMaxIndex(uint8_t arr[], uint8_t size)
{
    uint8_t maxIndex = 0;
    for (int i = 1; i < size; i++)
    {
        if (arr[i] > arr[maxIndex])
            maxIndex = i;
    }
    return maxIndex;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi -> Instance == SPI1)
	{
		startSig = true;
		lcdState = true;
		//UART_Print("SPI rxBuffer Received!\r\n");
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		for (int i = 0; i < 5; i ++)
			rxBuffer[i] = rxBuffer[i] - '0';
		HAL_SPI_Receive_IT(&hspi1, rxBuffer, sizeof(rxBuffer));
		xSemaphoreGiveFromISR(spiSem, &xHigherPriorityTaskWoken);
		//UART_Print("SPI_ISR spiSem Give!\r\n");
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void copyTask(void *argument)
{
	while(1)
	{
		// UART_Print("copyTask excution!\r\n");
		if(xSemaphoreTake(spiSem, portMAX_DELAY) == pdTRUE)
		{
			//UART_Print("copyTask spiSem Take!\r\n");
			memcpy(&whatCourse, &rxBuffer[0], sizeof(rxBuffer[0]));
			switch (whatCourse)
				{
				case 1 :
					for (int i = 0; i < 4; i ++)
						memcpy(&userData[i].changeCourse[0], &rxBuffer[i + 1], 1);
					break;
				case 2 :
					for (int i = 0; i < 4; i ++)
						memcpy(&userData[i].changeCourse[1], &rxBuffer[i + 1], 1);
					break;
				case 3 :
					for (int i = 0; i < 4; i ++)
						memcpy(&userData[i].changeCourse[2], &rxBuffer[i + 1], 1);
					break;
				}
			xSemaphoreGive(cookSem);
			//UART_Print("copyTask cookSem Give!\r\n");
		}
		vTaskDelay(500);
	}
}

void cookTask(void *argument)
{
	while(1)
	{
		if (startSig)
		{
			// UART_Print("cookTask (startSig == true)\r\n");
			if (xSemaphoreTake(cookSem, portMAX_DELAY) == pdTRUE)
			{
				switch (whatCourse)
				{
				case 0 :
					for (int i = 0; i < 6; i ++)
						remainingStuff[i] --;
					printRemainingStuff(Seaweed);
					printRemainingStuff(Potato);
					printRemainingStuff(SalmonRoe);
					printRemainingStuff(Deodeok);
					printRemainingStuff(Radish);
					printRemainingStuff(RedCabbage);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 0)!\r\n");
				break;
				case 1 :
					for (int i = 0; i < 4; i ++)
					{
						userData[i].courseData[1][0] += userData[i].changeCourse[0];
						if (userData[i].courseData[1][0] == AlmondWithCavior)
						{
							for (int j = 6; j < 10; j ++)
								remainingStuff[j] --;
						}
						else if (userData[i].courseData[1][0] == SnowCrabAndPickledChrysanthemum)
						{
							userData[i].courseData[1][1] += 2;
							for (int k = 10; k < 13; k ++)
								remainingStuff[k] --;
						}
						else if (userData[i].courseData[1][0] == AblaloneTaco)
						{
							userData[i].courseData[1][1] -= 2;
							remainingStuff[13] --;
						}
					}
					printRemainingStuff(Almond);
					printRemainingStuff(Cavior);
					printRemainingStuff(Tofu);
					printRemainingStuff(Egg);
					printRemainingStuff(Yuzu);
					printRemainingStuff(Snowcrab);
					printRemainingStuff(Beansprouts);
					printRemainingStuff(Ablalone);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 1)!\r\n");
					break;
				case 2 :
					for (int i = 0; i < 4; i ++)
					{
						userData[i].courseData[2][0] += userData[i].changeCourse[1];
						if (userData[i].courseData[2][0] == HearthOvenGrilledHanwoo)
						{
							for(int j = 14; j < 17; j ++)
								remainingStuff[j] --;
						}
						else if (userData[i].courseData[2][0] == EmberToastedAcornNoodle)
						{
							userData[i].courseData[2][1] -= 2;
							for (int k = 17; k < 20; k ++)
								remainingStuff[k] --;
						}
						else if (userData[i].courseData[2][0] == BlackSesameSeaurchinNasturtium)
						{
							userData[i].courseData[2][1] -= 2;
							remainingStuff[20] --;
						}
					}
					printRemainingStuff(KoreanBeef);
					printRemainingStuff(Apple);
					printRemainingStuff(Onion);
					printRemainingStuff(Acorn);
					printRemainingStuff(Truffle);
					printRemainingStuff(Noddle);
					printRemainingStuff(Sesame);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 2)!\r\n");
					break;
				case 3 :
					for (int i = 0; i < 4; i ++)
					{
						userData[i].courseData[3][0] += userData[i].changeCourse[2];
						if (userData[i].courseData[3][0] == BurdockTarteTatinWithSkinJuice)
							remainingStuff[21] --;
						else if (userData[i].courseData[3][0] == TilefishMustardBrassica)
						{
							userData[i].courseData[3][1] += 2;
							for (int j = 22; j < 25; j ++)
								remainingStuff[j] --;
						}
						else if (userData[i].courseData[3][0] == fattyTuna)
						{
							userData[i].courseData[3][1] -= 2;
							remainingStuff[25] --;
						}
					}
					printRemainingStuff(Burdock);
					printRemainingStuff(Tilefish);
					printRemainingStuff(Mustard);
					printRemainingStuff(Brassica);
					printRemainingStuff(Tuna);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 3)!\r\n");
					break;
				case 4 :
					for (int i = 26; i < 30; i ++)
						remainingStuff[i] --;
					printRemainingStuff(Tuna);
					printRemainingStuff(Hibiscus);
					printRemainingStuff(Peanut);
					printRemainingStuff(HoneyCookie);
					printRemainingStuff(Kombucha);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 4)!\r\n");
				break;
				}
			}
		}
		vTaskDelay(500);
	}
}

void lcdTask(void *argument)
{
	while(1)
	{
		if (!startSig)
		{
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Seaweed");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Potato");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("SalmonRoe");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Deodeok");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Radish");
			vTaskDelay(1000);
			lcd_put_cur(0, 0);
			lcd_send_string("RedCabbage");
			vTaskDelay(1000);
		}
		else
		{
			switch (whatCourse)
			{
			case 0 :
				if (xSemaphoreTake(lcdSem, portMAX_DELAY) == pdTRUE)
				{
					//UART_Print("lcdTask lcdSem Take(whatCourse == 0)!\r\n");
					if (lcdState)
					{
						for(int i = 0; i < 4; i ++)
							memcpy(&arrCourseTime[i], &userData[i].courseData[0][1], 1);
						maxIndex = findMaxIndex(arrCourseTime, 4);
						lcdState = false;
						xTimerStart(userTimerHandler, 0);
					}
					else
					{
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Almond");
						lcd_put_cur(0, 8);
						lcd_send_string("Cavior");
						lcd_put_cur(1, 0);
						lcd_send_string("Tofu");
						lcd_put_cur(1, 8);
						lcd_send_string("Egg");
						vTaskDelay(1000);
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Yuzu");
						lcd_put_cur(0, 8);
						lcd_send_string("Snowcrab");
						lcd_put_cur(1, 0);
						lcd_send_string("Beansprouts");
						vTaskDelay(1000);
						xSemaphoreGive(lcdSem);
						//UART_Print("lcdTask lcdSem Give (whatCourse == 0 && lcdState == false)!\r\n");
					}
				}
				break;
			case 1 :
				if (xSemaphoreTake(lcdSem, portMAX_DELAY) == pdTRUE)
				{
					//UART_Print("lcdTask lcdSem Take(whatCourse == 1)!\r\n");
					if (lcdState)
					{
						for(int i = 0; i < 4; i ++)
							memcpy(&arrCourseTime1[i], &userData[i].courseData[1][1], 1);
						maxIndex1 = findMaxIndex(arrCourseTime1, 4);
						lcdState = false;
						xTimerStart(userTimerHandler, 0);
					}
					else
					{
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("KoreanBeef");
						lcd_put_cur(1, 0);
						lcd_send_string("Apple");
						lcd_put_cur(1, 8);
						lcd_send_string("Onion");
						vTaskDelay(1000);
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Acorn");
						lcd_put_cur(0, 8);
						lcd_send_string("Truffle");
						lcd_put_cur(1, 0);
						lcd_send_string("Noddle");
						vTaskDelay(1000);
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Sesame");
						vTaskDelay(1000);
						xSemaphoreGive(lcdSem);
						//UART_Print("lcdTask lcdSem Give (whatCourse == 1 && lcdState == false)!\r\n");
					}
				}
				break;
			case 2 :
				if(xSemaphoreTake(lcdSem, portMAX_DELAY) == pdTRUE)
				{
					//UART_Print("lcdTask lcdSem Take(whatCourse == 2)!\r\n");
					if (lcdState)
					{
						for(int i = 0; i < 4; i ++)
							memcpy(&arrCourseTime2[i], &userData[i].courseData[2][1], 1);
						maxIndex2 = findMaxIndex(arrCourseTime2, 4);
						lcdState = false;
						xTimerStart(userTimerHandler, 0);
					}
					else
					{
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Burdock");
						vTaskDelay(1000);
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Tilefish");
						lcd_put_cur(0, 8);
						lcd_send_string("Mustard");
						lcd_put_cur(1, 0);
						lcd_send_string("Brassica");
						vTaskDelay(1000);
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Tuna");
						vTaskDelay(1000);
						xSemaphoreGive(lcdSem);
						//UART_Print("lcdTask lcdSem Give (whatCourse == 2 && lcdState == false)!\r\n");
					}
				}
				break;
			case 3 :
				if(xSemaphoreTake(lcdSem, portMAX_DELAY) == pdTRUE)
				{
					//UART_Print("lcdTask lcdSem Take(whatCourse == 3)!\r\n");
					if (lcdState)
					{
						for(int i = 0; i < 4; i ++)
							memcpy(&arrCourseTime3[i], &userData[i].courseData[3][1], 1);
						maxIndex3 = findMaxIndex(arrCourseTime3, 4);
						lcdState = false;
						xTimerStart(userTimerHandler, 0);
					}
					else
					{
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("Hibiscus");
						lcd_put_cur(1, 0);
						lcd_send_string("Peanut");
						vTaskDelay(1000);
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("HoneyCookie");
						lcd_put_cur(1, 0);
						lcd_send_string("Kombucha");
						vTaskDelay(1000);
						xSemaphoreGive(lcdSem);
						//UART_Print("lcdTask lcdSem Give (whatCourse == 3 && lcdState == false)!\r\n");
					}
				}

				break;
			case 4 :
				if(xSemaphoreTake(lcdSem, portMAX_DELAY) == pdTRUE)
				{
					if (lcdState)
					{
						//UART_Print("lcdTask lcdSem Take(whatCourse == 4)!\r\n");
						for(int i = 0; i < 4; i ++)
							memcpy(&arrCourseTime[i], &userData[i].courseData[0][1], 1);
						maxIndex = findMaxIndex(arrCourseTime, 4);
						lcdState = false;
						xTimerStart(userTimerHandler, 0);
					}
				}
				break;
			}
		}
		vTaskDelay(500);
	}
}

void buzzerTask(void *argument)
{
	while(1)
	{
		// UART_Print("buzzerTask execution!\r\n");
		if (buzzerSig)
		{
			txData = 0x31;
			HAL_SPI_Transmit(&hspi1, &txData, sizeof(txData), HAL_MAX_DELAY);
			//UART_Print("SPI txData transmit to master!\r\n");
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			vTaskDelay(500);
			txData = 0x30;
			if (whatCourse >= 4)
			{
				whatCourse = 0;
				startSig = false;
				dataInit();
			}
			buzzerSig = !buzzerSig;
		}
		vTaskDelay(500);
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartdefaultTask */
/**
  * @brief  Function implementing the deaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartdefaultTask */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
