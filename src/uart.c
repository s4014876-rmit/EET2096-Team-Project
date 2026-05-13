#ifndef __UART_H__
#define __UART_H__

#include "uart.h"

#include <stdbool.h>
#include <stdint.h>

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


bool UART_Receive (uint8_t* byte)
{
	if (USART3->SR & USART_SR_RXNE) {
		*input = (uint8_t) (USART3->DR & 0xFF);
    return true;
  } else {
    return false;
  }
}


bool UART_Transmit (uint8_t byte)
{
  if (USART3->SR & USART_SR_TXE) {
    USART3->DR = byte;
    return true;
  } else {
    return false;
  }
}


#endif  /* __UART_H__ */
