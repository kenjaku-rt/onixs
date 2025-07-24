
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <onixs.h>

// 1s
ISR(TIMER1_COMPA_vect) {
	static uint16_t sec_counter = 0;
	rtime = ds1307_get_time();
	// 10 minutes
	if (sec_counter >= 600) {
		saves_memory_cherry_pick(device_memory.prev_time, device_memory.prev_cell);
		sec_counter = 0;
	}
	sec_counter += 1;
}

uint16_t onixs_input_number(uint16_t num_of_characters) {
	char number_array[6] = {0, 0, 0, 0, 0, 0};
	uint8_t key_state = KEY_NO_TOUCH;
	for (uint8_t i = 0; i < num_of_characters; ++i) {
		while ((key_state = keyboard_scan()) == KEY_NO_TOUCH);
		number_array[i] = keyboard_key_to_digit(key_state);
		if (number_array[i] == '\0')
		break;
		LCD_CHAR(number_array[i]);
		while (keyboard_scan() != KEY_NO_TOUCH);
		_delay_ms(50);
	}
	return (uint16_t)atoi(number_array);
}

void onixs_pause() {
	_delay_ms(150);
	WAIT_KEY();
}

/*# ------VIEW-__----- #*/
/*# [ -- STAGE __ -- ] #*/
/*#   T: ___  v: ___   #*/
/*# dur: ___ min: ____ #*/
void onixs_view_cell(uint16_t cell_for_view) {
	
	MAKE_PSTR(label_view) = "------VIEW-";
	MAKE_PSTR(label_view2) = "-----";
	MAKE_PSTR(label_stage) = "[ -- STAGE ";
	MAKE_PSTR(label_stage2) = " -- ]";
	MAKE_PSTR(label_temp) = "T: ";
	MAKE_PSTR(label_vent) = " v: ";
	MAKE_PSTR(label_duration) = "dur: ";
	MAKE_PSTR(label_minute) = " min: ";
	MAKE_PSTR(label_on) = "ON";
	MAKE_PSTR(label_off) = "OFF";
	
	uint16_t num_of_stages = device_memory.cells.data[cell_for_view][0];
	uint16_t min_capacity = 0;
	for (uint16_t i = 0; i < num_of_stages; ++i) {
		LCD_CLR();
		LCD_CURSOR(0, 1);
		LCD_STR(label_view);
		LCD_XX(cell_for_view+1);
		LCD_STR(label_view2);
		
		LCD_CURSOR(1, 1);
		LCD_STR(label_stage);
		LCD_XX(i+1);
		LCD_STR(label_stage2);
		
		LCD_CURSOR(2, 3);
		LCD_STR(label_temp);
		LCD_INT(GET_TEMP(device_memory.cells.data[cell_for_view][NUM_OF_ELEM_SHIFT + i * 2 + 1]));
		LCD_CURSOR(2, 11);
		LCD_STR(label_vent);
		LCD_STR((GET_VENT(device_memory.cells.data[cell_for_view][NUM_OF_ELEM_SHIFT + i * 2 + 1]) ? label_on : label_off));
		
		uint16_t time_duration = device_memory.cells.data[cell_for_view][NUM_OF_ELEM_SHIFT + i * 2];
		min_capacity += time_duration;
		
		LCD_CURSOR(3, 1);
		LCD_STR(label_duration);
		LCD_INT(time_duration);
		LCD_CURSOR(3, 9);
		LCD_STR(label_minute);
		LCD_INT(min_capacity);
		WAIT_KEY();
		WAIT_KEY_INVERSE();
	}
}

#define PRESS_D false
#define PRESS_C true

/*# ------------------ #*/
/*#   Press D again to #*/
/*# delete the item... #*/
/*# ------------------ #*/

