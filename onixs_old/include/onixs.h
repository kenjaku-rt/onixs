#ifndef ONIXS_H_
#define ONIXS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdnoreturn.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "spi.h"
#include "usart.h"
#include "keyboard.h"

#include "saves.h"
#include "relay.h"
#include "max6675.h"
#include "lcd2004.h"
#include "ds1307.h"

#include "timer_isr.h"

#define TERMINATE_IN_FAILURE(func) if (!func) { return -1; }
#define MAKE_PSTR(name) static const char PROGMEM name[]

typedef struct {
	lcd_2004_t display;
	max6675_t thermocouple;
	relay_t relay_heating;
	relay_t relay_ventilation;
} onixs_device_t;


// Global variables ----------------------------------------->

static time_map_t device_memory;
static bool device_pause;

static const onixs_device_t PROGMEM device_pgmem = {
	.relay_heating = {
		.port = &PORTB,
		.pin  = PB3,
	},
	.relay_ventilation = {
		.port = &PORTC,
		.pin = PC2,
	},
	.thermocouple = {
		.port  = &PORTB,
		.miso_pin = PB6,
		.mosi_pin = PB5, // not used
		.sck_pin  = PB7,
		.ss_pin   = PB4,
	},
	.display = {
		.bus_port_reg     = &PORTD,
		.control_port_reg = &PORTB,
		.e_pin  = PB0,
		.rs_pin = PB1,
		.rw_pin = PB2,
	},
};

// Display ----------------------------------------->

#define LCD_HOME()       lcd_2004_home(device.display)
#define LCD_CURSOR(x, y) lcd_2004_cursor(device.display, x, y)
#define LCD_STR(str)     lcd_2004_pstr(device.display, (str))
#define LCD_CHAR(ch)    _lcd_2004_data(device.display, (ch))
#define LCD_INT(number)  lcd_2004_int(device.display, number, 10)
#define LCD_CLR()        lcd_2004_clear(device.display)
#define LCD_XX(val) {if (val < 10) { LCD_CHAR(' '); } LCD_INT(val);}

/*# ------------------ #*/
/*#     ONIXS v1.0     #*/
/*#   loading data...  #*/
/*# ------------------ #*/
#define LCD_INTRO_LOADING()	LCD_CURSOR(0, 1);\
							LCD_STR(label_display_line);\
							LCD_CURSOR(1, 5);\
							LCD_STR(label_device_name);\
							LCD_CURSOR(2, 3);\
							LCD_STR(label_loading_msg);\
							LCD_CURSOR(3, 1);\
							LCD_STR(label_display_line);


/*# ------------------ #*/
/*#  AN ERROR OCCURED  #*/
/*#  MEMORY DOWNLOAD!  #*/
/*# ------------------ #*/
#define LCD_MEMORY_DOWNLOAD_ERROR() LCD_CURSOR(0, 1);\
									LCD_STR(label_display_line);\
									LCD_CURSOR(1, 2);\
									LCD_STR(label_error_occured);\
									LCD_CURSOR(2, 2);\
									LCD_STR(label_error_eeprom_download);\
									LCD_CURSOR(3, 1);\
									LCD_STR(label_display_line);


/*# ------------------ #*/
/*#  AN ERROR OCCURED  #*/
/*#   MEMORY UPLOAD!  #*/
/*# ------------------ #*/
#define LCD_MEMORY_UPLOAD_ERROR()	LCD_CURSOR(0, 1);\
									LCD_STR(label_display_line);\
									LCD_CURSOR(1, 2);\
									LCD_STR(label_error_occured);\
									LCD_CURSOR(2, 3);\
									LCD_STR(label_error_eeprom_upload);\
									LCD_CURSOR(3, 1);\
									LCD_STR(label_display_line);


#define PREVIOUS_SESSION_NOT_FINISHED() (device_memory.prev_time != 0)

#endif /* ONIXS_H_ */
