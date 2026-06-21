#ifndef SYSTICK_H_
#define SYSTICK_H_

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void SysTick_Init(void);
uint32_t millis(void);

#endif
