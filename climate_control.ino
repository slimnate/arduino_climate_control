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

const int MINUTES = 60;

const float HUMIDITY_TARGET_DEFAULT = 90.0; // target 90% humidity
const float HUMIDITY_KICKON_DEFAULT = 75.0; // start when below 75% humidity
const int HUMIDITY_FAN_STOP_DEFAULT = 15; // fans run for 15 seconds after atomizer stops
const int HUMIDITY_UPDATE_DEFAULT = 30; // update humidity every 60 seconds by default

const Time LIGHT_DAY_START_DEFAULT = Time(07,00,00); //day start 7am
const Time LIGHT_NIGHT_START_DEFAULT = Time(19,00,00); //night start 7pm
const int LIGHT_UPDATE_DEFAULT = 1 * MINUTES; // update light every 1 minutes by default

const byte PIN_DHT22_ONE = 2;
const byte PIN_DHT22_TWO = 4;
const byte PIN_CTRL_ATOMIZER = 6;
const byte PIN_CTRL_FANS = 8;
const byte PIN_RELAY_DAY = 10;
const byte PIN_RELAY_NIGHT = 12;

HumidityController* humidityController;
LightController* lightController;

HumidityControllerSettings* humidityControllerSettings;
LightControllerSettings* lightControllerSettings;

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
    lightController = new LightController(PIN_RELAY_DAY, PIN_RELAY_NIGHT);
    lightControllerSettings = new LightControllerSettings(
        new FixedSchedule(
            new ScheduleEntry(
                LIGHT_DAY_START_DEFAULT,
                LIGHT_NIGHT_START_DEFAULT
            )
        )
    );
    lightController->configure(lightControllerSettings);

    //TODO: set up web server/bluetooth

    //set up update alarms
    setupTimers();
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

void updateLightController() {
    lightController->update();
}