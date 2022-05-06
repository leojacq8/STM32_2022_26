/*
 * DHT22.h
 *
 *  Created on: Apr 21, 2022
 *      Author: leojacq
 */


#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "main.h"
#include "tim.h"

 struct DHT22{

	 GPIO_TypeDef *GPIOx;
	 uint16_t GPIO_Pin;
	 float temperature;
	 float humidity;
};

/*Init sensor with port and pin attached with structure*/
void DHT22_Init (struct DHT22 *sensor_DHT22, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/*Switch pin direction to output */
void DHT22_Set_Output(struct DHT22 *sensor_DHT22);

/*Switch pin direction to input */
void DHT22_Set_Input(struct DHT22 *sensor_DHT22);

/*request to start communication with sensor*/
uint8_t DHT22_Start(struct DHT22 *sensor_DHT22);

/*Get response from the sensor after start */
uint8_t DHT22_Check_Response (struct DHT22 *sensor_DHT22);

/*Read byte bit per bit  */
uint8_t DHT22_Read_raw (struct DHT22 *sensor_DHT22, uint8_t * data);

/*Get temperature and humidity data and check checksum */
uint8_t DHT22_Read_Temp_Hum (struct DHT22 *sensor_DHT22);

#endif /* INC_DHT22_H_ */
