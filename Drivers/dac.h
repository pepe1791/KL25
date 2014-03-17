/*
 * DAC.h
 *
 *  Created on: Sep 1, 2013
 *      Author: Jose
 */

#ifndef DAC_H_
#define DAC_H_

#include "derivative.h"

void dac0_init(void);
void dac0_setValue (uint16_t dac_value);
void dac0_start(void);
void dac0_stop(void);

#endif /* DAC_H_ */
