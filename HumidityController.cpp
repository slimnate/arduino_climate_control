#include "Arduino.h"
#include "Time.h"
#include "TimeAlarms.h"
#include "DHT22.h"
#include "DateTime.h"
#include "AtomizerController.h"
#include "FanController.h"
#include "HumidityController.h"

// Create a new HumidityControllerSettings object with specified
// target humidity, kick on humidity, fan stop delay (s), and update interval (s)
HumidityControllerSettings::HumidityControllerSettings(float target, float kickOn, int fanStop, int update){
    targetHumidity = target;
    kickOnHumidity = kickOn;
    fanStopDelay = fanStop;
    updateInterval = update;
};

// static member initializers

DHT22 HumidityController::sensorOne = NULL;
DHT22 HumidityController::sensorTwo = NULL;
AtomizerController HumidityController::atomizer = NULL;
FanController HumidityController::fans = NULL;
HumidityControllerSettings* HumidityController::settings;
bool HumidityController::running = false;


// Initialize the humidity controller with provided sensor, atomizer and fan pins, and settings.
void HumidityController::init(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin,
                                  byte fansPin, HumidityControllerSettings* s) {
    //init sensors
    sensorOne = DHT22(sensorOnePin);
    sensorTwo = DHT22(sensorTwoPin);
    atomizer = AtomizerController(atomizerPin);
    fans = FanController(fansPin);

    //init settings and tracking vars
    settings = s;

    Serial.print("setting update interval: "); Serial.println(settings->updateInterval);
    //set up update interval
    Alarm.timerRepeat(settings->updateInterval, update);
};

// Update the humidity system status, called once per update interval
void HumidityController::update() {
    // update sensor readings
    sensorOne.updateValues();
    sensorTwo.updateValues();

    //check values against threshold
    float avgHumidity = averageHumidity();

    if(avgHumidity < settings->kickOnHumidity) {
        // start humidifier when kick-on humidity reached.
        if(!running) {
            runHumidifier();
            running = true;
        }
    } else if(avgHumidity >= settings->targetHumidity) {
        if(running) {
            // stop atomizer when target humidity reached.
            stopAtomizer();

            //set up delayed fan stop timer
            Alarm.timerOnce(settings->fanStopDelay, stopFans);
        }
    }
};

// Enable the humidifier and fans
void HumidityController::runHumidifier() {
    Serial.println("Turning ON humidifier");
    atomizer.enable();
    fans.enable();
};

// Stop the atomizer system
void HumidityController::stopAtomizer() {
    Serial.println("Turning OFF atomizer");
    atomizer.disable();
};

// Stop the fan system
void HumidityController::stopFans() {
    Serial.println("Turning OFF fans");
    fans.disable();
    running = false; // set running false so humidity check knows to start up again.
};

// Return the average humidity of the two DHT22 sensors
float HumidityController::averageHumidity() {
    float h1 = sensorOne.getHumidity();
    float h2 = sensorTwo.getHumidity();

    float avg = (h1 + h2) / 2;

    //print details about averages for debugging
    Serial.print("Average Humidity: ");
    Serial.print(avg);
    Serial.print(" ( "); Serial.print(h1);
    Serial.print(" , ");  Serial.print(h2); Serial.println(" )");

    return avg;
};

// Update the provided variables with the humidity system status
void HumidityController::status(float& avg, float& one, float& two, bool& aEnabled, bool& fEnabled) {
    avg = averageHumidity();
    one = sensorOne.getHumidity();
    two = sensorTwo.getTemperature();
    aEnabled = atomizer.isEnabled();
    fEnabled = fans.isEnabled();
};