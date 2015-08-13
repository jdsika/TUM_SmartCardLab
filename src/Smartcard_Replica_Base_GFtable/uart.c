/*
 * uart.c
 *
 *  Created on: May 7, 2015
 *      Author: ga68gug
 */

#include "uart.h"
#include "macro.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <string.h>


void uart_init()
{
	// Max Speed!
	UBRR0 = 0;
	SetBit(UCSR0A, U2X0);

	SetBit(UCSR0B, TXEN0); // enable transmit

	// use receive pin as debug pin instead
	SetBit(DDRD, PD0);
	ClrBit(PORTD, PD0);
}

void uart_putc(const char c)
{
	while (!GetBit(UCSR0A, UDRE0))
		; // wait until buffer free
	UDR0 = c;
}

void uart_puts(const char* s)
{
	while(*s) {
		uart_putc(*s++);
	}
}

void assert(bool cond, const char* error_string)
{
	if(!cond)
	{
		// Increment LED error counter
		if(PORTA == 0x00)
		{
			PORTA = 0x01;
		} else {
			PORTA ++;
		}

		// Print error message
		uart_puts(error_string);
	}
}
