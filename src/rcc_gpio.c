#include "rcc_gpio.h"
#include "gpioControl.h"
#include <stdint.h>

void configGPIO ()
{
  // Set up every GPIO that is used as GPIO, i.e not including peripherals.

  // Configure inputs.
  for (uint8_t i = 0; i < GPIO_COUNT_INPUT; i++) {
    GPIO_Config* gpio = &GPIO_Config_Input[i];
    gpio_configureGPIO(gpio);
  }

  // Configure outputs, and ensure they are set low on startup.
  for (uint8_t i = 0; i < GPIO_COUNT_OUTPUT; i++) {
    GPIO_Config* gpio = &GPIO_Config_Output[i];
    gpio_configureGPIO(gpio);
    gpio_resetGPIO(gpio->port, gpio->pin); 
  }
}



void GPIO_Toggle (GPIO_Config* gpio)
{
  bool output_value = (gpio->port->ODR & (1U << pin)) ? true : false;
  if (output_value) {
    gpio_resetGPIO(gpio->port, gpio->pin);
  } else {
    gpio_setGPIO(gpio->port, gpio->pin);
  }
}



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
    RCC_APB2ENR_ADC3EN;
    
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