/*# ------------------ #*/
/*#   Press C again to #*/
/*# create new item... #*/
/*# ------------------ #*/
void onixs_press(bool state) {
	MAKE_PSTR(label_display_line) = "------------------";
	MAKE_PSTR(label_press_x11) = "Press ";
	MAKE_PSTR(label_press_x12) = " again to";
	MAKE_PSTR(label_press_d2) = "delete the item...";
	MAKE_PSTR(label_press_c2) = "create new item...";
	
	LCD_CLR();
	LCD_CURSOR(0, 1);
	LCD_STR(label_display_line);
	LCD_CURSOR(1, 3);
	LCD_STR(label_press_x11);
	LCD_CHAR((state ? 'C' : 'D'));
	LCD_STR(label_press_x12);
	LCD_CURSOR(2, 1);
	LCD_STR((state ? label_press_c2 : label_press_d2));
	LCD_CURSOR(3, 1);
	LCD_STR(label_display_line);
}

/*# ------EDIT-__----- #*/
/*# [ -- STAGE __ -- ] #*/
/*#   T: ___  v: ___   #*/
/*# dur: ____ min: ___ #*/
/*
void onixs_edit_cell(uint16_t cell_for_edit) {
	MAKE_PSTR(label_view) = "------VIEW-";
	MAKE_PSTR(label_view2) = "-----";
	MAKE_PSTR(label_stage) = "[ -- STAGE ";
	MAKE_PSTR(label_stage2) = " -- ]";
	MAKE_PSTR(label_temp) = "T: ";
	MAKE_PSTR(label_vent) = " v: ";
	MAKE_PSTR(label_duration) = "dur: ";
	MAKE_PSTR(label_minute) = " min: ";
	MAKE_PSTR(label_on) = "ON";
	MAKE_PSTR(label_off) = "OFF";
	
	uint16_t current_stage = 0;
	while (1) {
		LCD_CLR();
		LCD_CURSOR(0, 1);
		LCD_STR(label_view);
		LCD_XX(cell_for_edit+1);
		LCD_STR(label_view2);
		
		LCD_CURSOR(1, 1);
		LCD_STR(label_stage);
		LCD_XX(current_stage);
		LCD_STR(label_stage2);
		
		LCD_CURSOR(2, 3);
		LCD_STR(label_temp);
		LCD_STR(PSTR("____"));
		LCD_CURSOR(2, 11);
		LCD_STR(label_vent);
		LCD_STR(PSTR("___"));

		LCD_CURSOR(3, 1);
		LCD_STR(label_duration);
		LCD_STR(PSTR("___"));
		LCD_CURSOR(3, 9);
		LCD_STR(label_minute);
		LCD_STR(PSTR("____"));

		for (bool data_is_set = false; data_is_set != true;) {
			
			LCD_CURSOR(2, 6);
			uint16_t input_data = keyboard_scan(4);
			
			if (input_data == 0)
				//
			
			
		}
	}
	
}
*/

/*# ------------------ #*/
/*#   Do you want to   #*/
/*# cont. pr. process? #*/
/*# -- press A or D -- #*/
bool onix_melting_pause(void) {
	MAKE_PSTR(label_display_line) = "------------------";
	MAKE_PSTR(label_continue1) = "Do you want to";
	MAKE_PSTR(label_continue2) = "cont. pr. process?";
	MAKE_PSTR(label_press_ad) = "-- press A or D --";
	
	LCD_CLR();
	LCD_CURSOR(0, 1);
	LCD_STR(label_display_line);
	LCD_CURSOR(1, 3);
	LCD_STR(label_continue1);
	LCD_CURSOR(2, 2);
	LCD_STR(label_continue1);
	LCD_CURSOR(3, 1);
	LCD_STR(label_continue1);

	bool ans = false;
	WAIT_KEY();
	uint16_t keyboard_state = keyboard_scan();
	if (keyboard_state == KEY_A) ans = true;
	WAIT_KEY_INVERSE();
	return ans;
}

