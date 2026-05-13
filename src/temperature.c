#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "temperature.h

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

#endif /* __TEMPERATURE_H__ */
