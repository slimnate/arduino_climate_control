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

const byte PIN_DHT22_ONE = 2;
const byte PIN_DHT22_TWO = 4;
const byte PIN_CTRL_ATOMIZER = 6;
const byte PIN_CTRL_FANS = 8;
const byte PIN_RELAY_DAY = 10;
const byte PIN_RELAY_NIGHT = 12;

HumidityController* humidityController;
LightController* lightController;


void setup()
{
	// set up humidity controller (sensors, atomizer and fan control)
    humidityController = new HumidityController(PIN_DHT22_ONE, PIN_DHT22_TWO, PIN_CTRL_ATOMIZER, PIN_CTRL_FANS);
    //humidityController.verify();

    //set up light controller (timer and relays)
    

    //set up web server/bluetooth
}

void loop()
{
	// check sensors, turn on/off humidifier

    //check time, actuate light control relays
}
