/*
 * aes_shuffling.c
 *
 *  Created on: Jul 10, 2015
 *      Author: gu95yiq
 */

#include "aes_shuffling.h"
#include "../rng.h"

uint8_t permutation_array[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


/**************************************************/
/***			Permutation Array				***/
/**************************************************/
void aes128_update_shuffling_array() {

	uint8_t i = 15; // array end
	uint8_t j = 0;  // init value
	do{
		//generate a random number [0, n-1]
		j = rng_get_random_byte() % (i+1);

		//swap the last element with element at random index
		uint8_t temp = permutation_array[i];
		permutation_array[i] = permutation_array[j];
		permutation_array[j] = temp;
		--i;
	} while ( i != 0);
}
