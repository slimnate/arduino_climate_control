#include <Time.h>
#include <TimeAlarms.h>
#include <SPI.h>
#include <WiFiNINA.h>

#include "secrets.h"
#include "globals.h"
#include "routes.h"

#include "HumidityController.h"
#include "LightController.h"
#include "WifiController.h"

// control pins

const byte PIN_DHT22_ONE = 2;
const byte PIN_DHT22_TWO = 4;
const byte PIN_CTRL_ATOMIZER = 6;
const byte PIN_CTRL_FANS = 8;
const byte PIN_RELAY_DAY = 10;
const byte PIN_RELAY_NIGHT = 12;

// settings defaults

const float HUMIDITY_TARGET_DEFAULT = 90.0; // target 90% humidity
const float HUMIDITY_KICKON_DEFAULT = 75.0; // start when below 75% humidity
const int HUMIDITY_FAN_STOP_DEFAULT = 20; // fans run for 15 seconds after atomizer stops
const int HUMIDITY_UPDATE_DEFAULT = 2; // update humidity every 10 seconds by default

const Time LIGHT_DAY_START_DEFAULT = Time(07,00,00); //day start 7am
const Time LIGHT_NIGHT_START_DEFAULT = Time(19,00,00); //night start 7pm
const int LIGHT_UPDATE_DEFAULT = 1 * SECS_PER_MIN; // update lights every 1 minutes by default

const bool WIFI_REQUIRE_LATEST_FIRMWARE = false;
const int WIFI_CONNECTION_CHECK_INTERVAL = 10 * SECS_PER_MIN;

// global functions

time_t timeProvider();
void registerRoutes();
bool updateFixedSchedule(String);
bool updateMonthlySchedule(String);

// Arduino setup functions
void setup()
{
    int serialStart = millis();
    //init serial
    Serial.begin(115200);
    while (!Serial && millis() - serialStart < 2000) {
        delay(500);
    }

	// set up humidity controller (sensors, atomizer and fan control)
    Serial.println("==========Initializing Humidity Controller==========");
    humidityControllerSettings = new HumidityControllerSettings(
        HUMIDITY_TARGET_DEFAULT,
        HUMIDITY_KICKON_DEFAULT,
        HUMIDITY_FAN_STOP_DEFAULT,
        HUMIDITY_UPDATE_DEFAULT
    );
    HumidityController::init(
        PIN_DHT22_ONE,
        PIN_DHT22_TWO,
        PIN_CTRL_ATOMIZER,
        PIN_CTRL_FANS,
        humidityControllerSettings
    );

    // set up light controller (timer and relays)
    Serial.println("==========Initializing Light Controller==========");
    lightControllerSchedule = new FixedSchedule(
        new ScheduleEntry(
            LIGHT_DAY_START_DEFAULT,
            LIGHT_NIGHT_START_DEFAULT
        )
    );
    lightControllerSettings = new LightControllerSettings(
        lightControllerSchedule,
        LIGHT_UPDATE_DEFAULT
    );
    LightController::init(
        PIN_RELAY_DAY,
        PIN_RELAY_NIGHT,
        lightControllerSettings
    );

    // set up wifi connection
    Serial.println("==========Initializing WiFi==========");
    wifiControllersettings = new WifiControllerSettings(
        SECRET_SSID,
        SECRET_PASS,
        WIFI_REQUIRE_LATEST_FIRMWARE,
        WIFI_CONNECTION_CHECK_INTERVAL
    );
    WifiController::init(wifiControllersettings);


    // set up NTP provider
    Serial.println("==========Initializing NTP==========");
    ntp.initUdp();
    setSyncProvider(timeProvider);
    setSyncInterval(5 * SECS_PER_MIN); //update every 5 min

    // print date and time on startup
    Date today = Date(year(), (byte)month(), (byte)day());
    Time now = Time((byte)hour(), (byte)minute(), (byte)second());
    Serial.println("Date: "); today.printSerial();
    Serial.println("Time: "); now.printSerial();

    // start web server
    Serial.println("==========Initializing Web Server==========");
    server = WebServer(); // initialize web server with no port defaults to port 80.
    Serial.println("Registering routes...");
    registerRoutes();
    Serial.println("...done.");
    server.listen();
    Serial.println("Server Listening...");
};

// Arduino loop function
void loop()
{
    //process incoming http requests each loop
    WebRequest req;
    if(server.processIncomingRequest(req) == 1) {
        Serial.println("Responding to request...");
        router.handle(req);
    }
    
    //check alarms twice per second
    Alarm.delay(500);
};

// Time provider functions
time_t timeProvider() {
    return ntp.getNTPTime();
};

// Register routes for the web server
void registerRoutes() {
    // test route for testing query params
    router.get("/test", route_getTest);

    //get server time
    router.get("/time", route_getTime);

    // get humidity status - humidity values and enabled status of fans and atomizers
    router.get("/humidity/status", route_getHumidityStatus);

    // get humidity settings
    router.get("/humidity/settings", route_getHumiditySettings);

    // update humidity settings
    router.post("/humidity/settings", route_postHumiditySettings);

    // get light status (day/night)
    router.get("/lights/status", route_getLightStatus);

    // get light schedule
    router.get("/lights/schedule", route_getLightSchedule);

    // update light schedule
    router.post("/lights/schedule", route_postLightSchedule);
};