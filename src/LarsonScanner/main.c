/*
 * main.c
 *
 * Larson Scanner to test the Smart Card toolchain and hardware
 *
 *  Created on: Apr 29, 2015
 *      Author: ga68gug
 */

#include <avr/io.h>
#include <util/delay.h>

int main() {

	DDRA = 0xFF;
	PORTA = 0x01;

	// Easy Scanner
	while(0) {
		PORTA <<= 1;
		if(PORTA == 0x00) PORTA = 0x01;
		_delay_ms(100);
	}

	// Better scanner
	while(1) {
		PORTA = 0b00000001;
		_delay_ms(250);
		PORTA = 0b00000010;
		_delay_ms(200);
		PORTA = 0b00000100;
		_delay_ms(150);
		PORTA = 0b00001000;
		_delay_ms(100);
		PORTA = 0b00010000;
		_delay_ms(100);
		PORTA = 0b00100000;
		_delay_ms(150);
		PORTA = 0b01000000;
		_delay_ms(200);
		PORTA = 0b10000000;
		_delay_ms(250);

		PORTA = 0b10000000;
		_delay_ms(250);
		PORTA = 0b01000000;
		_delay_ms(200);
		PORTA = 0b00100000;
		_delay_ms(150);
		PORTA = 0b00010000;
		_delay_ms(100);
		PORTA = 0b00001000;
		_delay_ms(100);
		PORTA = 0b00000100;
		_delay_ms(150);
		PORTA = 0b00000010;
		_delay_ms(200);
		PORTA = 0b00000001;
		_delay_ms(250);


	}
}
