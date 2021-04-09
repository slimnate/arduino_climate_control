#include "DateTime.h"
#include "Scheduling.h"


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
};