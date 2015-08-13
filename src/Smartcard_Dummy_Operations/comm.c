/*
 * comm.c
 *
 *  Created on: May 7, 2015
 *      Author: ga68gug
 */

#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/parity.h>

#include <string.h>

#include "macro.h"
#include "comm.h"
#include "uart.h"

enum mode_t {RECEIVING, TRANSMITTING};

volatile enum mode_t mode = RECEIVING;
volatile uint8_t bit_nr_being_received = 0;
volatile uint8_t byte_being_received = 0x00;
volatile bool byte_is_received = false;
volatile bool next_bit_to_send = false;
volatile bool bit_is_sent = true; // init true so we can send immediately


// Pin Change Interrupt Request 1
ISR(PCINT1_vect)
{
	if(bit_nr_being_received != 0)
	{
		// this should not happen: a start bit coming in, even though we did not receive all bits
		return;
	}

	if(GetBit(PINB, PB6) != 0)
	{
		// this is a rising edge, not a falling one.
		// this can mean that we are done with receiving of a byte and notice the trailing edge of the parity bit.
		// or it is bad, in case we missed a byte.
		return;
	}

	if(mode == RECEIVING)
	{
		// this is the startbit (low). the next 8 bits are data, then 1 even parity. we'll get them in the following timer interrupts
		SetBit(TCCR1B, CS10); // Set clock source to CPU/1 = enable timer
		TCNT1 = 0;

		bit_nr_being_received = 0;
		byte_being_received = 0x00;

		OCR1A = 372*1.50-120; // wait 1.5 ETU to sample in the middle of bit 0. subtract about 120 cycles for interrupt setup things

		// don't come here again before we completely received this byte
		ClrBit(PCICR,PCIE1); // disable pin change interrupt
		ClrBit(PCMSK1,PCINT14); // disable pin change interrupt for PDI_DATA pin
	}
}


ISR(TIMER1_COMPA_vect)
{
	switch(mode)
	{

	case RECEIVING:
		if(bit_nr_being_received == 0)
		{
			OCR1A = 372; // this will happen in the middle of bit 0. from now on, sample every 1 ETU.
		}

		if(bit_nr_being_received < 8) // data bit
		{
			// sample 5 times and do a majority decision
			int8_t r = 0;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r = (r>0)?1:0;

			// save the bit
			byte_being_received |= (r<<(bit_nr_being_received++));
		}
		else if (bit_nr_being_received == 8) // parity bit
		{
			// sample 5 times and do a majority decision
			int8_t r = 0;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r += (GetBit(PINB, PB6))?1:-1;
			r = (r>0)?1:0;

			assert(r == parity_even_bit(byte_being_received), "WP"); // Wrong Parity

			// we are done with this byte
			byte_is_received = true;
			bit_nr_being_received = 0;
			ClrBit(TCCR1B, CS10); // Set clock source to none = disable timer
		}
		break;


	case TRANSMITTING:
		if(next_bit_to_send)
		{
			// Send 1 / idle / high / high-Z bit by changing pin to input (and maybe activating internal pullup)
			ClrBit(DDRB, PB6);
		} else {
			// Send 0 / active / low / pull-down by changing pin to output and driving low
			SetBit(DDRB, PB6);
			ClrBit(PORTB, PB6);
		}
		bit_is_sent = true;
		break;

	}
}

void set_mode_receive()
{
	// In Empfangsmodus gehen
	mode = RECEIVING;
	byte_is_received = false;
	ClrBit(DDRB, PB6); // set PDI_DATA as input
	SetBit(PORTB, PB6); // interne Pullups aktivieren
	SetBit(PCICR,PCIE1); // enable pin change interrupt
	SetBit(PCMSK1,PCINT14); // enable pin change interrupt for PDI_DATA pin
}

void set_mode_transmit()
{
	// Empfangsmodus verlassen
	mode = TRANSMITTING;
	ClrBit(PCICR,PCIE1); // disable pin change interrupt
	ClrBit(PCMSK1,PCINT14); // disable pin change interrupt for PDI_DATA pin
	SetBit(DDRB, PB6); // set PDI_DATA as output
}

void comm_init()
{
	// In Empfangsmodus gehen
	ClrBit(DDRB, PB6); // set PDI_DATA as input
	SetBit(PORTB, PB6); // interne Pullups aktivieren
	PCICR |= (1<<PCIE1); // enable pin change interrupt
	PCMSK1 |= (1<<PCINT14); // enable pin change interrupt for PDI_DATA pin

	// Initialize Timer1, which counts to 372 CPU cycles for correct timing
	SetBit(TCCR1B, WGM12); // Set CTC Mode
	OCR1A = 372;
	SetBit(TIMSK1, OCIE1A); // interrupt when timer reaches OCR1A
}


// sends a bit via the PDI line. waits until 372 clock (1 ETU) have passed.
void send_bit(bool bit)
{
	while(!bit_is_sent); // wait until last bit was sent
	bit_is_sent = false; // we are "blocking" the pin
	next_bit_to_send = bit; // this bit will be sent in about 372 cycles
}

void comm_transmit_bytes(uint8_t* bytes, uint8_t length)
{
	while(length--) comm_transmit_byte(*(bytes++));
}

void comm_transmit_byte(uint8_t byte)
{
	uint8_t parity = 0;

	set_mode_transmit();
	TCNT1 = 0;
	SetBit(TCCR1B, CS10); // Set clock source to CPU/1 = enable timer

	// wait for at least 2.5 ETU so we don't violate the minimum time of 12 ETU between the leading edges of two start bits.
	for(int i = 0; i < 3; i++)
	{
		send_bit(true);
	}

	// send start bit (low)
	send_bit(false);

	// Shift masking bit 8 times
	for(uint8_t i = 0x01; i != 0; i <<= 1)
	{
		send_bit (byte & i);
		if(byte & i) parity = !parity;
	}

	// Sende Even Parity Bit
	send_bit(parity);
	// make sure the parity bit gets its full hold time
	while(!bit_is_sent); // wait until last bit was sent
	_delay_loop_2(372/4); // 1 ETU

	ClrBit(TCCR1B, CS10); // Set clock source to none = disable timer

	// In Empfangsmodus gehen
	set_mode_receive();
}

uint8_t comm_receive_byte()
{
	set_mode_receive();
	while(!byte_is_received);
	return byte_being_received;
}
