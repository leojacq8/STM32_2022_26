/*
 * irq.c
 *
 *  Created on: 10 mars 2022
 *      Author: LeoJacq
 */

#include "irq.h"
#include "stm32l1xx_hal_def.h"

static uint8_t RX_DMA_irq	= 0;

/*uart section*/
/*acc section*/
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
