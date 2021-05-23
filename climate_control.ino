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


const char* HEAD_TIME_UTC    = "x-Time-UTC";
const char* HEAD_TIME_YEAR   = "x-Time-Year";
const char* HEAD_TIME_MONTH  = "x-Time-Month";
const char* HEAD_TIME_DAY    = "x-Time-Day";
const char* HEAD_TIME_HOUR   = "x-Time-Hour";
const char* HEAD_TIME_MINUTE = "x-Time-Minute";
const char* HEAD_TIME_SECOND = "x-Time-Second";

const char* HEAD_HUMIDITY_TARGET   = "x-Humidity-Target";
const char* HEAD_HUMIDITY_KICKON   = "x-Humidity-KickOn";
const char* HEAD_HUMIDITY_FAN_STOP = "x-Humidity-FanStopDelay";
const char* HEAD_HUMIDITY_UPDATE   = "x-Humidity-UpdateInterval";
const char* HEAD_HUMIDITY_AVERAGE  = "x-Humidity-Average";
const char* HEAD_HUMIDITY_ONE      = "x-Humidity-SensorOne";
const char* HEAD_HUMIDITY_TWO      = "x-Humidity-SensorTwo";
const char* HEAD_HUMIDITY_FANS     = "x-Humidity-Fans";
const char* HEAD_HUMIDITY_ATOMIZER = "x-Humidity-Atomizer";

const char* HEAD_LIGHT_MODE = "x-Lights-Mode";

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
    // test route for testing query params
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

    //get server time
    router.get("/time", [](WebRequest& req, WebResponse& res){
        res.addHeader(HEAD_TIME_UTC, (long)now());
        res.addHeader(HEAD_TIME_YEAR, (long)year());
        res.addHeader(HEAD_TIME_MONTH, (long)month());
        res.addHeader(HEAD_TIME_DAY, (long)day());
        res.addHeader(HEAD_TIME_HOUR, (long)hour());
        res.addHeader(HEAD_TIME_MINUTE, (long)minute());
        res.addHeader(HEAD_TIME_SECOND, (long)second());

        res.send();
    });

    // get humidity settings
    router.get("/humidity/settings", [](WebRequest& req, WebResponse& res){
        res.addHeader(HEAD_HUMIDITY_TARGET, humidityControllerSettings->targetHumidity);
        res.addHeader(HEAD_HUMIDITY_KICKON, humidityControllerSettings->kickOnHumidity);
        res.addHeader(HEAD_HUMIDITY_FAN_STOP, (long)(humidityControllerSettings->fanStopDelay));
        res.addHeader(HEAD_HUMIDITY_UPDATE, (long)(humidityControllerSettings->updateInterval));

        res.send();
    });

    //update humidity settings
    router.post("/humidity/settings", [](WebRequest& req, WebResponse& res){
        HttpHeader target, kickOn, fanStop, updateInterval;
        byte valuesProvided = 0b0000; // bitflag to indicate which values should be updated

        //get relevant headers and set flag bits when found
        if (req.getHeader(HEAD_HUMIDITY_TARGET, target)) valuesProvided |= 0b1000;
        if (req.getHeader(HEAD_HUMIDITY_KICKON, kickOn)) valuesProvided |= 0b0100;
        if (req.getHeader(HEAD_HUMIDITY_FAN_STOP, fanStop)) valuesProvided |= 0b0010;
        if (req.getHeader(HEAD_HUMIDITY_UPDATE, updateInterval)) valuesProvided |= 0b0001;

        if(valuesProvided == 0b0000) {
            //return 400 BadRequest unless at least one new value provided
            res.status = HTTP_BAD_REQUEST;
            res.body = "No update values provided, unable to process request.";
        } else {
            if(valuesProvided & 0b1000) {
                //update target humidity
                Serial.println("Updating target humidity");
                humidityControllerSettings->targetHumidity = target.value.toFloat();
                res.addHeader(HEAD_HUMIDITY_TARGET, humidityControllerSettings->targetHumidity);
            }
            if(valuesProvided & 0b0100) {
                //update kickon humidity
                Serial.println("Updating kickon humidity");
                humidityControllerSettings->kickOnHumidity = kickOn.value.toFloat();
                res.addHeader(HEAD_HUMIDITY_KICKON, humidityControllerSettings->kickOnHumidity);
            }
            if(valuesProvided & 0b0010) {
                //update fan stop delay
                Serial.println("Updating fan stop delay");
                humidityControllerSettings->fanStopDelay = fanStop.value.toInt();
                res.addHeader(HEAD_HUMIDITY_FAN_STOP, (long)(humidityControllerSettings->fanStopDelay));
            }
            if(valuesProvided & 0b0001) {
                //update humidity update check interval
                Serial.println("Updating ");
                humidityControllerSettings->fanStopDelay = updateInterval.value.toInt();
                res.addHeader(HEAD_HUMIDITY_UPDATE, (long)(humidityControllerSettings->updateInterval));
            }
        }
    });

    //get humidity status - humidity values and enabled status of fans and atomizers
    router.get("/humidity/status", [](WebRequest& req, WebResponse& res){
        float average, sensorOne, sensorTwo;
        bool fansEnabled, atomizerEnabled;
        HumidityController::status(average, sensorOne, sensorTwo, fansEnabled, atomizerEnabled);
        
        res.addHeader(HEAD_HUMIDITY_AVERAGE, average);
        res.addHeader(HEAD_HUMIDITY_ONE, sensorOne);
        res.addHeader(HEAD_HUMIDITY_TWO, sensorTwo);
        res.addHeader(HEAD_HUMIDITY_FANS, fansEnabled ? "enabled" : "disabled");
        res.addHeader(HEAD_HUMIDITY_ATOMIZER, atomizerEnabled ? "enabled" : "disabled");

        res.send();
    });

    router.get("/lights/schedule", [](WebRequest& req, WebResponse& res){
        res.status = HTTP_NOT_FOUND;
        res.body = "Route: GET /lights/schedule not yet implemented";

        res.send();
    });

    router.post("/lights/schedule", [](WebRequest& req, WebResponse& res){
        res.status = HTTP_NOT_FOUND;
        res.body = "Route: POST /lights/schedule not yet implemented";
        
        res.send();
    });

    router.get("/lights/status", [](WebRequest& req, WebResponse& res){
        res.addHeader(HEAD_LIGHT_MODE, LightController::getStatusString());

        res.send();
    });
};