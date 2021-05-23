#ifndef LIGHT_CONTROLLER_H
#define LIGHT_CONTROLLER_H

#include "Arduino.h"
#include "Scheduling.h"
#include "DateTime.h"
#include "Time.h"
#include "TimeAlarms.h"

struct LightControllerSettings {
    int updateInterval;
    Schedule* schedule;

    LightControllerSettings(Schedule*, int);
};

class LightController {
    private:
        static byte dayControlPin, nightControlPin;
        static DayNight status;
        static LightControllerSettings* settings;
        
    public:
        static void init(byte dayControlPin, byte nightControlPin, LightControllerSettings*);
        static void update();
        static void enableLights(DayNight);
        static DayNight getStatus();
        static const char* getStatusString();
};

#endif