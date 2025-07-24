#ifndef TIMER_ISR_H_
#define TIMER_ISR_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#include "max6675.h"
#include "ds1307.h"
#include "keyboard.h"

static inline void global_interrupt_enable(void) {sei();}

// 125 ms
ISR(TIMER0_COMP_vect) {
	//keyboard_state = keyboard_scan();
}

// TIMER1 in main.c

// 125 ms / aka 250 ms.
ISR(TIMER2_COMP_vect) {
	static bool max6675_to_scan = false;
	if (max6675_to_scan) {
		max6675_update();
	}
	max6675_to_scan = !max6675_to_scan;
}

#endif /* TIMER_ISR_H_ */
