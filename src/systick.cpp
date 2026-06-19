#include "systick.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t g_millis = 0;

void SysTick_Init(void)
{
    // CTC Mode
  //  TCCR0 = (1 << WGM01);

    // Prescaler = 64
    TCCR0 |= (1 << CS01) | (1 << CS00);

    // 1 ms @ 8 MHz
   // OCR0 = 124;

    // Enable Compare Match Interrupt
    TIMSK |= (1 << TOIE0);
}

ISR(TIMER0_OVF_vect)
{
    g_millis += 2;
}

uint32_t millis(void)
{
    uint32_t time;

    uint8_t sreg = SREG;
    __asm__ __volatile__("cli" ::: "memory");
    time = g_millis;
    SREG = sreg;
    return time;
}