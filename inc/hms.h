#ifndef __HMS_H__
#define __HMS_H__

#include "stdint.h"
#include "stdbool.h"


typedef enum {
  AC_Inactive,
  AC_Heating,
  AC_Cooling
} AC_t;

typedef struct {
	bool light;
  AC_t ac;
	bool fan;

  bool _uart_received;
  bool _uart_timeout;
  uint8_t _uart_data;
  uint32_t _uart_timestamp;
} HMS_Output_t;

// Converts HMS type to byte.
// If HMS is invalid, returns 0xFF, which is impossible for normal input.
uint8_t HMS_HMS_To_Byte(HMS_Output_t* hms);

// Converts a byte to HMS.
// If the byte does not match the specification, it will return HMS with the
// invalid flag set.
bool HMS_Byte_To_HMS (HMS_Output_t* hms, uint8_t byte);

// Take in a HMS packet from the UART.
bool UART_Receive_HMS (HMS_Output_t* hms);

// Transmit HMS status to the output.
typedef enum {
  Code__UART_Transmit_MMS__Transmit_Begin,
  Code__UART_Transmit_MMS__Transmit_Complete,
  Code__UART_Transmit_MMS__Busy,
  Code__UART_Transmit_MMS__Unknown,
} Code__UART_Transmit_MMS_t;

Code__UART_Transmit_MMS_t
UART_Transmit_HMS (HMS_Output_t* hms, double temperature);

#endif /* __HMS_H__ */
