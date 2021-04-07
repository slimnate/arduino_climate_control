#include <Time.h>

#include <TimeAlarms.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiNINA.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#include <WiFiStorage.h>
#include <WiFiUdp.h>

#include "HumidityController.h"
#include "LightController.h"

const int SECONDS = 60;

const float HUMIDITY_TARGET_DEFAULT = 90.0;
const float HUMIDITY_KICKON_DEFAULT = 75.0;
const int HUMIDITY_FAN_STOP_DEFAULT = 15 * SECONDS;
const int HUMIDITY_UPDATE_DEFAULT = 60*SECONDS;

const byte PIN_DHT22_ONE = 2;
const byte PIN_DHT22_TWO = 4;
const byte PIN_CTRL_ATOMIZER = 6;
const byte PIN_CTRL_FANS = 8;
const byte PIN_RELAY_DAY = 10;
const byte PIN_RELAY_NIGHT = 12;

HumidityController* humidityController;
LightController* lightController;
HumidityControllerSettings* humidityControllerSettings;

void setup()
{
	// set up humidity controller (sensors, atomizer and fan control)
    humidityController = new HumidityController(PIN_DHT22_ONE, PIN_DHT22_TWO, PIN_CTRL_ATOMIZER, PIN_CTRL_FANS);
    humidityControllerSettings = new HumidityControllerSettings(
        HUMIDITY_TARGET_DEFAULT,
        HUMIDITY_KICKON_DEFAULT,
        HUMIDITY_FAN_STOP_DEFAULT,
        HUMIDITY_UPDATE_DEFAULT
    );
    humidityController->configure(humidityControllerSettings);

    //set up light controller (timer and relays)
    

    //set up web server/bluetooth
}

void loop()
{
	// check sensors, turn on/off humidifier

    //check time, actuate light control relays
}

void setupTimers() {

}

void updateHumidityController() {
    humidityController->update();
}