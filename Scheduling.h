#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "Arduino.h"
#include "DateTime.h"

// Represents a lighting schedule type
enum SCHEDULE_TYPE {
    FIXED   = 1,
    MONTHLY = 2,
};

// Represents a day/night status
enum DayNight { DAY, NIGHT };

//schedule entry class
struct ScheduleEntry {
    Time dayStart;
    Time nightStart;

    ScheduleEntry(Time, Time);
    DayNight getDayNight(Time);
    void toString(char*);
};

//abstract Schedule class
struct Schedule {
    virtual int getScheduleType();
    virtual ScheduleEntry* getEntry(Date) = 0;
    virtual void toString(char*);

    static bool validScheduleType(int i);
};

//fixed schedule implementation
struct FixedSchedule : public Schedule {
    ScheduleEntry* entry;

    FixedSchedule(ScheduleEntry*);

    int getScheduleType();
    ScheduleEntry* getEntry(Date);
    void toString(char*);
};

//monthly schedule implementation
struct MonthlySchedule : public Schedule {
    ScheduleEntry* schedules[12];

    MonthlySchedule(ScheduleEntry*[12]);

    int getScheduleType();
    ScheduleEntry* getEntry(Date);
    void toString(char*);
};

//TODO: other schedule implementations


#endif