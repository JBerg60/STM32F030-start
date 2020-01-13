/**
 * STM32F030 skeleton
 *
 * John Berg @ netbasenext.nl
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

void send(const char *ptr)
{
    while (*ptr != 0)
    {
        while (!(USART1->ISR & USART_ISR_TXE))
            ;
        USART1->TDR = *ptr++;
    }
}

// delay loop for 8 MHz CPU clock with optimizer enabled
void delay(uint32_t msec)
{
    for (uint32_t j = 0; j < 2000UL * msec; j++)
    {
        __asm("nop");
    }
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
    send("boot!\r\n");

    // Enable interrupt in NVIC
    NVIC_EnableIRQ(USART1_IRQn);

    while (1)
    {
        //Step 3: Reset PA4 low = on
        GPIOA->BSRR |= GPIO_BSRR_BR_4;
        delay(50);

        //Step 4: Set PA4 high
        GPIOA->BSRR |= GPIO_BSRR_BS_4;
        delay(1000);

        send("loop\r\n");
    }

    return 0;
}
