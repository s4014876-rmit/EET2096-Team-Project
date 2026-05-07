#include "hms.h"


HMS_t HMS;

HMS_Invalid = {
	
}


uint8_t HMS_to_Byte(HMS_t hms)
{
	uint8_t byte;
	if (hms.valid) {
		byte = 0b01000001               | \
			((hms.light)   ? 1U << 5 : 0) | \
			((hms.heater)  ? 1U << 4 : 0) | \
			((hms.cooling) ? 1U << 3 : 0) | \
			((hms.fan)     ? 1U << 2 : 0);
	} else {
		byte = 0xff;
	}
	return byte;
}



HMS_t HMS_from_Byte(uint8_t byte)
{
	const uint8_t mask_high = 0b01000001;
	const uint8_t mask_low  = 0b10000010;
	const uint8_t mask      = mask_high | mask_low;

	// Return value according to Table 2,3 specifications.
	HMS_t hms;
	if ( ((byte & mask) == mask_high) && ((~byte & mask) == mask_low) ) {
		hms.valid   = true;
		hms.light   = (byte & (1U << 5)) ? true : false;
		hms.heater  = (byte & (1U << 4)) ? true : false;
		hms.cooling = (byte & (1U << 3)) ? true : false;
		hms.fan     = (byte & (1U << 2)) ? true : false;
	} else {
		hms.valid   = false;
	}
	
	return hms;
}