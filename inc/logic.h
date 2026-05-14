#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <stdint.h>

// States for state machine, for debouncing an input switch.
typedef enum {
    Switch_Idle,
    Switch_Debounce_On,
    Switch_Pressed,
    Switch_Debounce_Off,
    Switch_Lockout
} Switch_State_t;

// 
typedef struct {
  Switch_State_t state;
  GPIO_Config* gpio;
  uint32_t timestamp;
  bool active;
} Switch_t;




// Using a state machine, samples _switch. 
void Switch_Input (Switch_t* _switch);


#endif /* __LOGIC_H__ */
