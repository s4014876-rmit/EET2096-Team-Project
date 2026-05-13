#ifndef __GPIO_H__
#define __GPIO_H__

#include "gpioControl.h"


// GPIO Output Configuration

#define GPIO_SETTINGS_DIGITAL_INPUT   \
	.mode       = GPIO_Input,           \
	.pullUpDown = GPIO_Pull_Up,         \
	.outputType = GPIO_Output_PushPull, \
	.speed      = GPIO_25MHz

#define GPIO_COUNT_INPUT 3

const GPIO_Config GPIO_Config_Input[GPIO_COUNT_INPUT] = {
  // Digital Inputs
	{ .port = GPIOA, .pin = Pin10, GPIO_SETTINGS_DIGITAL_INPUT  }, // Light Switch
  { .port = GPIOA, .pin = Pin8,  GPIO_SETTINGS_DIGITAL_INPUT  }, // Light Intensity Sensor
  { .port = GPIOB, .pin = Pin0,  GPIO_SETTINGS_DIGITAL_INPUT  }  // Fan Switch
};


// GPIO Input Configuration

#define GPIO_SETTINGS_DIGITAL_OUTPUT  \
	.mode       = GPIO_Output,          \
	.pullUpDown = GPIO_Pull_Up,         \
	.outputType = GPIO_Output_PushPull, \
	.speed      = GPIO_25MHz

#define GPIO_COUNT_OUTPUT 4

const GPIO_Config GPIO_Config_Output[GPIO_COUNT_OUTPUT] = {
	{ .port = GPIOF, .pin = Pin8,  GPIO_SETTINGS_DIGITAL_OUTPUT }, // Heater
	{ .port = GPIOB, .pin = Pin8,  GPIO_SETTINGS_DIGITAL_OUTPUT }, // Cooling
	{ .port = GPIOB, .pin = Pin1,  GPIO_SETTINGS_DIGITAL_OUTPUT }, // Fan Control
	{ .port = GPIOA, .pin = Pin9,  GPIO_SETTINGS_DIGITAL_OUTPUT }, // Light Control
};


// Set up every GPIO that is used as generic I/O, not including peripherals that
// may need extra actions during setup.
void GPIO_Setup ();

#endif /* __GPIO_H__ */
