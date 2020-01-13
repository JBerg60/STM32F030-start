/**
 * STM32F030 skeleton
 *
 * John Berg @ netbasenext.nl
 */

#include "stm32f0xx.h"

extern void uart_init();
extern void uart_send(const char *ptr);
extern int printf(const char *format, ...);

// delay loop for 8 MHz CPU clock with optimizer enabled
void delay(uint32_t msec)
{
    for (uint32_t j = 0; j < 2000UL * msec; j++)
    {
        __asm("nop");
    }
}

/**
 * redirect printf output to UASRT1
 */
void putchar(int c)
{
    while (!(USART1->ISR & USART_ISR_TXE))
        ;
    USART1->TDR = c;
}

int main(void)
{
    //Step 1: Enable the clock to PORT A
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    //Step 2: Configure PA4 as output
    GPIOA->MODER &= ~GPIO_MODER_MODER4_Msk;
    GPIOA->MODER |= (0x01 << GPIO_MODER_MODER4_Pos);

    // Configure PA4 as Push-Pull output
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;

    // Configure PA4 as High-Speed Output
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR4_Msk;
    GPIOA->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR4_Pos);

    // Disable PA4 Pull-up/Pull-down
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4_Msk;

    uart_init();
    printf("boot!\r\n");

    // Enable interrupt in NVIC
    NVIC_EnableIRQ(USART1_IRQn);

    int cnt = 0;

    while (1)
    {
        //Step 3: Reset PA4 low = on
        GPIOA->BSRR |= GPIO_BSRR_BR_4;
        delay(50);

        //Step 4: Set PA4 high
        GPIOA->BSRR |= GPIO_BSRR_BS_4;
        delay(1000);

        printf("loop [%d]\r\n", cnt++);
    }

    return 0;
}
