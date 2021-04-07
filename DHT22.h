#include <SimpleDHT.h>

class DHT22 {
    private:
        byte pin;
        SimpleDHT22 dht;
        float temperature;
        float humidity;
    
    public:

        DHT22(byte pin);

        void updateValues();
        float getTemperature();
        float getHumidity();
};