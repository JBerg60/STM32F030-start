/**
 * STM32F030 skeleton
 *
 * John Berg @ netbasenext.nl
 */

#include "device.hpp"

// delay loop for 8 MHz CPU clock with optimizer enabled
void delay(uint32_t msec)
{
    for (uint32_t j = 0; j < 2000UL * msec; j++)
    {
        __asm("nop");
    }
}

enum class PinMode 
{
    input,
    output,
    alternate,
    analog
};

struct Pin
{
    GPIO &gpio;
    const uint32_t pin; 
    const uint32_t mask{static_cast<uint32_t>(1 << pin)};
    const uint32_t pin2bit;
    const uint32_t mask2bit{static_cast<uint32_t>(0b11 << pin2bit)};

    Pin(GPIO &gpio, const uint32_t pin) : gpio(gpio), pin(pin), pin2bit(pin * 2) { }
    void setmode(PinMode mode) { gpio.MODER = (gpio.MODER & ~mask2bit) | static_cast<uint32_t>(mode) << pin2bit;}
    void on() { gpio.BSRR = mask; }
    void off() { gpio.BSRR = mask << 16; }
    void toggle() { gpio.ODR ^= (1 << pin); }
};

struct PA4 : public Pin
{
    PA4() : Pin(*new GPIOA, 4) {}
};


int main(void)
{
    RCC &rcc = *new RCC;

    //Step 1: Enable the clock to PORT A
    rcc.AHBENR.IOPAEN = 1;

    //Step 2: Configure PA4 as output
    PA4 led;
    led.setmode(PinMode::output);

    // Configure PA4 as Push-Pull output
    //GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;

    // Configure PA4 as High-Speed Output
    //GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR4_Msk;
    //GPIOA->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR4_Pos);

    // Disable PA4 Pull-up/Pull-down
    //GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4_Msk;

    while (1)
    {
        //Step 3: Reset PA4 low = on
        led.off();
        delay(50);

        //Step 4: Set PA4 high
        led.on();
        delay(500);
    }

    return 0;
}
