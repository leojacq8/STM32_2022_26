/*
 * DHT22.c
 *
 *  Created on: Jan 30, 2022
 *      Author: leojacq
 */

#include "DHT22.h"


void DHT22_Init (struct DHT22 *sensor_DHT22, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	 sensor_DHT22->GPIOx = GPIOx;
	 sensor_DHT22->GPIO_Pin = GPIO_Pin;
	 sensor_DHT22->temperature = 0.;
	 sensor_DHT22->humidity = 0.;
}

void DHT22_Set_Output(struct DHT22 *sensor_DHT22)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = sensor_DHT22->GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(sensor_DHT22->GPIOx, &GPIO_InitStruct);

}

void DHT22_Set_Input (struct DHT22 *sensor_DHT22)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = sensor_DHT22->GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(sensor_DHT22->GPIOx, &GPIO_InitStruct);
}

/*Set pinmode to output and send > 1ms low signal,  20-40 us high signal and set input*/
uint8_t DHT22_Start (struct DHT22 *sensor_DHT22)
{
	DHT22_Set_Output(sensor_DHT22);
	HAL_Delay(10);
    HAL_GPIO_WritePin (sensor_DHT22->GPIOx, sensor_DHT22->GPIO_Pin, GPIO_PIN_RESET);
	delay_us(1200);
	HAL_GPIO_WritePin (sensor_DHT22->GPIOx, sensor_DHT22->GPIO_Pin, GPIO_PIN_SET);
	delay_us(30);
	DHT22_Set_Input(sensor_DHT22);

	return 0;
}

/*Wait sensor response, 80 us low signal and 80 us high signal*/
uint8_t DHT22_Check_Response (struct DHT22 *sensor_DHT22)
{
	uint8_t wd_timer = 0;
	while(!(HAL_GPIO_ReadPin(sensor_DHT22->GPIOx, sensor_DHT22->GPIO_Pin)) && (wd_timer < 85))
	{
		delay_us(1);
		wd_timer++;
	}

	if(wd_timer == 85)
	{
		return 1;
	}
	else
	{

		wd_timer = 0;
		while((HAL_GPIO_ReadPin(sensor_DHT22->GPIOx, sensor_DHT22->GPIO_Pin)) && (wd_timer < 85))
		{
			delay_us(1);
			wd_timer++;
		}

		if(wd_timer == 85)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

}

uint8_t DHT22_Read_raw (struct DHT22 *sensor_DHT22, uint8_t * data)
{
	uint8_t i = 0;
	uint8_t wd_timer = 0;

	for (i=0;i<8;i++)
	{
		wd_timer = 0;

		//Start bit of 50us
		while(!(HAL_GPIO_ReadPin(sensor_DHT22->GPIOx, sensor_DHT22->GPIO_Pin)) && (wd_timer < 50))
		{
			delay_us(1);
			wd_timer++;
		}

		wd_timer = 0;

		while((HAL_GPIO_ReadPin(sensor_DHT22->GPIOx, sensor_DHT22->GPIO_Pin)) && (wd_timer < 90))
		{
			wd_timer += 10;
			delay_us(10);
		}


		if((wd_timer >= 20)&&(wd_timer <= 30 ))
		{
			*data &= ~1<<(7-i);
		}
		else if((wd_timer >= 60)&&(wd_timer <= 80 ))
		{
			*data |= 1<<(7-i);
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

uint8_t DHT22_Read_Temp_Hum (struct DHT22 *sensor_DHT22)
{
	uint8_t Rh_byte1 = 0, Rh_byte2 = 0, Temp_byte1 = 0, Temp_byte2 = 0 , SUM = 0, SUM_temp = 0, read_error = 0;
	uint16_t RH = 0, TEMP = 0;


	  read_error += DHT22_Read_raw(sensor_DHT22, &Rh_byte1);
	  read_error += DHT22_Read_raw(sensor_DHT22, &Rh_byte2);
	  read_error += DHT22_Read_raw(sensor_DHT22, &Temp_byte1);
	  read_error += DHT22_Read_raw(sensor_DHT22, &Temp_byte2);
	  read_error += DHT22_Read_raw(sensor_DHT22, &SUM);

	  if(read_error == 0)
	  {
		  SUM_temp = Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2 ;

		  if(SUM == SUM_temp)
		  {

			  TEMP = ((Temp_byte1<<8)|Temp_byte2);
			  RH = ((Rh_byte1<<8)|Rh_byte2);

			  sensor_DHT22->temperature = (float) (TEMP/10.0);
			  sensor_DHT22->humidity = (float) (RH/10.0);
			  return 0;
		  }
		  else
		  {
			  return 1;
		  }
	  }
	  else
	  {
		  return 1;
	  }
}
