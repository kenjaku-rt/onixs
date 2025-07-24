
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/eeprom.h>

#include <usart.h>
#include <saves.h>

#define _FREE_USAGE_MEMORY(); for (uint16_t j = 0; j < i; ++j) { free(result.cells.data[j]); } free(result.cells.data);

time_map_t saves_memory_download(void) {
	
	time_map_t result;
	
	// [prev time][prev cell][N][ptr...]
	eeprom_busy_wait();
	result.prev_time = eeprom_read_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_PREV_TIME_SHIFT)));
	eeprom_busy_wait();
	result.prev_cell = eeprom_read_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_PREV_CELL_SHIFT)));
	eeprom_busy_wait();
	result.cells.num = eeprom_read_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_NUM_OF_CELLS_SHIFT)));

	result.cells.data = (time_min_t**)malloc(sizeof(time_min_t*) * result.cells.num);
	
	if (!result.cells.data) {
		result.cells.num = CELLS_INVALID_NUMBER;
		return result;
	}

	eeprom_busy_wait();
	eeprom_read_block(result.cells.data, (uint16_t*)(EEPROM_GET_ADDR(EEPROM_CELLS_SHIFT)), result.cells.num * sizeof(uint16_t));

	for (uint16_t i = 0; i < result.cells.num; ++i) {
		eeprom_busy_wait();
		uint16_t num_of_mins = eeprom_read_word((uint16_t*)(EEPROM_GET_ADDR((uintptr_t)(result.cells.data[i])))) * 2;
		uint16_t* current_buffer = (uint16_t*)malloc((num_of_mins + 1) * sizeof(uint16_t)); 

		if (!current_buffer) {
			_FREE_USAGE_MEMORY();
			result.cells.num = 0;
			return result;
		}
		
		eeprom_read_block(current_buffer, (void*)(EEPROM_GET_ADDR((uintptr_t)(result.cells.data[i]))), (num_of_mins + 1) * sizeof(uint16_t));
		result.cells.data[i] = current_buffer;
	}

	return result;
}

#undef _FREE_USAGE_MEMORY

void saves_memory_upload(time_map_t current_state) {
	saves_memory_cherry_pick(current_state.prev_time, current_state.prev_cell);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_NUM_OF_CELLS_SHIFT)), current_state.cells.num); 

	// write in [N][ptrs 1] .... (1)<[n][mins...]>
	// write in [N][ptrs 1 2] .... (2)<[n][mins...]>
	// ...
	const uint16_t begin_of_cells_shift = EEPROM_CELLS_SHIFT + current_state.cells.num;
	uint16_t cells_shift = 0;
	for (uint16_t i = 0; i < current_state.cells.num; ++i) {
		eeprom_busy_wait();
		uint16_t current_cells_addr = begin_of_cells_shift + cells_shift;
		eeprom_write_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_CELLS_SHIFT + i)), current_cells_addr);
		uint16_t num_of_mins = (current_state.cells.data[i][0] * 2) + 1;
		eeprom_busy_wait();
		eeprom_write_block((uint16_t*)current_state.cells.data[i], (void*)EEPROM_GET_ADDR(current_cells_addr), num_of_mins * sizeof(uint16_t));
		cells_shift += num_of_mins;
	}

}


	/*memory_map_t mem;

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