typedef enum {
	MELTING_FINISHED,
	MELTING_INVALID_MINUTE,
	MELTING_NO_DATA,
} melting_status_t;
/*# -----23:57:20----- #*/
/*# T = _____ t: ____  #*/
/*# stage = __ v: ON   #*/
/*# now: ___ left: ___ #*/
int onixs_melting(uint16_t current_min, time_min_t* cells) {
	if (!cells)
		return MELTING_NO_DATA;
	
	
	device_memory.prev_time = current_min;
	// device_memory.prev_cell <- defined before
	
	uint16_t num_of_cells = cells[0];
	
	/*
		Explanation:
	
		[N][min][temp/vent][min][temp/vent]
		[0][ 1 ][    2    ][ 3 ][    4    ]
		
		We sum up the minutes until there is a next cell 
		with a value greater than the current one.
	
	*/
	
	uint16_t current_cell = UINT16_MAX;
	// parse only [min][temp/vent] <- mins
	uint16_t minute_capacity = 0, minute_max = 0;
	for (uint16_t i = 0; i < num_of_cells*2; i += 2) {
		minute_capacity += cells[NUM_OF_ELEM_SHIFT + i];
		if (minute_capacity > current_min) {
			current_cell = i;
			break;
		}
	}
	minute_max = minute_capacity;
	for (uint16_t i = current_cell; i < num_of_cells*2; i += 2) {
		minute_max += cells[NUM_OF_ELEM_SHIFT + i];
	}

	if (minute_capacity < current_min)
		return MELTING_INVALID_MINUTE;

	uint16_t current_temp = GET_TEMP(cells[current_cell + 1]);
	bool     current_vent = GET_VENT(cells[current_cell + 1]); 
	
	MAKE_PSTR(label_half_line) = "-----";
	MAKE_PSTR(label_thermo) = "T = ";
	MAKE_PSTR(label_await_temp) = "t:";
	MAKE_PSTR(label_stage) = "stage:";
	MAKE_PSTR(label_vent) = "v:";
	
	MAKE_PSTR(label_on) = "ON";
	MAKE_PSTR(label_off) = "OFF";
	const char* label_vent_status = (current_vent ? label_on : label_off);
	
	MAKE_PSTR(label_now) = "now:";
	MAKE_PSTR(label_left) = "left:";
	
	uint8_t prev_min = rtime.min;
	while(1) {
	
		if (prev_min < rtime.min) {
			prev_min = rtime.min;
			current_min++;
		}
	
		LCD_CLR();
		LCD_CURSOR(0, 1);
		LCD_STR(label_half_line);
		LCD_XX(rtime.hour);
		LCD_CHAR(':');
		LCD_XX(rtime.min);
		LCD_CHAR(':');
		LCD_XX(rtime.sec);
		LCD_STR(label_half_line);
		
		LCD_CURSOR(1, 1);
		LCD_STR(label_thermo);
		LCD_INT(max6675_temp);
		LCD_CURSOR(1, 11);
		LCD_STR(label_await_temp);
		LCD_INT(current_temp);
		
		LCD_CURSOR(2, 1);
		LCD_STR(label_stage);
		LCD_INT(current_cell);
		LCD_CURSOR(2, 11);
		LCD_STR(label_vent);
		LCD_STR(label_vent_status);
		
		uint16_t keyboard_state = keyboard_scan();
		if (keyboard_state != KEY_NO_TOUCH) {
			relay_modify(device.relay_heating, false);
			LCD_CURSOR(3, 1);
			LCD_STR(PSTR("------PAUSED------"));
			onixs_pause();
		} else {
			LCD_CURSOR(3, 1);
			LCD_STR(label_now);
			LCD_INT(current_min);
			LCD_STR(label_left);
			LCD_INT(minute_max - current_min);
		}
		
		if (max6675_temp < current_temp) relay_modify(device.relay_heating, true);
		else relay_modify(device.relay_heating, false);
		
		if (current_min > minute_capacity) {
			minute_capacity += cells[NUM_OF_ELEM_SHIFT + current_cell];
			current_cell += 2;
			if (current_cell / 2 == num_of_cells) {
				relay_modify(device.relay_heating, false);
				return MELTING_FINISHED;
			}
		}
		_delay_ms(150);
	}
}

