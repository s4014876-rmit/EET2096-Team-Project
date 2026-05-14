#include <stdint.h>

#include "gpio.h"
#include "logic.h"



void Switch_Input (Switch_t* _switch)
{

  uint8_t pressed = !gpio_getPinValue(_switch->gpio->port, _switch->gpio_pin);
  
  switch(_switch->state) {

  case Switch_Idle:
    // Change on press.
    if (pressed) {
      _switch->timestamp = msTick;
      _switch->state = Switch_Debounce_On;
    }
    break;

  case Switch_Debounce_On:
    // Debounce input over 10ms sampling period.
    if (!pressed) {
      _switch->state = Switch_Idle;
      break;
    }
    if ((msTick - _switch->timestamp) >= 10) {
      _switch->state = Switch_Pressed;
    }
    break;

  case Switch_Pressed:
    // Toggle the GPIO once.
    if (!_switch->active) {
      GPIO_Toggle(_switch->gpio);
      _switch->active = true;
    }
    // Catch extended bounce patterns / register button release.
    if (!pressed) {
      GPIO_Toggle(_switch->gpio);
      _switch->timestamp = msTick; 
      _switch->state = Switch_Debounce_Off;
    }
    break;

  case Switch_Debounce_Off:
    // Catch bounce patterns that exceed the 10ms window.
    if (pressed) {
      _switch->state = Switch_Pressed;
      break;
    }
    // If this was a button press, lock out input.
    if ((msTick - _switch->timestamp) >= 10) {
      _switch->state = Switch_Lockout;
    }
    break;
      
  case Switch_Lockout:
    // Ignore input for two seconds.
    _switch->active = false;
    if ((msTick - _switch->timestamp) >= 2000) {
      _switch->state = Switch_Idle;
    }
    break;

  }

}








