#include "Arduino.h"
#include "DHT22.h"

DHT22::DHT22(byte pin) : pin(pin), dht(pin) { };

void DHT22::updateValues() {
    int err = SimpleDHTErrSuccess;

    //update values
    err = dht.read2(&temperature, &humidity, NULL);

    //print any errors
    if(err != SimpleDHTErrSuccess) {
        Serial.print("Error reading DHT22 on pin:");
        Serial.println(pin);

        Serial.print(SimpleDHTErrCode(err));
        Serial.print(", ");
        Serial.println(SimpleDHTErrDuration(err));
    }
};

float DHT22::getTemperature() {
    return temperature;
};

float DHT22::getHumidity() {
    return humidity;
}