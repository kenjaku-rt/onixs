#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include <avr/io.h>

#include "bits.h"

#include "perph/twi.h"
#include "def_shortcuts.h"

#define DS1307_TWI_FREQ 100000U

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

typedef uint8_t rtc_data_t;
typedef struct {
	rtc_data_t sec;
	rtc_data_t min;
	rtc_data_t hour;
	rtc_data_t weekday;
	rtc_data_t date;
	rtc_data_t month;
	rtc_data_t year;
} rtc_time_t;
static rtc_time_t rtime;

static __attribute__((always_inline)) void rtc_init(void) {
	TWI_INIT(100000);

	TWI_START();
	TWI_WRITE(DS1307_WRITE_ADDR);
	TWI_WRITE(DS1307_REG_SECONDS);
	TWI_WRITE(BIN_TO_BCD(0) & 0x7F);  // CH = 0
	TWI_STOP();

	TWI_START();
	TWI_WRITE(DS1307_WRITE_ADDR);
	TWI_WRITE(DS1307_REG_CONTROL);
	TWI_WRITE(0x00);
	TWI_STOP();
}

static __attribute__((always_inline)) void rtc_set_time(rtc_time_t t) {
	TWI_START();
	TWI_WRITE(DS1307_WRITE_ADDR);
	TWI_WRITE(DS1307_REG_SECONDS);
	TWI_WRITE(BIN_TO_BCD(t.sec) & 0x7F);
	TWI_WRITE(BIN_TO_BCD(t.min));
	TWI_WRITE(BIN_TO_BCD(t.hour));
	TWI_WRITE(BIN_TO_BCD(t.weekday));
	TWI_WRITE(BIN_TO_BCD(t.date));
	TWI_WRITE(BIN_TO_BCD(t.month));
	TWI_WRITE(BIN_TO_BCD(t.year));
	TWI_STOP();
}

static __attribute__((always_inline)) void rtc_get_time(void) {
	TWI_START();
	TWI_WRITE(DS1307_WRITE_ADDR);
	TWI_WRITE(DS1307_REG_SECONDS);
	TWI_STOP();

	TWI_START();
	TWI_WRITE(DS1307_READ_ADDR);
	rtime.sec     = BCD_TO_BIN(TWI_READ_ACK() & 0x7F);
	rtime.min     = BCD_TO_BIN(TWI_READ_ACK());
	rtime.hour    = BCD_TO_BIN(TWI_READ_ACK() & 0x3F);
	rtime.weekday = BCD_TO_BIN(TWI_READ_ACK());
	rtime.date    = BCD_TO_BIN(TWI_READ_ACK());
	rtime.month   = BCD_TO_BIN(TWI_READ_ACK());
	rtime.year    = BCD_TO_BIN(TWI_READ_NACK());
	TWI_STOP();
}

#endif
