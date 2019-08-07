#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>

char transmit_buffer[8];
size_t transmit_buffer_length = 0;
size_t transmit_buffer_index = 0;

void uart_putc(u8 c)
{
	while (!(UCSRA & (1 << UDRE))) {}
	UDR = c;
}

void uart_put(const u8* c, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		uart_putc(c[i]);
	}
}
