
//******************************************************************************************************
//
// File Name    : Firmware AVR
// Title        : Firmware interface header file
// software     : version 2.0
// Target MCU   : AVR Series
// Created:     : 07/06/2026
// Author:      : Yaser Rashnabadi
//
//******************************************************************************************************
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Interface Header Library use Firmware <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "systick.h"
#include "Button.h"
//======================================================================================================
// --------------------------------------- Global Setting ----------------------------------------------
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

static uint32_t Code;
static uint32_t MirrorCode;
static uint8_t OutPut;

//======================================================================================================
// ----------------------------------------- Macro Setting ---------------------------------------------
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#define SAVE1 6
#define SAVE2 7
#define SAVE3 0
#define SAVE4 1
//----------------------------------------------------
#define DELETE1 4
#define DELETE2 3
#define DELETE3 1
#define DELETE4 0
//----------------------------------------------------
#define JUMPER1 5
#define JUMPER2 4
#define JUMPER3 3
#define JUMPER4 2
//----------------------------------------------------
#define LED1 3
#define RELEY2 2
#define RELEY3 1
#define RELEY4 0
//=============================================================================
//----------------------------Select PORT & PIN -------------------------------

#define PORT_Decoder PORTD
#define DDR__Decoder DDRD
#define RXD 2 // Pin 2

#define RESTART_STATE 0x89ABCDEF
//==================================================================================================
// ----------------------------------------------------------------------- Auxiliary functions Macro
//==================================================================================================

#define LED__ON(_byte_, _bit_) ((_byte_) |= (1UL << (_bit_)))
#define LED_OFF(_byte_, _bit_) ((_byte_) &= ~(1UL << (_bit_)))

//==================================================================================================
// ----------------------------------------------------------------------------- Auxiliary functions
//==================================================================================================
//----------------- Save functions - Turn ON relays for Save operation -----------------------------

bool task_LedOn(BUTTON &button)
{
    PORTC++;
    // uint8_t save = 0;
    // Reley__ON(save, RELEY1);
    // table.Field.ID = id++;
    // table.Field.Address = ev1527.data.Frame;
    // table.Field.Switch = (save |= 0xF0);
    // database.InsertRecord(table);
    // PORTC = OutPut | save;
    return true; //<-- Return true for execute handler only once after button state changed
}

bool task_LedOff(BUTTON &button)
{
    uint8_t save = 0;
    LED__ON(save, RELEY2);

    PORTC = OutPut | save;
    return true;
}

bool task_save3(BUTTON &button)
{
    uint8_t save = 0;
    LED__ON(save, RELEY3);

    PORTC = OutPut | save;
    return true;
}

bool task_save4(BUTTON &button)
{
    uint8_t save = 0;
    LED__ON(save, RELEY4);

    PORTC = OutPut | save;
    return true;
}
//==================================================================================================
//----------------- Delete functions - Turn OFF relays for Delete operation ------------------------

bool task_delete1(BUTTON &button)
{
    uint8_t _delete = 0xFF;

    LED_OFF(_delete, LED1);

    LED_OFF(OutPut, LED1);
    PORTC = OutPut;
    return true;
}

bool task_delete2(BUTTON &button)
{
    uint8_t _delete = 0xFF;

    LED_OFF(_delete, RELEY2);

    LED_OFF(OutPut, RELEY2);
    PORTC = OutPut;
    return true;
}

bool task_delete3(BUTTON &button)
{
    uint8_t _delete = 0xFF;

    LED_OFF(_delete, RELEY3);

    LED_OFF(OutPut, RELEY3);
    PORTC = OutPut;
    return true;
}

bool task_delete4(BUTTON &button)
{
    uint8_t _delete = 0xFF;

    LED_OFF(_delete, RELEY4);

    LED_OFF(OutPut, RELEY4);
    PORTC = OutPut;
    return true;
}
//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position -------------------

bool task_Jumper1(BUTTON &button)
{
    // PORTC++;
    // Reley_OFF(OutPut, RELEY1);
    // PORTC = OutPut;
    return true;
}

bool task_Jumper2(BUTTON &button)
{
    LED_OFF(OutPut, RELEY2);
    PORTC = OutPut;
    return true;
}

bool task_Jumper3(BUTTON &button)
{
    LED_OFF(OutPut, RELEY3);
    PORTC = OutPut;
    return true;
}