static uint16_t _get_cell_total_minutes(uint16_t cell_index) {
	uint16_t ans = 0, num_of_stages = device_memory.cells.data[cell_index][0];
	for (uint16_t i = 0; i < num_of_stages; ++i) {
		ans += device_memory.cells.data[cell_index][NUM_OF_ELEM_SHIFT + i * 2];
	}
	return ans;
}

static inline uint16_t _get_num_of_stages(uint16_t cell_index) {
	return device_memory.cells.data[cell_index][0];
}


/*# - Total cells: __ -#*/
/*# cell: __ min: ___  #*/
/*# - [__] - stages -- #*/
/*# press B, C to move #*/
noreturn static inline void onixs_menu(void) {

	MAKE_PSTR(label_total_modes) = "- Total cells: ";
	MAKE_PSTR(label_mode_n) = " cell: ";
	MAKE_PSTR(label_mode_min) = " min:";
	MAKE_PSTR(label_cells_1) = " - [";
	MAKE_PSTR(label_cells_2) = "] - stages --";
	MAKE_PSTR(label_help_line) = "press B, C to move";

	uint16_t cell_choice = 0;
	uint16_t total_minutes = _get_cell_total_minutes(cell_choice);
	uint16_t stages = _get_num_of_stages(cell_choice);
	while (1) {
		uint16_t keyboard_state = keyboard_scan();
		if (keyboard_scan() != KEY_NO_TOUCH) {
			if (keyboard_state == KEY_B && cell_choice > 0) {
				cell_choice--;
				total_minutes = _get_cell_total_minutes(cell_choice);
				stages = _get_num_of_stages(cell_choice);
				_delay_ms(50);
			} else if (keyboard_state == KEY_C) {
				if (cell_choice < device_memory.cells.num-1) {
					cell_choice++;	
				} else {
					onixs_press(PRESS_C);
					WAIT_KEY();
					keyboard_state = keyboard_scan();
					if (keyboard_state == KEY_C) {
						time_min_t** new_cells = realloc(device_memory.cells.data, (device_memory.cells.num + 1) * sizeof(time_min_t*));
						if (!new_cells) {
							// error. ---------------------------
						}
						device_memory.cells.data = new_cells;
						device_memory.cells.num++;
					}
				}
				total_minutes = _get_cell_total_minutes(cell_choice);
				stages = _get_num_of_stages(cell_choice);
				_delay_ms(50);
			} else if (keyboard_state == KEY_D) {

				onixs_press(PRESS_D);
				
				
			} else if (keyboard_state == KEY_HASH) {
				LCD_CLR();
				_delay_ms(100);	
				onixs_view_cell(cell_choice);
			} else if (keyboard_state == KEY_MUL) {
				//onixs_edit_cell(cell_choice);
			}
			
				
			
			LCD_CLR();
			LCD_CURSOR(0, 1);
			LCD_STR(label_total_modes);
			LCD_XX(device_memory.cells.num);
			
			LCD_CURSOR(1, 1);
			LCD_STR(label_mode_n);
			LCD_XX(cell_choice + 1);
			LCD_STR(label_mode_min);
			LCD_INT(total_minutes);
			
			LCD_CURSOR(2, 1);
			LCD_STR(label_cells_1);
			LCD_XX(stages);
			LCD_STR(label_cells_2);
			
			LCD_CURSOR(3, 1);
			LCD_STR(label_help_line);
		}
		WAIT_KEY_INVERSE();
		_delay_ms(10);
	}
}

