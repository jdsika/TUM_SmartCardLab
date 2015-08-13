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
  Fhreefish 1.0 - Fast Threefish for Atmel AVR 8-bit microcontrollers
 *********************************************************************


  Overview
  ========

  This is a highly optimized impleemntation of the Threefish encryption
  as published as part of the Skein hashing algorithm specification,
  version 1.2. For more information about Skein and Threefish, see
  http://www.skein-hash.info/
  
  Code size and speed statistics are available using the test program. You
  can save another 200 cycles per block by enabling some trickery. See file
  "threefish_macros.s" for details.

  Encryption is able to run in as little as 109 bytes of memory, including
  the memory used to pass plain-/ciphertext back and forth and stack
  space needed to comply to the C ABI

  You are not required to pass text inside the param struct. Input and
  output location are your choice, enabling zero-copy operation.

  A few caveats exist due to the optimization:
   - This implementation is not reentrant. This is probably irrelevant on
     such a limited device.
   - The param struct must be aligned to a 256 byte boundary. Since the
     compiler does not support such a large alignment, you must manually
     ensure it. Declaring it as the very first global const variable usually
     does the trick, then use a type-punned pointer to circumvent the
     const-ness. See test program for an example of this hack.
   - Interrupts must be disabled during en-/decryption, OR you must provide
     the stack space needed for your interrupt handlers right below the
     parameters struct. Alternatively, you can have the desired stack
     size included in the parameters struct, but this requires editing
     and recompiling the library. Search the source for THREEFISH_STACK.
   - Only 256 bit input/output size is supported.


  Building
  ========
  
  Compilation is straightforward. You need AVR-enabled binutils and GNU make.
  Edit "Makefile" for compiler/assembler flags, especially the MCU type.
  Optionallu edit "threefish_macros.s" to tweak advanced options. Then run "make".


  Using
  =====
  
  Add the build direcotry to the include and library search path. Add
  libfhreefish.a to your linker libraries. In your C source, #include <fhreefish.h>.


  API
  ===
  
  Threefish operates on Key, Tweak and (Plain/Cipher)text. The key is passed using
  a param struct which also serves as main temporary storage. The text is passed
  through pointers, and the param struct includes a field that may be used for
  text, supporting in-place en-/decryption. The tweak is passed in registers
  (function call parameters).
  There is no separate key setup. Key setup and scheduling is quite fast, and the
  main use of Threefish, namely Skein, changes the key on every invocation anyways.

  // Declare state
  threefish_param_t tparam; // double-check that the linker places this on a 256-byte boundary
  short tweak[8];
  char plaintext[32];
  char ciphertext[32];

  // Initialize state
  memcpy(tparam.key, your_key_bytes, 32);
  memset(tweak, 0, 16);
  strcpy(plaintext, "Hello World.");

  // Fully flexible en/decryption
  threefish_256_enc(&tparam, tweak[7], tweak[6], tweak[5], tweak[4], tweak[3], tweak[2], tweak[1], tweak[0], plaintext, ciphertext);
  threefish_256_dec(&tparam, tweak[7], tweak[6], tweak[5], tweak[4], tweak[3], tweak[2], tweak[1], tweak[0], ciphertext, plaintext);
  
  // Low-memory in-place usage -- plaintext and ciphertext are in tparam->text
  strcpy(tparam.text, "Hello World.");
  threefish_256_enci(&tparam, tweak[7], tweak[6], tweak[5], tweak[4], tweak[3], tweak[2], tweak[1], tweak[0]);
  threefish_256_deci(&tparam, tweak[7], tweak[6], tweak[5], tweak[4], tweak[3], tweak[2], tweak[1], tweak[0]);

*/

#ifndef FHREEFISH_H
#define FHREEFISH_H


/*
 Define the stack size your interrupt handlers need. This value must not
 exceed 186 bytes. Minimum value is 1.
 Note that you must change this value in threefish_macros.s as well and
 recompile the library for the new value.

 A different approach is to allocate stack space yourself, directly below
 the parameters struct. Advantage: no library recompilation. Disadvantage:
 more trouble getting alignment right.
*/
#define THREEFISH_STACK 4

/*
  Note the alignment constraint: The compiler is not able to ensure it,
  but I really mean it. The easiest way to ensure 256-byte alignment is to
  declare a param struct as the very first global variable in your main
  module.
*/
typedef struct threefish_param_struct {
	char stack[THREEFISH_STACK+4];
	char key[32];
	char text[32];
} threefish_param_t; // __attribute__((align(256)));
#ifndef I_KNOW_ABOUT_THREEFISH_ALIGNMENT_RESTRICTIONS
#warning "threefish_param_t must be 256-byte aligned. The compiler cannot do this for you. You must ensure this manually, sorry."
#endif

/* By using shorts, tweak0-7 are passed entirely in registers.
   (tweak0 & 0xff) is the first tweak byte, (tweak7 >> 8) is the last.
   Write them as 4-digit hex numbers and imagine they form a single
   little-endian 128-bit integer.
*/
extern void threefish_256_enc(threefish_param_t *param, short tweak7, short tweak6, short tweak5, short tweak4, short tweak3, short tweak2, short tweak1, short tweak0, const char *plaintext, char *ciphertext);
extern void threefish_256_dec(threefish_param_t *param, short tweak7, short tweak6, short tweak5, short tweak4, short tweak3, short tweak2, short tweak1, short tweak0, char *ciphertext, const char *plaintext);

/* Low memory in-place encryption convenience macros */
#define threefish_256_enci(p, t7, t6, t5, t4, t3, t2, t1, t0) \
	threefish_256_enc((p), (t7), (t6), (t5), (t4), (t3), (t2), (t1), (t0), ((p)->text), ((p)->text))
#define threefish_256_deci(p, t7, t6, t5, t4, t3, t2, t1, t0) \
	threefish_256_dec((p), (t7), (t6), (t5), (t4), (t3), (t2), (t1), (t0), ((p)->text), ((p)->text))

#endif
