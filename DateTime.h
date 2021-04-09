#ifndef DATE_TIME_COMPARABLE_H
#define DATE_TIME_COMPARABLE_H

#include "Arduino.h"

template <class T>
struct Comparable {
    virtual int compare(T) = 0;
};

struct Date : Comparable<Date> {
    byte day, month;
    int year;

    Date(byte, byte, int);
    Date(int, byte, byte);

    virtual int compare(Date);
    int compareYear(Date);
    int compareMonth(Date);
    int compareDay(Date);

    void printSerial();
};

struct Time : Comparable<Time> {
    byte hours, minutes, seconds;

    Time(byte, byte, byte);

    virtual int compare(Time);
    int compareHours(Time);
    int compareMinutes(Time);
    int compareSeconds(Time);

    void printSerial();
};

#endif
