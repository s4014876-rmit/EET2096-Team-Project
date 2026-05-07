
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

typedef enum {
    IDLE,
    DEBOUNCE,
    WAIT_RELEASE,
    LOCKOUT
} SwitchState_t;


//******************************************************************************//
// Function: configRCC()
// Input : None
// Return : None
// Description : Configuration for RCC
// *****************************************************************************//
void configRCC()
{
	RCC->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOAEN_Pos);
	RCC->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOBEN_Pos);
	RCC->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOFEN_Pos);
}

//******************************************************************************//
// Function: configADC3()
// Input : None
// Return : None
// Description : Configuration for ADC3
// *****************************************************************************//
void configADC3()
{
	//Enable the perpheral interfacce
	RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
	
	//Reset the peripheral interface
	RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;
	
	//Wait for 2 cycles
	__asm("NOP");
	__asm("NOP");
	
	//Clear the reset
	RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADCRST);
	
	//Wait for 2 cycles
	__asm("NOP");
	__asm("NOP");
	
	//Disable the battery sensing channel
	ADC123_COMMON->CCR &= (~ADC_CCR_VBATE);
	
	//ENable the temp control sensor channel
	ADC123_COMMON->CCR |= (ADC_CCR_TSVREFE) | (0x03 << ADC_CCR_ADCPRE_Pos);
	
	//Disable scan mode and set the resolution bit to 12
	ADC3->CR1 &= ~(ADC_CR1_SCAN | ADC_CR1_RES_Msk);
	
	//Alignment set to right & set single mode conversion
	ADC3->CR2 &= ~(ADC_CR2_CONT | ADC_CR2_ALIGN  | ADC_CR2_SWSTART);
	
	//Set to a single channel 8
	ADC3->SQR3 &= ~(ADC_SQR3_SQ1_Msk);
	ADC3->SQR3 |= 0x08;
	ADC3->SQR1 &= ~(ADC_SQR1_L_Msk);
	
	//Set the sample time registers to 56 cycles
	ADC3->SMPR2 &= ~(ADC_SMPR2_SMP8_Msk);
	ADC3->SMPR2 |= 0x03 << (ADC_SMPR2_SMP8_Pos);
	
	//Enable the ADC
	ADC3->CR2 |= ADC_CR2_ADON;
}
//******************************************************************************//
// Function: configGPIO()
// Input : None
// Return : None
// Description : Configuration for RCC
// *****************************************************************************//
void configGPIO()
{
	//Digital Input
	GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk);
	GPIOA->MODER &= ~(GPIO_MODER_MODE10_Msk);
	GPIOB->MODER &= ~(GPIO_MODER_MODE0_Msk);
	
	//Pull-up for switches
	GPIOA->PUPDR |= (1 << GPIO_PUPDR_PUPD8_Pos);
	GPIOA->PUPDR |= (1 << GPIO_PUPDR_PUPD10_Pos);
	GPIOB->PUPDR |= (1 << GPIO_PUPDR_PUPD0_Pos);
	
	//Digital Output (Clear and set)
	GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk);
	GPIOA->MODER |= (1 << GPIO_MODER_MODE9_Pos);
	
	GPIOB->MODER &= ~(GPIO_MODER_MODE1_Msk);
	GPIOB->MODER |= (1 << GPIO_MODER_MODE1_Pos);
	
	GPIOB->MODER &= ~(GPIO_MODER_MODE8_Msk);
	GPIOB->MODER |= (1 << GPIO_MODER_MODE8_Pos);
	
	GPIOF->MODER &= ~(GPIO_MODER_MODE1_Msk);
	GPIOF->MODER |= (1 << GPIO_MODER_MODE1_Pos);
	
	//Set all outputs to OFF initially
	GPIOA->ODR &= ~(GPIO_ODR_OD9_Msk);
	GPIOB->ODR &= ~(GPIO_ODR_OD1_Msk);
	GPIOB->ODR &= ~(GPIO_ODR_OD8_Msk);
	GPIOF->ODR &= ~(GPIO_ODR_OD1_Msk);
	
	//Alternate function 
	GPIOB->MODER &= ~(GPIO_MODER_MODE10_Msk);
	GPIOB->MODER |= (2 << GPIO_MODER_MODE10_Pos);
	
	GPIOB->MODER &= ~(GPIO_MODER_MODE11_Msk);
	GPIOB->MODER |= (2 << GPIO_MODER_MODE11_Pos);
	
	//Analog Input
	GPIOF->MODER |= GPIO_MODER_MODE10_Msk;
	GPIOF->PUPDR &= ~(GPIO_PUPDR_PUPD10_Msk);
	
	//Set AF7 for UART3
	GPIOB->AFR[1] &= ~(0xF >> 8);
	GPIOB->AFR[1] |= (7 << 8);		//PB10 = AF7
	
	GPIOB->AFR[1] &= ~(0xF >> 12);
	GPIOB->AFR[1] |= (7 << 12);		//PB11 = AF7
}



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
	
	// Bring up the GPIO for the power regulators.
	boardSupport_init();
	
  while (1)
  {
	switch(fanState)
		{
			case IDLE:
			//Waiting for a falling edge & Pin is active LOW
			if ((GPIOB->IDR & GPIO_IDR_ID0_Msk) == 0)
			{
				timeStamp = msTick;
				fanState = DEBOUNCE;
			}
			break;
			case DEBOUNCE:
			// Button pressed but need to be confirmed if it's a real press or noise
			// If pin released before 10ms considered as noise and reset
			if ((GPIOB->IDR & GPIO_IDR_ID0_Msk) == 1)
			{
				fanState = IDLE;
			}
			if ((msTick - timeStamp) >= 10 && (GPIOB->IDR & GPIO_IDR_ID0_Msk) == 0)
			{
				fanState = WAIT_RELEASE;
			}
			break;

			case WAIT_RELEASE:
			// Valid pressed + Rising edge meaning the button has been pressed
			if ((GPIOB->IDR & GPIO_IDR_ID0_Msk) == 1)
			{
				fanOutput ^= 1;
				timeStamp = msTick; 
				fanState = LOCKOUT;
			}
			break;

			case LOCKOUT:
			// Pressed has been registered now ignoring input for 2s
			if ((msTick - timeStamp) >= 2000)
			{
			 fanState = IDLE;
			}
			break;
		}
	}
	
	//Check fanOutput to set or clear the ODR bit
	if(fanOutput == 1)
	{
		GPIOB->ODR |= GPIO_ODR_OD1_Msk;
	}
	else
	{
		GPIOB->ODR &= ~(GPIO_ODR_OD1_Msk);
	}
	
	
	switch(lightState)
	{
			case IDLE:
			if ((GPIOA->IDR & GPIO_IDR_ID10_Msk) == 0)
			{
				timeStamp = msTick;
				lightState = DEBOUNCE;
			}
			break;
			
			case DEBOUNCE:
			// Button pressed but need to be confirmed if it's a real press or noise
			// If pin released before 10ms considered as noise and reset
			if ((GPIOA->IDR & GPIO_IDR_ID10_Msk) == 1)
			{
				lightState = IDLE;
			}
			if ((msTick - timeStamp) >= 10 && (GPIOA->IDR & GPIO_IDR_ID10_Msk) == 0)
			{
				lightState = WAIT_RELEASE;
			}
			break;

			case WAIT_RELEASE:
			// Valid pressed + Rising edge meaning the button has been pressed
			if ((GPIOA->IDR & GPIO_IDR_ID10_Msk) == 1)
			{
				lightOutput ^= 1;
				timeStamp = msTick; 
				lightState = LOCKOUT;
			}
			break;

			case LOCKOUT:
			// Pressed has been registered now ignoring input for 2s
			if ((msTick - timeStamp) >= 2000)
			{
			 lightState = IDLE;
			}
			break;
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
	
	if(lightOutput == 1)
	{
		GPIOA->ODR |= GPIO_ODR_OD10_Msk;
	}
	else
	{
		GPIOA->ODR &= ~(GPIO_ODR_OD10_Msk);
	}
  }
} 

