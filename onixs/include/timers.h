#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "relay.h"
#include "display.h"
#include "rtc.h"
#include "thermo.h"
#include "memory.h"
#include "interface.h"

static uint8_t timer_flag = 0x00;

#define FLAG_EIGHTH_BIT   0
#define FLAG_QUART_BIT    1
#define FLAG_ONE_SEC_BIT  2
#define FLAG_TEN_MINUTES  3

#define GLOBAL_INTERRUPT_ENABLE() sei()
#define GLOBAL_INTERRUPT_DISABLE() cli()

// 125 ms
#define TIMER0_INIT();\
TCCR0 |= BV(WGM01) | BV(CS02) | BV(CS00);\
TIMSK |= BV(OCIE0);\
OCR0 = 244;

// A: 0.25s
#define TIMER1_INIT();\
TCCR1B |= BV(WGM12) | BV(CS10) | BV(CS12);\
TIMSK |= BV(OCIE1A) | BV(OCIE1B);\
OCR1A = 488;


#define TIMERS_ENABLE(); TIMER0_INIT();\
TIMER1_INIT();\
GLOBAL_INTERRUPT_ENABLE();

ISR(TIMER0_COMP_vect) {
	timer_flag |= BV(FLAG_EIGHTH_BIT);
}

#define TEN_MINUTES_IN_SECS 600
ISR(TIMER1_COMPA_vect) {
	static uint16_t tim1_cnt_sec   = 0;
	static uint16_t tim1_cnt_10min = 0;

	timer_flag |= BV(FLAG_QUART_BIT);
	if (tim1_cnt_10min >= 4 * TEN_MINUTES_IN_SECS) {
		timer_flag |= BV(FLAG_TEN_MINUTES);
		tim1_cnt_10min = 0;
	}
	if (tim1_cnt_sec >= 4) {
		timer_flag |= BV(FLAG_ONE_SEC_BIT);
		tim1_cnt_sec = 0;
	}
	tim1_cnt_sec++;
	tim1_cnt_10min++;
}

#define CATCH_FLAG(flag_bit)\
((timer_flag & BV(flag_bit))\
? (timer_flag &= ~BV(flag_bit), 1)\
: 0)

#endif
