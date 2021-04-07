#include "HumidityController.h"
#include "TimeAlarms.h"

HumidityControllerSettings::HumidityControllerSettings(float target, float kickOn, int fanStop, int update){
    targetHumidity = target;
    kickOnHumidity = kickOn;
    fanStopDelay = fanStop;
    updateInterval = update;
};

HumidityController::HumidityController(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin, byte fansPin)
    : sensorOne(sensorOnePin), sensorTwo(sensorTwoPin), atomizer(atomizerPin), fans(fansPin) {
        running = false;
};

void HumidityController::configure(HumidityControllerSettings* s) {
    settings = s;
};

bool HumidityController::verify() {
    return true; //TODO: Implement
};

void HumidityController::update() {
    // update sensor readings
    sensorOne.updateValues();
    sensorTwo.updateValues();

    //check values against threshold
    float avgHumidity = averageHumidity();

    if(avgHumidity < settings->kickOnHumidity) {
        // start humidifier when kickon humidity reached.
        runHumidifier();
    } else if(avgHumidity >= settings->targetHumidity) {
        // stop humidifier when target humidity reached.
        stopHumidifier();
    }
};

void HumidityController::runHumidifier() {
    if(!running) {
        Serial.println("Turning ON humidifier");
        atomizer.enable();
        fans.enable();
        running = true;
    }
};

void HumidityController::stopHumidifier() {
    if(running) {
        Serial.println("Turning OFF humidifier");
        atomizer.disable();
        atomizer.disable();
    }
};

bool HumidityController::isRunning() {
    return running;
};

float HumidityController::averageHumidity() {
    float h1 = sensorOne.getHumidity();
    float h2 = sensorTwo.getHumidity();

    float avg = (h1 + h2) / 2;

    //print details about averages fro debugging
    Serial.print("Average Humidity: ");
    Serial.print(avg);
    Serial.print(" ( "); Serial.print(h1);
    Serial.print(",");  Serial.print(h2); Serial.println(")");

    return avg;
};