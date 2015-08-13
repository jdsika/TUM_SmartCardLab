/*
 * aes_shuffling.h
 *
 *  Created on: Jul 10, 2015
 *      Author: gu95yiq
 */

#ifndef AES_SHUFFLING_H_
#define AES_SHUFFLING_H_
#include <stdint.h>

extern uint8_t permutation_array[];

void aes128_update_shuffling_array();

#endif /* AES_SHUFFLING_H_ */
