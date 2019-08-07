#include "input.h"

#include <avr/io.h>

#define BP0 0b10000000
#define BP1 0b01000000
#define BP2 0b00100000
#define BPS (BP0 | BP1 | BP2)

#define SP3 0b00100000
#define SP2 0b01000000
#define SP1 0b00000100
#define SP0 0b00001000
#define SPS (SP0 | SP1 | SP2 | SP3)

static char s_input_buttons;
static unsigned int s_debounce_tick = 0;

void input_init(void)
{
	DDRA = (DDRA & ~BPS);
	DDRD = (DDRD & ~SPS);

	PORTA = BPS | (PORTA & ~BPS);
	PORTD = SPS | (PORTD & ~SPS);
}

char input_buttons(void)
{
	char i = ~PINA;
	char bA = (i & BP0) > 0;
	char bB = (i & BP1) > 0;
	char bC = (i & BP2) > 0;
	return (bB) | (bA<<1) | (bC<<2);
}

char input_debounce_buttons(void)
{
	char current_state = input_buttons();
	if (current_state != s_input_buttons)
	{
		if (s_debounce_tick > 100)
			s_input_buttons = current_state;
		s_debounce_tick = 0;
	}
	return s_input_buttons;
}

char input_switches(void)
{
	char i = ~PIND;
	char sA = (i & SP0) > 0;
	char sB = (i & SP1) > 0;
	char sC = (i & SP2) > 0;
	char sD = (i & SP3) > 0;
	return (sB) | (sA<<1) | (sD<<2) | (sC<<3);
}

void input_debounce_tick(void)
{
	if (s_debounce_tick != 0xFFFFU)
	{
		s_debounce_tick++;
	}
}

void input_debounce_released(void)
{
	while (input_debounce_buttons() != 0) {}
}
