#ifndef I2C_H_
#define I2C_H_

// For AtMega32A

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <util/twi.h>

void i2c_init(uint32_t freq) {
	TWSR = 0x00;
	TWBR |= ((F_CPU / freq) - 16) / 2;
	TWCR = (1 << TWEN);
}

void i2c_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

#endif /* I2C_H_ */
