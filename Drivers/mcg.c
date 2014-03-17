/*
 * Clock.c
 *
 *  Created on: Jun 25, 2013
 *      Author: Jose
 */

#include "mcg.h"

/*********************************************************************************************/
/* Function name : pll_init
 *
 * Mode transition: Option to move from FEI to PEE mode or to just initialize the PLL
 *
 * This function initializes PLL and transitions FEI to PEE mode.
 * Using the function parameter names the PLL frequency is calculated as follows:
 * PLL freq = ((crystalValue / extClockDivider) * pllMultiplyVal)

 * If an error occur due to PPL initialization or paraemter out of range the driver will exit
 * and return a fail/error code. 
 *
 * Parameters: crystalValue - external clock frequency in Hz either from a crystal or square
 *                           wave clock source
 *             highGainSelect     - selects whether low power or high gain mode is selected
 *                           for the crystal oscillator. This has no meaning if an
 *                           external clock is used.
 *             referenceSelect   - selects external clock (=0) or crystal osc (=1)
 *             extClockDivider   - value to divide the external clock source by to create the desired
 *                           PLL reference clock frequency
 *             pllMultiplyVal    - value to multiply the PLL reference clock frequency by
 *             mcgout_select  - 0 if the PLL is just to be enabled, non-zero if the PLL is used
 *                              to provide the MCGOUT clock for the system.
 *
 * Return value : PLL frequency (Hz) or error code
 */

