#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include "Arduino.h"

class FanController {
    private:
        byte controlPin;
        bool enabled;
        
    public:
        FanController(byte pin);

        bool isEnabled();
        void enable();
        void disable();
};

#endif