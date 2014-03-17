/*
 * DMA.h
 *
 *  Created on: Sep 1, 2013
 *      Author: Jose
 */

#ifndef DMA_H_
#define DMA_H_

#include "derivative.h"
#include "port.h"
#include "gpio.h" 
#include "nvic.h" 

#define SAMPLES_BUFFSIZE			 	128
#define TRANSACTION_SIZE_IN_BYTES 		2


extern uint16_t SamplesIn[SAMPLES_BUFFSIZE];
extern uint16_t SamplesOut[SAMPLES_BUFFSIZE];

extern uint16_t* gWrPtr;
extern uint16_t* gRdPtr;
extern uint8_t isReadyforProcessing;

void dma_ch0_init(void);
void DMA0_IRQHandler();

void dma_ch1_init(void);
void DMA1_IRQHandler();

#endif /* DMA_H_ */
