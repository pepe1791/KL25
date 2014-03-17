/*
 * GPIO.h
 *
 *  Created on: Jun 25, 2013
 *      Author: Jose
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "derivative.h" /* include peripheral declarations */
#include "port.h"

#define PTA				PTA_BASE_PTR
#define PTB				PTB_BASE_PTR
#define PTC				PTC_BASE_PTR
#define PTD				PTD_BASE_PTR
#define PTE				PTE_BASE_PTR

#define HIGH 	1
#define LOW		0

#define INPUT 				0
#define OUTPUT				1
#define INPUT_PULLUP 		2
#define INPUT_PULLDWN 		3
#define OUTPUT_HIGH			4
#define INPUT_PULLUP_INT 	5

#define GPIO_TOGGLE(ptx, pin)		GPIO_PTOR_REG(ptx) |= (1<<pin)
#define GPIO_SET(ptx, pin)			GPIO_PSOR_REG(ptx) |= (1<<pin)
#define GPIO_CLEAR(ptx, pin)		GPIO_PCOR_REG(ptx) |= (1<<pin)

#define GPIO_PIN_WRITE(ptx, pin, loglevel)		GPIO_PDOR_REG(ptx) = loglevel ? (GPIO_PDOR_REG(ptx) | (1<<pin)) : (GPIO_PDOR_REG(ptx) & ~ (1<<pin))
#define GPIO_PORT_WRITE(ptx, value)			GPIO_PDOR_REG(ptx) = ((uint32_t)(value))

#define GPIO_IS_PRESSED(ptx,pin)	(!(GPIO_PDIR_REG(ptx) & (1<<pin)))
#define GPIO_READ_PIN(ptx,pin)		(GPIO_PDIR_REG(ptx) & (1<<pin))?1:0

void gpio_set_pin_mode(GPIO_MemMapPtr gpioMapPtr, uint8_t pin, uint8_t mode);

void PORTA_IRQHandler();
void PORTD_IRQHandler();

/* TODO This code should be on their specific led .c and ,h files*/
#define RED_LED_PORT	PTB
#define RED_LED_PIN		PIN_18

#define GREEN_LED_PORT	PTB
#define GREEN_LED_PIN	PIN_19

#define BLUE_LED_PORT	PTD
#define BLUE_LED_PIN	PIN_1
void led_init(void);

#endif /* GPIO_H_ */
