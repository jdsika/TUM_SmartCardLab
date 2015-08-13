/*
 * aes_masking.c
 *
 *  Created on: Jul 9, 2015
 *      Author: gu95yiq
 */

#include "../rng.h"
#include "aes_masking.h"
#include "aes_dec.h"
#include "aes_invsbox.h"

uint8_t mask_in = 0x00;
uint8_t mask_out = 0x00;

uint8_t mask14_in[4] = {0x00,0x00,0x00,0x00};
uint8_t mask14_out[4] = {0x00,0x00,0x00,0x00};

void aes128_mask_key(aes128_ctx_t* ctx, aes128_ctx_t* ctx_masked)
{
	for(uint8_t j = 0; j < 11; ++j) {
		for(uint8_t i = 0; i < 16 / 4; ++i) {
			ctx_masked->key[j].ks[4*i + 0] = ctx->key[j].ks[4*i + 0] ^ (mask14_out[0] ^ mask_in);
			ctx_masked->key[j].ks[4*i + 1] = ctx->key[j].ks[4*i + 1] ^ (mask14_out[1] ^ mask_in);
			ctx_masked->key[j].ks[4*i + 2] = ctx->key[j].ks[4*i + 2] ^ (mask14_out[2] ^ mask_in);
			ctx_masked->key[j].ks[4*i + 3] = ctx->key[j].ks[4*i + 3] ^ (mask14_out[3] ^ mask_in);
		}
	}
	return;
}

void aes128_precompute_masks()
{
	// always use same seed
	// input and output mask
	mask_in = rng_get_random_byte();
	//mask_out = rng_get_random_byte();
	mask_out = mask_in;

	// precompute the current modified inverse sbox
	// if this function is NOT called the original values are present in the sbox
	aes_update_invsbox(mask_in, mask_out);
	//aes_update_sbox(mask_in, mask_out);		<- THIS ONE IS NOT NEEDED IF THE KEY SCEDULING IS NOT MASKED

	// masks for mixed columns
	for(uint8_t i = 0; i < 4; ++i) {
		mask14_out[i] = rng_get_random_byte();
		mask14_in[i] = mask14_out[i];
	}

	aes_invmixed_columns(mask14_in, 4);

	return;
}

void aes128_mask_state(uint8_t* state)
{
	for(uint8_t i = 0; i < 16 / 4; ++i) {
		state[4*i + 0] ^= mask14_out[0] ^ mask_in ^ mask_out;
		state[4*i + 1] ^= mask14_out[1] ^ mask_in ^ mask_out;
		state[4*i + 2] ^= mask14_out[2] ^ mask_in ^ mask_out;
		state[4*i + 3] ^= mask14_out[3] ^ mask_in ^ mask_out;
	}
	return;
}

void aes128_invmask_state(uint8_t* state)
{
	for(uint8_t i = 0; i < 16 / 4; ++i) {
			state[4*i + 0] ^= mask14_out[0];
			state[4*i + 1] ^= mask14_out[1];
			state[4*i + 2] ^= mask14_out[2];
			state[4*i + 3] ^= mask14_out[3];
		}
	return;
}

void aes128_invremask(uint8_t* state)
{
	for(uint8_t i = 0; i < 16 / 4; ++i) {
		state[4*i + 0] ^= (mask14_in[0] ^ mask_out);
		state[4*i + 1] ^= (mask14_in[1] ^ mask_out);
		state[4*i + 2] ^= (mask14_in[2] ^ mask_out);
		state[4*i + 3] ^= (mask14_in[3] ^ mask_out);
	}
	return;
}
