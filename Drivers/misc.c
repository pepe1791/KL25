/*
 * misc.c
 *
 *  Created on: Oct 19, 2013
 *      Author: Jose
 */

#include "misc.h"

void delay(uint32_t delayCounter){
	while(delayCounter--){		asm("nop");		}
}


