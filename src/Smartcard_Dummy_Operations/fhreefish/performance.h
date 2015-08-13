/*
	Performance measurement -- cycle counter
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

#include <avr/io.h>
#include <rprintf.h> // needs Procyon AVRlib

/* Usage:
	rprintfInit(some_output_func);
	
	TIME("main frobnication loop", frobnicate());
	
   Output:
	main frobnication loop: 1234 cycles
*/

#define TIMED_START() do { \
	int elapsed; \
	TCNT1 = 0;

#define TIMED_END(str, amount) \
	elapsed = TCNT1; \
	rprintfProgStrM("TIME: "); \
	rprintfNum(10, 5, 0, ' ', elapsed-1); \
	if (amount > 1) { \
		rprintfProgStrM(" ("); \
		rprintfNum(10, 4, 0, ' ', (elapsed-1)/amount); \
		rprintfProgStrM("): "); \
	} else { \
		rprintfProgStrM(" cycles: "); \
	} \
	rprintfProgStrM(str); \
	rprintfProgStrM("\n"); \
} while (0)

#define TIME(str, amount, expr) \
	TIMED_START(); \
	expr; \
	TIMED_END(str, amount)

#define TIME_INIT() do { \
	TCCR1A = 0; \
	TCCR1B = 1; \
} while (0)

#define STACK_CANARY 0xee

#define STACK_START() do { \
	static unsigned char *newstack, i; \
	{	newstack = (unsigned char *)SP; \
		for (i = 0; i < 0xff; i++) (*newstack--) = STACK_CANARY; \
	}

#define STACK_END(str) \
	for (i = 0, newstack = (unsigned char *)SP; i < 5; newstack--) { \
		if (*newstack == STACK_CANARY) i++; \
		else i = 0; \
	} \
	rprintfProgStrM("STACK: "); \
	rprintfNum(10, 4, 0, ' ', ((unsigned char *)SP)-newstack); \
	rprintfProgStrM(" bytes:  "); \
	rprintfProgStrM(str); \
	rprintfProgStrM("\n"); \
} while (0)
