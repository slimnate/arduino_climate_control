#include "WifiData.h"

// MacAddress class
MacAddress::MacAddress(byte b[6]){
    for(int i = 0; i < 5; i++) {
        bytes[i] = b[i];
    }
};

String MacAddress::toString() {
    String result;
    for(int i = 5; i >= 0; i--) {
        if(bytes[i] < 0xF) { //add leading zero if only one hex digit.
            result += "0";
        }

        result += String(bytes[i], HEX); //print digit(s)
        
        if(i > 0) { // add colon spearators
            result += ":";
        }
    }
    return result;
};