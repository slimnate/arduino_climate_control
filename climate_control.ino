#include <Time.h>
#include <TimeAlarms.h>
#include <SPI.h>
#include <WiFiNINA.h>

#include "secrets.h"

#include "HumidityController.h"
#include "LightController.h"
#include "WifiController.h"

#include "NTPClient.h"
#include "WebServer.h"
#include "Router.h"

const int MINUTES = 60;

const float HUMIDITY_TARGET_DEFAULT = 90.0; // target 90% humidity
const float HUMIDITY_KICKON_DEFAULT = 75.0; // start when below 75% humidity
const int HUMIDITY_FAN_STOP_DEFAULT = 20; // fans run for 15 seconds after atomizer stops
const int HUMIDITY_UPDATE_DEFAULT = 10; // update humidity every 10 seconds by default

const Time LIGHT_DAY_START_DEFAULT = Time(07,00,00); //day start 7am
const Time LIGHT_NIGHT_START_DEFAULT = Time(19,00,00); //night start 7pm
const int LIGHT_UPDATE_DEFAULT = 1 * MINUTES; // update lights every 1 minutes by default

const bool WIFI_REQUIRE_LATEST_FIRMWARE = false;
const int WIFI_CONNECTION_CHECK_INTERVAL = 10 * MINUTES;

const byte PIN_DHT22_ONE = 2;
const byte PIN_DHT22_TWO = 4;
const byte PIN_CTRL_ATOMIZER = 6;
const byte PIN_CTRL_FANS = 8;
const byte PIN_RELAY_DAY = 10;
const byte PIN_RELAY_NIGHT = 12;

HumidityControllerSettings* humidityControllerSettings;
LightControllerSettings* lightControllerSettings;
WifiControllerSettings* wifiControllersettings;

WiFiUDP udp;
NTPClient ntp = NTPClient(udp);

WebServer server;
Router router;

time_t getNTPTimeWrapper();
void registerRoutes();

void setup()
{
    //init serial
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
/*
	// set up humidity controller (sensors, atomizer and fan control)
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

    //set up light controller (timer and relays)
    lightControllerSettings = new LightControllerSettings(
        new FixedSchedule(
            new ScheduleEntry(
                LIGHT_DAY_START_DEFAULT,
                LIGHT_NIGHT_START_DEFAULT
            )
        ),
        LIGHT_UPDATE_DEFAULT
    );
    LightController::init(
        PIN_RELAY_DAY,
        PIN_RELAY_NIGHT,
        lightControllerSettings
    );
*/
    //set up wifi connection
    Serial.println("==========Initializing wifi==========");
    wifiControllersettings = new WifiControllerSettings(
        SECRET_SSID,
        SECRET_PASS,
        WIFI_REQUIRE_LATEST_FIRMWARE,
        WIFI_CONNECTION_CHECK_INTERVAL
    );
    WifiController::init(wifiControllersettings);


    //set up NTP provider
    ntp.initUdp();
    setSyncProvider(getNTPTimeWrapper);
    setSyncInterval(5 * MINUTES); //update every 5 min

    //print date and time on startup
    Date today = Date(year(), (byte)month(), (byte)day());
    Time now = Time((byte)hour(), (byte)minute(), (byte)second());
    Serial.println("Date: "); today.printSerial();
    Serial.println("Time: "); now.printSerial();

    //start web server
    server = WebServer(); // initialize web server with no port defaults to port 80.
    registerRoutes();
    server.listen();
};

void loop()
{
    //process incoming http requests each loop
    WebRequest req;
    if(server.processIncomingRequest(req) == 1) {
        Serial.println("Loop - responding to request...");
        router.handle(req);
    }
    
    //check alarms twice per second
    Alarm.delay(500);
};

time_t getNTPTimeWrapper() {
    return ntp.getNTPTime();
};

void registerRoutes() {
    router.get("/test", [](WebRequest& req, WebResponse& res) {
        Serial.println("GET /test callback");
        for(int i = 0; i < 4; i++) {
            Serial.print("params["); Serial.print(i); Serial.print("].key = "); Serial.println(req.params[i].key);
            Serial.print("params["); Serial.print(i); Serial.print("].value = "); Serial.println(req.params[i].value);
        }

        res.addHeader("Header-Test", "Test header");
        res.body="Test body contents";
        res.send();
    });
};