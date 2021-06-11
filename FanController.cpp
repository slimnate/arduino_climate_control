#include "Arduino.h"
#include "FanController.h"

// Create a new FanController with specified control pin
FanController::FanController(byte pin) : controlPin(pin) {
    pinMode(controlPin, OUTPUT);
    enabled = false;
};

// Return true if the fan controller is currently enabled
bool FanController::isEnabled() {
    return enabled;
};

// Enable the fan controller
void FanController::enable() {
    digitalWrite(controlPin, HIGH);
    enabled = true;
};

// Disable the fan controller
void FanController::disable() {
    digitalWrite(controlPin, LOW);
    enabled = false;
}