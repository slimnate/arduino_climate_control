#include "Arduino.h"
#include "Scheduling.h"
#include "DateTime.h"
#include "Time.h"
#include "TimeAlarms.h"
#include "LightController.h"

// LightControllerSettings class
LightControllerSettings::LightControllerSettings(Schedule* s, int interval) : updateInterval(interval) {
    schedule = s;
};

// static member initializers
byte LightController::dayControlPin;
byte LightController::nightControlPin;
DayNight LightController::status;
LightControllerSettings* LightController::settings;

// LightController class
void LightController::init(byte dayPin, byte nightPin, LightControllerSettings* s) {
    // init pins
    dayControlPin = dayPin;
    nightControlPin = nightPin;

    // init settings
    settings = s;

    // configure pin modes
    pinMode(dayControlPin, OUTPUT);
    pinMode(nightControlPin, OUTPUT);

    // set up update interval
    Alarm.timerRepeat(settings->updateInterval, update);
};

void LightController::update() {
    Date today = Date(year(), (byte)month(), (byte)day());
    Time now = Time((byte)hour(), (byte)minute(), (byte)second());

    ScheduleEntry* sched = settings->schedule->getEntry(today);
    DayNight newStatus = sched->getDayNight(now);

    // if the status has changed, switch lights
    if(status != newStatus) {
        //print notification message
        Serial.print("Switching lights from ");
        Serial.print(status == DAY ? "day" : "night");
        Serial.print(" to ");
        Serial.println(newStatus == DAY ? "day." : "night.");

        Serial.print("Date: ");
        today.printSerial();
        
        Serial.print("Time: ");
        now.printSerial();

        //enable appropriate lights
        enableLights(newStatus);
    }
};

DayNight LightController::getStatus() {
    return status;
};

void LightController::enableLights(DayNight newStatus) {
    //update light control pins
    if(newStatus == DAY) {
        digitalWrite(dayControlPin, HIGH);
        digitalWrite(nightControlPin, LOW);
    } else {
        digitalWrite(dayControlPin, LOW);
        digitalWrite(nightControlPin, HIGH);
    }

    //update saved status
    status = newStatus;
}