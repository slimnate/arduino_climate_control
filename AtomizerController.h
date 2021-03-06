#ifndef ATOMIZER_CONTROLLER_H
#define ATOMIZER_CONTROLLER_H

#include "Arduino.h"

class AtomizerController {
    private:
        byte controlPin;
        bool enabled;

    public:
        AtomizerController(byte pin);

        bool isEnabled();
        void enable();
        void disable();
};

#endif