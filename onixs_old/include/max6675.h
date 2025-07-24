#ifndef MAX6675_H_
#define MAX6675_H_

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include <avr/io.h>

#include "spi.h"

typedef spi_t max6675_t;
static float max6675_temp = 0.0f;
static max6675_t max6675_config;

static inline void max6675_init(const max6675_t config) {
	spi_init(config);
	max6675_config = config;
	max6675_temp = 0.0f;
}

static inline float max6675_update(void) {
	uint16_t raw = spi_read_16(max6675_config);
	if (raw & 0x04) {
		max6675_temp = INFINITY;
		return INFINITY;
	}
	max6675_temp = (raw >> 3) * 0.25;
	return max6675_temp;
}

static inline void max6675_timer_interrupt_enable(void) {
	TCCR2 |= _BV(WGM21) | _BV(CS22) | _BV(CS21) | _BV(CS20);
	TIMSK |= _BV(OCIE2);
	OCR2 = 244; // 125 ms
}


#endif /* MAX6675_H_ */
