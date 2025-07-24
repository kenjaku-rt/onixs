#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include "bits.h"
#include "def_shortcuts.h"

#define DISPLAY_DATA_BUS_PORT &PORTD
#define DISPLAY_CONTROL_PORT  &PORTB
#define DISPLAY_PIN_EN PB0
#define DISPLAY_PIN_RS PB1
#define DISPLAY_PIN_RW PB2

#define DISPLAY_CMD(cmd);\
	MAKE_PORT(DISPLAY_CONTROL_PORT) &= ~(BV(DISPLAY_PIN_RS)|BV(DISPLAY_PIN_RW));\
	MAKE_PORT(DISPLAY_DATA_BUS_PORT) = (cmd);\
	MAKE_PORT(DISPLAY_CONTROL_PORT) |= BV(DISPLAY_PIN_EN);\
	_delay_us(1);\
	MAKE_PORT(DISPLAY_CONTROL_PORT) &= ~(BV(DISPLAY_PIN_EN));\
	_delay_us(50);

#define DISPLAY_DATA(data);\
MAKE_PORT(DISPLAY_CONTROL_PORT) &= ~(BV(DISPLAY_PIN_RW));\
MAKE_PORT(DISPLAY_CONTROL_PORT) |= (BV(DISPLAY_PIN_RS));\
MAKE_PORT(DISPLAY_DATA_BUS_PORT) = (data);\
MAKE_PORT(DISPLAY_CONTROL_PORT) |= BV(DISPLAY_PIN_EN);\
_delay_us(1);\
MAKE_PORT(DISPLAY_CONTROL_PORT) &= ~(BV(DISPLAY_PIN_EN));\
_delay_us(50);

#define DISPLAY_CHAR(ch) DISPLAY_DATA(ch)

#define DISPLAY_CLEAR(data);\
DISPLAY_CMD(BV(0));\
_delay_ms(1.55);

#define DISPLAY_HOME()\
DISPLAY_CMD(BV(1));\
_delay_ms(1.55);

#define DISPLAY_CURSOR(line, column); {\
	static const uint8_t _addr_shift[] = {\
		[0] = 0x00, [1] = 0x40,\
		[2] = 0x14, [3] = 0x54,\
	};\
	DISPLAY_CMD(0x80 | (_addr_shift[(line)] + column));\
	_delay_us(39);\
} while(0);\


static void display_init(void) {
 	MAKE_DDR(DISPLAY_DATA_BUS_PORT)  = 0xFF;
 	MAKE_PORT(DISPLAY_DATA_BUS_PORT) = 0x00;
	MAKE_DDR(DISPLAY_CONTROL_PORT) |= (BV(DISPLAY_PIN_EN)|BV(DISPLAY_PIN_RS)|BV(DISPLAY_PIN_RW));
	MAKE_PORT(DISPLAY_CONTROL_PORT) &= ~(BV(DISPLAY_PIN_EN)|BV(DISPLAY_PIN_RS)|BV(DISPLAY_PIN_RW));

	DISPLAY_CMD(BV(4)|BV(5));
	_delay_ms(4.1);
	DISPLAY_CMD(BV(4)|BV(5));
	_delay_us(100);
	DISPLAY_CMD(BV(4)|BV(5));

	DISPLAY_CMD(BV(3)|BV(4)|BV(5)); // 8 bit, 2 lines, 5*8 px
	DISPLAY_CMD(BV(2)|BV(3));		// Display ON, cursor OFF, blink OFF
	DISPLAY_CLEAR();				// Clear display.
	DISPLAY_CMD(BV(1)|BV(2));		// Entry mode. Move cursor right.
}

static inline void display_str(const char* str) {
	while (*str != '\0') {
		DISPLAY_DATA(*str);
		++str;
	}
}

static void display_pstr(const char* progmem_str) {
	char c;
	while ((c = pgm_read_byte(progmem_str++))) {
		DISPLAY_DATA(c);
	}
}

static inline void display_int(int number, int radix) {
	char buffer[6] = {0, 0, 0, 0, 0, 0};
	itoa(number, buffer, radix);
	display_str(buffer);
}

#define DISPLAY_STR display_str
#define DISPLAY_PSTR display_pstr
#define DISPLAY_INT(num) display_int(num, 10)

#define pstr_t const char PROGMEM

#endif // DISPLAY_H_
