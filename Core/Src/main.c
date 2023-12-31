/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs_sd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "mfrc522.h"

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
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
FATFS fs;  				// file system
FIL fil; 					// File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  		// File read/write count

/**** microSD capacity related *****/
FATFS *pfs;
DWORD fre_clust;

/*RFID*/
uint8_t UID[8];
uint8_t TagType;
char buf_tx[50];

/*Mis variables*/
char buffer[128];	//buffer to write/read in microSD
char arraydatos[128];
char buffer2[4096];
char arraydatos2[4096];
int imprimir=0;
char mostrar_var[6];
LCD_t lcd;
char tiempo[32]; //hora
int cont_lin=0; //contador de linea w
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
static void bufclear(char *buf, uint16_t len);
//static void send_uart(char *string);
static void microSD_init(void);
//static void microSD_getSize(void);
static void microSD_put(char *name);
static void microSD_get(char *name);
void mostrarLCD(void);
void Alimentar(void);
void Identidicacion(void);
void Menu(void);
void hora(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int opcion1=0;
	int opcion2=0;

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
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */


	// LCD SETTINGS ----------------------------------------------
	lcd.RS_port = LCD_RS_GPIO_Port;
	lcd.RS_pin = LCD_RS_Pin;
	//lcd.RW_port = LCD_RW_GPIO_Port;
	//lcd.RW_pin = LCD_RW_Pin;
	lcd.EN_port = LCD_EN_GPIO_Port;
	lcd.EN_pin = LCD_EN_Pin;
	lcd.D4_port = D4_GPIO_Port;
	lcd.D4_pin = D4_Pin;
	lcd.D5_port = D5_GPIO_Port;
	lcd.D5_pin = D5_Pin;
	lcd.D6_port = D6_GPIO_Port;
	lcd.D6_pin = D6_Pin;
	lcd.D7_port = D7_GPIO_Port;
	lcd.D7_pin = D7_Pin;
	//Inicializamos el LCD
	lcd_begin(&lcd, 16, 2, LCD_5x8DOTS);
	imprimir=1;
	mostrarLCD();
	//Inicializo microSD
	microSD_init();
	//Inicializo RFID
	MFRC522_Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  opcion1=1;
	  switch(opcion1){
	  /* Menu */
	  case 1:
		  	Alimentar();
		  	opcion1=2;
	  		break;
	  case 2:
		  HAL_Delay(1500);
		  opcion1=0;
	  		break;
	  	   }

	  switch(opcion2){
	  /* Configuraciones */
	  case 1:
	  		break;
	  case 2:
	  		break;
	  case 3:
	  		break;
	  case 4:
	  		break;
	  	    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  HAL_PWR_EnableBkUpAccess();
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x9;
  sTime.Minutes = 0x30;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */


  /* USER CODE END RTC_Init 2 */

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
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
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
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, D4_Pin|D5_Pin|D6_Pin|D7_Pin
                          |MFRC522_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SD_CS_Pin|LCD_EN_Pin|LCD_RS_Pin|RFID_RST_Pin
                          |MFRC522_CS_Pin|MFRC522_SCK_Pin|MFRC522_MOSI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : D4_Pin D5_Pin D6_Pin D7_Pin
                           MFRC522_RST_Pin */
  GPIO_InitStruct.Pin = D4_Pin|D5_Pin|D6_Pin|D7_Pin
                          |MFRC522_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SD_CS_Pin LCD_EN_Pin LCD_RS_Pin RFID_RST_Pin
                           MFRC522_CS_Pin MFRC522_SCK_Pin MFRC522_MOSI_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin|LCD_EN_Pin|LCD_RS_Pin|RFID_RST_Pin
                          |MFRC522_CS_Pin|MFRC522_SCK_Pin|MFRC522_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MFRC522_MISO_Pin */
  GPIO_InitStruct.Pin = MFRC522_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MFRC522_MISO_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void MI_Menu(void){

}
//funcion hora
void hora(void){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	//RTC->CR |= RTC_CR_BYPSHAD;
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	sprintf(tiempo,";Hora: %d:%d:%d", sTime.Hours, sTime.Minutes, sTime.Seconds);
}
//Funcion para limiar el buffer
static void bufclear(char *buf, uint16_t len) {
	for (int i = 0; i < len; i++) {
		buf[i] = '\0';
	}
}
//------------------------------------------------------------

/*static void send_uart(char *string) {
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart1, (uint8_t*) string, len, 2000);
}*/
//------------------------------------------------------------

//Montamos el sistema de archivos
static void microSD_init(void) {
	fresult = f_mount(&fs, "/", 1);
	if (fresult != FR_OK){
		//send_uart("ERROR!!! montaje tarjeta microSD fallido...\n");
	}
	//else
		//send_uart("microSD CARD montada con exito\n");
		}


//guardamos informacion en la última posicion
static void microSD_put(char *name) {
	char num_linea[20];
	sprintf(num_linea,"\n %d Bovino:",cont_lin);
	cont_lin=cont_lin+1;
	//Open file to write/ create a file if it doesn't exist
	fresult = f_open(&fil, name,FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	//Move to offset to be the end of the file
	fresult = f_lseek (&fil, fil.fsize);
	//Write data
	f_puts(num_linea, &fil);
	f_puts(buf_tx, &fil);
	f_puts(tiempo, &fil);
	//Close file
	fresult = f_close(&fil);

	if (fresult == FR_OK) {
		//sprintf(buffer, "%s%s", name, " creado y los datos fueron escritos\n");
		//send_uart(buffer);
		}

	uint8_t len = strlen(buffer);
	bufclear(buffer, len);
}
//------------------------------------------------------------

static void microSD_get(char *name) {
	//Open file to read

	fresult = f_open(&fil, name, FA_READ);

	if (fresult == FR_OK) {
		//Read data from the file */
		//f_gets(buffer, f_size(&fil), &fil);
		f_read ( &fil, buffer2, fil.fsize, &br);
		strcpy(arraydatos2, buffer2);
		/* Close file */
		f_close(&fil);
		uint8_t len = strlen(buffer2);
		bufclear(buffer2, len);
		}
}
//------------------------------------------------------------
void mostrarLCD(void) {
	 switch(imprimir){
		  /* muestro en pantalla */
		  case 1:
			  	  //Mensaje de de inicio
			  		lcd_setCursor(&lcd, 0, 0);
			  		lcd_print(&lcd, "Sistemas Microprogramables");
			  		HAL_Delay(1500);
			  		for (int i = 0; i < 10; ++i) {
			  			lcd_scrollDisplayLeft(&lcd);
			  			HAL_Delay(400);
			  					  		}
			  		HAL_Delay(1000);
			  		lcd_clear(&lcd);
			  		lcd_home(&lcd);
			  		lcd_setCursor(&lcd, 4, 1);
			  		lcd_print(&lcd, "U.N.Ca.");
			  		HAL_Delay(2000);
			  		lcd_clear(&lcd);
			  		lcd_home(&lcd);
			  		lcd_setCursor(&lcd, 0, 0);
			  		lcd_print(&lcd, "Iniciando");
			  		HAL_Delay(1500);
			  		lcd_clear(&lcd);
			  		lcd_home(&lcd);
			  		imprimir=0;
		  		break;
		  case 2:
			  	  lcd_clear(&lcd);
			  	  lcd_home(&lcd);
			  	  lcd_setCursor(&lcd, 0, 0);
			  	  lcd_print(&lcd, "Alimentando");
			  	  lcd_setCursor(&lcd, 0, 1);
			  	  lcd_print(&lcd, mostrar_var);
			  	  HAL_Delay(1500);
			  	  imprimir=0;
		  		break;
		  case 3:
			  	  lcd_clear(&lcd);
			  	  lcd_home(&lcd);
			  	  lcd_setCursor(&lcd, 0, 0);
			  	  lcd_print(&lcd, mostrar_var);
			  	  lcd_setCursor(&lcd, 0, 1);
			  	  lcd_print(&lcd, "NO DAR DE ALIMENTAR");
			  	  HAL_Delay(1500);
			  	  imprimir=0;
		  		break;
		  case 4:
			  	  lcd_clear(&lcd);
			  	  lcd_home(&lcd);
			  	  lcd_setCursor(&lcd, 0, 0);
			  	  lcd_print(&lcd, "En Marcha");
			  	  HAL_Delay(1500);
			  	  imprimir=0;
		  		break;
		  case 5:
			  	  lcd_clear(&lcd);
			  	  lcd_home(&lcd);
			  	  lcd_setCursor(&lcd, 0, 0);
			  	  lcd_print(&lcd, "Identificando");
			  	  lcd_setCursor(&lcd, 0, 1);
			  	  lcd_print(&lcd, buf_tx);
			  	  HAL_Delay(2000);
			  	  imprimir=0;
			  	break;
		  case 6:
			  	  lcd_clear(&lcd);
			  	  lcd_home(&lcd);
			  	  lcd_setCursor(&lcd, 0, 0);
			  	  lcd_print(&lcd, "No hay tarjeta");
			  	  HAL_Delay(1500);
			  	  imprimir=0;
		  			  	break;

		  	    }

	}
void Identidicacion(void){
char UID_char[8];
//char Tag_char;
	if(MFRC522_IsCard(&TagType))
		{

			if(MFRC522_ReadCardSerial((uint8_t*)&UID))
				{
				sprintf(buf_tx, " ID: ");
				for(uint8_t i=0; i<4; i++){
					utoa(UID[i],&UID_char[i],16);}
				sprintf(buf_tx,UID_char);
				hora();
				HAL_Delay(10);
				microSD_put("Bovino_tiempo.txt");
				imprimir=5;
				mostrarLCD();
				}
			MFRC522_Halt();
		}
	}
void Alimentar(void){
	imprimir=4;
	mostrarLCD();
	Identidicacion();
	microSD_get("Bovino_tiempo.txt");
	}
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
