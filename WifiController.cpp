#include "Arduino.h"
#include "WiFi.h"

#include "WifiData.h"
#include "WifiController.h"


String encryptionTypeToString(byte encryptionType) {
    switch(encryptionType) {
        case ENC_TYPE_TKIP:
            return "WPA";
            break;
        case ENC_TYPE_WEP:
            return "WEP";
            break;
        case ENC_TYPE_CCMP:
            return "WPA2";
            break;
        case ENC_TYPE_AUTO:
            return "Auto";
            break;
        case ENC_TYPE_NONE:
            return "Open";
            break;
        case ENC_TYPE_UNKNOWN:
            return "Unknown";
            break;
    }
};

// WifiControllerSettings class
WifiControllerSettings::WifiControllerSettings(String ssid, String pass, bool requireLatestFirmware)
    : ssid(ssid), password(pass), requireLatestFirmware(requireLatestFirmware) { }


// WifiController class
String WifiController::firmwareVersion;
MacAddress WifiController::macAddress = NULL;
WifiControllerSettings* WifiController::settings;

void WifiController::init(WifiControllerSettings* s) {
    settings = s;

    //modules
    Serial.println("Checking module: ");
    if(!verifyModule()){
        Serial.println("Aborting WifiController.init()");
    }

    //firmware
    Serial.println("Checking firmware: ");
    bool fwCurrent = verifyFirmware();
    if(settings->requireLatestFirmware && !fwCurrent) {
        Serial.println("Aborting WifiController.init() - latest firmware required.");
    }

    //mac address
    Serial.println("Getting MAC Address:");
    macAddress = getMacAddress();
    Serial.println(macAddress.toString());

    printAvailableNetworks();

    //connect to network
    Serial.println("Connecting to network: " + settings->ssid);
    int ssidLen = settings->ssid.length();
    int passLen = settings->password.length();
    char ssid[ssidLen];
    char pass[passLen];
    
    settings->ssid.toCharArray(ssid, ssidLen + 1);
    settings->password.toCharArray(pass, passLen + 1);

    int attempts = 3;
    while(WiFi.status() != WL_CONNECTED && attempts > 0) {
        attempts--;

        Serial.print("Wifi.begin("); Serial.print(ssid); Serial.print(", "); Serial.print(pass); Serial.println(")");
        WiFi.begin(ssid, pass);
        delay(10000);
        Serial.println("Status: " + statusToString(WiFi.status()));
    }

    //display network info
    IPAddress address = WiFi.localIP();
    address.printTo(Serial);
};

String WifiController::statusToString(byte status) {
    switch(status) {
        case WL_NO_MODULE:
            return "No module available";
        case WL_IDLE_STATUS:
            return "Idle";
        case WL_NO_SSID_AVAIL:
            return "No SSID available";
        case WL_SCAN_COMPLETED:
            return "Scan completed";
        case WL_CONNECTED:
            return "Connected";
        case WL_CONNECT_FAILED:
            return "Connect failed";
        case WL_CONNECTION_LOST:
            return "Connection lost";
        case WL_DISCONNECTED:
            return "Disconnected";
        case WL_AP_CONNECTED:
            return "AP Connected";
        case WL_AP_FAILED:
            return "AP Failed";
        case WL_AP_LISTENING:
            return "AP Listening";
        default:
            return "N/A";
    }
};

bool WifiController::verifyModule() {
    byte moduleStatus = WiFi.status();
    if(moduleStatus == WL_NO_MODULE) {
        Serial.println("  Communication with WiFi module failed. Not connecting..");
        return false;
    }
    Serial.println("  Success - module status: " + statusToString(moduleStatus));
    return true;
};

bool WifiController::verifyFirmware() {
    firmwareVersion = WiFi.firmwareVersion();
    if(firmwareVersion < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("  Firmware outdated: current(" + firmwareVersion
            + ") latest(" + WIFI_FIRMWARE_LATEST_VERSION + ")");
        return false;
    }
    Serial.println("  Firmware up to date: current(" + firmwareVersion + ")");
    return true;
};

MacAddress WifiController::getMacAddress() {
    byte macBytes[6];
    WiFi.macAddress(macBytes);
    MacAddress mac = MacAddress(macBytes);
    return mac;
};

void WifiController::printAvailableNetworks() {
    Serial.println("Scanning networks...");

    byte networkCount = WiFi.scanNetworks();

    if(networkCount == -1) {
        Serial.println("Unable to find any networks");
        return;
    }

    Serial.print("Scan complete - ");
    Serial.print(networkCount);
    Serial.println(" networks found");

    for(byte i = 0; i < networkCount; i++) {
        printNetwork(i);
    }
};

void WifiController::printNetwork(byte i) {
    Serial.print(i+1); Serial.print(" : ");

    //print ssid
    Serial.print("SSID: "); Serial.print(WiFi.SSID(i));

    // print mac address of AP
    byte bssid[6];
    WiFi.BSSID(i, bssid);
    MacAddress apMacAddr = MacAddress(bssid);
    Serial.print("\tBSSID: "); Serial.print(apMacAddr.toString());

    // print encryption type
    Serial.print("\tEncryption: ");
    Serial.println(encryptionTypeToString(WiFi.encryptionType(i)));

    // print channel
    Serial.print("\tChannel: "); Serial.print(WiFi.channel(i));

    // print signal strength
    Serial.print("\tSignal: "); Serial.print(WiFi.RSSI(i)); Serial.println(" dBm");
};