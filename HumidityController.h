#include "DHT22.h"
#include "AtomizerController.h"
#include "FanController.h"

class HumidityController {
    private:
        DHT22 sensorOne;
        DHT22 sensorTwo;
        AtomizerController atomizer;
        FanController fans;
        float targetHumidity;
        float kickonHumidity;
        int fanStopDelay;

    public:
        HumidityController(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin, byte fansPin);

        void update();
};