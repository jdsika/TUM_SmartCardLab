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
#include "ubi.h"

static struct {
	char state[32];
	unsigned short block;
} data;

unsigned char skein_rand_len;
char skein_rand_data[32];

void skein_rand_seed(threefish_param_t *tparam, char seed[32])
{
	memset(data.state, 0, sizeof(data.state));
	skein_rand_reseed(tparam, seed);
}

void skein_rand_reseed(threefish_param_t *tparam, char seed[32])
{
	memcpy(skein_rand_data, seed, 32);
	skein_256_256(tparam, data.state, 64, data.state);
	data.block = 255;
	skein_rand_len = 0;
}

void skein_rand_restart(threefish_param_t *tparam)
{
	// kopiere data.state nach tparam.key
	memcpy(tparam->key, data.state, 32);

	// setze block zurück
	data.block = 0;

	// hasht message [data.block(b0-b7), data.block(b8-b15), 0, 0, 0, ...],
	// also [0, 0, 0, 0, 0, ...]
	// mit dem neuem key (= vorheriger data.state)
	// und schreibe ergebnis nach data.state
	ubi_iterate(tparam, (char *)&data.block, 2, Tout|SKEIN_SINGLE, 0, data.state);

	// setze buffer fill auf 0 damit skein_rand_256 benutzt wird um den buffer zu füllen
	skein_rand_len = 0;
}

unsigned char skein_rand_full(threefish_param_t *tparam)
{
	if (!skein_rand_len) {
		skein_rand_256(tparam, skein_rand_data);
		skein_rand_len = 32;
	}
	return skein_rand_data[--skein_rand_len];
}

void skein_rand_256(threefish_param_t *tparam, char output[32])
{
	if (data.block == 255) skein_rand_restart(tparam);
	data.block++;

	// hasht message [data.block(b0-b7), data.block(b8-b15), 0, 0, 0, ...],
	// also [(1-255), 0, 0, 0, 0, ...]
	// mit dem aktuellen key (= vorheriger data.state)
	// und schreibe ergebnis nach output
	ubi_iterate(tparam, (char *)&data.block, 2, Tout|SKEIN_SINGLE, 0, output);
}

