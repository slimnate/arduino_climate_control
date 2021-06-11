#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include "Arduino.h"

// Represents a DC fan controller
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