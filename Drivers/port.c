/*
 * port.c
 *
 *  Created on: Oct 27, 2013
 *      Author: Jose
 */

#include "port.h"

void port_init(void)
{
	// Enable Clocks to all ports
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
}

void port_set_pin_mux(PORT_MemMapPtr portMapPtr, uint8_t pin, uint8_t alt)
{
	PORT_PCR_REG(portMapPtr, pin) |= PORT_PCR_MUX(alt);
}
