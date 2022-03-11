/*
 * TMP_i2c.h

 */

#ifndef TMP_I2C_H_
#define TMP_I2C_H_

void TMP_Receive(uint16_t address,uint8_t *Data,uint16_t len);
void TMP_Transmit(uint16_t address, uint8_t *Data, uint16_t len);
void TMP_init(I2C_HandleTypeDef hi2c);
void Temp_read( float *temp, float *humidity);


#endif /* TMP_I2C_H_ */
