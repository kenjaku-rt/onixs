#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdnoreturn.h>

#include "relay.h"
#include "display.h"
#include "rtc.h"
#include "thermo.h"
#include "memory.h"

pstr_t label_line[] = "------------------";
pstr_t label_device_name[] = "ONIXS v1.0";
pstr_t label_loading[] = "loading data...";
pstr_t label_error_occured[] = "AN ERROR OCCURED";
pstr_t label_error_occured_no_ptr[] = "NOWHERE TO SAVE!";
pstr_t label_error_occured_alloc[] = "MEMORY EXCEEDED!";
pstr_t label_error_occured_no_mem[] = "  NO MEMORY!";

static __attribute__((always_inline)) void INTERFACE_INTRO(void) {
	DISPLAY_CURSOR(0, 1);
	DISPLAY_PSTR(label_line);
	DISPLAY_CURSOR(1, 5);
	DISPLAY_PSTR(label_device_name);
	DISPLAY_CURSOR(2, 3);
	DISPLAY_PSTR(label_loading);
	DISPLAY_CURSOR(3, 1);
	DISPLAY_PSTR(label_line);
}

 static __attribute__((always_inline)) noreturn void INTERFACE_STARTUP_ERROR(memory_get_error_t error_code) {
	DISPLAY_CURSOR(0, 1);
	DISPLAY_PSTR(label_line);
	DISPLAY_CURSOR(1, 2);
	DISPLAY_PSTR(label_error_occured);
	DISPLAY_CURSOR(2, 2);
	if (error_code == MEMORY_GET_ERROR_ALLOC)
		DISPLAY_PSTR(label_error_occured_alloc);
	else if (error_code == MEMORY_GET_NO_PTR)
		DISPLAY_PSTR(label_error_occured_no_ptr);
	else
		DISPLAY_PSTR(label_error_occured_no_mem);
	DISPLAY_CURSOR(3, 1);
	DISPLAY_PSTR(label_line);
	while (1);
}


#endif /* INTERFACE_H_ */