#include "Arduino.h"
#include "AtomizerController.h"

// Create new atomizer controller with specified control pin
AtomizerController::AtomizerController(byte pin) : controlPin(pin) {
    pinMode(controlPin, OUTPUT);
    enabled = false;
};

// Return true if the atomizer is enabled
bool AtomizerController::isEnabled() {
    return enabled;
};

// Enable the atomizer
void AtomizerController::enable() {
    digitalWrite(controlPin, HIGH);
    enabled = true;
};

// Disable the atomizer
void AtomizerController::disable() {
    digitalWrite(controlPin, LOW);
    enabled = false;
};