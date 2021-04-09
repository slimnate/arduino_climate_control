#include "Arduino.h"
#include "FanController.h"

FanController::FanController(byte pin) : controlPin(pin) {
    pinMode(controlPin, OUTPUT);
    enabled = false;
};

bool FanController::isEnabled() {
    return enabled;
};

void FanController::enable() {
    if(!enabled) {
        digitalWrite(controlPin, HIGH);
        enabled = true;
    }
};

void FanController::disable() {
    if(enabled) {
        digitalWrite(controlPin, LOW);
        enabled = false;
    }
}