#include "display.h"

#include "time.h"
#include "input.h"

#include <avr/io.h>
#include <util/delay.h>

static enum DisplayState s_state;
static u8 s_display[] = {0, 0, 0, 0};

#define DISPLAY_HOUR_H s_display[0]
#define DISPLAY_HOUR_T s_display[2]
#define DISPLAY_MINUTES_H s_display[1]
#define DISPLAY_MINUTES_T s_display[3]

#define DPA 0b1111
#define DPB 0b11110

void display_init(void)
{
	DISPLAY_HOUR_H = 1;
	DISPLAY_HOUR_T = 2;
	DISPLAY_MINUTES_H = 4;
	DISPLAY_MINUTES_T = 8;

	DDRB = DPB | (DDRB & ~DPB);
	DDRA = DPA | (DDRA & ~DPA);
}

void display_draw(void)
{
	static int row = 0;

	char show = s_display[row];
	PORTB = (PORTA & 0b11100001);
	PORTA = (PORTA & 0b11110000) | ((~show) & 0b1111);
	PORTB = (PORTB & 0b11100001) | (((1 << row)&0b1111)<<1);
	row = (row+1) % 4;
}

void display_update(void)
{
	switch (s_state)
	{
	case DS_HOURS_MINUTES: {
		u8 hour_h, hour_t, minutes_h, minutes_t;
		time_get_hours(&hour_h, &hour_t);
		time_get_minutes(&minutes_h, &minutes_t);
		if (input_switches() & SWITCH_ENABLE_ALARM)
		{
			hour_h |= 0b1000;
		}
		DISPLAY_HOUR_H = hour_h;
		DISPLAY_HOUR_T = hour_t;
		DISPLAY_MINUTES_H = minutes_h;
		DISPLAY_MINUTES_T = minutes_t;
		break;
	}
	case DS_MINUTES_SECONDS:
		time_get_minutes(&DISPLAY_HOUR_H, &DISPLAY_HOUR_T);
		time_get_seconds(&DISPLAY_MINUTES_H, &DISPLAY_MINUTES_T);
		break;
	case DS_CUSTOM:
		break;
	}
}

void display_set_state(enum DisplayState new_state)
{
	s_state = new_state;
}

void display_set(u8 a, u8 b, u8 c, u8 d)
{
	DISPLAY_HOUR_H = a;
	DISPLAY_HOUR_T = b;
	DISPLAY_MINUTES_H = c;
	DISPLAY_MINUTES_T = d;
}
