#ifndef __SYSTICK__
#define __SYSTICK__

#include <stdint.h>

//Global ms counter
extern volatile uint32_t msTick;

//******************************************************************************//
// Function: SysTick_Handler()
// Input : None
// Return : None
// Description : Runs every 1ms and increment counter
// *****************************************************************************//
void SysTick_Handler(void);


void configSysTick(void);

#endif