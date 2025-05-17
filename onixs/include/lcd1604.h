#ifndef LCD1604_H_
#define LCD1604_H_

// Check: https://cdn.soselectronic.com/productdata/7f/6b/5f682254/bc-1604a-bnheh-1.pdf

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>
#include <util/delay.h>

#include <i2c.h>

typedef struct {
	volatile uint8_t* bus_port_reg;
	volatile uint8_t* control_port_reg;
	const uint8_t rs_pin, rw_pin, e_pin;
} lcd_1604_t;

#define MAKE_DDR(port_addr) (*(volatile uint8_t*)((port_addr - 1)))
#define MAKE_PORT(port_addr) (*(volatile uint8_t*)((port_addr)))
#define MAKE_PIN(port_addr) (*(volatile uint8_t*)((port_addr - 2)))


static inline void _lcd_1604_cmd(const lcd_1604_t config, uint8_t cmd) {
	MAKE_PORT(config.control_port_reg) &= ~(_BV(config.rs_pin) | _BV(config.rw_pin));
	MAKE_PORT(config.bus_port_reg) = cmd;
	MAKE_PORT(config.control_port_reg) |= _BV(config.e_pin);
	_delay_us(1);
	MAKE_PORT(config.control_port_reg) &= ~(1 << config.e_pin);
	_delay_us(50);
}

static inline void _lcd_1604_data(const lcd_1604_t config, uint8_t data) {
	MAKE_PORT(config.control_port_reg) &= ~(_BV(config.rw_pin));
	MAKE_PORT(config.control_port_reg) |= _BV(config.rs_pin);
	MAKE_PORT(config.bus_port_reg) = data;
	
	MAKE_PORT(config.control_port_reg) |= _BV(config.e_pin);
	_delay_us(1);
	MAKE_PORT(config.control_port_reg) &= ~_BV(config.e_pin);
	_delay_us(50);
}

static inline void lcd_1604_init(const lcd_1604_t config) {
	MAKE_DDR(config.bus_port_reg)  = 0xFF;
	MAKE_PORT(config.bus_port_reg) = 0x00;
	MAKE_DDR(config.control_port_reg) |= _BV(config.rs_pin) |  _BV(config.rw_pin) | _BV(config.e_pin);
	MAKE_PORT(config.bus_port_reg)  &= ~(_BV(config.rs_pin) |  _BV(config.rw_pin) | _BV(config.e_pin));
	_delay_ms(15);
	
	_lcd_1604_cmd(config, (_BV(4) | _BV(5)));
	_delay_ms(4.1);
	
	_lcd_1604_cmd(config, (_BV(4) | _BV(5)));
	_delay_us(100);
	
	_lcd_1604_cmd(config, (_BV(4) | _BV(5)));
	
}

static inline void lcd_1604_clear(const lcd_1604_t config) {

}

static inline void lcd_1604_cursor(const lcd_1604_t config, uint8_t line, uint8_t column) {
	
}

static inline void lcd_1604_write(const lcd_1604_t config, char character) {
	(*config.control_port_reg) |= _BV(config.rs_pin);
	(*config.bus_port_reg) = character;
}

static inline void lcd_1604_write_str(const lcd_1604_t config, const char* str) {
	while (*str != '\0') {
		lcd_1604_write(config, *str);
		str++;
	}
}

#undef MAKE_DDR
#undef MAKE_PORT

#endif /* LCD1604_H_ */
