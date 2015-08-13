/*
 * main.c
 *
 *  Created on: May 6, 2015
 *      Author: jojo, laurenz
 */

#define I_KNOW_ABOUT_THREEFISH_ALIGNMENT_RESTRICTIONS

#include "fhreefish/fkein.h"

asm(".global tparam_asm"); // have tparam be seen by everyone
asm(".data"); // mach im datensegment weiter
asm(".balign 256"); // pad location pointer to align by 256 bytes
asm("tparam_asm: .fill 72, 0"); // definiere tparam und speicher 72 0en dort
asm(".text"); // mach im programmcode weiter
extern threefish_param_t tparam_asm;

threefish_param_t* tparam;

#include <avr/interrupt.h>
#include <util/delay.h>

#include "macro.h"
#include "uart.h"
#include "comm.h"
#include "protocol.h"
#include "entropy.h"
#include "rng.h"


int main()
{
	tparam = (void*) &tparam_asm;
	// Setup
	led_init();
	trigger_init();
	entropy_init();
	uart_init();
	rng_init();

	// Globally enable interrupts
	sei();

	led_set(MCUSR);
	_delay_ms(200);
	led_set(0);
	_delay_ms(200);

	for (uint8_t i = 1; i != 0; i*=2) {
		led_set(i);
		_delay_ms(100);
	}


	for(;;)
	{
		entropy_eeprom_renew();


		uint8_t r = skein_rand(tparam);
		led_set(r);
		uart_putc(r);//(r%('~'-'0'))+'0');

	}
}





