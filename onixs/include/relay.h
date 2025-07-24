#ifndef RELAY_H_
#define RELAY_H_

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "bits.h"
#include "def_shortcuts.h"

#define RELAY_HEATER_PORT &PORTB
#define RELAY_HEATER_PIN  PB3

#define RELAY_VENT_PORT	  &PORTC
#define RELAY_VENT_PIN	  PC2

#define RELAY_HEATER_INIT(); MAKE_DDR(RELAY_HEATER_PORT) |= BV(RELAY_HEATER_PIN);
#define RELAY_VENT_INIT();   MAKE_DDR(RELAY_VENT_PORT) |= BV(RELAY_VENT_PIN);

#define RELAY_INIT();\
RELAY_HEATER_INIT();\
RELAY_VENT_INIT();

#endif
