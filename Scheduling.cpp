#include "Arduino.h"
#include "DateTime.h"
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
};

void FixedSchedule::print() {
    Serial.print("Day Start = ");
    Serial.print(entry->dayStart.hours); Serial.print(":");
    Serial.print(entry->dayStart.minutes); Serial.print(":");
    Serial.print(entry->dayStart.seconds); Serial.println();
    Serial.print("Night Start = ");
    Serial.print(entry->nightStart.hours); Serial.print(":");
    Serial.print(entry->nightStart.minutes); Serial.print(":");
    Serial.print(entry->nightStart.seconds); Serial.println();
};


//MonthlySchedule implementation
MonthlySchedule::MonthlySchedule(ScheduleEntry* sched[12]) {
    for(int i=0; i<12; i++) {
        schedules[i] = sched[i];
    }
};

ScheduleEntry* MonthlySchedule::getEntry(Date d) {
    return schedules[d.month-1];
};

void MonthlySchedule::print() {
    ScheduleEntry* entry;
    for(int i=0; i<12; i++) {
        entry = schedules[i];
        Serial.print("Month "); Serial.print(i); Serial.println(": ");
        Serial.print("Day Start = ");
        Serial.print(entry->dayStart.hours); Serial.print(":");
        Serial.print(entry->dayStart.minutes); Serial.print(":");
        Serial.print(entry->dayStart.seconds); Serial.println();
        Serial.print("Night Start = ");
        Serial.print(entry->nightStart.hours); Serial.print(":");
        Serial.print(entry->nightStart.minutes); Serial.print(":");
        Serial.print(entry->nightStart.seconds); Serial.println();
    }
};