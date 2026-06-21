//****************************************************************************************************************************
//
// File Name		: systick.c
// Title		    : systick C Code file
// software         : version 1.0
// Target MCU		: Atmel AVR Series
// Created:         : 21/06/2026
// Author:          : Yaser Rashnabadi
//
//****************************************************************************************************************************
#include "systick.h"

#ifndef F_CPU
#error "F_CPU is not defined!"
#endif

volatile uint32_t g_millis = 0;
//****************************************************************************************************************************
//------------------------------------------------------------------------------------ Select the best prescaler automatically
//****************************************************************************************************************************

#if ((F_CPU / 1000UL / 64UL) <= 256UL)

#define SYSTICK_PRESCALER 64UL
#define SYSTICK_OCR ((F_CPU / (64UL * 1000UL)) - 1UL)

#if defined(TCCR0B)
#define SYSTICK_CS_BITS ((1 << CS01) | (1 << CS00))
#else
#define SYSTICK_CS_BITS ((1 << CS01) | (1 << CS00))
#endif

#elif ((F_CPU / 1000UL / 256UL) <= 256UL)

#define SYSTICK_PRESCALER 256UL
#define SYSTICK_OCR ((F_CPU / (256UL * 1000UL)) - 1UL)

#if defined(TCCR0B)
#define SYSTICK_CS_BITS (1 << CS02)
#else
#define SYSTICK_CS_BITS (1 << CS02)
#endif

#elif ((F_CPU / 1000UL / 1024UL) <= 256UL)

#define SYSTICK_PRESCALER 1024UL
#define SYSTICK_OCR ((F_CPU / (1024UL * 1000UL)) - 1UL)

#if defined(TCCR0B)
#define SYSTICK_CS_BITS ((1 << CS02) | (1 << CS00))
#else
#define SYSTICK_CS_BITS ((1 << CS02) | (1 << CS00))
#endif

#else
#error "Cannot generate 1 ms tick with Timer0"
#endif
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void SysTick_Init(void)
{
#if defined(TCCR0A) && defined(TCCR0B)
    /* ATmega48/88/168/328P */
    TCCR0A = (1 << WGM01);
    TCCR0B = SYSTICK_CS_BITS;
    OCR0A = (uint8_t)SYSTICK_OCR;
    TIMSK0 |= (1 << OCIE0A);
#else
    /* ATmega8/16/32 */
    TCCR0 = (1 << WGM01) | SYSTICK_CS_BITS;
    OCR0 = (uint8_t)SYSTICK_OCR;
    TIMSK |= (1 << OCIE0);
#endif
}

#if defined(TIMER0_COMPA_vect)
ISR(TIMER0_COMPA_vect)
{
    g_millis++;
}
#else
ISR(TIMER0_COMP_vect)
{
    g_millis++;
}
#endif
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
uint32_t millis(void)
{
    uint32_t time;
    uint8_t sreg = SREG;

    __asm__ __volatile__("cli" ::: "memory");
    time = g_millis;
    SREG = sreg;
    return time;
}
//****************************************************************************************************************************
//----------------------------------------------------------  END  -----------------------------------------------------------
//****************************************************************************************************************************


