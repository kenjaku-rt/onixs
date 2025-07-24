#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <avr/io.h>

static inline void usart_init(const uint16_t freq) {
	if (!freq)
		return;
		
	const uint16_t baud = F_CPU / 16 / freq-1;
	// enable tx/rx	
	UCSRB |= _BV(TXEN) | _BV(RXEN);
	// register select, 8 bit character size
	UCSRC |= _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
	// set freq
	UBRRL = (uint8_t)(baud & 0x00FF);
	UBRRH = (uint8_t)(baud >> 8);
}

static inline void usart_write(uint8_t byte) {
	while (!(UCSRA & _BV(UDRE)));
	UDR = byte;
}

static inline void usart_write_str(const char* str) {
	while (*str != '\0') {
		usart_write((uint8_t)*str);
		str++;
	}	
}

static inline uint8_t usart_read(void) {
	while (!(UCSRA & _BV(RXC)));
	return UDR;
}

#include <stdlib.h>

static inline void usart_write_int(int number) {
	char buffer[10];
	usart_write_str(itoa(number, buffer, 10));
}

#endif /* USART_H_ */
