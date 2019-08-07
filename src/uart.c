#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

char transmit_buffer[8];
size_t transmit_buffer_length = 0;
size_t transmit_buffer_index = 0;

/*static void transmit_character()
{
	while (!(UCSRA & (1 << UDRE))) {}
	if (transmit_buffer_length != transmit_buffer_index)
	{
		size_t send_index = transmit_buffer_index;
		transmit_buffer_index = (transmit_buffer_index + 1) % sizeof(transmit_buffer);
		UDR = transmit_buffer[send_index];
	}
}*/

/*ISR(USART_TXC_vect)
{
	transmit_character();
}*/

void uart_putc(u8 c)
{
	/*while ((transmit_buffer_length - 1) == transmit_buffer_index) {}
	transmit_buffer[transmit_buffer_length] = c;
	transmit_buffer_length = (transmit_buffer_length + 1) % sizeof(transmit_buffer);*/

	/*if (UCSRA & UDRE)
	{
		transmit_character();
	}*/
	//transmit_character();

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
