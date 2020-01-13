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

void uart_init()
{
    // Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Configure PA9 and PA10 as Alternate function
    GPIOA->MODER &= ~(GPIO_MODER_MODER9_Msk | GPIO_MODER_MODER10_Msk);
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODER9_Pos) | (0x02 << GPIO_MODER_MODER10_Pos);

    // Set PA9 and PA10 to AF1 (USART1)
    GPIOA->AFR[1] &= ~(0x00000FF0);
    GPIOA->AFR[1] |= (0x00000110);

    // Enable USART1 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Clear USART1 configuration (reset state)
    // 8-bit, 1 start, 1 stop, CTS/RTS disabled
    USART1->CR1 = 0x00000000;
    USART1->CR2 = 0x00000000;
    USART1->CR3 = 0x00000000;

    // Select PCLK (APB1) as clock source
    // PCLK on boot 8 MHz
    RCC->CFGR3 &= ~RCC_CFGR3_USART1SW_Msk;

    // Baud Rate = 115200
    // 8.000.000 / 115.200 = 69.44 * 2 = 138.83
    USART1->CR1 |= USART_CR1_OVER8;
    USART1->BRR = 139;

    // Enable both Transmitter and Receiver
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // Enable USART1
    USART1->CR1 |= USART_CR1_UE;

    // Enable receive-interrupt of USART1
    USART1->CR1 |= USART_CR1_RXNEIE;
}

void uart_send(const char *ptr)
{
    while (*ptr != 0)
    {
        while (!(USART1->ISR & USART_ISR_TXE))
            ;
        USART1->TDR = *ptr++;
    }
}

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