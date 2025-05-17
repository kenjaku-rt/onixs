#ifndef ONIXS_H_
#define ONIXS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "usart.h"

#include "saves.h"
#include "relay.h"
#include "max6675.h"
#include "lcd1604.h"

#define TERMINATE_IN_FAILURE(func) if (!func) { return -1; }

typedef struct {
	lcd_1604_t display;
	max6675_t thermocouple;
	relay_t relay;
	time_map_t memory;
} onixs_device_t;

bool onixs_init(onixs_device_t device) {
	
	lcd_1604_init(device.display);
	
	return true;
}

#endif /* ONIXS_H_ */
