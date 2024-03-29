#include "time.h"
#include "dfplayer.h"
#include "input.h"
#include "cdefs.h"

#include <stdbool.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

static u8 s_hours, s_minutes, s_seconds;
static u8 s_hours_h, s_hours_t, s_minutes_h, s_minutes_t, s_seconds_h, s_seconds_t;
static u8 s_alarm_hours, s_alarm_minutes;
static u16 s_calibration;
static bool s_alarm_triggered = false;
static i16 s_seconds_until_calibration = 0;

static u8 EEMEM e_alarm_hours = 8;
static u8 EEMEM e_alarm_minutes = 30;
static u16 EEMEM e_calibration = 0;

static void time_split(void)
{
	s_seconds_t = s_seconds % 10;
	s_seconds_h = s_seconds / 10;

	s_minutes_t = s_minutes % 10;
	s_minutes_h = s_minutes / 10;

	s_hours_t = s_hours % 10;
	s_hours_h = s_hours / 10;
}

void time_init(void)
{
	s_calibration = eeprom_read_word(&e_calibration);
	s_seconds_until_calibration = s_calibration & 0x7FFF;

	s_alarm_hours = eeprom_read_byte(&e_alarm_hours);
	s_alarm_minutes = eeprom_read_byte(&e_alarm_minutes);
}

void time_set(u8 hours, u8 minutes, u8 seconds)
{
	s_seconds = seconds;
	s_minutes = minutes;
	s_hours = hours;

	time_split();
}

void time_add_seconds(u8 seconds)
{
	s_seconds_until_calibration -= seconds;
	if (s_seconds_until_calibration < 0 && s_calibration != 0)
	{
		s_seconds_until_calibration = s_calibration & 0x7FFF;
		if (s_calibration & 0x8000)
		{
			seconds--;
		}
		else
		{
			seconds++;
		}
	}

	s_seconds += seconds;
	s_minutes += s_seconds / 60;
	s_hours += s_minutes / 60;

	s_seconds %= 60;
	s_minutes %= 60;
	s_hours %= 24;

	time_split();
}

void time_get_hours(u8* hour_h, u8* hour_t)
{
	*hour_h = s_hours_h;
	*hour_t = s_hours_t;
}

void time_get_minutes(u8* minutes_h, u8* minutes_t)
{
	*minutes_h = s_minutes_h;
	*minutes_t = s_minutes_t;
}

void time_get_seconds(u8* seconds_h, u8* seconds_t)
{
	*seconds_h = s_seconds_h;
	*seconds_t = s_seconds_t;
}

void time_set_calibration(u16 calibration)
{
	s_calibration = calibration;

	cli();
	eeprom_update_word(&e_calibration, s_calibration);
	sei();
}

u16 time_get_calibration(void)
{
	return s_calibration;
}

void alarm_test(void)
{
	if ((input_switches() & SWITCH_ENABLE_ALARM) != 0 &&  s_alarm_hours == s_hours && s_alarm_minutes == s_minutes && !s_alarm_triggered)
	{
		dfplayer_play_track(1);
		s_alarm_triggered = true;
	}
}

void alarm_set(u8 hours, u8 minutes)
{
	s_alarm_hours = hours;
	s_alarm_minutes = minutes;

	cli();
	eeprom_update_byte(&e_alarm_hours, s_alarm_hours);
	eeprom_update_byte(&e_alarm_minutes, s_alarm_minutes);
	sei();
}

void alarm_get(u8* hours, u8* minutes)
{
	*hours = s_alarm_hours;
	*minutes = s_alarm_minutes;
}

void alarm_mute(void)
{
	dfplayer_pause();
	s_alarm_triggered = false;
}
