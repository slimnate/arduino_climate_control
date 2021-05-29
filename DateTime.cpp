#include "Arduino.h"
#include "DateTime.h"

//compare functions
int compareWholeNumber(byte first, byte second) {
    if(first < second) {
        return -1;
    } else if(first > second) {
        return 1;
    }
    return 0;
};

int compareWholeNumber(int first, int second) {
    if(first < second) {
        return -1;
    } else if(first > second) {
        return 1;
    }
    return 0;
};


// Date object
Date::Date(byte day, byte month, int year) : day(day), month(month), year(year) { };

Date::Date(int year, byte month, byte day) : year(year), month(month), day(day) { };

int Date::compare(Date other) {
    int cYear, cMonth, cDay;
    cYear = compareYear(other);
    cMonth = compareMonth(other);
    cDay = compareDay(other);

    if(cYear == 0) {
        if(cMonth == 0) {
            if(cDay == 0) {
                return 0;
            } else return cDay;
        } else return cMonth;
    } else return cYear;
};

int Date::compareDay(Date other) {
    return compareWholeNumber(day, other.day);
};

int Date::compareMonth(Date other) {
    return compareWholeNumber(month, other.month);
};

int Date::compareYear(Date other) {
    return compareWholeNumber(year, other.year);
};

void Date::printSerial() {
    Serial.print(year); Serial.print("-");
    Serial.print(month); Serial.print("-");
    Serial.println(day);
};


//Time object
Time::Time(byte hours, byte minutes, byte seconds) : hours(hours), minutes(minutes), seconds(seconds) { };

int Time::compare(Time other) {
    int cHours, cMinutes, cSeconds;
    cHours = compareHours(other);
    cMinutes = compareMinutes(other);
    cSeconds = compareSeconds(other);

    if(cHours == 0) {
        if(cMinutes == 0) {
            if(cSeconds == 0) {

            } else return cSeconds;
        } else return cMinutes;
    } else return cSeconds;
};

int Time::compareHours(Time other) {
    return compareWholeNumber(hours, other.hours);
};

int Time::compareMinutes(Time other) {
    return compareWholeNumber(minutes, other.minutes);
};

int Time::compareSeconds(Time other) {
    return compareWholeNumber(seconds, other.seconds);
};

void Time::toString(char* dest) {
    // init char[]s for values
    char digitBuffer[TIME_COMP_SIZE];

    //add hours
    memset(digitBuffer, 0, 3); // clear digit buffer
    itoa(hours, digitBuffer, 10); // parse int into digit buffer
    if(hours > 9){ //two digits, no need for extra zero
        memcpy(dest, digitBuffer, 2);
    } else { //one digit, need to pad leading zero
        dest[0] = '0';
        memcpy(dest+1, digitBuffer, 1);
    }
    dest[2] = ':';

    //add min
    memset(digitBuffer, 0, 3);
    itoa(minutes, digitBuffer, 10);
    if(minutes > 9){ //two digits, no need for extra zero
        memcpy(dest+3, digitBuffer, 2);
    } else { //one digit, need to pad leading zero
        dest[3] = '0';
        memcpy(dest+4, digitBuffer, 1);
    }
    dest[5] = ':';

    //add sec
    memset(digitBuffer, 0, 3);
    itoa(seconds, digitBuffer, 10);
    if(seconds > 9){ //two digits, no need for extra zero
        memcpy(dest+6, digitBuffer, 2);
    } else { //one digit, need to pad leading zero
        dest[6] = '0';
        memcpy(dest+7, digitBuffer, 1);
    }
}

void Time::printSerial() {
    Serial.print(hours); Serial.print(":");
    Serial.print(minutes); Serial.print(":");
    Serial.println(seconds);
};
