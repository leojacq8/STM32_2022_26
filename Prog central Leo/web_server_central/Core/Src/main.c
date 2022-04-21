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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "lib_lcd.h"
#include "DHT22.h"
#include "irq.h"
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
static uint8_t USART1_BUFFER[USART1_BUFFER_SIZE] 	= "";
static uint8_t temp_string[DMA_SIZE] 				= "";
static uint8_t DMA_buff[DMA_SIZE] 					= "";
static uint8_t device_ID[] 							= "000";
//static uint8_t USART4_BUFFER[256];

static enum STATE        fsm_state;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	set_dma_irq(1);
	memcpy(temp_string,DMA_buff,sizeof(DMA_buff));
	memset(DMA_buff,0,sizeof(DMA_buff));
    HAL_UART_Receive_DMA(&huart4, DMA_buff, DMA_SIZE);
}


//CallBack function for IRQ

void rx_cb_USART_4()
{
    set_dma_irq(1);
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
  char *strToken					= NULL;

  struct Time_str date;

  uint8_t data_extract_temp[100] 	= "";
  uint8_t send_buffer[100] 			= "";
  uint8_t extract_data 				= 0;
  uint8_t check_sscanf 				= 0;
  uint8_t device_updt 				= 0;
  uint8_t to_update 				= 0;
  uint8_t temp[128] 				= "";
  uint8_t get_try 					= 0;
  uint8_t time_ok 					= 0;
  uint8_t data_ok 					= 0;
  uint8_t lenght 					= 0;

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
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  /*Timer init*/
  HAL_TIM_Base_Start(&htim2);

  /*DHT22 init*/
  DHT22_Init(&DHT22_1, DHT22_PORT, DHT22_PIN);

  /*Activate DMA on UART4*/

  HAL_UART_Receive_DMA(&huart4, DMA_buff, DMA_SIZE);

  /*First sequence of LCD*/
  lcd_init(&hi2c1, &rgbData);
  HAL_Delay(100);
  clearlcd();
  HAL_Delay(100);
  lcd_position(&hi2c1,0,0);
  HAL_Delay(10);
  lcd_print(&hi2c1,"Starting...");

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

				 memset(USART1_BUFFER, 0, sizeof(USART1_BUFFER));
				 memset(send_buffer,0, sizeof(send_buffer));
  				 memset(temp,0,sizeof(temp));

			     //fsm_state = ST_CHECK_DMA;
			     fsm_state = ST_CHECK_DMA;
	        	 break;

			 /***********************************/
			 /*				SEND DATA			*/
			 /***********************************/
			 case ST_CHECK_DMA:

				 if(get_dma_irq() == 1)
				 {
					    set_dma_irq(0);

						strToken = strtok((char*) temp_string,"$");

						while (( strToken != NULL) && (extract_data<DMA_MAX_DATA))
						{

							lenght = strlen(strToken);

							if (lenght > DATA_SIZE)
							{
								lenght = DATA_SIZE;
							}
							if (lenght == DATA_SIZE)
							{
								check_sscanf = sscanf(strToken, "&i=%*d&u=%c=%*s",&device_updt);
								if(check_sscanf == 1)
								{
									if(device_updt == 1)
									{
										to_update = 1;
									}
								}
								memset(data_extract_temp,0,sizeof(data_extract_temp));
								strcat((char *)data_extract_temp,"$");

					        	memset(temp,0, sizeof(temp));
					        	memcpy(temp,strToken,lenght-2);
					        	/*temp[DATA_SIZE-1] = 0;
					        	temp[DATA_SIZE-2] = 0;*/
								strcat((char *)data_extract_temp,(char *)temp);

								strToken = strtok( NULL, "$" );
								extract_data++;
							}
							else
							{
								strToken = strtok( NULL, "$" );
								extract_data++;
							}
						}


			    	  fsm_state = ST_GET_DATA;
			          break;
				 }
				 else
				 {
					 HAL_Delay(1000);
			    	 fsm_state = ST_CHECK_DMA;
			         break;
				 }


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
						    	   		HAL_Delay(1000);
				    	   				get_try++;
				    	   			  }

				    	   		  }
				    	   		  else
				    	   		  {
					    	   		    HAL_Delay(1000);
				    	   				get_try++;
				    	   		  }

				    	   	  }
				    	   	  else
				    	   	  {
				    	   		    HAL_Delay(1000);
			    	   				get_try++;
				    	   	  }
			    	   }

				      fsm_state = ST_SEND_DATA;
			          break;

			 /***********************************/
			 /*				SEND DATA			*/
			 /***********************************/
			 case ST_SEND_DATA:

	        	 strcat((char *)send_buffer,"$");

	        	 memset(temp,0, sizeof(temp));
	        	 sprintf((char *)temp,"&i=%s", device_ID);
	        	 strcat((char *)send_buffer,(char *)temp);

	        	// memset(temp,0, sizeof(temp));
	        	 //sprintf((char *)temp,"&i=%s", device_ID);
	        	// strcat((char *)send_buffer,(char *)"&u=1");

	        	 memset(temp,0, sizeof(temp));
	        	 sprintf((char *)temp,"&t=%.2f", DHT22_1.temperature);
	        	 strcat((char *)send_buffer,(char *)temp);

	        	 memset(temp,0, sizeof(temp));
	        	 sprintf((char *)temp,"&h=%.2f", DHT22_1.humidity);
	        	 strcat((char *)send_buffer,(char *)temp);

	        	 strcat((char *)send_buffer,(char *)temp_string);

	        	 strcat((char *)send_buffer,"\r\n");


				 memset(USART1_BUFFER, 0, sizeof(USART1_BUFFER));
				 HAL_UART_Transmit(&huart1,(uint8_t*)send_buffer, strlen((char *)send_buffer), 200);
				 to_update = 1;
				 if(to_update == 1)
				 {
					 HAL_UART_Receive(&huart1,(uint8_t*)USART1_BUFFER, USART1_BUFFER_SIZE - 1, 4000);

			    	 fsm_state = ST_CHECK_RESP;
			         break;
				 }
				 else
				 {
			    	 fsm_state = ST_IDLE;
			         break;
				 }

			 /***********************************/
			 /*				CHECK RESP			*/
			 /***********************************/
			 case ST_CHECK_RESP:

				  check_sscanf = sscanf((char*)USART1_BUFFER, "%d/%d/%d-%d:%d:%d",&date.day, &date.month,&date.year,&date.hour,&date.minute,&date.second);
				  time_ok = 0;

				  if(check_sscanf == 6)
				  {

					  if(date.year >= DEFAULT_YEAR)
					  {
						  if((date.month > 0) && (date.month <= 12))
						  {
							  if((date.day > 0) && (date.day <= 31))
							  {
								  if((date.hour >= 0) && (date.hour < 24))
								  {
									  if((date.minute >= 0) && (date.minute < 60))
									  {
										  if((date.second >= 0) && (date.second < 60))
										  {
											  time_ok = 1;
										  }
									  }
								  }
							  }
						  }
					  }
				  }

				  if(time_ok == 0)
				  {
					  memset(USART1_BUFFER, 0, sizeof(USART1_BUFFER));

					  date.year = DEFAULT_YEAR;
					  date.month = DEFAULT_MONTH;
					  date.day = DEFAULT_DAY;
					  date.hour = DEFAULT_HOUR;
					  date.minute = DEFAULT_MINUTE;
					  date.second = DEFAULT_SECOND;

					  sprintf((char*)USART1_BUFFER,"%d/%d/%d-%d:%d:%d", date.day, date.month, date.year, date.hour, date.minute, date.second);
				  }

		    	  fsm_state = ST_UPDATE;
		          break;

			 /***********************************/
			 /*				UPDATE				*/
			 /***********************************/
			 case ST_UPDATE:


				 HAL_UART_Transmit(&huart4,(uint8_t*)USART1_BUFFER, strlen((char *)USART1_BUFFER), 100);

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
