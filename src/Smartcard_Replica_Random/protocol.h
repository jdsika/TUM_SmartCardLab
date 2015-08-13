/*
 * protocol.h
 *
 *  Created on: May 6, 2015
 *      Author: jojo, laurenz
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

void protocol_challenge_response();
void protocol_get_auth_challenge(uint8_t* auth_challenge);
void protocol_send_auth_response(uint8_t* auth_response);
void protocol_receive_expected_data(const uint8_t* data, const uint8_t data_length);
void protocol_send_ATR();

#endif
