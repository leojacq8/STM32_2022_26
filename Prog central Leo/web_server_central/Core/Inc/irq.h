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

/*dma section*/
void set_dma_irq(uint8_t value);
uint8_t get_dma_irq();

/*uart1 section*/
void set_uart1_irq(uint8_t value);
uint8_t get_uart1_irq();

#endif /* INC_IRQ_H_ */
