#ifndef ROUTES_H
#define ROUTES_H
#include "Regexp.h"

#include "globals.h"
#include "WebServer.h"
#include "Bitflag.h"
#include "Lines.h"

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

const char* HEAD_TEMP_AVERAGE      = "x-Temperature-Average";
const char* HEAD_TEMP_ONE          = "x-Temperature-SensorOne";
const char* HEAD_TEMP_TWO          = "x-Temperature-SensorTwo";

const char* HEAD_LIGHT_MODE        = "x-Light-Mode"; // current light mode (day/night)
const char* HEAD_LS_TYPE           = "x-Light-Schedule-Type"; // light schedule type (fixed, monthly, etc.)


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
    Serial.println("Updating monthly schedule");
    char line[LINE_SIZE];
    char sDayHour[2], sDayMin[2], sDaySec[2], sNightHour[2], sNightMin[2], sNightSec[2];
    int dayHour, dayMin, daySec, nightHour, nightMin, nightSec;
    MatchState ms;
    ScheduleEntry* entries[LINE_COUNT];
    Lines lines = Lines::split(body.c_str());
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

void route_getTest(WebRequest& req, WebResponse& res) {
    for(int i = 0; i < 4; i++) {
        Serial.print("params["); Serial.print(i); Serial.print("].key = "); Serial.println(req.params[i].key);
        Serial.print("params["); Serial.print(i); Serial.print("].value = "); Serial.println(req.params[i].value);
    }

    res.addHeader("Header-Test", "Test header");
    res.body="Test body contents";
    res.send();
};

void route_getTime(WebRequest& req, WebResponse& res){
    res.addHeader(HEAD_TIME_UTC, (long)now());
    res.addHeader(HEAD_TIME_YEAR, (long)year());
    res.addHeader(HEAD_TIME_MONTH, (long)month());
    res.addHeader(HEAD_TIME_DAY, (long)day());
    res.addHeader(HEAD_TIME_HOUR, (long)hour());
    res.addHeader(HEAD_TIME_MINUTE, (long)minute());
    res.addHeader(HEAD_TIME_SECOND, (long)second());

    res.send();
};

void route_getHumiditySettings(WebRequest& req, WebResponse& res){
    res.addHeader(HEAD_HUMIDITY_TARGET, humidityControllerSettings->targetHumidity);
    res.addHeader(HEAD_HUMIDITY_KICK_ON, humidityControllerSettings->kickOnHumidity);
    res.addHeader(HEAD_HUMIDITY_FAN_STOP, (long)(humidityControllerSettings->fanStopDelay));
    res.addHeader(HEAD_HUMIDITY_UPDATE, (long)(humidityControllerSettings->updateInterval));

    res.send();
};

void route_postHumiditySettings(WebRequest& req, WebResponse& res){
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
};

void route_getHumidityStatus(WebRequest& req, WebResponse& res){
    float avgTemp, tempOne, tempTwo;
    float avgHum, humOne, humTwo;
    bool fansEnabled, atomizerEnabled;

    HumidityController::controlStatus(fansEnabled, atomizerEnabled);
    HumidityController::humidity(avgHum, humOne, humTwo);
    HumidityController::temperature(avgTemp, tempOne, tempTwo);
    
    res.addHeader(HEAD_TEMP_AVERAGE, avgTemp);
    res.addHeader(HEAD_TEMP_ONE, tempOne);
    res.addHeader(HEAD_TEMP_TWO, tempTwo);
    res.addHeader(HEAD_HUMIDITY_AVERAGE, avgHum);
    res.addHeader(HEAD_HUMIDITY_ONE, humOne);
    res.addHeader(HEAD_HUMIDITY_TWO, humTwo);
    res.addHeader(HEAD_HUMIDITY_FANS, fansEnabled ? "enabled" : "disabled");
    res.addHeader(HEAD_HUMIDITY_ATOMIZER, atomizerEnabled ? "enabled" : "disabled");

    res.send();
};

void route_getLightStatus(WebRequest& req, WebResponse& res){
    res.addHeader(HEAD_LIGHT_MODE, LightController::getStatusString());

    res.send();
};

void route_getLightSchedule(WebRequest& req, WebResponse& res){
    //add type header
    int schedTypeCode = lightControllerSchedule->getScheduleType();
    int bodySize = 25;
    res.addHeader(HEAD_LS_TYPE, (long)schedTypeCode);
    Serial.println(schedTypeCode);

    if(schedTypeCode == SCHEDULE_TYPE::MONTHLY) {
        bodySize = 12 * 25; // upgrade body size to hold 12 lines
    }

    //get body string
    char body[bodySize];
    memset(body, 0, bodySize);
    lightControllerSchedule->toString(body);

    // add body to request
    res.body = body;
    Serial.println(res.body);

    //send response
    res.send();
};

void route_postLightSchedule(WebRequest& req, WebResponse& res){
    HttpHeader schedType;
    bool validSchedProvided = false;
    int schedTypeCode = 0;
    Serial.println(req.body);

    // check schedule type header is valid
    if(req.getHeader(HEAD_LS_TYPE, schedType)) {
        schedTypeCode = schedType.value.toInt();
        validSchedProvided = Schedule::validScheduleType(schedTypeCode);
    }

    if(validSchedProvided) {
        bool updateSucceeded = false;
        // check which type we're updating to.
        switch(schedTypeCode) {
            case SCHEDULE_TYPE::FIXED:
            {
                Serial.println("fixed");
                // update fixed schedule
                updateSucceeded = updateFixedSchedule(req.body);
                break;
            }
            case SCHEDULE_TYPE::MONTHLY:
            {
                Serial.println("monthly");
                // update monthly schedule
                updateSucceeded = updateMonthlySchedule(req.body);
                break;
            }
        }

        if(updateSucceeded) {
            Serial.println("Success updating schedule");
            char body[300];
            memset(body, 0, 300);
            lightControllerSchedule->toString(body);
            Serial.println(body);
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
}

#endif