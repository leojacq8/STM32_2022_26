/*
 * caracter.h
 *
 *  Created on: 22 févr. 2019
 *      Author: Abbass
 */

#ifndef CARACTER_H_
#define CARACTER_H_

#include <stdbool.h>
#include <math.h>
#include "usart.h"
#include "stm32l1xx_Hal_uart.h"
#include "stm32l1xx_hal.h"


char rx_buffer[50], tx_buffer[50];

void reverse(char *str, int len);
int intToStr(int x, char str[], int d);
void ftoat(float n, char *res, int afterpoint);
void sendmessage(char string[100]);// utilisÃ© strcpy(string, "Hello, world!\n"); avant d'appeler cette fonction
void sendfloat(float a);





#endif /* CARACTER_H_ */
