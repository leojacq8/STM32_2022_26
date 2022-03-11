#include <stdio.h>
#include <stdarg.h>

#include "stm32l1xx_hal.h"
#include "outils.h"
#include "usart.h"

#define TIMEOUT 5000
UART_HandleTypeDef huart1;
void uart_begin(UART_HandleTypeDef huart)
{
	huart1 = huart;
}
void print(char *str, ...)
{
	uint16_t i = 0;
	uint16_t len = 0;
	unsigned char str_var[20];


	int entier;
	unsigned int u_entier;
	unsigned long long_u;
	double reel;
	float floatant;
	char caractere;



	va_list liste;
	va_start(liste, str);
	i = 0;
	while(str[i] != '\0')
	{
		if(str[i] == '%')
		{
			len = 0;
			i++;
			switch(str[i])
			{
				case 'D'	:
				case 'd'	:	
								entier = va_arg(liste, int);
								len = sprintf((char *)str_var, "%d", entier);
								break;
				case 'U'	:
				case 'u'	:	
								u_entier = va_arg(liste, unsigned int);
								len = sprintf((char *)str_var, "%u", u_entier);
								break;
				case 'L'	:
				case 'l'	:	i++;
								switch(str[i++])
								{
									case 'U'	:
									case 'u'	:	
													long_u = va_arg(liste, unsigned long);
													len = sprintf((char *)str_var, "%lu", long_u);
													break;
									case 'F'	:
									case 'f'	:	
													reel = va_arg(liste, double);
													len = sprintf((char *)str_var, "%f", reel);
													break;
									case 'x'	:	
									case 'X'	:	
													long_u = va_arg(liste, unsigned long);
													len = sprintf((char *)str_var, "%lX", long_u);
													break;
									default		: 	i--;
								}
				case 'x'	:
				case 'X'	:	
								u_entier = va_arg(liste, unsigned int);
								len = sprintf((char *)str_var, "%X", u_entier);
								break;
				case 'c'	:
				case 'C'	:	
								caractere = va_arg(liste, int);
								len = sprintf((char *)str_var, "%c", caractere);
								break;
				case 'f'	:
				case 'F'	:	floatant = va_arg(liste, double);
								len = sprintf((char *)str_var, "%f", floatant);
								break;
				default		:	i--;
			}
			if(len != 0)
				print_str(str_var, len);
			else
				print_char((unsigned char)str[i]);
		}
		else
		{
			print_char((unsigned char)str[i]);
		}
		i++;
	}
	va_end(liste);
}
char print_char(uint8_t c)
{
	HAL_StatusTypeDef HAL_StatusUart;
	HAL_StatusUart = HAL_UART_Transmit(&huart2, &c, 1, TIMEOUT);
	if(HAL_StatusUart != HAL_OK)
		return -1;
	else
		return 0;
}
char print_str(uint8_t *str, uint16_t len)
{
	HAL_StatusTypeDef HAL_StatusUart;
	HAL_StatusUart = HAL_UART_Transmit(&huart2, str, len, TIMEOUT);
	if(HAL_StatusUart != HAL_OK)
		return -1;
	else
		return 0;
}
