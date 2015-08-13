/*
	Debugging helpers
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

	This program has no output. Set a breakpoint at the end of main()
	and inspect memory.
*/

#ifndef FHREEFISH_DEBUG_H
#define FHREEFISH_DEBUG_H

#ifdef DEBUG_FHREEFISH

#include <rprintf.h> // needs Procyon AVRlib

#define DEBUG_64(mem, count) do { \
	int i, j; \
	for (i = 0; i < count; i++) { \
		rprintfProgStrM(" "); \
		for (j = 0; j < 4; j++) rprintfu08(mem[i*8+7-j]); \
		rprintfProgStrM("."); \
		for (j = 0; j < 4; j++) rprintfu08(mem[i*8+3-j]); \
	} \
	rprintfProgStrM("\n"); \
} while (0)

#else

#define DEBUG_64(mem,count)

#endif

#endif