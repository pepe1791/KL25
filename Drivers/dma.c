/*
 * DMA.c
 *
 *  Created on: Sep 1, 2013
 *      Author: Jose
 */

#include "dma.h"

uint16_t SamplesIn[SAMPLES_BUFFSIZE] __attribute__ ((aligned (128*2))) = {0};
uint16_t SamplesOut[SAMPLES_BUFFSIZE] __attribute__ ((aligned (128*2))) = {31, 63, 95,  127,  159,  191,  223,  255,  287,  319,  351,  383,  415,  447,  479,  511,  543,  575,  607,  639,  671,  703,  735,  767,  799,  831,  863,  895,  927,  959,  991, 1023, 1055, 1087, 1119, 1151, 1183, 1215, 1247, 1279, 1311, 1343, 1375, 1407, 1439, 1471, 1503, 1535, 1567, 1599, 1631, 1663, 1695, 1727, 1759, 1791, 1823, 1855, 1887, 1919, 1951, 1983, 2015, 2047, 2079, 2111, 2143, 2175, 2207, 2239, 2271, 2303, 2335, 2367, 2399, 2431, 2463, 2495, 2527, 2559, 2591, 2623, 2655, 2687, 2719, 2751, 2783, 2815, 2847, 2879, 2911, 2943, 2975, 3007, 3039, 3071, 3103, 3135, 3167, 3199, 3231, 3263, 3295, 3327, 3359, 3391, 3423, 3455, 3487, 3519, 3551, 3583, 3615, 3647, 3679, 3711, 3743, 3775, 3807, 3839, 3871, 3903, 3935, 3967, 3999, 4031, 4063, 4095};

uint16_t* gWrPtr;
uint16_t* gRdPtr;
uint8_t isReadyforProcessing = 0;

