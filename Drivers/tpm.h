/*
 * TPM.h
 *
 *  Created on: Aug 9, 2013
 *      Author: Jose
 */

#ifndef TPM_H_
#define TPM_H_

#include "derivative.h" /* include peripheral declarations */

#define TPM0			TPM0_BASE_PTR
#define TPM1			TPM1_BASE_PTR
#define TPM2			TPM2_BASE_PTR

#define TPM_DIS			0
#define TPM_PLLFLL 		1
#define TPM_OSCERCLK 	2
#define TPM_MCGIRCLK	3

#define	TPM_CNT_DISABLED	0
#define	TPM_CLK				1
#define	TPM_EXT_CLK			2

#define TPM_OC_TOGGLE	( TPM_CnSC_MSA_MASK | TPM_CnSC_ELSA_MASK )
#define TPM_OC_CLR		( TPM_CnSC_MSA_MASK | TPM_CnSC_ELSB_MASK )
#define TPM_OC_SET		( TPM_CnSC_MSA_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK )
#define TPM_OC_OUTL		( TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK  | TPM_CnSC_ELSB_MASK )
#define TPM_OC_OUTH		( TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK  | TPM_CnSC_ELSA_MASK )

#define TPM_PWM_H 		( (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK) & (~TPM_CnSC_ELSA_MASK) )
#define TPM_PWM_L		(  TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK )

#define PS_1	0
#define PS_2	1
#define PS_4	2
#define PS_8	3
#define PS_16	4
#define PS_32	5
#define PS_64	6
#define PS_128	7
																												
#define TPMx_SET_CHn_VAL(tpm_x,ch,value)	TPM_CnV_REG(tpm_x,ch) = (value > TPM_MOD_REG(tpm_x))? TPM_MOD_REG(tpm_x): value

void tpmx_chn_init(TPM_MemMapPtr tpm_x, uint8_t ch, uint8_t clkSource, uint16_t periodModuleOverflowValue, uint8_t ps, uint8_t mode, uint16_t dutyCycleOverflowValue);
void tpmx_start(TPM_MemMapPtr tpm_x, uint8_t clkMode);
void tpmx_stop(TPM_MemMapPtr tpm_x, uint8_t clkMode);
void tpmx_set_ch_val(TPM_MemMapPtr tpm_x, uint8_t ch, uint16_t value);
	
void FTM0_IRQHandler(void);
void FTM1_IRQHandler(void);
void FTM2_IRQHandler(void);

#endif /* TPM_H_ */
