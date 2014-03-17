/*
 * port.h
 *
 *  Created on: Oct 27, 2013
 *      Author: Jose
 */

#ifndef PORT_H_
#define PORT_H_

#include "derivative.h"

#define PIN_0	((uint32_t)(0)) 	
#define PIN_1	((uint32_t)(1))
#define PIN_2	((uint32_t)(2))
#define PIN_3	((uint32_t)(3))
#define PIN_4	((uint32_t)(4))
#define PIN_5	((uint32_t)(5))
#define PIN_6	((uint32_t)(6))
#define PIN_7	((uint32_t)(7))
#define PIN_8	((uint32_t)(8))
#define PIN_9	((uint32_t)(9))
#define PIN_10	((uint32_t)(10))
#define PIN_11	((uint32_t)(11))
#define PIN_12	((uint32_t)(12))
#define PIN_13	((uint32_t)(13))
#define PIN_14	((uint32_t)(14))
#define PIN_15	((uint32_t)(15))
#define PIN_16	((uint32_t)(16))
#define PIN_17	((uint32_t)(17))
#define PIN_18	((uint32_t)(18))
#define PIN_19	((uint32_t)(19))
#define PIN_20	((uint32_t)(20))
#define PIN_21	((uint32_t)(21))
#define PIN_22	((uint32_t)(22))
#define PIN_23	((uint32_t)(23))
#define PIN_24	((uint32_t)(24))
#define PIN_25	((uint32_t)(25))
#define PIN_26	((uint32_t)(26))
#define PIN_27	((uint32_t)(27))
#define PIN_28	((uint32_t)(28))
#define PIN_29	((uint32_t)(29))
#define PIN_30	((uint32_t)(30))
#define PIN_31	((uint32_t)(31))

#define PORTA 	PORTA_BASE_PTR
#define PORTB 	PORTB_BASE_PTR
#define PORTC 	PORTC_BASE_PTR
#define PORTD 	PORTD_BASE_PTR
#define PORTE 	PORTE_BASE_PTR

#define ALT_0 	0x00
#define ALT_1 	0x01
#define ALT_2 	0x02
#define ALT_3 	0x03

void port_init(void);
void port_set_pin_mux(PORT_MemMapPtr portMapPtr, uint8_t pin, uint8_t alt);

#endif /* PORT_H_ */
