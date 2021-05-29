#ifndef HUMIDITY_CONTROLLER_H
#define HUMIDITY_CONTROLLER_H

#include "Arduino.h"
#include "Time.h"
#include "TimeAlarms.h"
#include "DHT22.h"
#include "DateTime.h"
#include "AtomizerController.h"
#include "FanController.h"

// Encapsulates settings for the HumidityController
struct HumidityControllerSettings {
    float targetHumidity;
    float kickOnHumidity;
    int fanStopDelay;
    int updateInterval;

    HumidityControllerSettings(float target, float kickOn, int fanStop, int update);
};

// HumidityController class provides methods for controlling the sensors, atomizer, and fan system.
class HumidityController {
    private:
        static DHT22 sensorOne;
        static DHT22 sensorTwo;
        static AtomizerController atomizer;
        static FanController fans;
        static HumidityControllerSettings* settings;
        static bool running;

        static float averageHumidity();

    public:
        static void init(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin,
                         byte fansPin, HumidityControllerSettings* s);
        static void update();
        static void runHumidifier();
        static void stopAtomizer();
        static void stopFans();

        static void status(float&, float&, float&, bool&, bool&);
};

#endif