#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "Arduino.h"
#include "DateTime.h"

enum DayNight { DAY, NIGHT };

//schedule entry class
struct ScheduleEntry {
    Time dayStart;
    Time nightStart;

    ScheduleEntry(Time, Time);
    DayNight getDayNight(Time);
};

//abstract Schedule class
struct Schedule {
    virtual ScheduleEntry* getEntry(Date) = 0;
    virtual void print();
};

//fixed schedule implementation
struct FixedSchedule : public Schedule {
    ScheduleEntry* entry;

    FixedSchedule(ScheduleEntry*);
    ScheduleEntry* getEntry(Date);
    void print();
};

//monthly schedule implementation
struct MonthlySchedule : public Schedule {
    ScheduleEntry* schedules[12];

    MonthlySchedule(ScheduleEntry*[12]);
    ScheduleEntry* getEntry(Date);
    void print();
};

//TODO: other schedule implementations


#endif