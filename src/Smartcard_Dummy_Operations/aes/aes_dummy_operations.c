/*
 * aes_dummy_operations.c
 *
 *  Created on: Jul 10, 2015
 *      Author: gu95yiq
 */

#include "aes_dummy_operations.h"
#include "aes_invsbox.h"
#include "../rng.h"

#include <avr/pgmspace.h>

volatile uint8_t dummy_result;
uint8_t dummy_RK_operations = 1;
uint8_t dummy_SB_operations = 3;

static const uint8_t maximumOperations = 80;
static uint8_t completedOperations = 0;

/**********************************************************/
/***				 dummy operations					***/
/**********************************************************/
void aes_dummy_operations(uint8_t max)
{

	uint8_t counter = 0;
	uint8_t operations;

	// Important: Always use the same amount of operations per aes execution
	uint8_t todo = maximumOperations - completedOperations;

	if(todo == 0)
		return;

	uint8_t modulo;

	if (max > todo)
		modulo = todo;
	else
		modulo = max;

	if( max == 0) {
		operations = todo;
	}
	else {
		operations = rng_get_random_byte() % (modulo + 1);
	}

	// get a random value for the XOR operations
	uint8_t value = rng_get_random_byte();

	// this loop will at least execute once
	// operations = rng_get_random_byte() % (modulo + 1);
	do {
		// use arbitrary index for table lookup
		uint8_t rndIndex = rng_get_random_byte();

		// dummy operation should be similar to the
		// original SB operation
		value ^= pgm_read_byte(aes_invsbox+rndIndex);

		++completedOperations;
		++counter;
	} while (counter < operations);

	// XOR on volatile to avoid compiler optimization
	dummy_result ^= value;

	return;
}

void aes_reset_dummy_operation_counter()
{
	// reset number of operations
	completedOperations = 0;
	// new rdm value for the "result"
	dummy_result = rng_get_random_byte();
}
