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
	USART3->CR1 &= ~USART_CR1_UE;
	USART3->CR1 &= ~USART_CR1_RE;

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
	USART3->CR1 |= USART_CR1_UE;
	USART3->CR1 |= USART_CR1_RE;

}





void Float_2_Char (char* buffer, double temperature)
{
	// Add the sign.
	buffer[0] = (temperature > 0.0f) ? '+' : '-';
	
	// Intermediate values. Each should be a two digit integer.
	uint8_t mantissa = (uint8_t) temperature;
	uint8_t fraction = ((uint8_t)(temperature * 100.0f)) % (mantissa * 100);
	
	// Fill the string with digits.
	buffer[1] = (mantissa / 10) + '0';
	buffer[2] = (mantissa % 10) + '0';
	buffer[2] = '.';
	buffer[4] = (fraction / 10) + '0';
	buffer[5] = (fraction % 10) + '0';
}






// Fills buffer with input. Returns true if user input is a string of length 3.
// Non-blocking function. Returns false in any case that is not the reception of
// a string of length 3. Other correctness checks are for higher levels.
HMS_t UART_Receive_Packet(char* buffer)
{
	const HMS_t invalid = {
		.valid = false;
	}
	// Tracks how many characters have been received.
	static bool get_data = false;
		
	// Grab any user input.
	if (USART3->SR & USART_SR_RXNE) {
		uint8_t input = (uint8_t) (USART3->DR & 0xFF);

		if (get_data) {
			return HMS_from_Byte(input);
		}

		if (input == 0x26) {
			get_data = true;
		}

		
	}

	// On a complete reception, reset the static variables, then decide if the
	// received string is valid.
	if (receive_complete) {
		uint8_t length_old = length;
		length = 0;
		receive_complete = false;
		
		// Return a null-terminated string if the length is correct.
		if (length_old == 3) {
			buffer[3] = '\0';
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
	
}

