
typedef enum {
    Switch_Idle,
    Switch_Debounce_On,
    Switch_Pressed,
    Switch_Debounce_Off,
    Switch_Lockout
} Switch_State_t;


typedef struct {
  Switch_State_t state;
  GPIO_Config* gpio;
  uint32_t timestamp;
} Switch_t;



void Switch_Input (Switch_t* _switch)
{

  uint8_t pressed = !gpio_getPinValue(_switch->gpio->port, _switch->gpio_pin);
  
  switch(_switch->state)
		{

    case Switch_Idle:
			//Waiting for a falling edge & Pin is active LOW
			if (pressed) {
        _switch->timestamp = msTick;
        _switch->state = Switch_Debounce_On;
      }
			break;

    case Switch_Debounce_On:
			// Button pressed but need to be confirmed if it's a real press or noise
			// If pin released before 10ms considered as noise and reset
			if (!pressed) {
          _switch->state = Switch_Idle;
          break;
      }
			if ((msTick - _switch->state) >= 10) {
        _switch->state = Switch_Pressed;
      }
			break;

    case Switch_Pressed:
			// Valid pressed + Rising edge meaning the button has been pressed
			if (!pressed) {
        GPIO_Toggle(_switch->gpio);
        _switch->timestamp = msTick; 
        _switch->state = Switch_Debounce_Off;
      }
			break;

    case Switch_Debounce_Off:
      // Debouncing off is used to catch bounce patterns exceeding the 10ms
      // length given for Switch_Debounce_On.
      // https://www.ganssle.com/debouncing.htm
      if (pressed) {
        _switch->state = Switch_Pressed;
        break;
      }      
			if ((msTick - _switch->state) >= 10) {
        _switch->state = Switch_Lockout;
      }
      break;
      
    case Switch_Lockout:
			// Pressed has been registered now ignoring input for 2s
			if ((msTick - _switch->timestamp) >= 2000) {
        _switch->state = Switch_Idle;
      }
			break;
		}

}



