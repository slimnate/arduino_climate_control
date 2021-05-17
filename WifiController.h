#ifndef WIFI_CONTROLlER_H
#define WIFI_CONTROLLER_H

#include "Arduino.h"
#include "WifiData.h"

struct WifiControllerSettings {
    String ssid;
    String password;
    bool requireLatestFirmware;

    WifiControllerSettings(String, String, bool);
};

class WifiController {
    private:
        static String firmwareVersion;
        static MacAddress macAddress;
        static WifiControllerSettings* settings;
    public:
        static void init(WifiControllerSettings*);
        static bool verifyModule();
        static bool verifyFirmware();
        static MacAddress getMacAddress();

        static void printAvailableNetworks();
        static void printNetwork(byte);

        static String statusToString(byte);
};

#endif