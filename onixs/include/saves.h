#ifndef SAVES_H_
#define SAVES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include <avr/io.h>
#include <avr/eeprom.h>

/***************************************************************************/
/*                          <Memory managment>							   */
/* [        0        ][      N       ][n][mins...][NULL][n][mins...][NULL] */
/*   <previous time>   <num of cells>									   */
/***************************************************************************/

#define EEPROM_BEGIN 0
#define EEPROM_PREV_TIME_SHIFT    0
#define EEPROM_PREV_CELL_SHIFT    1
#define EEPROM_NUM_OF_CELLS_SHIFT 2
#define EEPROM_CELLS_SHIFT	      3

#define EEPROM_GET_ADDR(shift) (EEPROM_BEGIN + sizeof(uint16_t) * (shift))

typedef uint16_t time_min_t;

typedef struct {
	uint16_t num;
	time_min_t** data;
} time_cells_t;

typedef struct {
	time_min_t   prev_time;
	uint16_t     prev_cell;
	time_cells_t cells;
} time_map_t;

#define CELLS_INVALID_NUMBER UINT16_MAX

extern time_map_t onixs_memory_download(void);
extern void onixs_memory_upload(time_map_t current_state);

inline void onixs_memory_cherry_pick(const time_min_t current_time, const uint16_t current_mode) {
	eeprom_busy_wait();
	eeprom_write_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_PREV_TIME_SHIFT)), current_time);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t*)(EEPROM_GET_ADDR(EEPROM_PREV_CELL_SHIFT)), current_mode);
}

inline void onixs_memory_full_cleanup(const size_t limit) {
	for (size_t i = 0; i < limit; ++i) {
		eeprom_busy_wait();
		eeprom_write_word((uint16_t*)i, 0);
	}
} 

#endif /* SAVES_H_ */