void dma_ch0_init(void)
{
	// Enable DMAMUX and DMA modules
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
	
	// DMA source DMA Mux, ADC0 source
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_SOURCE(40);   //@todo selec dmach0 trigger 
	
	// Clear pending errors and/or the done bit
	if (((DMA_DSR_BCR0 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK))
		DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	// Set Source Address (ADC Result Register
	DMA_SAR0 = (uint32_t) &ADC0_RA;

	// Set BCR to know how many bytes to transfer
	DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(SAMPLES_BUFFSIZE*(sizeof(SamplesIn[0]))/2);

	// Clear Source size and Destination size fields.
	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK | DMA_DCR_SINC_MASK);
	
	// Set DMA as follows:
	DMA_DCR0  |= (DMA_DCR_EINT_MASK		// Enable interrupt on completion of transfer
			|DMA_DCR_ERQ_MASK           // Enables peripheral request to initiate transfer
			|DMA_DCR_CS_MASK            // Forces a single read/write transfer per request
			//| DMA_DCR_AA_MASK			// Alignment 
			//|DMA_DCR_D_REQ_MASK       // Disable request. ERQ bit is cleared when the BCR is exhausted
			|DMA_DCR_SSIZE(0b10)        // Source size. 0b10 is 16 bits
			|DMA_DCR_DSIZE(0b10)        // Destination size. 0b10 is 16 bits
			|DMA_DCR_DMOD(0b0101)      	// Source address modulo 32 bytes
			|DMA_DCR_DINC_MASK          // Destination increment. The SAR increments by 1, 2, 4 as determined by the transfer size.
	); 

	// Set destination address
	DMA_DAR0 = (uint32_t) &SamplesIn[0];
	
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK;

}
//DMA0 ADC to memory Rx/In buffer 
void DMA0_IRQHandler()
{
	// Clear pending errors or the done bit 
	if (
			  ((DMA_DSR_BCR0 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
//			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
//			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
//			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK)
		)
	{
		DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;	// clear flag
		
		// Re-initialise destination address
		// DMA_DAR0 = (uint32_t) &ADC_Samples_IN[0];	 
		//uint32_t aux_0 = ( (DMA_DAR0 - (SAMPLES_BUFFSIZE/2)*TRANSACTION_SIZE_IN_BYTES) & ((SAMPLES_BUFFSIZE*TRANSACTION_SIZE_IN_BYTES)-1) );
		gRdPtr = (uint16_t*)((uint32_t)(&SamplesIn[0]) + (uint32_t)( (DMA_DAR0 - (SAMPLES_BUFFSIZE/2)*TRANSACTION_SIZE_IN_BYTES) & ((SAMPLES_BUFFSIZE*TRANSACTION_SIZE_IN_BYTES)-1) ));
		isReadyforProcessing = 1;
		// Set BCR to know how many bytes to transfer
		DMA_DSR_BCR0 = DMA_DSR_BCR_BCR((uint32_t)SAMPLES_BUFFSIZE*((uint32_t)sizeof(SamplesIn[0]))/2);
	}
}


void dma_ch1_init(void)
{
	// Enable DMAMUX and DMA modules
	//SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	//SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
	
	// DMA source DMA Mux, TPM1 source
	DMAMUX0_CHCFG1 |= DMAMUX_CHCFG_SOURCE(32);      //@todo selec dmac1 trigger 
	
	// Clear pending errors and/or the done bit
	if (((DMA_DSR_BCR1 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
			| ((DMA_DSR_BCR1 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
			| ((DMA_DSR_BCR1 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
			| ((DMA_DSR_BCR1 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK))
		DMA_DSR_BCR1 |= DMA_DSR_BCR_DONE_MASK;

	// Set Source Address (ADC Result Register
	DMA_SAR1 = (uint32_t) &SamplesOut[0];

	// Set BCR to know how many bytes to transfer
	DMA_DSR_BCR1 = DMA_DSR_BCR_BCR(SAMPLES_BUFFSIZE*(sizeof(SamplesOut[0]))/2);

	// Clear Source size and Destination size fields.
	DMA_DCR1 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK | DMA_DCR_SINC_MASK);
	
	// Set DMA as follows:
	DMA_DCR1  |= (DMA_DCR_EINT_MASK		// Enable interrupt on completion of transfer
			|DMA_DCR_ERQ_MASK           // Enables peripheral request to initiate transfer
			|DMA_DCR_CS_MASK            // Forces a single read/write transfer per request
			//| DMA_DCR_AA_MASK			// Alignment 
			//|DMA_DCR_D_REQ_MASK       // Disable request. ERQ bit is cleared when the BCR is exhausted
			|DMA_DCR_SSIZE(0b10)        // Source size. 0b10 is 16 bits
			|DMA_DCR_DSIZE(0b10)        // Destination size. 0b10 is 16 bits
			|DMA_DCR_SMOD(0b0101)      	// Source address modulo 32 bytes
			|DMA_DCR_SINC_MASK          // Destination increment. The SAR increments by 1, 2, 4 as determined by the transfer size.
	); 

	// Set destination address
	DMA_DAR1 = (uint32_t) &DAC0_DATL(0);
	
	DMAMUX0_CHCFG1 |= DMAMUX_CHCFG_ENBL_MASK;

}
// DMA1 memory to DAC Tx/Out buffer
void DMA1_IRQHandler()
{
	// Clear pending errors or the done bit 
	if (
			  ((DMA_DSR_BCR1 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
//			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
//			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
//			| ((DMA_DSR_BCR0 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK)
		)
	{
		DMA_DSR_BCR1 |= DMA_DSR_BCR_DONE_MASK;	// clear flag
		GPIO_TOGGLE(PTB, PIN_0);
		// Re-initialise destination address
		// DMA_DAR0 = (uint32_t) &ADC_Samples_IN[0];
		//uint32_t aux_1 = ( (DMA_SAR1 + (SAMPLES_BUFFSIZE/2)*TRANSACTION_SIZE_IN_BYTES) & ((SAMPLES_BUFFSIZE*TRANSACTION_SIZE_IN_BYTES)-1) );
		gWrPtr = (uint16_t*)((uint32_t)(&SamplesOut[0]) + (uint32_t)( (DMA_SAR1 + (SAMPLES_BUFFSIZE/2)*TRANSACTION_SIZE_IN_BYTES) & ((SAMPLES_BUFFSIZE*TRANSACTION_SIZE_IN_BYTES)-1) ));
		// Set BCR to know how many bytes to transfer
		DMA_DSR_BCR1 = DMA_DSR_BCR_BCR((uint32_t)SAMPLES_BUFFSIZE*((uint32_t)sizeof(SamplesOut[0]))/2);
	}
}
