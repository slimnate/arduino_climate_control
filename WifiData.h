#ifndef WIFI_DATA_H
#define WIFI_DATA_H
#include "Arduino.h"

struct MacAddress {
    byte bytes[6];
    
    MacAddress(byte[6]);
    String toString();
};

struct WifiData {
  byte bytes[4];
};

#endif