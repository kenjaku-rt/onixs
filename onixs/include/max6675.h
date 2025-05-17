#ifndef MAX6675_H_
#define MAX6675_H_

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include <avr/io.h>

#include "spi.h"

typedef spi_t max6675_t;
float max6675_temp = 0.0f;

static inline void max6675_init(const max6675_t config) {
	spi_init(config);
	max6675_temp = 0.0f;
}

static inline float max6675_update(const max6675_t config) {
	uint16_t raw = spi_read_16(config);
	if (raw & 0x04) {
		max6675_temp = INFINITY;
		return INFINITY;
	}
	max6675_temp = (raw >> 3) * 0.25;
	return max6675_temp;
}


#endif /* MAX6675_H_ */
