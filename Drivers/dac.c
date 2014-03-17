/*
 * DAC.c
 *
 *  Created on: Sep 1, 2013
 *      Author: Jose
 */

#include "dac.h"

void dac0_init(void)
{
	  /* Enable device clock gate */
	  /* SIM_SCGC6: DAC0=1 */
	  SIM_SCGC6 |= SIM_SCGC6_DAC0_MASK;         
	  
	  //Disable module while initialising
	  DAC0_C0 &= (~DAC_C0_DACEN_MASK); 
	  
	  // Clear DAC Data Registers
	  DAC0_DAT0H = 0x00;                                                   
	  DAC0_DAT0L = 0x00;          
	  
	  // Clear flags
	  DAC0_SR &= (DAC_SR_DACBFRPBF_MASK | DAC_SR_DACBFRPTF_MASK); 
	  
	  DAC0_C0 &= (DAC_C0_DACBTIEN_MASK | DAC_C0_DACBBIEN_MASK); // Disable interrupts
			  
	  DAC0_C0 |= DAC_C0_DACRFS_MASK		// VDDA Ref. selected
			  |	 DAC_C0_DACTRGSEL_MASK;	// SW Trigger selected
	  
	  DAC0_C1 &=(~DAC_C1_DACBFEN_MASK);	// Buffer read pointer is disabled  
	  
	  DAC0_C1 |= (DAC_C1_DMAEN_MASK); 	// Enable DMA
	  
	  //DAC0_C2 = (DAC_C2_DACBFUP_MASK);                         
	  	                                                    
	  // Clear flags
	  DAC0_SR &= (DAC_SR_DACBFRPBF_MASK | DAC_SR_DACBFRPTF_MASK);  
	  
	  dac0_start();
}

void dac0_start(void)
{
	  // Enable DAC module
	  DAC0_C0 |= DAC_C0_DACEN_MASK;   
}

void dac0_stop(void)
{
	  // Disable DAC module
	  DAC0_C0 &= (~DAC_C0_DACEN_MASK);   
}


void dac0_setValue (uint16_t dac_value)
{
	DAC0_DAT0H = (uint8_t)((dac_value >> 8) & 0x000F);
	DAC0_DAT0L = (uint8_t)((dac_value) & 0x00FF);
}
