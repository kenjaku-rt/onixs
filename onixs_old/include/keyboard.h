#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <avr/io.h>

#include "def_shortcuts.h"

typedef enum {
	KEY_D,
	KEY_HASH,
	KEY_0,
	KEY_MUL,
	KEY_C,
	KEY_9,
	KEY_8,
	KEY_7,
	KEY_B,
	KEY_6,
	KEY_5,
	KEY_4,
	KEY_A,
	KEY_3,
	KEY_2,
	KEY_1,
	KEY_NO_TOUCH = 0xFF,
} keyboard_state_t;

static volatile uint8_t* keyboard_port;

void keyboard_timer_interrupt_enable(volatile uint8_t* port) {
	keyboard_port = port;
	MAKE_DDR(keyboard_port) = 0x00;
	TCCR0 |= _BV(WGM01) | _BV(CS02) | _BV(CS00);
	TIMSK |= _BV(OCIE0);
	OCR0 = 244; // for delay 125 ms
	
	MAKE_DDR(keyboard_port) |= 0x0F;
	MAKE_PORT(keyboard_port) |= 0x0F;
	
	MAKE_DDR(keyboard_port) &= ~0xF0;
	MAKE_PORT(keyboard_port) |= 0xF0;
}

uint8_t keyboard_scan(void) {
	for (uint8_t row = 0; row < 4; row++) {
		MAKE_PORT(keyboard_port) = ((MAKE_PORT(keyboard_port) & ~0x0F) | (1 << row)) ^ 0x0F;
		_delay_us(5);
		uint8_t cols = (MAKE_PIN(keyboard_port) & 0xF0) >> 4;
		if (cols != 0x0F) {
			uint8_t mask = ~cols & 0x0F;
			uint8_t col = __builtin_ctz(mask);
			return (row << 2) | col;
		}
	}
	return 0xFF;
}
#define WAIT_KEY() while (keyboard_scan() == KEY_NO_TOUCH);
#define WAIT_KEY_INVERSE()  while (keyboard_scan() != KEY_NO_TOUCH);

char keyboard_key_to_digit(uint8_t code) {
	switch (code) {
		case  KEY_1: return '1';
		case  KEY_2: return '2';
		case  KEY_3: return '3';
		case  KEY_4: return '4';
		case  KEY_5: return '5';
		case  KEY_6: return '6';
		case  KEY_7: return '7';
		case  KEY_8: return '8';
		case  KEY_9: return '9';
		case  KEY_0: return '0';
		default: return '\0';
	}
}

#include "undef_shortcuts.h"

#endif /* KEYBOARD_H_ */
