/*
 * comm.h
 *
 *  Created on: May 7, 2015
 *      Author: ga68gug
 */

#ifndef COMM_H_
#define COMM_H_

void comm_init();
void comm_transmit_bytes(uint8_t* bytes, uint8_t length);
void comm_transmit_byte(uint8_t byte);
uint8_t comm_receive_byte();

#endif /* COMM_H_ */
