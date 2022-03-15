/*
 * irq.h
 *
 *  Created on: 10 mars 2022
 *      Author: LeoJacq
 */

#ifndef INC_IRQ_H_
#define INC_IRQ_H_

#include "main.h"
#include "usart.h"
#include "stm32l1xx_hal_def.h"

/*uart section*/
void set_usar1_irq(uint8_t value);
uint8_t get_usar1_irq();

void set_dma_irq(uint8_t value);
uint8_t get_dma_irq();

void USART1_SendIT(UART_HandleTypeDef *huart);
void USART_Enable_IT(UART_HandleTypeDef *huart);


#endif /* INC_IRQ_H_ */
