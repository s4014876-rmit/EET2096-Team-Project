#include "temperature.h"

#include <stdint.h>
#include "stm32f439xx.h"

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



double Temperature_Sample()
{
	uint16_t sample_raw;
  double sample = 0;
	
	//Trigger & ADC conversion
  const uint16_t adc_mask = (1 << 12) - 1;

	ADC3->CR2 |= ADC_CR2_SWSTART;	
	while ((ADC3->SR & ADC_SR_EOC) == 0x00);
	sample_raw = (ADC3->DR & adc_mask);

  // Maps the sampled value such that
  //   0 = 55.0, 4095 = -30.0
  // and in-between values will be linearly interpolated.
  sample = (double) sample_raw;
  sample = sample * (-1.0 * (30.0 + 55.0) / 4095.0) + 55.0;
  
	return sample;
}




void Temperature_String (char* buffer, double temperature)
{
	// Add the sign.
	buffer[0] = (temperature > 0.0f) ? '+' : '-';
	
	// Intermediate values. Each should be a two digit integer.
	uint8_t mantissa = (uint8_t) temperature;
	uint8_t fraction = ((uint8_t)(temperature * 100.0f)) % (mantissa * 100);
	
	// Fill the string with the digits retrieved from the temperature variable.
	buffer[1] = (mantissa / 10) + '0';
	buffer[2] = (mantissa % 10) + '0';
	buffer[3] = '.';
	buffer[4] = (fraction / 10) + '0';
	buffer[5] = (fraction % 10) + '0';
}
