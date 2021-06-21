#ifndef WIFI_DATA_H
#define WIFI_DATA_H
#include "Arduino.h"

// Represents a MC address
struct MacAddress {
    byte bytes[6];
    
    MacAddress(byte[6]);
    String toString();
};

#endif