int32_t pll_init(int32_t crystalValue, uint8_t highGainSelect, uint8_t referenceSelect, int8_t extClockDivider, int8_t pllMultiplyVal, uint8_t mcgout_select)
{
	uint8_t frDividerValue;
	uint8_t tempReg;
	uint8_t actualCrystalDivider, actualPLLMultiplier;
	int16_t i;
	int32_t referenceFreq;
	int32_t pllFrequency;

	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(OUTDIV1) | SIM_CLKDIV1_OUTDIV4(OUTDIV4);

	// Check if in FEI mode
	if (!((((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x0) &&		// Check CLKS mux has selcted FLL output
			(MCG_S & MCG_S_IREFST_MASK) &&                                  // Check FLL ref is internal ref clk
			(!(MCG_S & MCG_S_PLLST_MASK))))                                 // Check PLLS mux has selected FLL
	{
		return 0x1;   // Return error code
	}

	// Check external frequency is less than the maximum frequency
	if  (crystalValue > 50000000) 
	{
		return kCrystalInvalidFrequency;
	}

	// Check crystal frequency is within spec. if crystal osc. is being used as PLL ref.
	if (referenceSelect)
	{
		if ((crystalValue < 3000000) || (crystalValue > 32000000)) 
		{
			return kCrystalFrequencyOutOfRange;	// Return if one of the available crystal options is not available
		} 
	}

	// Make sure HGO will never be greater than 1. Could return an error instead if desired.
	if (highGainSelect > 0)
	{
		highGainSelect = 1; // force highGainSelect to 1 if > 0
	}

	// Check PLL divider settings are within spec.
	if ((extClockDivider < 1) || (extClockDivider > 25)) 
	{
		return kClockDividerOutOfSpec;
	}
	if ((pllMultiplyVal < 24) || (pllMultiplyVal > 50)) 
	{
		return kPLLMultiplierOutOfSpec;
	}

	// Check PLL reference clock frequency is within spec.
	referenceFreq = crystalValue / extClockDivider;
	if ((referenceFreq < 2000000) || (referenceFreq > 4000000))
	{
		return kPLLReferenceClockOutOfSpec;
	}

	// Set PLL output frequency and check it is within spec.
	pllFrequency = (crystalValue / extClockDivider) * pllMultiplyVal;
	if ((pllFrequency < 48000000) || (pllFrequency > 100000000)) 
	{
		return kPLLFrequencyOutOfSpec;
	}

	// Configure the MCG_C2 register
	// The RANGE value is determined by the external frequency. Since the RANGE parameter affects the FRDIV divide value
	// It still needs to be set correctly even if the oscillator is not being used
	tempReg = MCG_C2;
	tempReg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK); // Clear fields before writing new values

	if (crystalValue <= 8000000)
	{
		tempReg |= (MCG_C2_RANGE0(1) | (highGainSelect << MCG_C2_HGO0_SHIFT) | (referenceSelect << MCG_C2_EREFS0_SHIFT));
	}
	else
	{
		tempReg |= (MCG_C2_RANGE0(2) | (highGainSelect << MCG_C2_HGO0_SHIFT) | (referenceSelect << MCG_C2_EREFS0_SHIFT));
	}
	MCG_C2 = tempReg;

	// Determine FRDIV based on reference clock frequency
	// Since the external frequency has already been checked only the maximum frequency for each FRDIV value needs to be compared here.
	if (crystalValue <= 1250000) {frDividerValue = 0;}
	else if (crystalValue <= 2500000) {frDividerValue = 1;}
	else if (crystalValue <= 5000000) {frDividerValue = 2;}
	else if (crystalValue <= 10000000) {frDividerValue = 3;}
	else if (crystalValue <= 20000000) {frDividerValue = 4;}
	else {frDividerValue = 5;}

	// Select external oscillator and Reference Divider and clear IREFS to start ext osc
	// If IRCLK is required it must be enabled outside of this driver, existing state will be maintained
	// CLKS=2, FRDIV=frDividerValue, IREFS=0, IRCLKEN=0, IREFSTEN=0
	tempReg = MCG_C1;
	tempReg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK); // Clear values in these fields
	tempReg |= (MCG_C1_CLKS(2) | MCG_C1_FRDIV(frDividerValue)); // Set the required CLKS and FRDIV values
	MCG_C1 = tempReg;

	// If the external oscillator is used need to wait for OSCINIT to set
	if (referenceSelect)
	{
		for (i = 0 ; i < 20000 ; i++)
		{
			// Jump out early if OSCINIT sets before loop finishes
			if (MCG_S & MCG_S_OSCINIT0_MASK)
			{
				break; 
			}
		}

		if (!(MCG_S & MCG_S_OSCINIT0_MASK))
		{
			return 0x23; 	// Check bit is really set and return with error if not set
		}
	}

	// Wait for Reference clock Status bit to clear
	for (i = 0 ; i < 2000 ; i++)
	{
		// Jump out early if IREFST clears before loop finishes
		if (!(MCG_S & MCG_S_IREFST_MASK))
		{
			break; 
		}
	}

	if (MCG_S & MCG_S_IREFST_MASK) 
	{
		return 0x11; // Check bit is really clear and return with error if not set
	}

	// Wait for clock status bits to show clock source is ext ref clk
	for (i = 0 ; i < 2000 ; i++)
	{
		// Jump out early if CLKST shows EXT CLK slected before loop finishes
		if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2)
		{
			break; 
		}
	}

	if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2) 
	{
		return kExtClockNotSelectedError; // Check EXT CLK is really selected and return with error if not
	}

	// Now in FBE
	// It is recommended that the clock monitor is enabled when using an external clock as the clock source/reference.
	// It is enabled here but can be removed if this is not required.
	MCG_C6 |= MCG_C6_CME0_MASK;

	// Configure PLL
	// Configure MCG_C5
	// If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed in here or in user code.
	tempReg = MCG_C5;
	tempReg &= ~MCG_C5_PRDIV0_MASK;
	tempReg |= MCG_C5_PRDIV0(extClockDivider - 1);    //set PLL ref divider
	tempReg |= MCG_C5_PLLCLKEN0_MASK; // @todo pepe
	tempReg |= MCG_C5_PLLSTEN0_MASK;  // @todo pepe
	MCG_C5 = tempReg;

	// Configure MCG_C6
	// The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk
	// The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6
	tempReg = MCG_C6; // Store present C6 value
	tempReg &= ~MCG_C6_VDIV0_MASK; // Clear actualPLLMultiplier settings
	tempReg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(pllMultiplyVal - 24); // Write new actualPLLMultiplier and enable PLL
	MCG_C6 = tempReg; // Update MCG_C6

	// Wait for PLLST status bit to set
	for (i = 0 ; i < 2000 ; i++)
	{
		// Jump out early if PLLST sets before loop finishes
		if (MCG_S & MCG_S_PLLST_MASK) 
		{
			break; 
		}
	}

	// Check bit is really set and return with error if not set
	if (!(MCG_S & MCG_S_PLLST_MASK))
	{
		return 0x16; 
	}

	// Wait for LOCK bit to set
	for (i = 0 ; i < 4000 ; i++)
	{
		// Jump out early if LOCK sets before loop finishes
		if (MCG_S & MCG_S_LOCK0_MASK)
		{
			break; 
		}
	}

	// Check bit is really set and return with error if not set
	if (!(MCG_S & MCG_S_LOCK0_MASK)) 
	{
		return 0x44; 
	}

	// Use actual PLL settings to calculate PLL frequency
	actualCrystalDivider = ((MCG_C5 & MCG_C5_PRDIV0_MASK) + 1);
	actualPLLMultiplier = ((MCG_C6 & MCG_C6_VDIV0_MASK) + 24);

	// Now in PBE

	// Clear CLKS to switch CLKS mux to select PLL as MCG_OUT
	MCG_C1 &= ~MCG_C1_CLKS_MASK; 

	// Wait for clock status bits to update
	for (i = 0 ; i < 2000 ; i++)
	{
		// Jump out early if CLKST = 3 before loop finishes
		if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3)
		{
			break; 
		}
	}

	// Check CLKST is set correctly and return with error if not
	if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3) return 0x1B; 

	//MCGOUT equals PLL output frequency
	return ((crystalValue / actualCrystalDivider) * actualPLLMultiplier); 
} // pll_init
