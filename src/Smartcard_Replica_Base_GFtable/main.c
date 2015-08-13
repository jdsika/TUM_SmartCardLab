/*
 * main.c
 *
 *  Created on: May 6, 2015
 *      Author: jojo, laurenz
 */

#include "macro.h"
#include "uart.h"
#include "comm.h"
#include "protocol.h"

#include "aes/aes_keyschedule.h"

#include <avr/interrupt.h>


extern uint8_t key[16];
extern aes128_ctx_t ctx;

int main()
{
	// Setup
	led_init();
	trigger_init();
	uart_init();
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





