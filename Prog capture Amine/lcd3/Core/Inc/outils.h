/*
 * outils.h

 */

#ifndef OUTILS_H_
#define OUTILS_H_


void uart_begin(UART_HandleTypeDef huart);
void print(char *str, ...);
char print_char(uint8_t c);
char print_str(uint8_t *str, uint16_t len);

#endif /* OUTILS_H_ */
