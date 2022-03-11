/*
 * irq.c
 *
 *  Created on: 10 mars 2022
 *      Author: LeoJacq
 */

#include "irq.h"

/*uart section*/
void set_usar1_irq(uint8_t value)
{
    uint32_t prim;
    prim = __get_PRIMASK();

    __disable_irq();

	RX_UART1_irq	= value;

    if (!prim) {
          __enable_irq();
    }
}

uint8_t get_usar1_irq()
{
	uint8_t ret = 0;
    uint32_t prim;

    prim = __get_PRIMASK();
    __disable_irq();

	ret	= RX_UART1_irq;


    if (!prim) {
          __enable_irq();
    }

	return ret;
}
/*acc section*/
void set_usar2_irq(uint8_t value)
{
    uint32_t prim;
    prim = __get_PRIMASK();

    __disable_irq();

	RX_UART2_irq	= value;

    if (!prim) {
          __enable_irq();
    }
}

uint8_t get_usar2_irq()
{
	uint8_t ret = 0;
    uint32_t prim;

    prim = __get_PRIMASK();
    __disable_irq();

	ret	= RX_UART2_irq;


    if (!prim) {
          __enable_irq();
    }

	return ret;
}

void USART1_SendIT(UART_HandleTypeDef *huart)
{

    /* Disable the UART Data Register not empty Interrupt */
    __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

    /* Disable the UART Parity Error Interrupt */
    __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

    /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

    /* Rx process is completed, restore huart1.RxState to Ready */
    huart1.RxState = HAL_UART_STATE_READY;

    /*Call legacy weak Rx complete callback*/
    rx_cb_USART_1();
}

void USART1_Enable_IT(UART_HandleTypeDef *huart)
{
    //Reactivate interrupt after callback
    __HAL_LOCK(huart);
    /* Set Reception type to Standard reception */
    huart1.ReceptionType = HAL_UART_RECEPTION_STANDARD;
    huart1.ErrorCode = HAL_UART_ERROR_NONE;
    huart1.RxState = HAL_UART_STATE_BUSY_RX;
    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
}
