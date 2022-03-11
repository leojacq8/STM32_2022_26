#include "stm32l1xx_hal.h"
#include "outils.h"
/*
 * TMP_i2c.c

 */

#define TIMEOUT 5000
#define ADRESSETEMP (0x44<<1)

I2C_HandleTypeDef hi2c_tmp;
void TMP_Receive(uint16_t address, uint8_t *Data, uint16_t len)
{


	 HAL_I2C_Master_Receive(&hi2c_tmp, address, Data, len, TIMEOUT);

}

void TMP_init(I2C_HandleTypeDef hi2c)
{
	hi2c_tmp = hi2c;
}
void TMP_Transmit(uint16_t address, uint8_t *Data, uint16_t len)
{
	 HAL_I2C_Master_Transmit(&hi2c_tmp, address, Data, len, TIMEOUT);
}

void Temp_read( float *temp, float *humidity)
{
	float temperature=0;

	uint8_t Data[6]={0x24, 0x00};
	TMP_Transmit(ADRESSETEMP, Data, 2);
	HAL_Delay(50);
	TMP_Receive(ADRESSETEMP, Data, 6);

	temperature = Data[0] * 256 + Data[1];
	temperature = -45 + (175 * temperature / 65535.0);
	*temp=-45+175*(Data[0]<<8 | Data[1])/65535.0 ;
	*humidity=100*(Data[3]<<8 | Data[4])/65535.0 ;

}

