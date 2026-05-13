#include "uart.h"

#include <stdbool.h>

#include "stm32f439xx.h"
#include "gpioControl.h"


void UART_Setup()
{
	// Define GPIO configurations for UART.
	GPIO_Config gpio_rx = {
		.port       = GPIOB,
		.pin        = Pin11,				
		.mode       = GPIO_AF,			
		.pullUpDown = GPIO_Pull_Up,
		.outputType = GPIO_Output_PushPull,
		.speed      = GPIO_25MHz			
	};
	
	GPIO_Config gpio_tx = {
		.port       = GPIOB,
		.pin        = Pin10,				
		.mode       = GPIO_AF,			
		.pullUpDown = GPIO_No_Pull,
		.outputType = GPIO_Output_PushPull,
		.speed      = GPIO_25MHz			
	};

	// Turn off UART for configuration.
  uint32_t USART_ENABLE_MASK = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE
	USART3->CR1 &= ~USART_ENABLE_MASK;

	//TODO Change to the new baud rate
	// Baud rate set to 31.25 kHz.
	USART3->BRR &= ~USART_BRR_DIV_Mantissa;
	USART3->BRR |= 84 << USART_BRR_DIV_Mantissa_Pos;

	// Configure UART pins.
	gpio_configureGPIO(&gpio_rx);
	gpio_configureGPIO(&gpio_tx);

	// Manually modify the AFRH/AFRL registers.
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL11;
	GPIOB->AFR[1] |= 7 << GPIO_AFRH_AFSEL11_Pos;
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL10;
	GPIOB->AFR[1] |= 7 << GPIO_AFRH_AFSEL10_Pos;

	// Enable UART device and receiver.
	USART3->CR1 |= USART_ENABLE_MASK;

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





bool UART_Receive (uint8_t* byte)
{
	if (USART3->SR & USART_SR_RXNE) {
		*input = (uint8_t) (USART3->DR & 0xFF);
  }
}





bool UART_Transmit (uint8_t byte)
{
  if (USART3->
}
