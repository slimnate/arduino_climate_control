#include "DHT22.h"
#include "Arduino.h"

#include "DateTime.h"
#include "AtomizerController.h"
#include "FanController.h"

struct HumidityControllerSettings {
    float targetHumidity;
    float kickOnHumidity;
    int fanStopDelay;
    int updateInterval;

    HumidityControllerSettings(float target, float kickOn, int fanStop, int update);
};

class HumidityController {
    private:
        static DHT22 sensorOne;
        static DHT22 sensorTwo;
        static AtomizerController atomizer;
        static FanController fans;
        static HumidityControllerSettings* settings;
        static time_t lastRun, lastStop;
        static bool running;

        static float averageHumidity();

    public:
        static void init(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin,
                         byte fansPin, HumidityControllerSettings* s);
        static bool verify();
        static void update();
        static void runHumidifier();
        static void stopAtomizer();
        static void stopFans();
};