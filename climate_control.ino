#include <Time.h>
#include <TimeAlarms.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Regexp.h>

#include "secrets.h"

#include "HumidityController.h"
#include "LightController.h"
#include "WifiController.h"

#include "NTPClient.h"
#include "WebServer.h"
#include "Router.h"
#include "Bitflag.h"
#include "Lines.h"

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
const int HUMIDITY_UPDATE_DEFAULT = 10; // update humidity every 10 seconds by default

const Time LIGHT_DAY_START_DEFAULT = Time(07,00,00); //day start 7am
const Time LIGHT_NIGHT_START_DEFAULT = Time(19,00,00); //night start 7pm
const int LIGHT_UPDATE_DEFAULT = 1 * SECS_PER_MIN; // update lights every 1 minutes by default

const bool WIFI_REQUIRE_LATEST_FIRMWARE = false;
const int WIFI_CONNECTION_CHECK_INTERVAL = 10 * SECS_PER_MIN;

// web server headers

const char* HEAD_TIME_UTC          = "x-Time-UTC";
const char* HEAD_TIME_YEAR         = "x-Time-Year";
const char* HEAD_TIME_MONTH        = "x-Time-Month";
const char* HEAD_TIME_DAY          = "x-Time-Day";
const char* HEAD_TIME_HOUR         = "x-Time-Hour";
const char* HEAD_TIME_MINUTE       = "x-Time-Minute";
const char* HEAD_TIME_SECOND       = "x-Time-Second";

const char* HEAD_HUMIDITY_TARGET   = "x-Humidity-Target";
const char* HEAD_HUMIDITY_KICK_ON  = "x-Humidity-KickOn";
const char* HEAD_HUMIDITY_FAN_STOP = "x-Humidity-FanStopDelay";
const char* HEAD_HUMIDITY_UPDATE   = "x-Humidity-UpdateInterval";
const char* HEAD_HUMIDITY_AVERAGE  = "x-Humidity-Average";
const char* HEAD_HUMIDITY_ONE      = "x-Humidity-SensorOne";
const char* HEAD_HUMIDITY_TWO      = "x-Humidity-SensorTwo";
const char* HEAD_HUMIDITY_FANS     = "x-Humidity-Fans";
const char* HEAD_HUMIDITY_ATOMIZER = "x-Humidity-Atomizer";

const char* HEAD_LIGHT_MODE        = "x-Light-Mode"; // current light mode (day/night)
const char* HEAD_LS_TYPE           = "x-Light-Schedule-Type"; // light schedule type (fixed, monthly, etc.)

const int SCHED_TYPE_FIXED   = 1;
const int SCHED_TYPE_MONTHLY = 2;

// global vars

HumidityControllerSettings* humidityControllerSettings;
LightControllerSettings* lightControllerSettings;
WifiControllerSettings* wifiControllersettings;
Schedule* lightControllerSchedule;

WiFiUDP udp;
NTPClient ntp = NTPClient(udp);

WebServer server;
Router router;

// global functions

time_t timeProvider();
void registerRoutes();
bool updateFixedSchedule(String);
bool updateMonthlySchedule(String);

// Arduino setup functions
void setup()
{
    //init serial
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

	// set up humidity controller (sensors, atomizer and fan control)
    Serial.println("==========Initializing humidity controller==========");
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
    Serial.println("==========Initializing light controller==========");
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
    Serial.println("==========Initializing wifi==========");
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
    registerRoutes();
    server.listen();
};

// Arduino loop function
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

// Time provider functions
time_t timeProvider() {
    return ntp.getNTPTime();
};

