/******************************************************************************
* source/adc/adc.c                                                            *
* ================                                                            *
*                                                                             *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/robolib                                   *
******************************************************************************/

//**************************<File version>*************************************
#define SYSTEM_ADC_VERSION \
    "source/adc/adc.c 23.09.2014 V1.0.0"

//**************************<Included files>***********************************
#include "system/adc.h"

#include "system/string.h"

#include <avr/io.h>

//**************************<Variables>****************************************
volatile uint16_t system_adc_values[8];

//**************************<Prototypes>***************************************

//**************************<Renaming>*****************************************

//**************************<Files>********************************************
#if    defined (__AVR_ATmega2561__) // switch micro controller
#  include <source/adc/adc_atmega2561.c>
#  define MCU atmega2561
#elif  defined (__AVR_ATmega64__)   // switch micro controller
#  include <source/adc/adc_atmega64.c>
#  define MCU atmega64
#else                               // switch micro controller
#  error "device is not supported (yet)"
#endif                              // switch micro controller

//**************************[adc_get]****************************************** 17.09.2014
uint16_t adc_get(uint8_t nr) {
  uint16_t result;

  if (system_adc_wait() == 0x00) {
    // system_adc_wait aktiviert Interrupts
    return 0xFFFF;
  }
  // system_adc_wait deaktiviert Interrupts

  system_adc_savevalue();
  system_adc_setchannel(nr);

  if (system_adc_wait() == 0x00) {
    // system_adc_wait aktiviert Interrupts
    return 0xFFFF;
  }
  // system_adc_wait deaktiviert Interrupts
  if (system_adc_savevalue() == nr) {
    result = ADC;
  } else if (nr < 8) {
    result = system_adc_values[nr];
  } else {
    result = 0xFFFF;
  }

  sei();
  return result;
}

//**************************[adc_buffer_get]*********************************** 17.09.2014
uint16_t adc_buffer_get(uint8_t nr) {

  if (nr > 7) {
    return 65535;
  }

  uint16_t result;
  uint8_t mSREG = SREG;

  cli();
  result = system_adc_values[nr];
  SREG = mSREG;

  return result;
}

//**************************[adc_enable]*************************************** 06.09.2014
void adc_enable(void) {
  uint8_t mSREG = SREG;
  cli();

  if ((ADCSRA & _BV(ADEN)) == 0x00) {
    ADCSRA|= _BV(ADEN);
  }

  SREG = mSREG;
}

//**************************[adc_disable]************************************** 06.09.2014
void adc_disable(void) {
  uint8_t mSREG = SREG;
  cli();

  ADCSRA&= ~_BV(ADEN);

  SREG = mSREG;
}

//**************************[adc_is_enabled]*********************************** 11.09.2014
uint8_t adc_is_enabled(void) {
  if (ADCSRA & _BV(ADEN)) {
    return 0xFF;
  } else {
    return 0x00;
  }
}

//**************************[system_adc_print]********************************* 16.09.2014
void system_adc_print(void (*out)(uint8_t)) {
  string_from_const(out, "adc:"                                        "\r\n");

  string_from_const_length(out, "  enabled", 15); string_from_const(out, ": ");
  string_from_bool(out, adc_is_enabled());      string_from_const(out, "\r\n");

  uint8_t nr;
  for (nr = 0; nr < 8; nr++) {
    string_from_const(out, "  channel["); out(nr + 48);
    string_from_const_length(out, "]", 15 - 11);
    string_from_const(out, ": ");
    string_from_uint(out, system_adc_values[nr], 4);
    string_from_const(out, "\r\n");
  }

  string_from_const(out,                                               "\r\n");
}

//**************************[system_adc_print_compiled]************************ 16.09.2014
void system_adc_print_compiled(void (*out)(uint8_t)) {
  string_from_const(out, SYSTEM_ADC_VERSION                            "\r\n");
  string_from_const(out, SYSTEM_ADC_SUB_VERSION                        "\r\n");

  string_from_const_length(out, "  MCU"    , 15); string_from_const(out, ": ");
  string_from_macro(out, MCU);                  string_from_const(out, "\r\n");

  string_from_const(out,                                               "\r\n");
}

//**************************[system_adc_init]**********************************
// siehe controllerspezifische Datei

//**************************[system_adc_adcok]********************************* 16.09.2014
uint8_t system_adc_adcok(void) {
  // adc disabled ?
  if (adc_is_enabled() == 0x00) { return 0x00;}
  // conversation in progress ?
  if (ADCSRA & _BV(ADSC)) { return 0x00;}

  return 0xFF;
}

//**************************[system_adc_getchannel]****************************
// siehe controllerspezifische Datei

//**************************[system_adc_setchannel]****************************
// siehe controllerspezifische Datei

//**************************[system_adc_savevalue]***************************** 16.09.2014
uint8_t system_adc_savevalue(void) {
  uint8_t mSREG = SREG;
  uint8_t temp;

  cli();
  if (system_adc_adcok() == 0x00) {
    SREG = mSREG;
    return 0xFF;
  }

  temp = system_adc_getchannel();

  if (temp <= 7) {
    system_adc_values[temp] = ADC;
  }
  SREG = mSREG;

  return temp;
}

//**************************[system_adc_wait]********************************** 16.09.2014
// Achtung: bei positiven Ausgang bleiben die Interrupts deaktiviert!
uint8_t system_adc_wait(void) {
  uint8_t mSREG = SREG;

  while (1) {
    cli();
    if (system_adc_adcok()) { return 0xFF;}
    SREG = mSREG;

    if (adc_is_enabled() == 0x00) {
      return 0x00;
    }
  }
}

