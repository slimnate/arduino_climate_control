#include "HumidityController.h"

HumidityController::HumidityController(byte sensorOnePin, byte sensorTwoPin, byte atomizerPin, byte fansPin) :
    sensorOne(sensorOnePin), sensorTwo(sensorTwoPin), atomizer(atomizerPin), fans(fansPin) { };

void HumidityController::update() {
    // update sensor readings
    sensorOne.updateValues();
    sensorTwo.updateValues();

    //check values against threshold
}
