/******************************************************************************
* source/display_buffer/display_buffer.c                                      *
* ======================================                                      *
*                                                                             *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/robolib                                   *
******************************************************************************/

//**************************<File version>*************************************
#define SYSTEM_DISPLAY_VERSION \
    "source/display_buffer/display_buffer.c 27.09.2015 V1.0.0"

//**************************<Included files>***********************************
#include "system/display_buffer.h"

#include "system/assembler.h"
#include "system/string.h"

#include <avr/io.h>

//**************************<Variables>****************************************
#define SYSTEM_DISPLAY_BUFFER_SIZE DISPLAY_BUFFER_WIDTH * DISPLAY_BUFFER_HEIGHT
volatile uint8_t system_display_buffer_position;
volatile uint8_t system_display_buffer_buffer[SYSTEM_DISPLAY_BUFFER_SIZE];
volatile uint8_t system_display_buffer_current[SYSTEM_DISPLAY_BUFFER_SIZE];

//**************************<Prototypes>***************************************

//**************************<Files>********************************************

//**************************[display_buffer_print]***************************** 27.09.2015
void display_buffer_print(uint8_t data){

    uint8_t mSREG = SREG;
    uint8_t pos;

    cli();
    pos = system_display_buffer_position;

    if (pos >= SYSTEM_DISPLAY_BUFFER_SIZE) {
        SREG = mSREG;
        return;
    }

    system_display_buffer_buffer[pos] = data;

    if ((pos % DISPLAY_BUFFER_WIDTH) < (DISPLAY_BUFFER_WIDTH - 1)) {
        pos++;
    } else {
        pos = 0xFF;
    }

    system_display_buffer_position = pos;
    SREG = mSREG;
}

//**************************[display_buffer_clear]***************************** 27.09.2015
void display_buffer_clear(void){

    uint8_t mSREG = SREG;
    uint8_t pos;

    cli();

    for (pos = 0; pos < SYSTEM_DISPLAY_BUFFER_SIZE; pos++) {
        system_display_buffer_buffer[pos] = ' ';
    }

    system_display_buffer_position = 0;
    SREG = mSREG;
}

//**************************[display_buffer_gotoxy]**************************** 27.09.2015
void display_buffer_gotoxy(uint8_t x,uint8_t y) {

    if (x >= DISPLAY_BUFFER_WIDTH) {
        system_display_buffer_position = 0xFF;
        return;
    }
    if (y >= DISPLAY_BUFFER_HEIGHT) {
        system_display_buffer_position = 0xFF;
        return;
    }

    system_display_buffer_position = y * DISPLAY_BUFFER_WIDTH + x;
}

//**************************[display_buffer_update]**************************** 27.09.2015
void display_buffer_update(void){

    uint8_t gotoxy = 0xFF;
    uint8_t pos;
    uint8_t data;

    for (pos = 0; pos < SYSTEM_DISPLAY_BUFFER_SIZE; pos++) {
        data = system_display_buffer_buffer[pos];

        if (data == system_display_buffer_current[pos]) {
            gotoxy = 0xFF;
            continue;
        }

        if ((pos % DISPLAY_BUFFER_WIDTH) == 0) {
            display_gotoxy(0, pos / DISPLAY_BUFFER_WIDTH);
        } else if (gotoxy) {
            display_gotoxy(pos % DISPLAY_BUFFER_WIDTH,
              pos / DISPLAY_BUFFER_WIDTH);
        }

        display_print(data);
        system_display_buffer_current[pos] = data;

        gotoxy = 0x00;
    }

    pos = system_display_buffer_position;
    display_gotoxy(pos % DISPLAY_BUFFER_WIDTH, pos / DISPLAY_BUFFER_WIDTH);
}

