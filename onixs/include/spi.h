#ifndef SPI_H_
#define SPI_H_

// For AtMega32A

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

typedef struct {
	volatile uint8_t* port;
	uint8_t ss_pin, mosi_pin, miso_pin, sck_pin;	
} spi_t;

#define MAKE_DDR(port_addr)  (*(volatile uint8_t*)(port_addr - 1))
#define MAKE_PORT(port_addr) (*(volatile uint8_t*)(port_addr))

static inline void spi_init(const spi_t config) {
	MAKE_DDR(config.port) |= _BV(config.mosi_pin) | _BV(config.ss_pin) | _BV(config.sck_pin);
	MAKE_PORT(config.port) |= _BV(config.ss_pin);
	// 2 Mz -> no prescaler
	SPCR |= _BV(SPE) | _BV(MSTR);
	(void)SPSR;
	(void)SPDR;
}

static inline void spi_write(uint8_t byte_of_data) {
	SPDR = byte_of_data;
	while (!(SPSR & _BV(SPIF)));
}

static inline uint8_t spi_transfer(uint8_t byte_of_data) {
	SPDR = byte_of_data;
	while (!(SPSR & _BV(SPIF)));
	return SPDR;
}

static inline uint16_t spi_read_16(const spi_t config) {
	MAKE_PORT(config.port) &= ~_BV(config.ss_pin);
	uint8_t high_8 = spi_transfer(0x00);
	uint8_t low_8  = spi_transfer(0x00);
	MAKE_PORT(config.port) |= _BV(config.ss_pin);
	_delay_us(10);
	return ((high_8 << 8) | low_8);
}

static inline void spi_write_str(const char* str) {
	while (*str != '\0') {
		spi_write(*str);
		str++;
	}
}

#undef MAKE_DDR
#undef MAKE_PORT

#endif /* SPI_H_ */
