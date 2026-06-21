//******************************************************************************************************
//
// File Name    : Firmware AVR
// Title        : Firmware interface header file
// software     : version 1.0
// Target MCU   : AVR Series
// Created:     : 07/06/2026
// Author:      : Yaser Rashnabadi
//
//******************************************************************************************************
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "systick.h"
#include "Button.h"

#define BUTTON_DELAY_ONCE 0
#define BUTTON_DELAY_REPEAT 1
#define BUTTON_RELEASE_DELAY_ONCE 2
#define BUTTON_RELEASE_DELAY_REPEAT 3
#define Jamper_REPEAT 7
//=================================================================================================
// Auxiliary functions
//=================================================================================================

bool task_DelayOnce(BUTTON &button)
{
    PORTA++;
    return true;
}

bool task_DelayeRepeat(BUTTON &button)
{
    PORTA++;
    return true;
}

bool task_ReleaseDelayOnce(BUTTON &button)
{
    PORTB++;
    return true;
}

bool task_ReleaseDelayeRepeat(BUTTON &button)
{
    PORTB++;
    return true;
}

bool task_JamperDelayeRepeat(BUTTON &button)
{
    PORTD++;
    return true;
}

int main()
{
    PORTA = 0x00;
    DDRA = 0xFF;

    PORTB = 0x00;
    DDRB = 0xFF;

    PORTC = 0xFF;
    DDRC = 0x00;

    PORTD = 0x00;
    DDRD = 0xFF;

    BUTTON button1(PORTC, DDRC, PINC, BUTTON_DELAY_ONCE, BUTTON_MODE_PULL_UP, 10, 3000);
    BUTTON button2(PORTC, DDRC, PINC, BUTTON_DELAY_REPEAT, BUTTON_MODE_PULL_UP, 10, 500);

    button1.bind(BUTTON_MODE_DELAY_ONCE, task_DelayOnce);
    button2.bind(BUTTON_MODE_DELAY_REPEAT, task_DelayeRepeat);

    BUTTON button3(PORTC, DDRC, PINC, BUTTON_RELEASE_DELAY_ONCE, BUTTON_MODE_PULL_UP, 10, 1000);
    BUTTON button4(PORTC, DDRC, PINC, BUTTON_RELEASE_DELAY_REPEAT, BUTTON_MODE_PULL_UP, 10, 500);

    button3.bind(BUTTON_MODE_RELEASE_DELAY_ONCE, task_ReleaseDelayOnce);
    button4.bind(BUTTON_MODE_RELEASE_DELAY_REPEAT, task_ReleaseDelayeRepeat);

    BUTTON button5(PORTC, DDRC, PINC, Jamper_REPEAT, BUTTON_MODE_PULL_UP, 10, 0);
    button5.bind(BUTTON_MODE_RAW, task_JamperDelayeRepeat);

    SysTick_Init();
    sei();

    while (1)
    {
        button1.dispatch(millis());
        button2.dispatch(millis());

        button3.dispatch(millis());
        button4.dispatch(millis());

        button5.dispatch(millis());
    }
}
