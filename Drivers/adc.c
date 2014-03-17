/*
 * ADC.c
 *
 *  Created on: Aug 14, 2013
 *      Author: Jose
 */

#include "adc.h"
#include "nvic.h" 

uint16_t g_adc0ResultA = 0;
uint16_t g_adc0ResultB = 0;
uint16_t g_adcFlag = 0;

void adc0_pre_init(uint8_t resMode)
{	
	ADC0_CFG1 = ADLPC_NORMAL 				// No low power mode
			| ADC_CFG1_ADIV(ADIV_2) 		// Divide input by 2
			| ADLSMP_LONG 					// Long sample time
			| ADC_CFG1_MODE(resMode)		// Single ended 12-bit conversion
			| ADC_CFG1_ADICLK(ADICLK_BUS); 	// Bus clock selected (bus_clk = core_clk/2)

	ADC0_CFG2 = MUXSEL_ADCA 				// Select the A side of the ADC channel.
			| ADACKEN_DISABLED
			| ADHSC_HISPEED					// ADC_CFG2_ADHSC_MASK = 1
			| ADC_CFG2_ADLSTS(ADLSTS_2);	//Extra long sample Time (20 extra clocks)
			

	ADC0_CV1 = 0x0000; 			// Comparators don't matter for calibration
	ADC0_CV2 = 0xFFFF;

	ADC0_SC2  = ADTRG_SW 		// @todo Select triggers 
			| ACFE_DISABLED 	// Disable comparator
			| ACFGT_GREATER
			| ACREN_ENABLED
			| DMAEN_DISABLED 	//@todo IMPORTANT CHECK DMA
			| ADC_SC2_REFSEL(REFSEL_EXT); //External Reference VREFH/VREFL

	ADC0_SC3 = CAL_OFF  
			| ADCO_SINGLE
			| AVGE_DISABLED
			| ADC_SC3_AVGS(AVGS_4);
}

void adc0_init(uint8_t resolutionMode)
{
	SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK);
	disable_irq(INT_ADC0-16); 
	
	adc0_trigger();
	
	// Lets calibrate the ADC. 1st setup how the channel will be used.
	adc0_pre_init(resolutionMode);
	
	// Configure ADC as it will be used, but because ADC_SC1_ADCH is 31,
	// the ADC will be inactive.  Channel 31 is just disable function.
	// There really is no channel 31.
	ADC0_SC1A = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);
	ADC0_SC1B = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);
	
	// Calibrate the ADC in the configuration in which it will be used:
	adc0_cal();                    // Do the calibration

	// Redo configuration
	adc0_pre_init(resolutionMode);
}

void adc0_trigger(void)
{
// Configure ADC trigger
//	SIM_SOPT7 |= (SIM_SOPT7_ADC0ALTTRGEN_MASK |
//				 !SIM_SOPT7_ADC0PRETRGSEL_MASK |
//				  SIM_SOPT7_ADC0TRGSEL(TPM1_TRG)) ;
}

void adc0_read_interrupt(uint8_t channel, uint8_t mux_select)
{
	if(ADC0_MUX_A == mux_select)
	{
		ADC0_CFG2 &= ~ADC_CFG2_MUXSEL_MASK; 	// Select the A side of the mux
	}else
	{
		ADC0_CFG2 |= ADC_CFG2_MUXSEL_MASK;  	// Select the B side of the mux
	}
	
	ADC0_SC1A = (ADC_SC1_AIEN_MASK | channel); 	// Interrupt

}

uint16_t adc0_read_polling(uint8_t channel, uint8_t mux_select)
{
	uint16_t adcResult = 0;
	
	if(ADC0_MUX_A == mux_select)
	{
		ADC0_CFG2 &= (~ADC_CFG2_MUXSEL_MASK); 	// Select the A side of the mux
	}else
	{
		ADC0_CFG2 |= (ADC_CFG2_MUXSEL_MASK);  	// Select the B side of the mux
	}
				
	ADC0_SC1A = (channel); 							// Polling
	while ( !( ADC0_SC1A & ADC_SC1_COCO_MASK ) );	// Wait for conversion to finish
	adcResult = ADC0_RA;
	
	return adcResult;
}

uint8_t adc0_cal(void)
{

    uint16_t cal_temp;
    
	ADC0_SC3 |= ADC_SC3_CALF_MASK;		// Clear any existing calibration failed flag
    ADC0_SC3 |= ADC_SC3_CAL_MASK;		// Start Calibration
	while(ADC0_SC3 & ADC_SC3_CAL_MASK){	/* Wait for calibration to finish */ };

	if(ADC0_SC3 & ADC_SC3_CALF_MASK) 		// There was a fault during calibration
	{
		return 1;
	}

	// Initialize (clear)
	cal_temp = 0;
	
    // Add the following plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, CLPS and CLPD to the variable.
	cal_temp += ADC0_CLP0;
	cal_temp += ADC0_CLP1;
	cal_temp += ADC0_CLP2;
	cal_temp += ADC0_CLP3;
	cal_temp += ADC0_CLP4;
	cal_temp += ADC0_CLPS;

	// Divide the variable by two.
	cal_temp = cal_temp >> 1;
	// Set the MSB of the variable.
	cal_temp |= 0x8000;
	
	// Store the value in the plus-side gain calibration registers ADCPG
	ADC0_PG = cal_temp;
	
	// Repeat procedure for the minus-side gain calibration value.
	// Initialize (clear) a 16b variable in RAM.
	cal_temp = 0;

    // Add the following minus-side calibration results CLM0, CLM1, CLM2, CLM3, CLM4, CLMS and CLMD to the variable.
	cal_temp += ADC0_CLM0;
	cal_temp += ADC0_CLM1;
	cal_temp += ADC0_CLM2;
	cal_temp += ADC0_CLM3;
	cal_temp += ADC0_CLM4;
	cal_temp += ADC0_CLMS;
	
	// Divide the variable by two.
	cal_temp = cal_temp >> 1;
	// Set the MSB of the variable.
	cal_temp |= 0x8000;
	
	// Store the value in the minus-side gain calibration registers ADCMG
	ADC0_MG = cal_temp;

	return 0;
}


void ADC0_IRQHandler()
{
	// Check if mux A triggered the conversion
	if ( ( ADC0_SC1A & ADC_SC1_COCO_MASK ) == ADC_SC1_COCO_MASK ) 
	{  
		g_adc0ResultA = ADC0_RA;           // This will clear the COCO bit that is also the interrupt flag
		(void) g_adc0ResultA;
		g_adcFlag = ADC0_MUX_A;
	}
	// Check if mux B triggered the conversion
	if ( ( ADC0_SC1B & ADC_SC1_COCO_MASK ) == ADC_SC1_COCO_MASK ) 
	{  
		g_adc0ResultB = ADC0_RB;           // This will clear the COCO bit that is also the interrupt flag
		(void) g_adc0ResultB;
		g_adcFlag = ADC0_MUX_B;
	}
}