bool updateFixedSchedule(String body) {
    Serial.println("Updating fixed schedule...");
    char bodyChars[body.length()+1];
    char sDayHour[2], sDayMin[2], sDaySec[2], sNightHour[2], sNightMin[2], sNightSec[2];
    int dayHour, dayMin, daySec, nightHour, nightMin, nightSec;
    MatchState ms;

    // perform matching
    memset(bodyChars,0,body.length()+1);
    Serial.println(body);
    Serial.println(body.length());
    body.toCharArray(bodyChars, body.length()+1, 0);

    ms.Target(bodyChars);
    int res = ms.Match("D{(%d%d):(%d%d):(%d%d)}N{(%d%d):(%d%d):(%d%d)}", 0); // (%d%d):(%d%d):(%d%d)
    Serial.println(res);
    Serial.println(ms.level);
    if(res != REGEXP_MATCHED) {
        Serial.print("error matching: "); Serial.println(res);
        return false;
    }
    if(ms.level != 6) {
        Serial.println("not enough matches");
        return false;
    }

    // capture matches
    ms.GetCapture(sDayHour, 0);
    ms.GetCapture(sDayMin, 1);
    ms.GetCapture(sDaySec, 2);
    ms.GetCapture(sNightHour, 3);
    ms.GetCapture(sNightMin, 4);
    ms.GetCapture(sNightSec, 5);

    // convert to ints
    dayHour = String(sDayHour).toInt();
    dayMin = String(sDayMin).toInt();
    daySec = String(sDaySec).toInt();
    nightHour = String(sNightHour).toInt();
    nightMin = String(sNightMin).toInt();
    nightSec = String(sNightSec).toInt();

    // update schedule
    Serial.println("updating");
    lightControllerSchedule = new FixedSchedule(
        new ScheduleEntry(
            Time(dayHour, dayMin, daySec),
            Time(nightHour, nightMin, nightSec)
        )
    );

    return true;
};

