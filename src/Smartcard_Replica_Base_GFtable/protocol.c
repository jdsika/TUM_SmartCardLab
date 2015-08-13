/*
 * protocol.c
 *
 *  Created on: May 6, 2015
 *      Author: jojo, laurenz
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>

#include "aes/aes128_dec.h"
#include "aes/aes_keyschedule.h"
#include "protocol.h"
#include "comm.h"
#include "macro.h"
#include "uart.h"


const uint8_t key[16] = {0x64, 0x89, 0x21, 0x15, 0x88, 0xB4, 0xCE, 0xE0, 0x18, 0x4A, 0x14, 0x44, 0xE4, 0xDF, 0xC7, 0x58};
aes128_ctx_t ctx;


void protocol_challenge_response()
{
	uint8_t auth_challenge[16];

	protocol_get_auth_challenge(auth_challenge);

	uint8_t* auth_response = auth_challenge;

	// Decrypt, and help attackers by pulling the trigger line high during aes.
	trigger_set();
	aes128_dec(auth_challenge, &ctx);
	trigger_clr();

	uint8_t response[] = {0x61, 0x10};
	comm_transmit_bytes(response, SIZE(response));

	protocol_send_auth_response(auth_response);
}


void protocol_get_auth_challenge(uint8_t* auth_challenge)
{
	const uint8_t expected_request[] = {0x88, 0x10, 0x00, 0x00, 0x10};
	protocol_receive_expected_data(expected_request, SIZE(expected_request));

	for(int i = 0; i < 16; i++)
	{
		comm_transmit_byte(0xEF);
		auth_challenge[i] = comm_receive_byte();
	}

}

void protocol_send_auth_response(uint8_t* auth_response)
{
	uint8_t expected_request[] = {0x88, 0xC0, 0x00, 0x00, 0x10};
	protocol_receive_expected_data(expected_request, SIZE(expected_request));

	comm_transmit_byte(0xC0);
	comm_transmit_bytes(auth_response, 16); // we know that auth_response will always be 16bytes long.

	const uint8_t response[] = {0x90, 0x00};
	comm_transmit_bytes((uint8_t*) response, SIZE(response));
}

void protocol_receive_expected_data(const uint8_t* data, const uint8_t data_length)
{
	for(int i = 0; i < data_length; i++)
	{
		uint8_t byte_expected = data[i];
		uint8_t byte_received = comm_receive_byte();
		if(byte_expected != byte_received){
			char buf[50];
			sprintf(buf, "Received %02X instead of %02X at byte %d", byte_received, byte_expected, i);
			assert(false, buf); // Receive Unexpected Data
		}
	}
}

void protocol_send_ATR()
{
	const uint8_t atr[] = {0x3B, 0x90, 0x11, 0x00}; // Standard ATR Code
	comm_transmit_bytes((uint8_t*) atr, SIZE(atr));
}
