/*
 * Clock.h
 *
 *  Created on: Jun 25, 2013
 *      Author: Jose
 */

#include "derivative.h" /* include peripheral declarations */

#define CORE_CLK 		(48000000)
#define BUS_CLK 		(24000000)

#ifndef MCG_H_
#define MCG_H_

#define LOW_POWER 0
#define HIGH_GAIN 1

#define CANNED_OSC  0
#define CRYSTAL 1

#define PLL_0 0
#define PLL_1 1

#define PLL_ONLY 0
#define MCGOUT 1

#define PLL_MUL_MHZ		48
#define CRYSTAL_DIV		4

#define OUTDIV1 1
#define OUTDIV4 2

typedef enum
{
	kCrystalInvalidFrequency,
	kCrystalFrequencyOutOfRange,
	kClockDividerOutOfSpec,
	kPLLMultiplierOutOfSpec,
	kPLLReferenceClockOutOfSpec,
	kPLLFrequencyOutOfSpec,
	kExtClockNotSelectedError
}kMCGStatus;


int32_t pll_init(int32_t crystalValue, uint8_t highGainSelect, uint8_t referenceSelect, int8_t extClockDivider, int8_t pllMultiplyVal, uint8_t mcgout_select);

#endif /* MCG_H_ */
