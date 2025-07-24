#ifndef THERMO_H_
#define THERMO_H_

#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#include <avr/io.h>
#include <util/delay.h>

#include "def_shortcuts.h"
#include "perph/spi.h"

static float thermo_temp = 0.0f;

#define THERMO_INIT(); SPI_INIT();

static inline float thermo_update(void) {
	uint16_t raw = spi_read_16();
	if (raw & 0x04) {
		thermo_temp = INFINITY;
		return INFINITY;
	}
	thermo_temp = (raw >> 3) * 0.25;
	return thermo_temp;
}

#endif