bool task_Jumper4(BUTTON &button)
{
    LED_OFF(OutPut, RELEY4);
    PORTC = OutPut;
    return true;
}
//==================================================================================================
// -------------------------------------------- Setup ---------------------------------------------
//==================================================================================================

int main()
{

    PORTA = 0xFF;
    DDRA = 0x00;

    PORTB = 0xFF;
    DDRB = 0x00;

    PORTC = (uint8_t)(0xFF & ~((1 << LED1) | (1 << RELEY2) | (1 << RELEY3) | (1 << RELEY4)));
    DDRC = (uint8_t)(0x00 | ((1 << LED1) | (1 << RELEY2) | (1 << RELEY3) | (1 << RELEY4)));

    PORTD = 0xFF;
    DDRD = 0x00;

    ACSR = 0x80;
    SFIOR = 0x00;

    BUTTON Save1(PORTD, DDRD, PIND, SAVE1, BUTTON_MODE_PULL_UP, 10, 10); //<-- Save1 connected to GND
    BUTTON Save2(PORTD, DDRD, PIND, SAVE2, BUTTON_MODE_PULL_UP, 10, 10);
    BUTTON Save3(PORTB, DDRB, PINB, SAVE3, BUTTON_MODE_PULL_UP, 10, 10);
    BUTTON Save4(PORTB, DDRB, PINB, SAVE4, BUTTON_MODE_PULL_UP, 10, 10);

    BUTTON Delete1(PORTD, DDRD, PIND, DELETE1, BUTTON_MODE_PULL_UP, 1, 10); //<-- Delete1 connected to GND
    BUTTON Delete2(PORTD, DDRD, PIND, DELETE2, BUTTON_MODE_PULL_UP, 1, 10);
    BUTTON Delete3(PORTD, DDRD, PIND, DELETE3, BUTTON_MODE_PULL_UP, 1, 10);
    BUTTON Delete4(PORTD, DDRD, PIND, DELETE4, BUTTON_MODE_PULL_UP, 1, 10);

    BUTTON Jumper1(PORTB, DDRB, PINB, JUMPER1, BUTTON_MODE_PULL_UP, 1000, 10); //<-- Jumper connected to GND
    BUTTON Jumper2(PORTB, DDRB, PINB, JUMPER2, BUTTON_MODE_PULL_UP, 10, 10);
    BUTTON Jumper3(PORTB, DDRB, PINB, JUMPER3, BUTTON_MODE_PULL_UP, 10, 10);
    BUTTON Jumper4(PORTB, DDRB, PINB, JUMPER4, BUTTON_MODE_PULL_UP, 10, 10);

    Save1.bind(BUTTON_MODE_DELAY_ONCE, task_LedOn);
    Save2.bind(BUTTON_MODE_DELAY_ONCE, task_LedOff);
    Save3.bind(BUTTON_MODE_DELAY_ONCE, task_save3);
    Save4.bind(BUTTON_MODE_DELAY_ONCE, task_save4);

    Delete1.bind(BUTTON_MODE_DELAY_ONCE, task_delete1);
    Delete2.bind(BUTTON_MODE_DELAY_ONCE, task_delete2);
    Delete3.bind(BUTTON_MODE_DELAY_ONCE, task_delete3);
    Delete4.bind(BUTTON_MODE_DELAY_ONCE, task_delete4);

    Jumper1.bind(BUTTON_MODE_RAW, task_Jumper1);
    Jumper2.bind(BUTTON_MODE_RAW, task_Jumper2);
    Jumper3.bind(BUTTON_MODE_RAW, task_Jumper3);
    Jumper4.bind(BUTTON_MODE_RAW, task_Jumper4);

    SysTick_Init();
    sei();
    //==================================================================================================
    //--------------------------------------------- Loop -----------------------------------------------
    //==================================================================================================
    while (1)
    {

        Save1.dispatch(millis());
        Save2.dispatch(millis());
        Save3.dispatch(millis());
        Save4.dispatch(millis());

        Jumper1.dispatch(millis());
        Jumper2.dispatch(millis());
        Jumper3.dispatch(millis());
        Jumper4.dispatch(millis());

        // Delete1.dispatch(millis());
        // Delete2.dispatch(millis());
        // Delete3.dispatch(millis());
        // Delete4.dispatch(millis());
    }
}
