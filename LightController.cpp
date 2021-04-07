#include "LightController.h"
#include "Scheduling.h"
#include "DateTime.h"

#include <Time.h>
#include <Arduino.h>

// LightControllerSettings class
LightControllerSettings::LightControllerSettings(Schedule* s) {
    schedule = s;
};


// LightController class
LightController::LightController(byte dayPin, byte nightPin)
    : dayControlPin(dayPin), nightControlPin(nightPin) {
    //configure pin modes
    pinMode(dayControlPin, OUTPUT);
    pinMode(nightControlPin, OUTPUT);
};

void LightController::configure(LightControllerSettings* s) {
    settings = s;
};

void LightController::update() {
    Date today = Date(year(), (byte)month(), (byte)day());
    Time now = Time((byte)hour(), (byte)minute(), (byte)second());

    ScheduleEntry* sched = settings->schedule->getEntry(today);
    DayNight newStatus = sched->getDayNight(now);

    if(status != newStatus) {
        // if the status has changed, switch lights
        Serial.print("Switching lights from ");
        Serial.print(status == DAY ? "day" : "night");
        Serial.print(" to ");
        Serial.println(newStatus == DAY ? "day." : "night.");

        Serial.print("Date: ");
        Serial.print(today.year); Serial.print("-");
        Serial.print(today.month); Serial.print("-");
        Serial.println(today.day);

        Serial.print("Time: ");
        Serial.print(now.hours); Serial.print(":");
        Serial.print(now.minutes); Serial.print(":");
        Serial.println(now.seconds);

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