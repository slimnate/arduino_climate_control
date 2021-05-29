#ifndef GLOBALS_H
#define GLOBALS_H

#include "WiFiNINA.h"
#include "HumidityController.h"
#include "LightController.h"
#include "WifiController.h"
#include "NTPClient.h"
#include "WebServer.h"
#include "Router.h"

// global vars

HumidityControllerSettings* humidityControllerSettings;
LightControllerSettings* lightControllerSettings;
WifiControllerSettings* wifiControllersettings;
Schedule* lightControllerSchedule;

WiFiUDP udp;
NTPClient ntp = NTPClient(udp);

WebServer server;
Router router;

#endif