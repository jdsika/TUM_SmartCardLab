/*
 * macro.h
 *
 *  Created on: May 7, 2015
 *      Author: ga68gug
 */

#ifndef MACRO_H_
#define MACRO_H_

#include <avr/io.h>

#define SIZE(a)	sizeof(a)/sizeof(a[0])

#define SetBit(PORT, BIT)	PORT |= (1<<BIT)
#define ClrBit(PORT, BIT)	PORT &= ~(1<<BIT)
#define TglBit(PORT, BIT)	PORT ^= (1<<BIT)
#define GetBit(PIN, BIT)	((PIN & (1<<BIT))?true:false)

#define led_init()		DDRA = 0xFF
#define led_set(VAL)	PORTA = VAL

#define trigger_init()	{SetBit(DDRB, PB4); ClrBit(PORTB, PB4);}
#define trigger()		{SetBit(PORTB, PB4); ClrBit(PORTB, PB4);}
#define trigger_set()	SetBit(PORTB, PB4)
#define trigger_clr()	ClrBit(PORTB, PB4)

#endif /* MACRO_H_ */
