#include "AtomizerController.h"
#include <Arduino.h>

AtomizerController::AtomizerController(byte pin) : controlPin(pin) {
    pinMode(controlPin, OUTPUT);
};

bool AtomizerController::isEnabled() {
    return enabled;
}

void AtomizerController::enable() {
    if(!enabled) {
        digitalWrite(controlPin, HIGH);
        enabled = true;
    }
}

void AtomizerController::disable() {
    if(enabled) {
        digitalWrite(controlPin, LOW);
        enabled = false;
    }
}