#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#include <bits.h>
#include "def_shortcuts.h"

// For Atmega32A
#define SPI_PORT		&PORTB
#define SPI_PIN_SS		PB4
#define SPI_PIN_MOSI	PB5
#define SPI_PIN_MISO	PB6
#define SPI_PIN_SCK		PB7

#define SPI_INIT();\
MAKE_DDR(SPI_PORT) |= BV(SPI_PIN_MOSI) | BV(SPI_PIN_SS) | BV(SPI_PIN_SCK);\
MAKE_PORT(SPI_PORT) |= BV(SPI_PIN_SS);\
SPCR |= BV(SPE) | BV(MSTR); /*| _BV(SPR0);*/\
SPSR |= BV(SPI2X);\
(void)SPSR;\
(void)SPDR;

#define SPI_WRITE(data)\
SPDR = (data);\
while (!(SPSR & BV(SPIF)));

static __attribute__((always_inline)) uint8_t SPI_TRANSFER(uint8_t data) {
	SPDR = data;
	while (!(SPSR & BV(SPIF)));
	return SPDR;
}

static uint16_t spi_read_16(void) {
	MAKE_PORT(SPI_PORT) &= ~BV(SPI_PIN_SS);
	uint8_t high_8 = SPI_TRANSFER(0x00);
	uint8_t low_8  = SPI_TRANSFER(0x00);
	MAKE_PORT(SPI_PORT) |= BV(SPI_PIN_SS);
	_delay_us(10);
	return ((high_8 << 8) | low_8);
}

#endif
