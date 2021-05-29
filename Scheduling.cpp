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

void ScheduleEntry::toString(char* dest) {
    char timeBuffer[9];

    //get day start time
    memset(timeBuffer, 0, 9);
    dayStart.toString(timeBuffer);

    //add day start time
    strcat(dest, "D{");
    strcat(dest, timeBuffer);

    //get night start time
    memset(timeBuffer, 0, 9);
    dayStart.toString(timeBuffer);

    //add night start time
    strcat(dest, "}N{");
    strcat(dest, timeBuffer);
    strcat(dest, "}");
}


bool Schedule::validScheduleType(int i) {
    if(i >= SCHEDULE_TYPE::FIXED && i <= SCHEDULE_TYPE::MONTHLY) {
        return true;
    }
    return false;
};


// Fixed schedule implementation
FixedSchedule::FixedSchedule(ScheduleEntry* entry) {
    this->entry = entry;
};

ScheduleEntry* FixedSchedule::getEntry(Date d) {
    //ignores the date passed and always returns the same entry
    return entry;
};

int FixedSchedule::getScheduleType() {
    return SCHEDULE_TYPE::FIXED;
}

void FixedSchedule::toString(char* dest) {
    entry->toString(dest);
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

int MonthlySchedule::getScheduleType() {
    return SCHEDULE_TYPE::MONTHLY;
}

void MonthlySchedule::toString(char* dest) {
    ScheduleEntry* entry;
    char lineBuffer[24];
    for(int i=0; i<12; i++) {
        memset(lineBuffer, 0, 24);
        entry = schedules[i];
        entry->toString(lineBuffer);
        strcat(dest, lineBuffer);
        strcat(dest, "\r\n");
    }
};