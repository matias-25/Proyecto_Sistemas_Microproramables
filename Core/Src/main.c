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
#include <string.h>
#include <stdio.h>
#include "lcd.h"
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

char buffer[128];	//buffer to write/read in microSD
char arraydatos[128];
int imprimir=0;
char mostrar_var[6];
LCD_t lcd;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
static void bufclear(char *buf, uint16_t len);
static void send_uart(char *string);
static void microSD_init(void);
//static void microSD_getSize(void);
//static void microSD_put(char *name);
static void microSD_get(char *name);
void mostrarLCD(void);
void Alimentar(void);
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
	//microSD_getSize();
	//microSD_put("prueba1.txt");
	//microSD_get("prueba1.txt");
	//microSD_put("prueba1.csv");
	//microSD_get("prueba1.csv");
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SD_CS_Pin|LCD_EN_Pin|LCD_RS_Pin|D7_Pin
                          |D6_Pin|D5_Pin|D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SD_CS_Pin LCD_EN_Pin LCD_RS_Pin D7_Pin
                           D6_Pin D5_Pin D4_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin|LCD_EN_Pin|LCD_RS_Pin|D7_Pin
                          |D6_Pin|D5_Pin|D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//Funcion para limiar el buffer
static void bufclear(char *buf, uint16_t len) {
	for (int i = 0; i < len; i++) {
		buf[i] = '\0';
	}
}
//------------------------------------------------------------

static void send_uart(char *string) {
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart1, (uint8_t*) string, len, 2000);
}
//------------------------------------------------------------

//Montamos el sistema de archivos
static void microSD_init(void) {
	fresult = f_mount(&fs, "/", 1);
	if (fresult != FR_OK){
		send_uart("ERROR!!! montaje tarjeta microSD fallido...\n");
	}
	else
		send_uart("microSD CARD montada con exito\n");
		}
//------------------------------------------------------------

/*//leemos la capacidad y el espacio libre
static void microSD_getSize(void) {
	f_getfree("", &fre_clust, &pfs);	//check free space

	//get microSD size in KB
	uint32_t total = (uint32_t) ((pfs->n_fatent - 2) * pfs->csize * 0.5);
	sprintf(buffer, "SD CARD Total Size: \t%luKB\n", total);
	send_uart(buffer);
	uint16_t len = strlen(buffer);
	bufclear(buffer, len);

	//get free space in KB
	uint32_t freeSpace;
	freeSpace = (uint32_t) (fre_clust * pfs->csize * 0.5);
	sprintf(buffer, "SD CARD Free Space: \t%luKB\n\n", freeSpace);
	send_uart(buffer);
	len = strlen(buffer);
	bufclear(buffer, len);
}*/
//------------------------------------------------------------

//guardamos informacion con PUTS
/*static void microSD_put(char *name) {
	//Open file to write/ create a file if it doesn't exist
	fresult = f_open(&fil, name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

	//Write data
	//f_puts("Prueba Escribiendo datos\n", &fil);
	f_puts("A1:Bovino Nº1;B2:comió 4kg;D3:18/02/2023;E5:hora 16:30", &fil);
	//Close file
	fresult = f_close(&fil);

	if (fresult == FR_OK) {
		sprintf(buffer, "%s%s", name, " creado y los datos fueron escritos\n");
		send_uart(buffer);
		}

	uint8_t len = strlen(buffer);
	bufclear(buffer, len);
}*/
//------------------------------------------------------------

//guardamos informacion con PUTS
static void microSD_get(char *name) {
	//Open file to read

	fresult = f_open(&fil, name, FA_READ);

	if (fresult == FR_OK) {
		//Read data from the file */
		//f_gets(buffer, f_size(&fil), &fil);
		f_read ( &fil, buffer, fil.fsize, &br);

		strcpy (arraydatos, buffer);
		/*for(int i = 0; i < 128; ++i){
		arraydatos[i]=buffer[i];
			}*/
		/*char msg[50];
		sprintf(msg, "%s%s", name,
				" Fue abierto y contiene los siguientes datos:\n");

		send_uart(msg);
		send_uart(buffer);
		send_uart("\n");*/

		/* Close file */
		f_close(&fil);
		uint8_t len = strlen(buffer);
		bufclear(buffer, len);
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
			  		lcd_setCursor(&lcd, 0, 1);
			  		lcd_print(&lcd, "Universidad Nacional de Catamarca");
			  		HAL_Delay(1500);
			  		for (int i = 0; i < 20; ++i) {
			  			lcd_scrollDisplayLeft(&lcd);
			  			HAL_Delay(400);
			  					  		}
			  		HAL_Delay(1000);
			  		lcd_clear(&lcd);
			  		lcd_home(&lcd);
			  		lcd_setCursor(&lcd, 0, 0);
			  		lcd_print(&lcd, "Iniciando");
			  		HAL_Delay(500);
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
		  	    }

	}
void Alimentar(void){
	imprimir=4;
	mostrarLCD();
	char vect_var[12];
	char var[6];
	char var_com[6]="Vaca 0";
	int num_vaca=0;
	float tiempo=0;
	float hora=0;
	float min=0;
	//int k=0;
	microSD_get("Bovino_tiempo.txt");

	for(int n=0;n<=9; ++n){
		num_vaca=n;
		var_com[5]=(char)(num_vaca);

		for(int j=0;j <12; ++j){vect_var[j]=arraydatos[j];}
		//k=k+11;
		for (int i = 0; i < 6; ++i) {var[i]=vect_var[i];
									mostrar_var[i]=var[i];}
		hora=(float)(vect_var[7])+(float)(vect_var[8]);
						min=vect_var[10]+vect_var[11];
			if(var[5] ==var_com[5]){
				//hora=(int)(vect_var[7])*10+(int)(vect_var[8]);
				//min=vect_var[10]*10+vect_var[11];
				tiempo= hora + min/60;
				if(tiempo<=24.00){
					imprimir=3;
					mostrarLCD();
				}else{
					imprimir=2;
					mostrarLCD();
				}

			}
		}

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
