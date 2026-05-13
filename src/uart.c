#ifndef __UART_H__
#define __UART_H__

#include "uart.h"

#include <stdbool.h>
#include <stdint.h>

#include "stm32f439xx.h"
#include "gpioControl.h"


void configUART ()
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
  const uint32_t USART_ENABLE_MASK = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE
	USART3->CR1 &= ~USART_ENABLE_MASK;

  // Set up a 9-bit word with odd parity enabled.
  // Stop bit is by default set to 1.
  const uint32_t USART_CR1_MASK = \
    (1U << USART_CR1_M_Pos)    |  \
    (1U << USART_CR1_PCE_Pos)  |  \
    (1U << USART_CR1_PS_Pos);
  USART3->CR1 |= USART_CR1_MASK;
  
  // Baud rate set to 57,600 kbps
  // 42e6 / (16 * 57600) ~= 45.5729
  // Mantissa is 45.
  // Fraction is 0.5729 * 16 ~= 9
  USART3->BRR &= ~(USART_BRR_DIV_Mantissa | USART_BRR_DIV_Fraction);
	USART3->BRR |= 45 << USART_BRR_DIV_Mantissa_Pos;
  USART3->BRR |=  9 << USART_BRR_DIV_Fraction_Pos;

	// Manually modify the AFRH/AFRL registers.
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL11;
	GPIOB->AFR[1] |= 7 << GPIO_AFRH_AFSEL11_Pos;
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL10;
	GPIOB->AFR[1] |= 7 << GPIO_AFRH_AFSEL10_Pos;

	// Enable UART device and receiver.
	USART3->CR1 |= USART_ENABLE_MASK;
  
	// Configure UART pins.
	gpio_configureGPIO(&gpio_rx);
	gpio_configureGPIO(&gpio_tx);
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