bool updateMonthlySchedule(String body) {
    Serial.println("a");
    char line[LINE_SIZE];
    Serial.println("b");
    char sDayHour[2], sDayMin[2], sDaySec[2], sNightHour[2], sNightMin[2], sNightSec[2];
    Serial.println("c");
    int dayHour, dayMin, daySec, nightHour, nightMin, nightSec;
    Serial.println("d");
    MatchState ms;
    Serial.println("e");
    ScheduleEntry* entries[LINE_COUNT];
    Serial.println("f");
    Lines lines = Lines::split(body.c_str());
    Serial.println("g");
    lines.printLines();

    for(int i = 0; i < LINE_COUNT; i++) {
        // get line and match
        Serial.print("Checking line: "); Serial.println(i);
        lines.getLine(line, i);
        Serial.println(line);
        ms.Target(line);
        char res = ms.Match("D{(%d%d):(%d%d):(%d%d)}N{(%d%d):(%d%d):(%d%d)}", 0);

        if(res != REGEXP_MATCHED) {
            Serial.print("error matching: "); Serial.println(res);
            return false;
        }
        if(ms.level != 6) {
            Serial.println("not enough matches");
            return false;
        }

        // capture matches
        ms.GetCapture(sDayHour, 0);
        ms.GetCapture(sDayMin, 1);
        ms.GetCapture(sDaySec, 2);
        ms.GetCapture(sNightHour, 3);
        ms.GetCapture(sNightMin, 4);
        ms.GetCapture(sNightSec, 5);

        // convert to ints
        dayHour = String(sDayHour).toInt();
        dayMin = String(sDayMin).toInt();
        daySec = String(sDaySec).toInt();
        nightHour = String(sNightHour).toInt();
        nightMin = String(sNightMin).toInt();
        nightSec = String(sNightSec).toInt();

        entries[i] = new ScheduleEntry(Time(dayHour, dayMin, daySec), Time(nightHour, nightMin, nightSec));
    }

    lightControllerSchedule = new MonthlySchedule(entries);
    return true;
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

    // ========== Humididty controller routes ==========

    // get humidity settings
    router.get("/humidity/settings", [](WebRequest& req, WebResponse& res){
        res.addHeader(HEAD_HUMIDITY_TARGET, humidityControllerSettings->targetHumidity);
        res.addHeader(HEAD_HUMIDITY_KICK_ON, humidityControllerSettings->kickOnHumidity);
        res.addHeader(HEAD_HUMIDITY_FAN_STOP, (long)(humidityControllerSettings->fanStopDelay));
        res.addHeader(HEAD_HUMIDITY_UPDATE, (long)(humidityControllerSettings->updateInterval));

        res.send();
    });

    //update humidity settings
    router.post("/humidity/settings", [](WebRequest& req, WebResponse& res){
        HttpHeader target, kickOn, fanStop, updateInterval;
        Bitflag valuesProvided; // bitflag to indicate which values should be updated

        //get relevant headers and set flag bits when found
        if (req.getHeader(HEAD_HUMIDITY_TARGET, target)) {
            valuesProvided.setBit(BIT_HUM_TARGET);
        }
        if (req.getHeader(HEAD_HUMIDITY_KICK_ON, kickOn)) {
            valuesProvided.setBit(BIT_HUM_KICK_ON);
        }
        if (req.getHeader(HEAD_HUMIDITY_FAN_STOP, fanStop)) {
            valuesProvided.setBit(BIT_HUM_FAN_STOP);
        }
        if (req.getHeader(HEAD_HUMIDITY_UPDATE, updateInterval)) {
            valuesProvided.setBit(BIT_HUM_UPDATE);
        }

        if(!valuesProvided.checkAny()) {
            //return 400 BadRequest unless at least one new value provided
            res.status = HTTP_BAD_REQUEST;
            res.body = "No update values provided, unable to process request.";
        } else {
            if(valuesProvided.checkBit(BIT_HUM_TARGET)) {
                //update target humidity
                Serial.println("Updating target humidity");
                humidityControllerSettings->targetHumidity = target.value.toFloat();
                res.addHeader(HEAD_HUMIDITY_TARGET, humidityControllerSettings->targetHumidity);
            }
            if(valuesProvided.checkBit(BIT_HUM_KICK_ON)) {
                //update kickon humidity
                Serial.println("Updating kickon humidity");
                humidityControllerSettings->kickOnHumidity = kickOn.value.toFloat();
                res.addHeader(HEAD_HUMIDITY_KICK_ON, humidityControllerSettings->kickOnHumidity);
            }
            if(valuesProvided.checkBit(BIT_HUM_FAN_STOP)) {
                //update fan stop delay
                Serial.println("Updating fan stop delay");
                humidityControllerSettings->fanStopDelay = fanStop.value.toInt();
                res.addHeader(HEAD_HUMIDITY_FAN_STOP, (long)(humidityControllerSettings->fanStopDelay));
            }
            if(valuesProvided.checkBit(BIT_HUM_UPDATE)) {
                //update humidity update check interval
                Serial.println("Updating humidity check interval");
                humidityControllerSettings->fanStopDelay = updateInterval.value.toInt();
                res.addHeader(HEAD_HUMIDITY_UPDATE, (long)(humidityControllerSettings->updateInterval));
            }
        }

        res.send();
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
        
    });

    router.post("/lights/schedule", [](WebRequest& req, WebResponse& res){
        HttpHeader schedType;
        bool validSchedProvided = false;
        int schedTypeCode = 0;
        Serial.println(req.body);

        // check schedule type header is valid
        if(req.getHeader(HEAD_LS_TYPE, schedType)) {
            schedTypeCode = schedType.value.toInt();
            if(schedTypeCode >= 1 && schedTypeCode <= 2) {
                validSchedProvided = true;
            }
        }

        if(validSchedProvided) {
            bool updateSucceeded = false;
            // check which type we're updating to.
            switch(schedTypeCode) {
                case SCHED_TYPE_FIXED:
                {
                    Serial.println("fixed");
                    // update fixed schedule
                    updateSucceeded = updateFixedSchedule(req.body);
                    break;
                }
                case SCHED_TYPE_MONTHLY:
                {
                    Serial.println("monthly");
                    // update monthly schedule
                    updateSucceeded = updateMonthlySchedule(req.body);
                    break;
                }
            }

            if(updateSucceeded) {
                Serial.println("Success updating schedule");
                lightControllerSchedule->print();
            } else {
                Serial.println("Failed to update schedule");
                res.status = HTTP_SERVER_ERROR;
            }

            // send response
            res.send();
        } else {
            // return bad formatted request
            res.status = HTTP_BAD_REQUEST;
            res.body = "Invalid request, unable to parse";
            res.send();
        }
    });

    router.get("/lights/status", [](WebRequest& req, WebResponse& res){
        res.addHeader(HEAD_LIGHT_MODE, LightController::getStatusString());

        res.send();
    });
};