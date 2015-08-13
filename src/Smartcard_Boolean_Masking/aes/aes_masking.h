/*
 * aes_masking.h
 *
 *  Created on: Jul 9, 2015
 *      Author: gu95yiq
 */

#ifndef AES_MASKING_H_
#define AES_MASKING_H_

#include <stdint.h>
#include "aes_types.h"

extern uint8_t mask_in;
extern uint8_t mask_out;

extern uint8_t mask14_in[];
extern uint8_t mask14_out[];

void aes128_precompute_masks();

void aes128_mask_state(uint8_t* state);
void aes128_invmask_state(uint8_t* state);

void aes128_invremask(uint8_t* state);

void aes128_mask_key(aes128_ctx_t* ctx, aes128_ctx_t* ctx_masked);

#endif /* AES_MASKING_H_ */
