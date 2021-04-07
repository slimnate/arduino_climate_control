#include "DateTime.h"
#include "Scheduling.h"


struct LightControllerSettings {
    Schedule* schedule;

    LightControllerSettings(Schedule*);
};

class LightController {
    private:
        byte dayControlPin, nightControlPin;
        DayNight status;
        LightControllerSettings* settings;
        
    public:
        LightController(byte dayControlPin, byte nightControlPin);

        void configure(LightControllerSettings*);
        void update();
        void enableLights(DayNight);
        DayNight getStatus();
};