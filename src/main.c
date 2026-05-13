
/********************************************
*			STM32F439 Main (C Startup File)  			*
*			Developed for the STM32								*
*			Author: Dr. Glenn Matthews						*
*			Source File														*
*     Updated: 04/03/2026 	  							*
********************************************/

#include <stdint.h>
#include "boardSupport.h"
#include "main.h"

#include "systick.h"




//******************************************************************************//
// Function: main()
// Input : None
// Return : None
// Description : Entry point into the application.
// *****************************************************************************//
int main(void)
{
	configRCC();
	configGPIO();
	configSysTick();
	configADC3();
  configUART();
	
	// Bring up the GPIO for the power regulators.
	boardSupport_init();
	
	SwitchState_t fanState = IDLE;
	SwitchState_t lightState = IDLE;
	uint32_t timeStamp = 0;
	uint8_t fanOutput = 0;
	uint8_t lightOutput = 0;
	
  while (1)
  {


    //Check fanOutput to set or clear the ODR bit
		if(fanOutput == 1)
		{
			GPIOB->ODR |= GPIO_ODR_OD1_Msk;
		}
		else
		{
			GPIOB->ODR &= ~(GPIO_ODR_OD1_Msk);
		}
		
					
		//Light intensity sensor
		if ((GPIOA->IDR & GPIO_IDR_ID8_Msk) == 0)
		{
			lightOutput = 0;
		}
		
		//Light switch toggle
		if(lightOutput == 1)
		{
			GPIOA->ODR |= GPIO_ODR_OD9_Msk;
		}
		else
		{
			GPIOA->ODR &= ~(GPIO_ODR_OD9_Msk);
		}

		
		//Auto-control logic
		if (tempC < 22) 
		{
			//Heater ON, Cooling OFF, Fan ON
			GPIOF->ODR |= (GPIO_ODR_OD8_Msk);	
			GPIOB->ODR &= ~(GPIO_ODR_OD8_Msk);
			GPIOB->ODR |= (GPIO_ODR_OD1_Msk);
			
		}
		else if (tempC > 24)
		{
			//Heater OFF, Cooling ON, Fan ON
			GPIOF->ODR &= ~(GPIO_ODR_OD8_Msk);
			GPIOB->ODR |= (GPIO_ODR_OD8_Msk);
			GPIOB->ODR |= (GPIO_ODR_OD1_Msk);
		}
		else
		{
			//BOTH OFF, Fan ON
			GPIOF->ODR &= ~(GPIO_ODR_OD8_Msk);
			GPIOB->ODR &= ~(GPIO_ODR_OD8_Msk);
			GPIOB->ODR |= (GPIO_ODR_OD1_Msk);
		}
	}
} 

