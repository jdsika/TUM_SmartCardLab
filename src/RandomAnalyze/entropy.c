/*
 * entropy.c
 *
 *  Created on: May 12, 2015
 *      Author: jojo
 */

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <stdbool.h>
#include "entropy.h"
#include "macro.h"
#include "rng.h"
#include "uart.h"

#define WDT_MAX_8INT 0xFF
#define WDT_MAX_16INT 0xFFFF
#define WDT_MAX_32INT 0xFFFFFFFF
#define WDT_BUFFER_SIZE 32
#define ENTROPY_POOL_SIZE 4

uint8_t WDT_buffer[WDT_BUFFER_SIZE];
uint8_t WDT_buffer_position;

volatile uint8_t entropy_pool_start;
volatile uint8_t entropy_pool_count;
volatile uint32_t entropy_pool[ENTROPY_POOL_SIZE];
uint8_t new_seed_write_index=0;

bool entropy_already_renewed_in_session = false;

union ENTROPY_LONG_WORD
{
  uint32_t int32;
  uint16_t int16[2];
  uint8_t int8[4];
} ENTROPY_LONG_WORD;

union ENTROPY_LONG_WORD share_entropy;

// This interrupt service routine is called every time the WDT interrupt is triggered.
// With the default configuration that is approximately once every 16ms, producing
// approximately two 32-bit integer values every second.
//
// The pool is implemented as an 8 value circular buffer
volatile bool wdt_tcnt_buffer_ready = false;

ISR(WDT_vect) {
	if(!wdt_tcnt_buffer_ready)
	{
		WDT_buffer[WDT_buffer_position] = TCNT0;
		WDT_buffer_position++;// = (WDT_buffer_position+1) % WDT_BUFFER_SIZE; // every time the WDT interrupt is triggered

		if (WDT_buffer_position >= WDT_BUFFER_SIZE)
		{
			wdt_tcnt_buffer_ready = true;
			WDT_buffer_position = 0; // Start collecting the next 32 bytes of Timer 1 counts
		}
	}

	// don't go to system reset mode on the next wdt interrupt
	_WD_CONTROL_REG = _BV(WDIE);


}

void entropy_do_jenkins()
{
	if (wdt_tcnt_buffer_ready && entropy_pool_count < 4)
	{
		uint32_t hash = entropy_pool[entropy_pool_count];

		for (uint8_t i = 0; i < WDT_BUFFER_SIZE; ++i) {
			hash += WDT_buffer[i];
			hash +=	(hash << 10);
			hash ^=	(hash >> 6);
		}

		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);

		entropy_pool[entropy_pool_count] = hash;

		wdt_tcnt_buffer_ready = false;
		entropy_pool_count++;
	}
}


void entropy_eeprom_renew()
{

	if(entropy_already_renewed_in_session)
		return;

	// create random from cached watchdog-timer-jitters (if now already done)
	entropy_do_jenkins();

	// do we already have 4*32bits?
	if(entropy_pool_count >= 4) // number of unsigned long values in the entropy pool
	{
		uint8_t* addr = 0;

		for(uint8_t i = 0; i <4 ; i++)
		{
			uint32_t hash = entropy_pool[i];
			eeprom_write_byte(addr++, (hash & 0xFF000000)>>24);
			eeprom_write_byte(addr++, (hash & 0x00FF0000)>>16);
			eeprom_write_byte(addr++, (hash & 0x0000FF00)>> 8);
			eeprom_write_byte(addr++, (hash & 0x000000FF)    );
		}

		entropy_already_renewed_in_session = true;

		entropy_deinit();
	}
}

// This function initializes the global variables needed to implement the circular entropy pool and
// the buffer that holds the raw Timer 1 values that are used to create the entropy pool.  It then
// Initializes the Watch Dog Timer (WDT) to perform an interrupt every 2048 clock cycles, (about
// 16 ms) which is as fast as it can be set.
void entropy_init() {
	WDT_buffer_position = 0;
	entropy_pool_start = 0;
	entropy_pool_count = 0;

	MCUSR = 0; // Use the MCU status register to reset flags for WDR, BOR, EXTR, and POWR
	// WDT control register, This sets the Watchdog Change Enable (WDCE) flag, which is
	// needed to set the Watchdog system reset (WDE) enable and the Watchdog interrupt enable (WDIE)
	_WD_CONTROL_REG |= _BV(_WD_CHANGE_BIT) | _BV(WDE);
	_WD_CONTROL_REG = _BV(WDIE);

	TCNT0 = 0;
	SetBit(TCCR0B, CS00);
}

void entropy_deinit()
{
	// Stop Timer0
	ClrBit(TCCR0B, CS00);

	// Stop WDT
	MCUSR = 0; // Use the MCU status register to reset flags for WDR, BOR, EXTR, and POWR
	// WDT control register, This sets the Watchdog Change Enable (WDCE) flag, which is
	// needed to set the Watchdog system reset (WDE) enable and the Watchdog interrupt enable (WDIE)
	_WD_CONTROL_REG |= _BV(_WD_CHANGE_BIT) & ~_BV(WDE);
	_WD_CONTROL_REG = 0x00;//_BV(WDIE);

}
