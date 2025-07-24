#ifndef ONIXS_H_
#define ONIXS_H_

/**
 * @file onixs.h
 * @brief Основной файл для контроллера печи.
 * 
 * @author Матвей Рыбалкин
 * @date 22.05.2025
 * @version 2.0
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

// Next includes contain global variables:
#include "relay.h"
#include "display.h"
#include "rtc.h"
#include "thermo.h"
#include "memory.h"
#include "interface.h"
#include "timers.h"

#define ONIXS_PERPH_INIT();\
RELAY_INIT();\
display_init();\
rtc_init();\
THERMO_INIT();




#endif // ONIXS_H_
