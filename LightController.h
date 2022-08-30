#ifndef LIGHT_CONTROLLER_H
#define LIGHT_CONTROLLER_H

#include "Arduino.h"
#include "Scheduling.h"
#include "DateTime.h"
#include "Time.h"
#include "TimeAlarms.h"

// Represents the LightControllerSettings status
struct LightControllerSettings {
    int updateInterval;
    Schedule* schedule;

    LightControllerSettings(Schedule*, int);
};

// Represents the LightController class, provides methods for controlling the light system.
class LightController {
    private:
        static byte dayControlPin, nightControlPin;
        static LightStatus status;
        static LightControllerSettings* settings;
        
    public:
        static void init(byte dayControlPin, byte nightControlPin, LightControllerSettings*);
        static void update();
        static void enableLights(LightStatus);
        static LightStatus getStatus();
        static const char* getStatusString();
};

#endif