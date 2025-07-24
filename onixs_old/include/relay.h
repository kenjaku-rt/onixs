#ifndef RELAY_H_
#define RELAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include <avr/io.h>

#include "def_shortcuts.h"

typedef struct {
	volatile uint8_t* port;
	uint8_t pin;	
} relay_t;

static inline void relay_init(const relay_t config) {
	MAKE_DDR(config.port) |= _BV(config.pin);
}

static inline void relay_modify(const relay_t config, bool state) {
	if (state)
		MAKE_PORT(config.port) |= _BV(config.pin);
	else
		MAKE_PORT(config.port) &= ~_BV(config.pin);
}

#include "undef_shortcuts.h"

#endif /* RELAY_H_ */
