/*
	AVR 8-bit microcontroller Skein hashing -- http://www.skein-hash.info/
	Copyright © 2010 Jörg Walter <info@syntax-k.de>

	This library is free software: you can redistribute it and/or modify
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

#define I_KNOW_ABOUT_THREEFISH_ALIGNMENT_RESTRICTIONS
#include "fkein.h"
#include <string.h>
#include <avr/pgmspace.h>
#include "ubi.h"
#include "debug.h"

static PROGMEM const char skein_default_iv[32] = { 
	0x49, 0xB4, 0x48, 0xD0, 0x60, 0xA8, 0x9D, 0xFC,
	0x33, 0xD8, 0xA7, 0x9F, 0x47, 0x66, 0xCA, 0x2F,
	0x0F, 0x84, 0x56, 0x66, 0x89, 0xC3, 0x3B, 0xB3,
	0x69, 0xDA, 0xE8, 0xFD, 0x20, 0xE9, 0x54, 0x6A 
};

char skein_iv[32];

void skein_256_256_default_iv()
{
	memcpy_P(skein_iv, skein_default_iv, 32);
}

void skein_256_256_personalize_M(threefish_param_t *tparam)
{
	unsigned char len = 31;
	memcpy_P(tparam->key, skein_default_iv, 32);
	while (len >= 0 && !tparam->text[len]) len--;
	ubi_iterate(tparam, tparam->text, 32, (SKEIN_SINGLE | Tprs) << 8, len+1, skein_iv);
}

void skein_256_256(threefish_param_t *tparam, const char *message, short message_len, char *out)
{
	char tweak7 = SKEIN_FIRST | Tmsg;
	short position_offset = 0;
	memcpy(tparam->key, skein_iv, 32);
	DEBUG_64(tparam->key, 4);

	while (message_len > 32) {
		position_offset += 32;
		message = ubi_iterate(tparam, message, 32, tweak7 << 8, position_offset, tparam->key);
		DEBUG_64(tparam->key, 4);
		tweak7 &= ~SKEIN_FIRST;
		message_len -= 32;
	}

	tweak7 |= SKEIN_LAST;
	position_offset += message_len;
	ubi_iterate(tparam, message, message_len, tweak7 << 8, position_offset, tparam->key);
	DEBUG_64(tparam->key, 4);

	ubi_iterate(tparam, NULL, 0, (SKEIN_SINGLE | Tout) << 8, 8, out);
	DEBUG_64(out, 4);
}

void skein_256_256_start(threefish_param_t *tparam)
{
	unsigned long *offset = (unsigned long *)tparam->stack;
	*offset = 0;
	tparam->stack[4] = SKEIN_FIRST;
	memcpy(tparam->key, skein_iv, 32);
	DEBUG_64(tparam->key, 4);
}

void skein_256_256_add(threefish_param_t *tparam, const char *message)
{
	unsigned long *offset = (unsigned long *)tparam->stack;
	char flags = tparam->stack[4];

	ubi_iterate(tparam, message, 32, (flags | Tmsg) << 8, (*offset)+=32, tparam->key);
	tparam->stack[4] = 0;
	DEBUG_64(tparam->key, 4);
}

void skein_256_256_finish(threefish_param_t *tparam, const char *message, char message_len, char *out)
{
	unsigned long *offset = (unsigned long *)tparam->stack;
	char flags = tparam->stack[4] | SKEIN_LAST;

	ubi_iterate(tparam, message, message_len, (flags | Tmsg) << 8, (*offset)+=message_len, tparam->key);
	DEBUG_64(tparam->key, 4);
	ubi_iterate(tparam, NULL, 0, (SKEIN_SINGLE | Tout) << 8, 8, out);
	DEBUG_64(out, 4);
}
