/*
 * STM32F030 skeleton
 *
 * John Berg @ netbasenext.nl
 * 
 * USART1 @ 115200 Full Duplex
 * 1 start - 8-bit - 1 stop
 * TX -> PA9 (AF1)
 * RX -> PA10 (AF1)
 */

#include "stm32f0xx.h"

// Called after each received character
extern "C" void USART1_IRQHandler(void)
{
    // read the received character
    char received = USART1->RDR;

    // send echo back
    while (!(USART1->ISR & USART_ISR_TXE))
        ;
    USART1->TDR = received;
}