/*
 * main.c
 *
 *  Created on: May 6, 2015
 *      Author: jojo, laurenz
 */

#define I_KNOW_ABOUT_THREEFISH_ALIGNMENT_RESTRICTIONS
#include "fhreefish/fkein.h"

extern threefish_param_t tparam_asm;

volatile threefish_param_t* tparam;

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "aes/aes_keyschedule.h"

#include "macro.h"
#include "uart.h"
#include "comm.h"
#include "protocol.h"
#include "entropy.h"
#include "rng.h"


extern uint8_t key[16];
extern aes128_ctx_t ctx;

int main()
{
	tparam = (void*) &tparam_asm;

	// Setup
	led_init();
	trigger_init();
	entropy_init();
	uart_init();
	rng_init();
	comm_init();
	aes128_init(key, &ctx);

	// Globally enable interrupts
	sei();

	// Tell the cardreader we're there
	protocol_send_ATR();

	// Cardreader comm loop
	for(;;)
	{
		protocol_challenge_response();
	}
}





