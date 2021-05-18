#include "Arduino.h"
#include "AtomizerController.h"

AtomizerController::AtomizerController(byte pin) : controlPin(pin) {
    pinMode(controlPin, OUTPUT);
    enabled = false;
};

bool AtomizerController::isEnabled() {
    return enabled;
}

void AtomizerController::enable() {
    digitalWrite(controlPin, HIGH);
    enabled = true;
}

void AtomizerController::disable() {
    digitalWrite(controlPin, LOW);
    enabled = false;
}