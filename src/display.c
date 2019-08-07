/*
 * display.c
 *
 *  Created on: Oct 30, 2016
 *      Author: seeseemelk
 */

#include "display.h"

#include <avr/io.h>
#include <util/delay.h>

#include "time.h"
#include "input.h"

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
	DISPLAY_HOUR_H = 1; // 0b0110
	DISPLAY_HOUR_T = 2;  // 0b1001
	DISPLAY_MINUTES_H = 4;  // 0b1001
	DISPLAY_MINUTES_T = 8; // 0b1111

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
	//char display[4];

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
	/*for (int i = 0; i < 4; i++)
	{
		char show = display[i];

		PORTB = (PORTA & 0b11100001);
		PORTA = (PORTA & 0b11110000) | (((~show) & 0b1111));
		PORTB = (PORTB & 0b11100001) | (((((1 << i))&0b1111))<<1);

		_delay_ms(2);
	}*/
}

void display_draw_for(int ms)
{
	while (ms > 0)
	{
		display_draw();
		ms -= 5 * 4;
	}
}

void display_set_state(enum DisplayState new_state)
{
	s_state = new_state;
}

void display_set(char hoursH, char hoursT, char minutesH, char minutesT)
{
	DISPLAY_HOUR_H = hoursH;
	DISPLAY_HOUR_T = hoursT;
	DISPLAY_MINUTES_H = minutesH;
	DISPLAY_MINUTES_T = minutesT;
}
