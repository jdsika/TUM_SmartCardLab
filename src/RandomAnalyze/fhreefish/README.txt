Fhreefish - fast AVR 8-bit implementation of Threefish and Skein
================================================================

This project is a library for the popular Atmel AVR 8-bit microcontrollers.
It implements a high-performance version of the Skein hashing algorithm,
one of the candidates for the upcoming SHA-3 standard. The algorithm was
designed by a group of experienced cryptanalysts, most notably including
Bruce Schneier

There is already an implementation of Skein and the associated Threefish
encryption algorithm as part of AVR-Crypto-Lib. While that library is
feature-complete and offers all sizes/modes, it doesn't even remotely
meet the performance estimate presented in the Skein specification. For
this reason, I have reimplemented the core threefish algorithm in highly
optimized assembler. The Threefish primitive can run in about 8900 cycles,
which is 600 less than estimated.


Features / Specifications
-------------------------

 * Threefish-256 encryption and decryption
 * Skein-256-256 hashing
 * optional hash personalization
 * cryptographically secure PRNG based on Skein
 * not reentrant, but interrupt safe
 * no separate key setup required -- included in the performance figures
 * reasonable code size: 5.3kiB for all features, 2.4kiB for encryption only
 * low memory usage: about 100 bytes including input and output text
 * message size up to 2^32-1
 * flexible AVR-GCC C API and calling convention
 * zero-copy operation and in-place hashing/encryption available
 * buildable in AVR Studio or via GNU make
 * documentation in fkein.h and fhreefish.h
 * maintainable assembler source code


Building
--------

AVR Studio can be used, a sample project file is supplied. Using avr-gcc
and GNU make is the recommended way, you end up with a library
"libfhreefish.a", which can be used like any plain library is used.

The test program can be built using AVR Studio, but again, GNU make is
recommended. The Makefile runs it if you have a recent version of simulavr
installed. It will print various performance statistics and will complain
if results do not match the test vectors. This functionality definitely
requires simulavr. Running it in AVR Studio works, but misses the easily
readable output.

Try -O3 and -Os in CFLAGS for speed or size optimization. See
threefish_macros.s for temp variable optimization.


Using
-----

See fhreefish.h for details on usage in your programs.


Speed
-----

 * Threefish in 8900 cycles
 * Skein in 18700 cycles for short messages
 * 300 cycles per byte for long messages
 * PRNG in 285 cycles per byte (for big amounts)

All figures include the AVR C ABI function call overhead and use speed
optimized compiler options.

Actually, these figures could be even lower. During development, encryption
was at 8500 cycles, but the result was awkward to use. The current
implementation tries to balance flexibility and performance by sacrificing
a little speed.


Code Size
---------

 * Threefish encryption 2498 bytes, decryption 2522 bytes, both 4438 bytes
 * Skein-256 in 3200 bytes
 * PRNG in 3492 bytes
 * Full library in 5456 bytes

All figures exclude global variables. Program memory only. Size optimized
compiler options are used, yielding about 2% less performance in Skein and
PRNG. Speed optimization increases Skein and PRNG by about 1k each,
increasing the full library size to 7.2k.


Memory size
-----------

The global state structure used in all operations is at least 32+32+5 = 69
bytes. Note that for ease of debugging, the default state structure size is
72 bytes. See fhreefish.h to change that value. Another four bytes of
global variables are used, for a minimum total of 73 bytes.

Skein needs an external input buffer for message text, but can return the
calculated hash inside the state structure, so 32 bytes for the message
buffer must be calculated. In addition, the pre-calculated IV is cached in
RAM, needing another 32 bytes. This results 64 additional bytes of RAM.

PRNG needs 67 bytes of global variables on top of that, which includes the
Skein input buffer.

Stack usage (second figure below) was measured using simulavr in speed
optimized settings. The final results are:

 * Threefish in 73+36 = 109 bytes
 * Skein-256 in 73+64+64 = 201 bytes
 * PRNG in 73+64+32+67 = 236 bytes

The stats structure may be clobbered between individual calculations of a
hash value, making it possible to reuse it as temporary storage for other
program parts. External input buffers are likewise temporary. Subtracting
temporary memory, the permanent RAM usage is:

 * Threefish in 0 bytes
 * Skein-256 in 32 bytes
 * PRNG in 99 bytes

If you're willing to re-calculate the IV for every hash (317 cycles for
the default IV, 10k cycles for a personalized one), Skein drops to 0, while
PRNG needs just its internal state of 67 bytes.


License
-------

Copyright (C) 2010 Jörg Walter <info@syntax-k.de>

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
