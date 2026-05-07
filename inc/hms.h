#ifndef __HMS__
#define __HMS__

#include "stdint.h"
#include "stdbool.h"

typedef struct {
	bool valid;
	bool light;
	bool heater;
	bool cooling;
	bool fan;
} HMS_t;

// Converts HMS type to byte.
// If HMS is invalid, then it returns 0xFF, which is otherwise impossible.
uint8_t HMS_to_Byte(HMS_t hms);

// Converts a byte to HMS.
// If the byte does not match the specification, it will return HMS with the
// invalid flag set.
HMS_t HMS_from_Byte(uint8_t byte);

#endif