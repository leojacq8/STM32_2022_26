/*
 * irq.c
 *
 *  Created on: 10 mars 2022
 *      Author: LeoJacq
 */

#include "irq.h"
#include "stm32l1xx_hal_def.h"

static uint8_t RX_DMA_irq	= 0;
static uint8_t UART1_irq	= 0;

/*dma section*/
void set_dma_irq(uint8_t value)
{
    uint32_t prim;
    prim = __get_PRIMASK();

    __disable_irq();

	RX_DMA_irq	= value;

    if (!prim) {
          __enable_irq();
    }
}

uint8_t get_dma_irq()
{
	uint8_t ret = 0;
    uint32_t prim;

    prim = __get_PRIMASK();
    __disable_irq();

	ret	= RX_DMA_irq;


    if (!prim) {
          __enable_irq();
    }

	return ret;
}

/*uart1 section*/
void set_uart1_irq(uint8_t value)
{
    uint32_t prim;
    prim = __get_PRIMASK();

    __disable_irq();

    UART1_irq	= value;

    if (!prim) {
          __enable_irq();
    }
}

uint8_t get_uart1_irq()
{
	uint8_t ret = 0;
    uint32_t prim;

    prim = __get_PRIMASK();
    __disable_irq();

	ret	= UART1_irq;


    if (!prim) {
          __enable_irq();
    }

	return ret;
}

