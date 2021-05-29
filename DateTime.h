#ifndef DATE_TIME_COMPARABLE_H
#define DATE_TIME_COMPARABLE_H

#include "Arduino.h"

// Size of a null-terminated time string (in format HH:MM:SS)
#define TIME_STR_SIZE 9

// Size of a null-terminated time component string (digit pair)
#define TIME_COMP_SIZE 3

template <class T>
struct Comparable {
    virtual int compare(T) = 0;
};

// Represents a Date object with day, month, and year
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

// Represents a Time object with hours, minutes, and seconds
struct Time : Comparable<Time> {
    byte hours, minutes, seconds;

    Time(byte, byte, byte);

    virtual int compare(Time);
    int compareHours(Time);
    int compareMinutes(Time);
    int compareSeconds(Time);

    void toString(char*);
    void printSerial();
};

#endif
