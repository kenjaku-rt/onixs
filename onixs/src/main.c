#include <onixs.h>

int main(void) {
	ONIXS_PERPH_INIT();
	INTERFACE_INTRO();

	memory_get_error_t error_code = memory_get(&device_eeprom);
	if (error_code != MEMORY_GET_OK)
		INTERFACE_STARTUP_ERROR(error_code);
	
	TIMERS_ENABLE();

	while (1) {
		DISPLAY_HOME();
		DISPLAY_CHAR('_');
		DISPLAY_INT(device_eeprom.session.cell_n);
		DISPLAY_CHAR(':');
		DISPLAY_INT(device_eeprom.cells.data[0][0]);
		_delay_ms(100);
		DISPLAY_CLEAR();
	}

	return 0;
}


	/*
	memory_map_t mem;

	mem.session.rtc_hour = 20;
	mem.session.rtc_min = 10;
	mem.session.cell_n = 0;
	mem.session.cnt_min = 100;

	mem.cells.number = 6;
	mem.cells.data = (sys_t**)MALLOC(6, sizeof(sys_t*));

	const sys_t num_of_stages = 5;
	for (sys_t i = 0; i < mem.cells.number; ++i) {
		mem.cells.data[i] = calloc(1 + num_of_stages * 2, sizeof(sys_t));
		mem.cells.data[i][0] = num_of_stages;
	}

	memory_set(mem);
*/
