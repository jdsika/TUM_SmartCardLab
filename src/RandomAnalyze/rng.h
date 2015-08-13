/*
 * rng.h
 *
 *  Created on: Jul 5, 2015
 *      Author: jojo
 */

#ifndef RNG_H_
#define RNG_H_
#include <inttypes.h>

#define RANDOM_MEMORY_POOL_SIZE 1025

void rng_init();
void rng_get_seed_from_eeprom(uint8_t* seed);
uint8_t rng_get_seed_from_eeprom_singlebyte(uint8_t seed_index);
void rng_write_seed_to_eeprom_singlebyte(uint8_t seed_index, uint8_t seed);
void rng_entropy_ring_fill(uint16_t n);
uint8_t rng_get_random_byte(); // <-------------------- use this one
uint16_t rng_buffer_fill_state();
uint8_t rng_wrapper();

#endif /* RNG_H_ */
