#include "Scheduling.h"

// ScheduleEntry class
ScheduleEntry::ScheduleEntry(Time dayStart, Time nightStart)
    : dayStart(dayStart), nightStart(nightStart) { };

DayNight ScheduleEntry::getDayNight(Time t) {
    // if time lt dayStart OR gte nightStart, night
    if(t.compare(dayStart) < 0 || t.compare(nightStart) >= 0) {
        return NIGHT;
    }

    // if time gte dayStart AND lt nightStart, day
    if(t.compare(dayStart) >= 0 && t.compare(nightStart) < 0) {
        return DAY;
    }

    // default to DAY if for some reason neither test matches.
    // Also log error, since this should not happen.
    Serial.println("ERROR: ScheduleEntry.getDayNight() did not match either day or night. Defaulting to DAY.");

    Serial.print("Current time: ");
    t.printSerial();

    Serial.print("Day start time: ");
    dayStart.printSerial();

    Serial.print("Night start time: ");
    nightStart.printSerial();

    return DAY;
};

// Fixed schedule implementation
FixedSchedule::FixedSchedule(ScheduleEntry* entry) {
    this->entry = entry;
};

ScheduleEntry* FixedSchedule::getEntry(Date d) {
    //ignores the date passed and always returns the same entry
    return entry;
}


//MonthlySchedule implementation
MonthlySchedule::MonthlySchedule(ScheduleEntry* sched[12]) {
    for(int i=0; i<12; i++) {
        schedules[i] = sched[i];
    }
};

ScheduleEntry* MonthlySchedule::getEntry(Date d) {
    return schedules[d.month-1];
}