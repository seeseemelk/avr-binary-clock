#include "dfplayer.h"

#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>

static u16 dfplayer_calculate_checksum(const u8* buffer, size_t length)
{
	u16 checksum = 0;
	for (size_t i = 0; i < length; i++)
	{
		checksum += (u16) buffer[i];
	}
	return -checksum;
}

static void dfplayer_send_command(u8 command, bool require_response, u16 parameters)
{
	u8 buffer[10];

	buffer[0] = 0x7E;
	buffer[1] = 0xFF;
	buffer[2] = 6;

	buffer[3] = command;
	buffer[4] = require_response;
	buffer[5] = parameters >> 8;
	buffer[6] = parameters;

	u16 checksum = dfplayer_calculate_checksum(buffer + 1, 6);
	buffer[7] = checksum >> 8;
	buffer[8] = checksum;
	buffer[9] = 0xEF;

	uart_put(buffer, sizeof(buffer));
}

void dfplayer_play_track(u16 track)
{
	dfplayer_send_command(0x03, false, track);
	_delay_ms(4);
}

void dfplayer_pause(void)
{
	dfplayer_send_command(0x0E, false, 0);
	_delay_ms(4);
}

void dfplayer_reset(void)
{
	dfplayer_send_command(0x0C, false, 0);
	_delay_ms(4);
}

void dfplayer_init(void)
{
	// 0.2% error when U2X = 1
	UBRRL = 0x0C;
	UBRRH = 0x00;

	UCSRA = (1 << U2X);
	UCSRB = (0 << RXCIE) | (0 << TXCIE) | (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
	_delay_ms(10);

	dfplayer_reset();
}
