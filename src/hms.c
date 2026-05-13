#include "hms.h"


uint8_t HMS_HMS_To_Byte(HMS_Output_t* hms)
{
	uint8_t byte;
  byte = 0b01000001               | \
    ((hms->light)   ? 1U << 5 : 0) | \
    ((hms->heater)  ? 1U << 4 : 0) | \
    ((hms->cooling) ? 1U << 3 : 0) | \
    ((hms->fan)     ? 1U << 2 : 0);
	return byte;
}


static bool HMS_Byte_Validate (uint8_t byte)
{
	const uint8_t mask_high = 0b01000001;
	const uint8_t mask_low  = 0b10000010;
	const uint8_t mask      = mask_high | mask_low;

  if ((( byte & mask) == mask_high) && \
      ((~byte & mask) == mask_low ) ) {
    return true;
  } else {
    return false;
  }
}


bool HMS_Byte_To_HMS (HMS_Output_t* hms, uint8_t byte)
{
	// Return value according to Table 2,3 specifications.
	if (HMS_Byte_Validate(byte)) {
		hms->light   = (byte & (1U << 5)) ? true : false;
		hms->heater  = (byte & (1U << 4)) ? true : false;
		hms->cooling = (byte & (1U << 3)) ? true : false;
		hms->fan     = (byte & (1U << 2)) ? true : false;
    return true;
	}

  // Otherwise return false.
	return false;
}



bool UART_Receive_HMS (HMS_Output_t* hms)
{
  static bool header = false;
		
	// Grab any user input.
	if (USART3->SR & USART_SR_RXNE) {
		uint8_t input = (uint8_t) (USART3->DR & 0xFF);

    // Header found, so wait for data to change the HMS.
		if (input == 0x26) {
			header = true;
		}
    // If the prior byte was the header, the next byte should be HMS information.
    // If the byte was a valid HMS byte, return true, otherwise return false.
    else if (header) {
      header = false;
      hms* = HMS_from_Byte(input);
      return hms->valid;
		}

	}

  // Flow through to this point indicates a complete packet has
  // not yet been received.
  return false;
}



typedef enum {
  Code__UART_Transmit_MMS__Transmit_Begin,
  Code__UART_Transmit_MMS__Transmit_Complete,
  Code__UART_Transmit_MMS__Busy,
  Code__UART_Transmit_MMS__Unknown,
} Code__UART_Transmit_MMS_t;


Code__UART_Transmit_MMS_t
UART_Transmit_HMS (HMS_Output_t* hms, double temperature)
{
  Code__UART_Transmit_MMS_t code;
  static bool busy = false;
  static uint8_t packet[10];
  static uint8_t packet_reverse_index = 0;

  const uint8_t Packet_Header    = 0x26;
  const uint8_t Packet_Delimiter = 0x7E;

  // If this function is not busy, then set it up for work.
  if (!busy) {

    busy = true;
    packet_index = 10;
    code = Code__UART_Transmit_MMS__Transmit_Begin;

    // Construct the packet to send.
    packet[0] = Packet_Header;
    packet[1] = Packet_Delimiter; 
    Temperature_String(&packet[2], temperature);  
    packet[8] = Packet_Delimiter;
    packet[9] = HMS_HMS_To_Byte(hms);
  }

  // If busy, and if either the prior transmission is complete or we are just
  // beginning to transmit, then transmit.
  if (busy && ((USART3->SR & USART_SR_TC) ||
               (code == Code__UART_Transmit_MMS__Transmit_Begin))) {

    // Send character, select next character.
    USART3->DR = packet[10 - packet_reverse_index];
    packet_reverse_index--;

    // If all characters were sent,
    if (packet_reverse_index == 0) {
      busy = false;
      code = Code__UART_Transmit_MMS__Transmit_Complete;
    } else {
      if (code != Code__UART_Transmit_MMS__Transmit_Begin) {
        code = Code__UART_Transmit_MMS__Busy;
      }
    }
    
  }
  else if (busy) {
    code = Code__UART_Transmit_MMS__Busy;
  }
  else {
    code = Code__UART_Transmit_MMS__Unknown;
  }

  return code;
}
