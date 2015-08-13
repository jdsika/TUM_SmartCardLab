/*
 * uart.h
 *
 *  Created on: May 7, 2015
 *      Author: ga68gug
 */

#ifndef UART_H_
#define UART_H_

#include <stdbool.h>

void uart_init();
void uart_putc(const char c);
void uart_puts(const char* c_str);

void assert(bool cond, const char* error_string);

#endif /* UART_H_ */
