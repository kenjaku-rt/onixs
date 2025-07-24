#ifndef LCD2004_H_
#define LCD2004_H_

// Check: https://cdn.soselectronic.com/productdata/7f/6b/5f682254/bc-2004a-bnheh-1.pdf

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "i2c.h"
#include "def_shortcuts.h"

typedef struct {
	volatile uint8_t* bus_port_reg;
	volatile uint8_t* control_port_reg;
	const uint8_t rs_pin, rw_pin, e_pin;
} lcd_2004_t;

static inline void _lcd_2004_cmd(const lcd_2004_t config, uint8_t cmd) {
	MAKE_PORT(config.control_port_reg) &= ~(_BV(config.rs_pin) | _BV(config.rw_pin));
	MAKE_PORT(config.bus_port_reg) = cmd;
	MAKE_PORT(config.control_port_reg) |= _BV(config.e_pin);
	_delay_us(1);
	MAKE_PORT(config.control_port_reg) &= ~(1 << config.e_pin);
	_delay_us(50);
}

static inline void _lcd_2004_data(const lcd_2004_t config, uint8_t data) {
	MAKE_PORT(config.control_port_reg) &= ~(_BV(config.rw_pin));
	MAKE_PORT(config.control_port_reg) |= _BV(config.rs_pin);
	MAKE_PORT(config.bus_port_reg) = data;
	MAKE_PORT(config.control_port_reg) |= _BV(config.e_pin);
	_delay_us(1);
	MAKE_PORT(config.control_port_reg) &= ~_BV(config.e_pin);
	_delay_us(50);
}

static inline void lcd_2004_init(const lcd_2004_t config) {
	MAKE_DDR(config.bus_port_reg)  = 0xFF;
	MAKE_PORT(config.bus_port_reg) = 0x00;
	MAKE_DDR(config.control_port_reg) |= _BV(config.rs_pin) |  _BV(config.rw_pin) | _BV(config.e_pin);
	MAKE_PORT(config.bus_port_reg)  &= ~(_BV(config.rs_pin) |  _BV(config.rw_pin) | _BV(config.e_pin));
	_delay_ms(15);
	
	_lcd_2004_cmd(config, (_BV(4) | _BV(5)));
	_delay_ms(4.1);
	_lcd_2004_cmd(config, (_BV(4) | _BV(5)));
	_delay_us(100);
	_lcd_2004_cmd(config, (_BV(4) | _BV(5)));
	
	_lcd_2004_cmd(config, (_BV(3) | _BV(4) | _BV(5)));	// 8 bit, 2 lines, 5*8 px
	_lcd_2004_cmd(config, (_BV(2) | _BV(3)));			// Display ON, cursor OFF, blink OFF
	_lcd_2004_cmd(config, (_BV(0)));					// Clear display
	_delay_ms(1.55);
	_lcd_2004_cmd(config, (_BV(2) | _BV(1)));			// Entry mode. Move cursor right.
}

static inline void lcd_2004_clear(const lcd_2004_t config) {
	_lcd_2004_cmd(config, (_BV(0)));
	_delay_ms(1.55);
}

static inline void lcd_2004_home(const lcd_2004_t config) {
	_lcd_2004_cmd(config, (_BV(1)));
	_delay_ms(1.55);
}

static inline void lcd_2004_cursor(const lcd_2004_t config, uint8_t line, uint8_t column) {
	static const uint8_t lcd_addr_shift[] = {
		[0] = 0x00, [1] = 0x40,
		[2] = 0x14, [3] = 0x54,
	};
	_lcd_2004_cmd(config, 0x80 | (lcd_addr_shift[line] + column));
	_delay_us(39);
}

static inline void lcd_2004_pstr(const lcd_2004_t config, const char* flash_str) {
	char c;
	while ((c = pgm_read_byte(flash_str++))) {
		_lcd_2004_data(config, c);
	}
}

	static inline void lcd_2004_str(const lcd_2004_t config, const char* str) {
		while (*str != '\0') {
			_lcd_2004_data(config, *str);
			++str;
		}
	}

static inline void lcd_2004_int(const lcd_2004_t config, const int number, const int radix) {
	char buffer[6];
	memset(buffer, '\0', 6 * sizeof(char));
	itoa(number, buffer, radix);
	lcd_2004_str(config, buffer);
}

#include "undef_shortcuts.h"

#endif /* LCD2004_H_ */
