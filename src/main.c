#include "display.h"
#include "input.h"
#include "time.h"
#include "dfplayer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define STATE_DISPLAY 0
#define STATE_SET_TIME 1

enum State
{
	S_DISPLAY,
	S_SET_TIME,
	S_SET_ALARM,
	S_CALIBRATE
};

static enum State state = S_DISPLAY;

unsigned int seconds = 0;
ISR(TIMER2_OVF_vect)
{
	time_add_seconds(1);
	display_update();
}

ISR(TIMER0_COMP_vect)
{
	display_draw();
	input_debounce_tick();
}

int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

static void state_display(void)
{
	display_update();

	u8 buttons = input_debounce_buttons();

	if (buttons & BUTTON_TIME)
	{
		input_debounce_released();
		if (input_switches() & SWITCH_CALIBRATE)
		{
			state = S_CALIBRATE;
		}
		else
		{
			state = S_SET_TIME;
		}
	}
	else if (buttons & BUTTON_MUTE)
	{
		alarm_mute();
	}
	else if (buttons & BUTTON_ALARM)
	{
		input_debounce_released();
		state = S_SET_ALARM;
	}
	else if (input_switches() & SWITCH_CALIBRATE)
	{
		display_set_state(DS_CUSTOM);

		u16 calibration = time_get_calibration();

		u8 calibration_a = (calibration >> 12) & 0xF;
		u8 calibration_b = (calibration >> 8) & 0xF;
		u8 calibration_c = (calibration >> 4) & 0xF;
		u8 calibration_d = (calibration >> 0) & 0xF;

		display_set(calibration_a, calibration_b, calibration_c, calibration_d);
	}
	else if (input_switches() & SWITCH_SHOW_ALARM)
	{
		u8 hours, minutes;
		alarm_get(&hours, &minutes);

		display_set_state(DS_CUSTOM);
		display_set(hours / 10, hours % 10, minutes / 10,  minutes % 10);
	}
	else if (input_switches() & SWITCH_SHOW_SECONDS)
	{
		display_set_state(DS_MINUTES_SECONDS);
		alarm_test();
	}
	else
	{
		display_set_state(DS_HOURS_MINUTES);
		alarm_test();
	}
}

void flash_display(char hh, char ht, char mh, char mt, int index)
{
	static unsigned char count = 0;
	if (count > 3)
	{
		hh = (index == 0) ? 0 : hh;
		ht = (index == 1) ? 0 : ht;
		mh = (index == 2) ? 0 : mh;
		mt = (index == 3) ? 0 : mt;
	}
	display_set(hh, ht, mh, mt);
	count = (count + 1) % 5;
}

void state_set_time(void)
{
	display_set_state(DS_CUSTOM);

	u8 hours_h, hours_t, minutes_h, minutes_t;
	time_get_hours(&hours_h, &hours_t);
	time_get_minutes(&minutes_h, &minutes_t);

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		hours_h = input_switches() & 0b0111;
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 0);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		hours_t = min(input_switches(), 9);
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 1);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		minutes_h = min(input_switches(), 5);
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 2);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		minutes_t = min(input_switches(), 9);
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 3);
	}
	input_debounce_released();

	time_set(
			hours_h * 10 + hours_t,
			minutes_h * 10 + minutes_t,
			0
	);

	state = STATE_DISPLAY;
}

void state_set_alarm(void)
{
	display_set_state(DS_CUSTOM);

	u8 hours, minutes;
	alarm_get(&hours, &minutes);

	display_set(hours / 10, hours % 10, minutes / 10,  minutes % 10);

	u8 hours_h = hours / 10;
	u8 hours_t = hours % 10;
	u8 minutes_h = minutes / 10;
	u8 minutes_t = minutes % 10;

	while (input_debounce_buttons() != BUTTON_ALARM)
	{
		hours_h = input_switches() & 0b0111;
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 0);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_ALARM)
	{
		hours_t = min(input_switches(), 9);
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 1);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_ALARM)
	{
		minutes_h = min(input_switches(), 5);
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 2);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_ALARM)
	{
		minutes_t = min(input_switches(), 9);
		flash_display(hours_h, hours_t, minutes_h, minutes_t, 3);
	}

	alarm_set(
			hours_h * 10 + hours_t,
			minutes_h * 10 + minutes_t
	);

	state = STATE_DISPLAY;
	input_debounce_released();
}

void state_calibrate(void)
{
	display_set_state(DS_CUSTOM);

	u16 calibration = time_get_calibration();

	u8 calibration_a = (calibration >> 12) & 0xF;
	u8 calibration_b = (calibration >> 8) & 0xF;
	u8 calibration_c = (calibration >> 4) & 0xF;
	u8 calibration_d = (calibration >> 0) & 0xF;

	display_set(calibration_a, calibration_b, calibration_c, calibration_d);

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		calibration_a = input_switches();
		flash_display(calibration_a, calibration_b, calibration_c, calibration_d, 0);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		calibration_b = input_switches();
		flash_display(calibration_a, calibration_b, calibration_c, calibration_d, 1);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		calibration_c = input_switches();
		flash_display(calibration_a, calibration_b, calibration_c, calibration_d, 2);
	}
	input_debounce_released();

	while (input_debounce_buttons() != BUTTON_TIME)
	{
		calibration_d = input_switches();
		flash_display(calibration_a, calibration_b, calibration_c, calibration_d, 3);
	}
	input_debounce_released();

	time_set_calibration(
			(calibration_d) | (calibration_c << 4) | (calibration_b << 8) | (calibration_a << 12)
	);

	state = STATE_DISPLAY;
}

int main(void)
{
	cli();

	time_init();
	input_init();
	display_init();

	time_set(12, 48, 50);

	// Setup timer 2
	ASSR |= (1<<AS2);
	TCCR2 = (1<<CS22) | (0<<CS21) | (1<<CS20);
	TIMSK |= (1<<TOIE2);

	// Setup timer 0
	TCCR0 = (1<<WGM01) | (0<<CS02) | (1<<CS00);
	OCR0 = 0x01;
	TIMSK |= (1<<OCIE0);

	sei();

	dfplayer_init();

	while (1)
	{
		switch (state)
		{
			case S_DISPLAY:
				state_display();
				break;
			case S_SET_TIME:
				state_set_time();
				break;
			case S_SET_ALARM:
				state_set_alarm();
				break;
			case S_CALIBRATE:
				state_calibrate();
				break;
		}
	}
}
