/******************************************************************************
* source/tick/tick.c                                                          *
* ==================                                                          *
*                                                                             *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/robolib                                   *
******************************************************************************/

//**************************<File version>*************************************
#define SYSTEM_TICK_VERSION \
    "source/tick/tick.c 27.09.2015 V1.0.0"

//**************************<Included files>***********************************
#include "system/tick.h"

#include "system/string.h"

#include <avr/io.h>

//**************************<Variables>****************************************
volatile uint16_t system_tick_time;

//**************************<Prototypes>***************************************

//**************************<Renaming>*****************************************

//**************************<Files>********************************************

#if    defined (__AVR_ATmega2561__) // switch micro controller
    #include <source/tick/tick_atmega2561.c>
    #define MCU atmega2561
#elif  defined (__AVR_ATmega64__)   // switch micro controller
    #include <source/tick/tick_atmega64.c>
    #define MCU atmega64
#else                               // switch micro controller
    #error "device is not supported (yet)"
#endif                              // switch micro controller


//**************************[tick_delay]*************************************** 27.09.2015
void tick_delay(uint16_t mseconds) {

    uint16_t start;
    mseconds/= 10;

    start = tick_get();

    while (tick_get() - start < mseconds) {
        sei();
    }
}

//**************************[tick_get]***************************************** 27.09.2015
uint16_t tick_get() {

    uint16_t result;
    uint8_t mSREG = SREG;
    cli();
    result = system_tick_time;
    SREG = mSREG;

    return result;
}

//**************************[tick_reset]*************************************** 27.09.2015
void tick_reset() {

    uint8_t mSREG = SREG;
    cli();
    system_tick_time = 0;
    SREG = mSREG;
}


//**************************[tick_tomin]*************************************** 27.09.2015
uint8_t tick_tomin(uint16_t time) {

    return time / (uint16_t) 6000;
}

//**************************[tick_tosec]*************************************** 27.09.2015
uint8_t tick_tosec(uint16_t time) {

    return (time / (uint16_t) 100) % (uint16_t) 60;
}

//**************************[tick_tohsec]************************************** 27.09.2015
uint8_t tick_tohsec(uint16_t time) {

    return time % (uint16_t) 100;
}

//**************************[tick_tohsec]************************************** 27.09.2015
uint16_t tick_tomsec(uint16_t time) {

    return (time % (uint16_t) 100) * (uint16_t) 10;
}

//**************************[system_tick_print]******************************** 27.09.2015
void system_tick_print(void (*out)(uint8_t)) {

    string_from_const(out, "tick:"                                    "\r\n");

    string_from_const_length(out, "  tics" , 15); string_from_const(out, ": ");
    string_from_uint(out, system_tick_time,5); string_from_const(out, "\r\n");

    string_from_const(out,                                             "\r\n");
}

//**************************[system_tick_print_compiled]*********************** 27.09.2015
void system_tick_print_compiled(void (*out)(uint8_t)) {

    string_from_const(out, SYSTEM_TICK_VERSION                        "\r\n");
    string_from_const(out, SYSTEM_TICK_SUB_VERSION                    "\r\n");

    string_from_const_length(out, "  MCU"  , 15); string_from_const(out, ": ");
    string_from_macro(out, MCU);                string_from_const(out, "\r\n");

    string_from_const_length(out, "  F_CPU", 15); string_from_const(out, ": ");
    string_from_macro(out, F_CPU);              string_from_const(out, "\r\n");

    string_from_const_length(out, "  SYSTICK", 15);
                                                  string_from_const(out, ": ");
    string_from_macro(out, TICK_SYSTICK);      string_from_const(out, "\r\n");

    string_from_const(out,                                             "\r\n");
}

//**************************[system_tick_init]*********************************
// siehe controllerspezifische Datei
