/*
 * hiding.c
 *
 *  Created on: May 28, 2015
 *      Author: ga56xak
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "macro.h"
#include "entropy.h"
#include "hiding.h"
#include "uart.h"

bool eeprom_rewritten = false;

ISR(TIMER0_COMPA_vect)
{
	SetBit(PORTD, PD0);

	uint8_t r = rand() & 0xFF;
//	_delay_loop_1(r); //% (MAX_DELAY_IN_HIDING_INT/3)
	OCR0A = r; //random() & 0xFF; //MIN_DELAY_BETWEEN_HIDING_INT + (random() % (MAX_DELAY_BETWEEN_HIDING_INT-MIN_DELAY_BETWEEN_HIDING_INT));

	ClrBit(PORTD, PD0);
}

/*
void hiding_init()
{
	SetBit(PORTD, PD0);

	SetBit(TCCR0A, WGM01);
	SetBit(TIMSK0, OCIE0A);

	uint32_t seed = eeprom_read_dword((uint32_t*) 0);
	srandom(seed);
	srand(seed);
	uint32_t new_seed = random();
//
//	char buf[100];
//	sprintf(buf, "0x%08lX -> 0x%08lX", seed, new_seed);
//	uart_puts(buf);

	eeprom_write_dword((uint32_t*) 0, new_seed);

//	uint32_t test = eeprom_read_dword((uint32_t*) 0);
//	if(test == new_seed) uart_puts("Hooray for EEMEM\n");
//	else uart_puts("EEMEM is fucked up!\n");

	ClrBit(PORTD, PD0);
}
*/
void hiding_start_timer()
{
}

void hiding_stop_timer()
{
	ClrBit(TCCR0B, CS00);
}

