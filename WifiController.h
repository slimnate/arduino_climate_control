#ifndef WIFI_CONTROLLER_H
#define WIFI_CONTROLLER_H

#include "Arduino.h"
#include "WifiData.h"

// Represents settings for a WifiController object
struct WifiControllerSettings {
    String ssid;
    String password;
    bool requireLatestFirmware;
    int connectionCheckInterval;

    WifiControllerSettings(String, String, bool, int);
};

// Provides methods for managing wifi connections
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
        static void connect();
        static void checkConnectionStatus();

        static void printAvailableNetworks();
        static void printNetwork(byte);

        static String statusToString(byte);
};

#endif