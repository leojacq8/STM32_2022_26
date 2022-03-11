/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "irq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "lib_lcd.h"
#include "DHT22.h"
#include "string.h"
#include "stdio.h"
#include <stm32l1xx_hal.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DHT22_PORT GPIOA
#define DHT22_PIN GPIO_PIN_1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

DMA_HandleTypeDef hdma_usart4_rx;

rgb_lcd rgbData;

static struct DHT22 DHT22_1;
static uint8_t device_ID = 1;
static uint8_t DMA_buff[256] = "";
static uint8_t USART1_BUFFER[256];
static uint8_t USART2_BUFFER[256];

static enum STATE        fsm_state;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
 HAL_Delay(1);
}

//CallBack function for IRQ
void rx_cb_USART_1()
{
	set_usar1_irq(1);
}

void rx_cb_USART_2()
{
    set_usar2_irq(1);
}

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
  uint8_t send_buffer[100] ="";
  uint8_t temp[128] = "";
  uint8_t data_ok = 0;
  uint8_t get_try = 0;
  uint32_t	wd_timer			= 0;

  fsm_state = ST_IDLE;
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
  MX_I2C1_Init();
  MX_TIM2_Init();
  //MX_USART1_UART_Init();
  MX_DMA_Init();
  //MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  //Timer init
  HAL_TIM_Base_Start(&htim2);

  //DHT22 init
  DHT22_Init(&DHT22_1, DHT22_PORT, DHT22_PIN);

  //Activate DMA on UART2
  //HAL_UART_Receive_DMA (&huart2, DMA_buff, 12);

  //Enable IRQ on UART1
  //USART1_Enable_IT(&huart1);

  //First sequence of LCD
  lcd_init(&hi2c1, &rgbData);
  HAL_Delay(100);
  clearlcd();
  HAL_Delay(100);
  lcd_position(&hi2c1,0,0);
  HAL_Delay(10);
  lcd_print(&hi2c1,"Starting...");


  //HAL_UART_Transmit(&huart1,"AT\r\n", strlen("AT\r\n"), 100);

  //HAL_UART_Receive(&huart1,USART2_BUFFER, strlen((char *)USART2_BUFFER), 5000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  switch (fsm_state)
	  {

	  	  	  /************************************/
	          /*				IDLE			  */
	          /************************************/
	          case ST_IDLE:

	        	 // memset(send_buffer,0, sizeof(send_buffer));
	        	  //HAL_UART_Receive_DMA (&huart1, DMA_buff, 12);
	        	 /* HAL_UART_Receive(&huart1, DMA_buff, 10, 5000);
	        	  HAL_Delay(2000);*/
	        	  //           HAL_UART_Transmit(&huart1, (uint8_t*)"leo\n", strlen("leo\n"), 100);
	        	  HAL_Delay(1000);

		    	  fsm_state = ST_GET_DATA;
	        	  break;

	          /***********************************/
		      /*				GET_DATA			*/
		      /***********************************/
		       case ST_GET_DATA:

	   			  get_try = 0;
	   			  data_ok = 0;

		    	   while((get_try < 3 ) && (data_ok == 0))
		    	   {

			    	   if(DHT22_Start(&DHT22_1) == 0)
			    	   	  {
			    	   		  if( DHT22_Check_Response(&DHT22_1)== 0)
			    	   		  {
			    	   			  if(DHT22_Read_Temp_Hum(&DHT22_1) == 0)
			    	   			  {
			    	   				  clearlcd();

			    	   				  lcd_position(&hi2c1,0,0);

			    	   				  memset(temp,0,sizeof(temp));
			    	   				  sprintf((char*)temp,"Hum: %.2f %c ",DHT22_1.humidity, 0x25);
			    	   				  lcd_print(&hi2c1,(char*)temp);

			    	   				  lcd_position(&hi2c1,0,1);

			    	   				  memset(temp,0,sizeof(temp));
			    	   				  sprintf((char*)temp,"Temp: %.2fC  ",DHT22_1.temperature);
			    	   				  lcd_print(&hi2c1,(char*)temp);

			    	   				  get_try = 0;
			    	   				  data_ok = 1;
			    	   			  }
			    	   			  else
			    	   			  {
					    	   		HAL_Delay(10);
			    	   				get_try++;
			    	   			  }

			    	   		  }
			    	   		  else
			    	   		  {
				    	   		    HAL_Delay(10);
			    	   				get_try++;
			    	   		  }

			    	   	  }
			    	   	  else
			    	   	  {
			    	   		    HAL_Delay(10);
		    	   				get_try++;
			    	   	  }
		    	   }

			      fsm_state = ST_IDLE;
		    	 // fsm_state = ST_CHECK_DMA;
		          break;

			 /***********************************/
			 /*				SEND DATA			*/
			 /***********************************/
			 case ST_CHECK_DMA:

					/*memcpy(temp_string,string,sizeof(string));

					strToken = strtok((char*) temp_string,"$");

					while (( strToken != NULL) && (i<NB_DATA_PACKET))
					{
						memset(&nmea_dat, 0, sizeof(nmea_dat));
						memset(NMEA_SEQ_to_WR,120,sizeof(NMEA_SEQ_to_WR));
						memset(data_extract_temp,0,sizeof(data_extract_temp));

						NMEA_SEQ_to_WR[0] = 0;

						lenght = strlen(strToken);
						if (lenght>RAW_DATA_XBEE)
						{
							lenght = RAW_DATA_XBEE;
						}
						if (lenght>=RAW_DATA_GPS_MIN)
						{
							data_extract_temp[0] = '$';
							memcpy(&data_extract_temp[1],strToken,lenght);
							strToken = strtok( NULL, "$" );
							i++;
						}
						strToken = strtok( NULL, "$" );
						i++;
					}*/

				 // HAL_Delay(10000);

				 // HAL_UART_Receive_DMA(&huart4, DMA_buff, sizeof(DMA_buff));

		    	  fsm_state = ST_SEND_DATA;
		          break;

			 /***********************************/
			 /*				SEND DATA			*/
			 /***********************************/
			 case ST_SEND_DATA:

	        	  memset(send_buffer,0, sizeof(send_buffer));
	        	  strcat((char *)send_buffer,"$");

	        	  memset(temp,0, sizeof(temp));
	        	  sprintf((char *)temp,"&i=%d", device_ID);
	        	  strcat((char *)send_buffer,(char *)temp);

	        	  memset(temp,0, sizeof(temp));
	        	  sprintf((char *)temp,"&t=%.2f", DHT22_1.temperature);
	        	  strcat((char *)send_buffer,(char *)temp);

	        	  memset(temp,0, sizeof(temp));
	        	  sprintf((char *)temp,"&h=%.2f", DHT22_1.humidity);
	        	  strcat((char *)send_buffer,(char *)temp);
	        	  strcat((char *)send_buffer,(char *)"\r\n");

	        	  strcat((char *)send_buffer,"&i=");
	        	  strcat((char *)send_buffer,(char *)DMA_buff);
	        	  strcat((char *)send_buffer,"\r\n");

				 memset(USART1_BUFFER, 0, sizeof(USART1_BUFFER));

				 set_usar1_irq(0);
				 HAL_UART_Transmit(&huart1,(uint8_t*)send_buffer, strlen((char *)send_buffer), 100);
				 while (get_usar1_irq()!=1 && wd_timer <= 50)
				 {
				 	HAL_Delay(1);
				 	wd_timer++;
				 }
				 if (get_usar1_irq() ==1)
				 {
					HAL_Delay(30);
					HAL_UART_Receive(&huart1,(uint8_t*)send_buffer, strlen((char *)send_buffer), 1);
				 }/*
				 if(strstr((char*)USART1_BUFFER, (char*)ref_str)!=NULL)
				 {
				  return ERR_NONE;
				 }
				 else
				 {
				 	return ERR_FAILURE;
				 }*/

		    	 fsm_state = ST_CHECK_RESP;
		         break;

			 /***********************************/
			 /*				CHECK RESP			*/
			 /***********************************/
			 case ST_CHECK_RESP:

		    	  fsm_state = ST_UPDATE;
		          break;

			 /***********************************/
			 /*				UPDATE				*/
			 /***********************************/
			 case ST_UPDATE:

		    	  fsm_state = ST_SLEEP;
		          break;

			 /***********************************/
			 /*				SLEEP       		*/
			 /***********************************/
			 case ST_SLEEP:

		    	  fsm_state = ST_IDLE;
		          break;

			 /***********************************/
			 /*				default       		*/
			 /***********************************/
			 default:

				  Error_Handler();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
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
