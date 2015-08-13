/*
 * rng.c
 *
 *  Created on: Jul 5, 2015
 *      Author: jojo
 */
#include "rng.h"
#include "fhreefish/fkein.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "macro.h"

extern volatile threefish_param_t* tparam;

uint8_t random_ring[RANDOM_MEMORY_POOL_SIZE];
uint16_t random_ring_start = 0;
uint16_t random_ring_end = 0;
uint8_t rng_counter = 0;

void rng_init()
{
	random_ring_start = 0;
	random_ring_end = 0;

	uint8_t seed[32];

	rng_get_seed_from_eeprom(seed);


	skein_256_256_default_iv();
	skein_rand_seed(tparam, (char*) seed);

	uint8_t position = rng_wrapper() % 16;
	uint8_t newseedbyte = rng_wrapper();

	rng_write_seed_to_eeprom_singlebyte(position, newseedbyte);
}

uint8_t rng_wrapper()
{
//	if(++rng_counter == 32)
//	{
//		skein_rand_next(tparam);
////		char newseed[32];
////		for(uint8_t i=0; i<32; ++i)
////		{
////			newseed[i] = skein_rand(tparam);
////		}
////		skein_rand_reseed(tparam, newseed);
//		rng_counter = 0;
//	}
	uint8_t out = skein_rand(tparam);
//	uart_putc(out);
	return out;
}

void rng_get_seed_from_eeprom(uint8_t* seed)
{
	uint8_t* baseaddr = 0;

	for(uint8_t i = 0; i < 16; i++)
	{
		seed[i] = eeprom_read_byte((uint8_t*) baseaddr++);
		seed[i+16] = seed[i];
//		uart_putc(seed[i]);
	}
}

uint8_t rng_get_seed_from_eeprom_singlebyte(uint8_t seed_index)
{
	uint8_t* baseaddr = 0 + seed_index;

	return eeprom_read_byte(baseaddr);
}

void rng_write_seed_to_eeprom_singlebyte(uint8_t seed_index, uint8_t seed)
{
	uint8_t* baseaddr = 0 + seed_index;
	eeprom_write_byte(baseaddr, seed);
}

void rng_entropy_ring_fill(uint16_t n)
{
	cli();
	while (n--)
	{
		if((random_ring_end+1) % RANDOM_MEMORY_POOL_SIZE == random_ring_start)
		{
			break;
		}

		random_ring[random_ring_end] = rng_wrapper(tparam);
		random_ring_end = (random_ring_end+1) % RANDOM_MEMORY_POOL_SIZE;
	}
	sei();
}

uint8_t rng_get_random_byte()
{
	if(random_ring_start != random_ring_end)
	{
		uint8_t entropy_val = random_ring[random_ring_start];
		random_ring_start = (random_ring_start+1) % RANDOM_MEMORY_POOL_SIZE;
		return entropy_val;
	}
	else
	{
//		assert(true, "random buffer underflow");
		return 0;
	}
}

uint16_t rng_buffer_fill_state()
{
	if(random_ring_end >= random_ring_start)
	{
		return random_ring_end-random_ring_start;
	}
	else
	{
		// RANDOM_MEMORY_POOL_SIZE - (random_ring_start - random_ring_end);
		return random_ring_end-random_ring_start + RANDOM_MEMORY_POOL_SIZE;
	}
}

