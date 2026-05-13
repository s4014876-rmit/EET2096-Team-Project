#include "gpio.h"
#include "gpioControl.h"
#include <stdint.h>

void GPIO_Setup ()
{
  // Set up every GPIO that is used as GPIO, i.e not including peripherals.

  // Configure inputs.
  for (uint8_t i = 0; i < GPIO_COUNT_INPUT; i++) {
    GPIO_Config* gpio = &GPIO_Config_Output[i];
    gpio_configureGPIO(gpio);
  }

  // Configure outputs, and ensure they are set low on startup.
  for (uint8_t i = 0; i < GPIO_COUNT_OUTPUT; i++) {
    GPIO_Config* gpio = &GPIO_Config_Output[i];
    gpio_configureGPIO(gpio);
    gpio_resetGPIO(gpio->port, gpio->pin); 
  }
}
