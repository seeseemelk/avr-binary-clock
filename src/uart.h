#ifndef UART_H
#define UART_H

#include "cdefs.h"

#include <stddef.h>

void uart_putc(u8 c);
void uart_put(const u8* c, size_t length);

#endif
