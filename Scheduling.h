#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "DateTime.h"

enum DayNight { DAY, NIGHT };

//abstract Schedule class
struct Schedule {
    virtual ScheduleEntry* getEntry(Date) = 0;
};

//schedule entry class
struct ScheduleEntry {
    Time dayStart;
    Time nightStart;

    ScheduleEntry(Time, Time);
    DayNight getDayNight(Time);
};

//fixed schedule implementation
struct FixedSchedule : public Schedule {
    ScheduleEntry* entry;

    FixedSchedule(ScheduleEntry*);
    ScheduleEntry* getEntry(Date);
};

//monthly schedule implementation
struct MonthlySchedule : public Schedule {
    ScheduleEntry* schedules[12];

    MonthlySchedule(ScheduleEntry*[12]);
    ScheduleEntry* getEntry(Date);
};

//TODO: other schedule implementations


#endif