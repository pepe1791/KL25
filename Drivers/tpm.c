/*
 * TPM.c
 *
 *  Created on: Aug 9, 2013
 *      Author: Jose
 */
#include "tpm.h"

void tpmx_chn_init(TPM_MemMapPtr tpm_x, uint8_t ch, uint8_t clkSource, uint16_t periodModuleOverflowValue, uint8_t ps, uint8_t mode, uint16_t dutyCycleOverflowValue)
{
	SIM_SCGC6 				|= (SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK);	
	SIM_SOPT2 				|= (SIM_SOPT2_TPMSRC(clkSource) | SIM_SOPT2_PLLFLLSEL_MASK); /* Set the chosen clock source for the TPM module */\
	TPM_MOD_REG(tpm_x) 		 = periodModuleOverflowValue;
	TPM_SC_REG(tpm_x) 		|= (TPM_SC_TOF_MASK | TPM_SC_PS(ps));
	TPM_CnSC_REG(tpm_x,ch) 	|= mode;	
	TPM_CnV_REG(tpm_x,ch) = dutyCycleOverflowValue;	
}

void tpmx_start(TPM_MemMapPtr tpm_x, uint8_t clkMode)
{
	TPM_SC_REG(tpm_x) |= TPM_SC_CMOD(clkMode); 
}

void tpmx_stop(TPM_MemMapPtr tpm_x, uint8_t clkMode)
{
	TPM_SC_REG(tpm_x) &= (~TPM_SC_CMOD_MASK); 
}

void tpmx_set_ch_val(TPM_MemMapPtr tpm_x, uint8_t ch, uint16_t value)
{
	TPM_CnV_REG(tpm_x,ch) = (value > TPM_MOD_REG(tpm_x))? TPM_MOD_REG(tpm_x): value;
}

void FTM0_IRQHandler(void)
{
	if(TPM0_STATUS & TPM_STATUS_TOF_MASK)
	{
		TPM0_SC |= TPM_SC_TOF_MASK;			// Clear TPM module interrupt flag
	}
	
	if(TPM0_STATUS & TPM_STATUS_CH0F_MASK)
	{
		TPM0_C0SC |= TPM_CnSC_CHF_MASK;		// Clear channel 0 interrupt flag	
	}
	
	if(TPM0_STATUS & TPM_STATUS_CH1F_MASK)
	{
		TPM0_C1SC |= TPM_CnSC_CHF_MASK;		// Clear channel 1 interrupt flag	
	}
}

void FTM1_IRQHandler(void)
{
	if(TPM1_STATUS & TPM_STATUS_TOF_MASK)
	{
		TPM1_SC |= TPM_SC_TOF_MASK;			// Clear TPM module interrupt flag
	}
	
	if(TPM1_STATUS & TPM_STATUS_CH0F_MASK)
	{
		TPM1_C0SC |= TPM_CnSC_CHF_MASK;		// Clear channel 0 interrupt flag	
	}
	
	if(TPM1_STATUS & TPM_STATUS_CH1F_MASK)
	{
		TPM1_C1SC |= TPM_CnSC_CHF_MASK;		// Clear channel 1 interrupt flag	
	}
}

																									
void FTM2_IRQHandler(void)
{
	if(TPM2_STATUS & TPM_STATUS_TOF_MASK)
	{
		TPM2_SC |= TPM_SC_TOF_MASK;			// Clear TPM module interrupt flag
	}
	
	if(TPM2_STATUS & TPM_STATUS_CH0F_MASK)
	{
		TPM2_C0SC |= TPM_CnSC_CHF_MASK;		// Clear channel 0 interrupt flag	
	}
	
	if(TPM2_STATUS & TPM_STATUS_CH1F_MASK)
	{
		TPM2_C1SC |= TPM_CnSC_CHF_MASK;		// Clear channel 1 interrupt flag	
	}
}
