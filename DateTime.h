#ifndef DATE_TIME_COMPARABLE_H
#define DATE_TIME_COMPARABLE_H

#include "Arduino.h"

struct Comparable {
    virtual int compare(Comparable);
};

struct Date : public Comparable {
    byte day, month;
    int year;

    Date(byte, byte, int);
    Date(int, byte, byte);

    int compare(Date);
    int compareYear(Date);
    int compareMonth(Date);
    int compareDay(Date);

    void printSerial();
};

struct Time : public Comparable {
    byte hours, minutes, seconds;

    Time(byte, byte, byte);

    int compare(Time);
    int compareHours(Time);
    int compareMinutes(Time);
    int compareSeconds(Time);

    void printSerial();
};

#endif