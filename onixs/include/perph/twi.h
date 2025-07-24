#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <avr/io.h>

#include "bits.h"

#define TWI_INIT(freq_32);\
TWSR = 0x00;\
TWBR |= ((F_CPU / freq_32) - 16) / 2;\
TWCR = (1 << TWEN);

#define TWI_START();\
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));

#define TWI_STOP();\
TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

#define TWI_WRITE(data);\
TWDR = (data);\
TWCR = (1 << TWINT) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));

static __attribute__((always_inline)) uint8_t TWI_READ_ACK(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

static __attribute__((always_inline)) uint8_t TWI_READ_NACK(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

#define BCD_TO_BIN(bcd) (((bcd) >> 4) * 10) + ((bcd) & 0x0F)
#define BIN_TO_BCD(bn) ((((bn) / 10) << 4) | ((bn) % 10))

#endif
