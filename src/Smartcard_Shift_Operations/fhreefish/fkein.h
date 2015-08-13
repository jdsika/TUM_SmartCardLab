/*
	AVR 8-bit microcontroller Threefish primitive -- http://www.skein-hash.info/
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

/*********************************************************************
  Fkein 1.2 - Fast Skein hashing for Atmel AVR 8-bit microcontrollers
 *********************************************************************


  Overview
  ========

  This is an implementation of the Skein hash function based on the
  Fhreefish implementation of Threefish. It supports simple hashing
  using Skein-256-256 with or without personalization. No other modes
  of operation are currently implemented. Currently, the Skein
  specification version 1.2 is implemented.

  Note that none of the functions in this library are reentrant. This
  should pose no problem given the complexity of the algorithm -- you
  wouldn't want to run it from interrupt handlers anyways.


  Building
  ========
  
  See fhreefish.h for details.


  Using
  =====
  
  Add the build direcotry to the include and library search path. Add
  libfhreefish.a to your linker libraries. In your C source, #include <fkein.h>.


  API
  ===

  IMPORTANT: The Fhreefish implementation manipulates the stack pointer.
  If you want to call any of these functions with interrupts enabled, you
  need to make special preparations. See fhreefish.h for details. This
  also means that interrupts are disabled during some parts of the
  operation. Interrupts are disabled for up to 200 cycles.

  All function calls use temporary work space. It doesn't need to be
  initialized, but it MUST be 256-byte aligned. The compiler cannot
  ensure this, so you must check it manually. The best way to get the
  required alignment is to declare the variable as the very first global
  const variable in your main source file, then using a type-punned pointer
  to that struct:
  
  const threefish_param_t tparam_hack;
  threefish_param_t tparam = (threefish_param_t *) tparam_hack;

  Hashing happens in two steps. First, a personalized IV is calculated:
  
  skein_256_256_personalize(tparam, "20100202 test@example.com FOO/bar");

  Then calculate any number of hash values:
  
  char message[32], result[32];
  short message_len;
  skein_256_256(tparam, message, message_len, result);

  If the message to be hashed exceeds available memory, use incremental mode:
  
  char buffer[33];
  skein_256_256_start(tparam);
  message_len = fetch_up_to_33_bytes(buffer)-1;
  while (message_len == 32) {
      skein_256_256_add(tparam, buffer);
      buffer[0] = buffer[32];
      message_len = fetch_up_to_32_bytes(buffer+1);
  }
  skein_256_256_finish(tparam, buffer, message_len+1, result);

  If you are absolutely sure you don't want personalization (using
  personalization increases security against cross-protocol attacks), you
  can use the plain IV instead:

  skein_256_256_default_iv();

  Output doesn't require an external buffer. You can use part of tparam for
  the return value:
  
  skein_256_256(tparam, message, message_len, tparam->text);

  This library also includes the PRNG application of Skein as described
  in the Skein spec, using 128 bit of internal state:

  char rnd, rnd_block[32];
  char seed[32];
  // initial seed
  skein_rand_seed(tparam, seed);
  // fast operation
  rnd = skein_rand(tparam);
  rnd = skein_rand(tparam);
  rnd = skein_rand(tparam);
  // ...

  // add new randomness to the existing state
  skein_rand_reseed(tparam, seed);
  rnd = skein_rand(tparam);

  // secure operation: overwrite state after each block of data
  skein_rand_restart(tparam);
  rnd = skein_rand(tparam);
  rnd = skein_rand(tparam);
  rnd = skein_rand(tparam);
  skein_rand_clear(tparam);

  // fetch more random data, this time a whole 32-byte block at once
  skein_rand_restart(tparam);
  rnd = skein_rand256(tparam, rnd_block);
  skein_rand_clear(tparam);
  // ...
  // Note: You _must_ call skein_rand_restart if you use tparam for something else
  // between two calls to skein_rand.

*/

#ifndef FKEIN_H
#define FKEIN_H

#include "fhreefish.h"
#include <avr/pgmspace.h>

#define SKEIN_VERSION 1.2.2

extern void skein_256_256_personalize_M(threefish_param_t *tparam);
#define skein_256_256_personalize(tparam, p) do { memset((tparam)->text, 0, 32); strcpy_P((tparam)->text, PSTR((p))); skein_256_256_personalize_M((tparam)); } while (0)
extern void skein_256_256_default_iv();

extern void skein_256_256(threefish_param_t *tparam, const char *message, short message_len, char *out);

extern void skein_256_256_start(threefish_param_t *tparam);
extern void skein_256_256_add(threefish_param_t *tparam, const char *message);
extern void skein_256_256_finish(threefish_param_t *tparam, const char *message, char message_len, char *out);

extern unsigned char skein_rand_len;
extern char skein_rand_data[32];

#define skein_rand(tparam) (skein_rand_len?skein_rand_data[--skein_rand_len]:skein_rand_full(tparam))
extern unsigned char skein_rand_full(threefish_param_t *tparam);
extern void skein_rand_256(threefish_param_t *tparam, char output[32]);
extern void skein_rand_restart(threefish_param_t *tparam);
extern void skein_rand_seed(threefish_param_t *tparam, char seed[32]);
extern void skein_rand_reseed(threefish_param_t *tparam, char seed[32]);
#define skein_rand_clear(ptparam) (memset(ptparam->key, 0, sizeof(ptparam->key)))

#endif
