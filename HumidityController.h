#include "DHT22.h"

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
        DHT22 sensorOne;
        DHT22 sensorTwo;
        AtomizerController atomizer;
        FanController fans;
        HumidityControllerSettings* settings;
        bool running;

        float averageHumidity();

    public:
        HumidityController(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin, byte fansPin);

        void configure(HumidityControllerSettings* s);
        bool verify();
        void update();
        bool isRunning();
        void runHumidifier();
        void stopHumidifier();
};