//**************************[display_buffer_updateall]************************* 27.09.2015
void display_buffer_updateall(void){

    uint8_t pos;
    uint8_t data;

    for (pos = 0; pos < SYSTEM_DISPLAY_BUFFER_SIZE; pos++) {
        data = system_display_buffer_buffer[pos];

        if ((pos % DISPLAY_BUFFER_WIDTH) == 0) {
            display_gotoxy(0, pos / DISPLAY_BUFFER_WIDTH);
        }

        display_print(data);
        system_display_buffer_current[pos] = data;
    }

    pos = system_display_buffer_position;
    display_gotoxy(pos % DISPLAY_BUFFER_WIDTH, pos / DISPLAY_BUFFER_WIDTH);
}

//**************************[system_display_buffer_print]********************** 27.09.2015
void system_display_buffer_print(void (*out)(uint8_t)) {

    uint8_t nr, pos;
    uint8_t data;

    string_from_const(out, "display:"                                  "\r\n");

    // position
    pos = system_display_buffer_position;
    string_from_const_length(out, "  x" , 12);    string_from_const(out, ": ");
    if (pos >= SYSTEM_DISPLAY_BUFFER_SIZE) {
        string_from_const(out, "error");
    } else {
        string_from_uint(out, pos % DISPLAY_BUFFER_WIDTH, 1);
    }
                                                string_from_const(out, "\r\n");

    string_from_const_length(out, "  y" , 12);    string_from_const(out, ": ");
    if (pos >= SYSTEM_DISPLAY_BUFFER_SIZE) {
        string_from_const(out, "error");
    } else {
        string_from_uint(out, pos / DISPLAY_BUFFER_WIDTH, 1);
    }
                                                string_from_const(out, "\r\n");

    // buffer
    for (nr = 0; nr < DISPLAY_BUFFER_HEIGHT; nr++) {
        string_from_const(out, "  buffer[");
        string_from_uint(out, nr, 1);
        string_from_const(out, "] : \"");

        for (pos = 0; pos < DISPLAY_BUFFER_WIDTH; pos++) {
            data = system_display_buffer_buffer[
              pos + nr * DISPLAY_BUFFER_WIDTH];
            if        (data <  32) {
                data = '.';
            } else if (data > 127) {
                data = '?';
            }

            out(data);
        }
                                              string_from_const(out, "\"\r\n");
    }

    // current
    for (nr = 0; nr < DISPLAY_BUFFER_HEIGHT; nr++) {
        string_from_const(out, "  current[");
        string_from_uint(out, nr, 1);
        string_from_const(out, "]: \"");

        for (pos = 0; pos < DISPLAY_BUFFER_WIDTH; pos++) {
            data = system_display_buffer_current[
              pos + nr * DISPLAY_BUFFER_WIDTH];
            if        (data <  32) {
                data = '.';
            } else if (data > 127) {
                data = '?';
            }

            out(data);
        }
                                              string_from_const(out, "\"\r\n");
    }

    string_from_const(out,                                             "\r\n");
}

//**************************[system_display_buffer_print_compiled]************* 27.09.2015
void system_display_buffer_print_compiled(void (*out)(uint8_t)) {

    string_from_const(out, SYSTEM_DISPLAY_VERSION                      "\r\n");

    string_from_const_length(out, "  DISPLAY_BUFFER_HEIGHT", 23);
                                                  string_from_const(out, ": ");
    string_from_macro(out, DISPLAY_BUFFER_HEIGHT);
                                                string_from_const(out, "\r\n");

    string_from_const_length(out, "  DISPLAY_BUFFER_WIDTH" , 23);
                                                  string_from_const(out, ": ");
    string_from_macro(out, DISPLAY_BUFFER_WIDTH );
                                                string_from_const(out, "\r\n");


    string_from_const(out,                                             "\r\n");
}

//**************************[system_display_buffer_init]*********************** 27.09.2015
void system_display_buffer_init(void) {

    uint8_t pos;

    for (pos = 0; pos < SYSTEM_DISPLAY_BUFFER_SIZE; pos++) {
        system_display_buffer_buffer[pos] = ' ';
        system_display_buffer_current[pos] = ' ';
    }
}
