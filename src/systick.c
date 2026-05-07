#include "systick.h"
#include "stm32f439xx.h"

//Global ms counter
volatile uint32_t msTick = 0;

//******************************************************************************//
// Function: SysTick_Handler()
// Input : None
// Return : None
// Description : Runs every 1ms and increment counter
// *****************************************************************************//
void SysTick_Handler(void)
{
	msTick++;
}

//******************************************************************************//
// Function: configSysTick()
// Input : None
// Return : None
// Description : 
// *****************************************************************************//
void configSysTick(void)
{
	SysTick->LOAD = 167999;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}