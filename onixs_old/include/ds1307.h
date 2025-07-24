#ifndef DS1307_H_
#define DS1307_H_

#include <avr/io.h>
#include <stdint.h>

#include "i2c.h"

#define DS1307_BASE_ADDR     0x68U  // 0b1101000
#define DS1307_WRITE_ADDR    (DS1307_BASE_ADDR << 1)
#define DS1307_READ_ADDR     ((DS1307_BASE_ADDR << 1) | 1)

#define DS1307_REG_SECONDS   0x00U
#define DS1307_REG_MINUTES   0x01U
#define DS1307_REG_HOURS     0x02U
#define DS1307_REG_WEEKDAY   0x03U
#define DS1307_REG_DATE      0x04U
#define DS1307_REG_MONTH     0x05U
#define DS1307_REG_YEAR      0x06U
#define DS1307_REG_CONTROL   0x07U

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t weekday;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} ds1307_time_t;

static ds1307_time_t rtime;

static uint8_t bcd_to_bin(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t bin_to_bcd(uint8_t bin) {
	return ((bin / 10) << 4) | (bin % 10);
}

uint16_t rtime_diff_min(uint8_t current_hours, uint8_t current_mins, uint8_t prev_hours, uint8_t prev_mins) {
	return ((current_hours < prev_hours) ? 24 - prev_hours + current_hours : current_hours - prev_hours) 
	* 60 + ((current_mins < prev_mins  ) ? 60 - prev_mins  + current_mins  : current_mins  - prev_mins);
}

void ds1307_init(void) {
	 i2c_init(100000);

	 i2c_start();
	 i2c_write(DS1307_WRITE_ADDR);
	 i2c_write(DS1307_REG_SECONDS);
	 i2c_write(bin_to_bcd(0) & 0x7F);  // CH = 0
	 i2c_stop();

	 i2c_start();
	 i2c_write(DS1307_WRITE_ADDR);
	 i2c_write(DS1307_REG_CONTROL);
	 i2c_write(0x00);
	 i2c_stop();
}

void ds1307_timer_interrupt_enable() {
	TCCR1B |= _BV(WGM12) | _BV(CS10) | _BV(CS12);
	TIMSK |= _BV(OCIE1A);
	OCR1A = 1952;
}

void ds1307_set_time(ds1307_time_t t) {
	i2c_start();
	i2c_write(DS1307_WRITE_ADDR);
	i2c_write(DS1307_REG_SECONDS);
	i2c_write(bin_to_bcd(t.sec) & 0x7F);
	i2c_write(bin_to_bcd(t.min));
	i2c_write(bin_to_bcd(t.hour));
	i2c_write(bin_to_bcd(t.weekday));
	i2c_write(bin_to_bcd(t.date));
	i2c_write(bin_to_bcd(t.month));
	i2c_write(bin_to_bcd(t.year));
	i2c_stop();
}


ds1307_time_t ds1307_get_time(void) {
	ds1307_time_t t;

	i2c_start();
	i2c_write(DS1307_WRITE_ADDR);
	i2c_write(DS1307_REG_SECONDS);
	i2c_stop();

	i2c_start();
	i2c_write(DS1307_READ_ADDR);
	t.sec     = bcd_to_bin(i2c_read_ack() & 0x7F);
	t.min     = bcd_to_bin(i2c_read_ack());
	t.hour    = bcd_to_bin(i2c_read_ack() & 0x3F);
	t.weekday = bcd_to_bin(i2c_read_ack());
	t.date    = bcd_to_bin(i2c_read_ack());
	t.month   = bcd_to_bin(i2c_read_ack());
	t.year    = bcd_to_bin(i2c_read_nack());
	i2c_stop();

	return t;
}

uint8_t ds1307_get_minutes(void) {
	i2c_start();
	i2c_write(DS1307_WRITE_ADDR);
	i2c_write(DS1307_REG_MINUTES);
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	i2c_write(DS1307_READ_ADDR);
	uint8_t raw = i2c_read_nack();
	i2c_stop();
	return bcd_to_bin(raw);
}



#endif /* DS1307_H_ */
