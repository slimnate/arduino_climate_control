#include <SimpleDHT.h>

class DHT22 {
    private:
        byte pin;
        SimpleDHT22 dht;
    
    public:
        float temperature;
        float humidity;

        DHT22(byte pin);

        void updateValues();
};