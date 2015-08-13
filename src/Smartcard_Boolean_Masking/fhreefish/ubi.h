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

inline static const char *ubi_iterate(threefish_param_t *tparam, const char *message, unsigned char message_len, short tweak7, unsigned long position_offset, char *out)
{
	unsigned char i;
	const char *real_msg = message;

	if (message_len < 32) {
		memcpy(tparam->text, message, message_len);
		memset(tparam->text+message_len, 0, 32-message_len);
		message = tparam->text;
	}
	threefish_256_enc(tparam, tweak7, 0, 0, 0, 0, 0, position_offset >> 16, position_offset & 0xffff, message, out);
	for (i = 0; i < message_len; i++) out[i] ^= *real_msg++;
	return real_msg;
}

#define SKEIN_FIRST (0x40)
#define SKEIN_LAST (0x80)
#define SKEIN_SINGLE (SKEIN_FIRST|SKEIN_LAST)

#define Tcfg 4
#define Tprs 8
#define Tmsg 48
#define Tout 63