int main(void) {

	// Initialization ----------------------------------------->
	//relay_init(device.relay_heating);
	//relay_init(device.relay_ventilation);
	lcd_2004_init(device.display);
	//max6675_init(device.thermocouple);
	//ds1307_init();
	//rtime = ds1307_get_time();
	
	//global_interrupt_enable();
	//max6675_timer_interrupt_enable();
	//ds1307_timer_interrupt_enable();
	//keyboard_timer_interrupt_enable(&PORTA);
	
	// Constants ----------------------------------------->
	
	// General
	MAKE_PSTR(label_error_occured) = "AN ERROR OCCURED";
	MAKE_PSTR(label_device_name) = "ONIXS v1.0";
	MAKE_PSTR(label_loading_msg) = "loading data...";
	MAKE_PSTR(label_display_line) = "------------------";
	
	// Errors
	MAKE_PSTR(label_error_eeprom_download) = "MEMORY DOWNLOAD!";
	// MAKE_PSTR(label_error_eeprom_upload)   = "MEMORY UPLOAD!";
	
	// Main part ----------------------------------------->
	
	time_map_t example_map;
	example_map.prev_time = 40;
	example_map.prev_cell = 1;
	example_map.cells.num = 3;
	//example_map.prev_ds1307_hour = 10;
	//example_map.prev_ds1307_min = 20;

	example_map.cells.data = (time_min_t**)calloc(3, sizeof(time_min_t*));

	for (int i = 0; i < 3; ++i) {
		example_map.cells.data[i] = calloc((3 * 2) + 1, sizeof(time_min_t));
		example_map.cells.data[i][0] = 3;
		for (int j = 0 + 1; j < (3 * 2) + 1; j += 2) {
			example_map.cells.data[i][j] = 15;
			example_map.cells.data[i][j+1] = 0xFFFF;
		}
	}
	
	LCD_INTRO_LOADING();
	saves_memory_upload(example_map);

	/*
	device_memory = saves_memory_download();
	
	if (device_memory.cells.data == NULL) {
		LCD_MEMORY_DOWNLOAD_ERROR();
		return -1;
	}
	
	if (device_memory.prev_time != 0) {
		uint16_t time_diff = rtime_diff_min(rtime.hour, rtime.min, device_memory.prev_ds1307_hour, device_memory.prev_ds1307_min);
		// More than 30 minutes
		if (time_diff >= 30 * 60) {
			
		} else {
			onixs_melting(device_memory.prev_time, device_memory.cells.data[device_memory.prev_cell]);
		}
	}
		
	onixs_menu();
	*/
	return 0;
}


/*
		static char num_buffer[10];
		memset(num_buffer, '\0', sizeof(char) * 10);
		dtostrf(max6675_temp, 6, 2, num_buffer);
		lcd_2004_home(device.display);
		lcd_2004_str(device.display, "T = ");
		lcd_2004_str(device.display, num_buffer);
		_delay_ms(150);*/

/*ds1307_time_t ti = ds1307_time_get();
		char buffer[10];
		memset(buffer, '\0', sizeof(char) * 10);
		itoa(ti.sec, buffer, 10);
		lcd_2004_str(device.display, buffer);
		_delay_ms(2000);
*/

/*

time_map_t example_map;
example_map.prev_time = 40;
example_map.prev_cell = 1;
example_map.cells.num = 3;

example_map.cells.data = (time_min_t**)calloc(3, sizeof(time_min_t*));

for (int i = 0; i < 3; ++i) {
	example_map.cells.data[i] = calloc((5 * 2) + 1, sizeof(time_min_t));
	example_map.cells.data[i][0] = 5;
	for (int j = 0 + 1; j < (5 * 2) + 1; j += 2) {
		example_map.cells.data[i][j] = 15;
		example_map.cells.data[i][j+1] = 0xFFFF;
	}
}

saves_memory_upload(example_map);
*/