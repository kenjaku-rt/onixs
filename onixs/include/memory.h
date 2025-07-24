#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include <avr/io.h>
#include <avr/eeprom.h>

#include "rtc.h"

// sizeof() = 2 bytes
typedef uint16_t sys_t;

#define EEPROM_BEGIN 0U
#define EEPROM_RTC_TIME_SHIFT 1U
#define EEPROM_CNT_TIME_SHIFT 2U
#define EEPROM_CELL_N_SHIFT   3U
#define EEPROM_CELLS_N_SHIFT  4U
#define EEPROM_CELLS_SHIFT	  5U
#define EEPROM_GET_ADDR(shift) (uint16_t*)(EEPROM_BEGIN + sizeof(sys_t) * shift)

typedef struct {
	sys_t number;
	sys_t** data;
} cells_t;

typedef struct {
	struct {
		rtc_data_t	rtc_min;
		rtc_data_t	rtc_hour;
		sys_t		cnt_min;
		sys_t		cell_n;
	} session;
	cells_t			cells;
} memory_map_t;

static memory_map_t device_eeprom;

#define NUM_OF_ELEM_AT_EACH_STAGE 2
#define CELL_BLOCK_SIZE(cell) ((1 + (cell)[0] * NUM_OF_ELEM_AT_EACH_STAGE) * sizeof(sys_t))
#define MALLOC(num, size_of) malloc((num) * (size_of))
#define FREE_USED_MEMORY(); for (sys_t j = 0; j < i; ++j) { free(result.cells.data[j]); } free(result.cells.data);


static __attribute__((always_inline)) sys_t eeprom_read_sys(sys_t shift) {
	eeprom_busy_wait();
	return eeprom_read_word(EEPROM_GET_ADDR(shift));
}

static __attribute__((always_inline)) void eeprom_write_sys(sys_t shift, sys_t data) {
	eeprom_busy_wait();
	eeprom_write_word(EEPROM_GET_ADDR(shift), data);
}


typedef enum {
	MEMORY_GET_OK,
	MEMORY_GET_NO_MEMORY,
	MEMORY_GET_NO_PTR,
	MEMORY_GET_ERROR_ALLOC,
} memory_get_error_t;


static memory_get_error_t memory_get(memory_map_t* map_ptr) {

	if (!map_ptr)
		return MEMORY_GET_NO_PTR; 

	memory_map_t result;

	uint16_t rtc_raw = eeprom_read_sys(EEPROM_RTC_TIME_SHIFT);
	result.session.rtc_hour = (rtc_raw >> 8);
	result.session.rtc_min  = (rtc_raw & 0x00FF);

	result.session.cnt_min = eeprom_read_sys(EEPROM_CNT_TIME_SHIFT);
	result.session.cell_n = eeprom_read_sys(EEPROM_CELL_N_SHIFT);

	result.cells.number = eeprom_read_sys(EEPROM_CELLS_N_SHIFT);
	DISPLAY_CLEAR();
	DISPLAY_CURSOR(0, 0);
	DISPLAY_CHAR('^'); DISPLAY_INT(result.cells.number); 
	_delay_ms(2000);
	if (result.cells.number == 0) {
		return MEMORY_GET_NO_MEMORY;
	}

	result.cells.data = MALLOC(result.cells.number, sizeof(sys_t*));

	if (!result.cells.data)
		return MEMORY_GET_ERROR_ALLOC;
	
	eeprom_busy_wait();
	eeprom_read_block(result.cells.data, EEPROM_GET_ADDR(EEPROM_CELLS_SHIFT), sizeof(sys_t) * result.cells.number);

	for (sys_t i = 0; i < result.cells.number; ++i) {
		sys_t num_of_stages = 1 + eeprom_read_sys((uintptr_t)result.cells.data[i]) * NUM_OF_ELEM_AT_EACH_STAGE;
		DISPLAY_CLEAR();
		DISPLAY_CURSOR(1, 0);
		DISPLAY_INT(num_of_stages);
		_delay_ms(2000);
		sys_t* stages = MALLOC(num_of_stages, sizeof(sys_t));
		if (!stages) {
			FREE_USED_MEMORY();
			return MEMORY_GET_ERROR_ALLOC;
		}
		eeprom_busy_wait();
		eeprom_read_block(stages, EEPROM_GET_ADDR((uintptr_t)result.cells.data[i]), num_of_stages * sizeof(sys_t));
		result.cells.data[i] = stages;
	}
	(*map_ptr) = result;
	return MEMORY_GET_OK;
}

#undef FREE_USED_MEMORY

static void memory_set(memory_map_t data) {
	if (!data.cells.data)
		return;

	sys_t rtc_raw = (data.session.rtc_hour << 8) | data.session.rtc_min;
	eeprom_write_sys(EEPROM_RTC_TIME_SHIFT, rtc_raw);
	eeprom_write_sys(EEPROM_CNT_TIME_SHIFT, data.session.cnt_min);
	eeprom_write_sys(EEPROM_CELL_N_SHIFT, data.session.cell_n);
	eeprom_write_sys(EEPROM_CELLS_N_SHIFT, data.cells.number);
	
	sys_t cells_begin_shift = EEPROM_CELLS_SHIFT + data.cells.number;
	for (
			sys_t i = 0, 
			block_size = CELL_BLOCK_SIZE(data.cells.data[i]), 
			current_cell_shift = cells_begin_shift; 

			i < data.cells.number; 

			++i,
			block_size = CELL_BLOCK_SIZE(data.cells.data[i]),
			current_cell_shift += block_size
		) {
		eeprom_write_sys(EEPROM_CELLS_SHIFT + i, current_cell_shift);
		eeprom_write_block(data.cells.data[i], EEPROM_GET_ADDR(current_cell_shift), block_size);
	}
}


#endif // MEMORY_H_
