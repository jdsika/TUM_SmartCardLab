/*
 * entropy.h
 *
 *  Created on: May 12, 2015
 *      Author: jojo
 */

#ifndef ENTROPY_H_
#define ENTROPY_H_

void entropy_init();
void entropy_deinit();
void entropy_eeprom_renew();
void entropy_do_jenkins();

#endif /* ENTROPY_H_ */
