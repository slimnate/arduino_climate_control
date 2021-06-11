#ifndef BITFLAG_H
#define BITFLAG_H

#include "Arduino.h"

const byte BIT_HUM_TARGET   = 0b1000; // Humidity target percent flag
const byte BIT_HUM_KICK_ON  = 0b0100; // Humidity kick on percent flag
const byte BIT_HUM_FAN_STOP = 0b0010; // Humidity fan stop delay flag
const byte BIT_HUM_UPDATE   = 0b0001; // Humidity update interval flag

struct Bitflag {
    private:
        int _bits = 0b0000;
    public:
        void setBit(int toSet);
        bool checkBit(int toCheck);
        bool checkAny();
};

#endif