/* aes.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <string.h>
#include "gf256mul.h"
#include "aes_invsbox.h"
#include "aes_dec.h"
#include <avr/pgmspace.h>

// does not affect the masking process
void aes_invshiftcol(void* data, uint8_t shift)
{
	uint8_t tmp[4];
	tmp[0] = ((uint8_t*)data)[ 0];
	tmp[1] = ((uint8_t*)data)[ 4];
	tmp[2] = ((uint8_t*)data)[ 8];
	tmp[3] = ((uint8_t*)data)[12];
	((uint8_t*)data)[ 0] = tmp[(4-shift+0)&3];
	((uint8_t*)data)[ 4] = tmp[(4-shift+1)&3];
	((uint8_t*)data)[ 8] = tmp[(4-shift+2)&3];
	((uint8_t*)data)[12] = tmp[(4-shift+3)&3];
}

// MIXED Columns
void aes_invmixed_columns(uint8_t* s, uint8_t maxIndex)
{
	// Inverse mixed columns
	/* 14 11 13 09
	 * E  B  D  9
	 * 9  E  B  D
	 * D  9  E  B
	 * B  D  9  E
	 */

	uint8_t i;
	uint8_t t,u,v,w;

	/* mixColums */
	for(i = 0; i < maxIndex / 4; ++i){
		// a2 XOR a3
		t = s[4*i+3] ^ s[4*i+2];

		// a0 XOR a1
		u = s[4*i+1] ^ s[4*i+0];

		// a0 XOR a1 XOR a2 XOR a3		= A
		v = t ^ u;

		// 9 * A						= B
		v = gf256mul(0x09, v, 0x1b); 			// 0x1b strange modulo GF8

		// (4 * (a0 XOR a2)) XOR B		= C
		w = v ^ gf256mul(0x04, s[4*i+2] ^ s[4*i+0], 0x1b);

		// (4 * (a1 XOR a3)) XOR B		= D
		v = v ^ gf256mul(0x04, s[4*i+3] ^ s[4*i+1], 0x1b);

		// a3 XOR D XOR 2*(a0 XOR a3)	= b3
		s[4*i+3] = s[4*i+3] ^ v ^ gf256mul(0x02, s[4*i+0] ^ s[4*i+3], 0x1b);

		// a1 XOR D XOR 2*(a1 XOR a2)	= b1
		s[4*i+1] = s[4*i+1] ^ v ^ gf256mul(0x02, s[4*i+2] ^ s[4*i+1], 0x1b);

		// a2 XOR C XOR 2*(a3 XOR a2)	= b2
		s[4*i+2] = s[4*i+2] ^ w ^ gf256mul(0x02, t, 0x1b);

		// a0 XOR C XOR 2*(a0 XOR a1)
		s[4*i+0] = s[4*i+0] ^ w ^ gf256mul(0x02, u, 0x1b);
	}

	return;
}

// encryption: SB -> SR -> MC -> RK
// decryption: other way around
static
void aes_dec_round(aes_cipher_state_t* state, const aes_roundkey_t* k)
{
	uint8_t i;

	/* keyAdd */
	for(i=0; i<16; ++i){
		state->s[i] ^= k->ks[i]; //Bitwise Exclusive-Or (XOR)
	}

	// Inverse mixed columns
	/* 14 11 13 09
	 * E  B  D  9
	 * 9  E  B  D
	 * D  9  E  B
	 * B  D  9  E
	 */

	aes_invmixed_columns( state->s, 16);

	/* shiftRows */
	// WHY is shiftcol used here????
	aes_invshiftcol(state->s+1, 1);
	aes_invshiftcol(state->s+2, 2);
	aes_invshiftcol(state->s+3, 3);

	/* subBytes */
	for(i=0; i<16; ++i){
		state->s[i] = pgm_read_byte(aes_invsbox+state->s[i]);
	}
}


// encryption: SB -> SR -> RK
// decryption: other way around
static
void aes_dec_firstround(aes_cipher_state_t* state, const aes_roundkey_t* k){
	uint8_t i;
	/* keyAdd */
	for(i=0; i<16; ++i){
		state->s[i] ^= k->ks[i];
	}
	/* shiftRows */
	aes_invshiftcol(state->s+1, 1);
	aes_invshiftcol(state->s+2, 2);
	aes_invshiftcol(state->s+3, 3);		
	/* subBytes */
	for(i=0; i<16; ++i){
		state->s[i] = pgm_read_byte(aes_invsbox+state->s[i]);
	}
}

// when decrypting the algorithm has the opposite order compared to the encryption
// the PDF gives us the encryption direction
// the comments use the naming conventions from the PDF
void aes_decrypt_core(aes_cipher_state_t* state, const aes_genctx_t* ks, uint8_t rounds){
	uint8_t i;

	// buffer == state
	// aes_cipher_state_t represents the 4x4 byte cipher-matrix

	// "last round"
	aes_dec_firstround(state, &(ks->key[i=rounds])); // start with rounds == 10

	// 9 "normal rounds"
	// including "first round" and "second last round"
	for(;rounds>1;--rounds){
		// i is the counter for the roundkey which is applied backwards
		--i;
		aes_dec_round(state, &(ks->key[i]));
	}

	// "Initial Round"
	// adding roundkey
	for(i=0; i<16; ++i){
		state->s[i] ^= ks->key[0].ks[i];
	}
}
