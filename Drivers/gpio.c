/*
 * GPIO.c
 *
 *  Created on: Jun 25, 2013
 *      Author: Jose
 */

#include "port.h"
#include "gpio.h"
#include "nvic.h"

void led_init(void)
{
	port_init();
	
	gpio_set_pin_mode(RED_LED_PORT,   RED_LED_PIN,   OUTPUT);
	gpio_set_pin_mode(GREEN_LED_PORT, GREEN_LED_PIN, OUTPUT);
	gpio_set_pin_mode(BLUE_LED_PORT,  BLUE_LED_PIN,  OUTPUT);

	GPIO_PIN_WRITE(RED_LED_PORT,   RED_LED_PIN,   HIGH);
	GPIO_PIN_WRITE(GREEN_LED_PORT, GREEN_LED_PIN, HIGH);
	GPIO_PIN_WRITE(BLUE_LED_PORT,  BLUE_LED_PIN,  HIGH);
}


void gpio_set_pin_mode(GPIO_MemMapPtr gpioMapPtr, uint8_t pin, uint8_t mode)
{
	PORT_MemMapPtr portMapPtr;

	if(gpioMapPtr == PTA)
	{
		portMapPtr = PORTA;
	}else if(gpioMapPtr == PTB)
	{
		portMapPtr = PORTB;
	}else if(gpioMapPtr == PTC)
	{
		portMapPtr = PORTC;
	}else if(gpioMapPtr == PTD)
	{
		portMapPtr = PORTD;
	}else if(gpioMapPtr == PTE)
	{
		portMapPtr = PORTE;
	}

	PORT_PCR_REG(portMapPtr, pin) |= PORT_PCR_MUX(0x01);
	
	switch(mode)
	{
		case INPUT:  			
			GPIO_PDDR_REG(gpioMapPtr) &= (~(1<<pin));
		break;

		case INPUT_PULLUP: 		
			GPIO_PDDR_REG(gpioMapPtr) &= (~(1<<pin));
			PORT_PCR_REG(portMapPtr, pin) |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
		break;
		
		case INPUT_PULLDWN: 	
			GPIO_PDDR_REG(gpioMapPtr) &= (~(1<<pin));
			PORT_PCR_REG(portMapPtr, pin) |= (PORT_PCR_PE_MASK);
			PORT_PCR_REG(portMapPtr, pin) &= (~PORT_PCR_PS_MASK);
		break;
		
		case OUTPUT: 			
			GPIO_PDDR_REG(gpioMapPtr) |= ((1<<pin));
		break;
		
		case OUTPUT_HIGH: 		
			GPIO_PDDR_REG(gpioMapPtr) |= ((1<<pin));\
			PORT_PCR_REG(portMapPtr, pin) |= PORT_PCR_DSE_MASK;
		break;
		
		case INPUT_PULLUP_INT: 		
			GPIO_PDDR_REG(gpioMapPtr) &= (~(1<<pin));
			PORT_PCR_REG(portMapPtr, pin) |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA));
		break;
		
		default: break;
	}
}


void PORTA_IRQHandler()
{
	if (PORTA_ISFR & (1<<PIN_1))
	{
		PORTA_ISFR |= (1<<PIN_1);	
	}

}
