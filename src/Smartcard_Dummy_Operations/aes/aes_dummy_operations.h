/*
 * aes_dummy_operations.h
 *
 *  Created on: Jul 10, 2015
 *      Author: gu95yiq
 */

#ifndef AES_DUMMY_OPERATIONS_H_
#define AES_DUMMY_OPERATIONS_H_
#include <stdint.h>

extern volatile uint8_t dummy_result;

extern uint8_t dummy_RK_operations;
extern uint8_t dummy_SB_operations;

void aes_dummy_operations(uint8_t max);

void aes_reset_dummy_operation_counter();

#endif /* AES_DUMMY_OPERATIONS_H_ */
