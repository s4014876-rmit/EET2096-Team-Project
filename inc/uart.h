#include <stdbool.h>
#include <stdint.h>

// Sets up UART GPIO and peripheral configuration.
void UART_Setup();

// Attempts to receive a byte.
// Returns true if something is received.
bool UART_Receive (uint8_t* byte);

// Attempts to transmit a byte.
// Returns true if transmission was able to be done.
bool UART_Transmit (uint8_t byte);
