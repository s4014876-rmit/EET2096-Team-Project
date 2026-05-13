
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

	const uint32_t AHB1ENR_Mask  = \
		RCC_AHB1ENR_GPIOAEN        | \
		RCC_AHB1ENR_GPIOBEN        | \
		RCC_AHB1ENR_GPIOFEN;
	
	const uint32_t AHB1RSTR_Mask = \
		RCC_AHB1RSTR_GPIOARST      | \
		RCC_AHB1RSTR_GPIOBRST      | \
		RCC_AHB1RSTR_GPIOFRST;
	
	const uint32_t APB1ENR_Mask  = \
		RCC_APB1ENR_TIM6EN         | \
		RCC_APB1ENR_USART3EN;
	
	const uint32_t APB1RSTR_Mask = \
		RCC_APB1RSTR_TIM6RST       | \
		RCC_APB1RSTR_USART3RST;

  const uint32_t APB2ENR_Mask  = \
    RCC_APB2EN_ADC3EN;
    
	const uint32_t APB2RSTR_Mask = \
		RCC_APB2RSTR_ADC3RST;
  
	// AHB1 Enable
	RCC->AHB1ENR  |= AHB1ENR_Mask;
	RCC->AHB1RSTR |= AHB1RSTR_Mask;
	__asm("NOP");
	__asm("NOP");
	RCC->AHB1RSTR &= ~AHB1RSTR_Mask;
	__asm("NOP");
	__asm("NOP");	

	// APB1 Enable
	RCC->APB1ENR  |= APB1ENR_Mask;
	RCC->APB1RSTR |= APB1RSTR_Mask;
	__asm("NOP");
	__asm("NOP");
	RCC->APB1RSTR &= ~APB1RSTR_Mask;
	__asm("NOP");
	__asm("NOP");
  
	// APB2 Enable
	RCC->APB2ENR  |= APB2ENR_Mask;
	RCC->APB2RSTR |= APB2RSTR_Mask;
	__asm("NOP");
	__asm("NOP");
	RCC->APB2RSTR &= ~APB2RSTR_Mask;
	__asm("NOP");
	__asm("NOP");
  
}

//******************************************************************************//
// Function: configADC3()
// Input : None
// Return : None
// Description : Configuration for ADC3
// *****************************************************************************//
void configADC3()
{

  // GPIO configuration.
  const GPIO_Config gpio_adc = {
    .port       = GPIOF,
		.pin        = Pin10,				
		.mode       = GPIO_Analog,			
		.pullUpDown = GPIO_No_Pull,
		.outputType = GPIO_Output_PushPull,
		.speed      = GPIO_25MHz			
	};
  
  gpio_configureGPIO(&gpio_adc);
  
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

  const GPIO_Config digital_input = {
    .MODER
  };

  
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
	
	GPIOF->MODER &= ~(GPIO_MODER_MODE8_Msk);
	GPIOF->MODER |= (1 << GPIO_MODER_MODE8_Pos);
	
	//Set all outputs to OFF initially
	GPIOA->ODR &= ~(GPIO_ODR_OD9_Msk);
	GPIOB->ODR &= ~(GPIO_ODR_OD1_Msk);
	GPIOB->ODR &= ~(GPIO_ODR_OD8_Msk);
	GPIOF->ODR &= ~(GPIO_ODR_OD8_Msk);
	
	//Alternate function 
	GPIOB->MODER &= ~(GPIO_MODER_MODE10_Msk);
	GPIOB->MODER |= (2 << GPIO_MODER_MODE10_Pos);
	
	GPIOB->MODER &= ~(GPIO_MODER_MODE11_Msk);
	GPIOB->MODER |= (2 << GPIO_MODER_MODE11_Pos);
	
	//Analog Input
	GPIOF->MODER |= GPIO_MODER_MODE10_Msk;
	GPIOF->PUPDR &= ~(GPIO_PUPDR_PUPD10_Msk);
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
	configADC3();
	
	// Bring up the GPIO for the power regulators.
	boardSupport_init();
	
	SwitchState_t fanState = IDLE;
	SwitchState_t lightState = IDLE;
	uint32_t timeStamp = 0;
	uint8_t fanOutput = 0;
	uint8_t lightOutput = 0;
	
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
			if ((GPIOB->IDR & GPIO_IDR_ID0_Msk) != 0)
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
			if ((GPIOB->IDR & GPIO_IDR_ID0_Msk) != 0)
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
				if ((GPIOA->IDR & GPIO_IDR_ID10_Msk) != 0)
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
				if ((GPIOA->IDR & GPIO_IDR_ID10_Msk) != 0)
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

		//Store result and convert to temp
		uint16_t adcVal = sample();
		float tempC = 55.0f - ((float)adcVal * 85.0f/4095.0f);
		
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

