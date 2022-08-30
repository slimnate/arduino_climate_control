#include "Arduino.h"
#include "DateTime.h"
#include "Scheduling.h"

// Create new ScheduleEntry with specified 'dayStart' and 'nightStart'
ScheduleEntry::ScheduleEntry(Time dayStart, Time nightStart)
    : dayStart(dayStart), nightStart(nightStart) { };

// Return the correct LightStatus value for the specified time
LightStatus ScheduleEntry::getLightStatus(Time t) {
    Serial.print("Current time: ");
    t.printSerial();

    Serial.print("Day start time: ");
    dayStart.printSerial();

    Serial.print("Night start time: ");
    nightStart.printSerial();
    
    // if time lt dayStart OR gte nightStart, night
    if(t.compare(dayStart) < 0 || t.compare(nightStart) >= 0) {
        Serial.println("getLightStatus = NIGHT");
        return NIGHT;
    }

    // if time gte dayStart AND lt nightStart, day
    if(t.compare(dayStart) >= 0 && t.compare(nightStart) < 0) {
        Serial.println("getLightStatus = DAY");
        return DAY;
    }

    // default to DAY if for some reason neither test matches.
    // Also log error, since this should not happen.
    Serial.println("ERROR: ScheduleEntry.getLightStatus() did not match either day or night. Defaulting to DAY.");

    Serial.print("Current time: ");
    t.printSerial();

    Serial.print("Day start time: ");
    dayStart.printSerial();

    Serial.print("Night start time: ");
    nightStart.printSerial();

    return DAY;
};

// Convert the schedule entry into a serialized string representation for sending in web responses
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
    nightStart.toString(timeBuffer);

    //add night start time
    strcat(dest, "}N{");
    strcat(dest, timeBuffer);
    strcat(dest, "}");
}


// Return true if 'i' is a valid schedule type
bool Schedule::validScheduleType(int i) {
    if(i >= ScheduleType::FIXED && i <= ScheduleType::MONTHLY) {
        return true;
    }
    return false;
};


// Create new fixed schedule
FixedSchedule::FixedSchedule(ScheduleEntry* entry) {
    this->entry = entry;
};

// Get the ScheduleEntry corresponding to date 'd'
ScheduleEntry* FixedSchedule::getEntry(Date d) {
    //ignores the date passed and always returns the same entry
    return entry;
};

// Get the schedule type
int FixedSchedule::getScheduleType() {
    return ScheduleType::FIXED;
}

// Convert the schedule into a serialized string representation for sending in web responses
void FixedSchedule::toString(char* dest) {
    entry->toString(dest);
};


//MonthlySchedule implementation
MonthlySchedule::MonthlySchedule(ScheduleEntry* sched[12]) {
    for(int i=0; i<12; i++) {
        schedules[i] = sched[i];
    }
};

// Get the ScheduleEntry corresponding to date 'd'
ScheduleEntry* MonthlySchedule::getEntry(Date d) {
    return schedules[d.month-1];
};

// Get the schedule type
int MonthlySchedule::getScheduleType() {
    return ScheduleType::MONTHLY;
}

// Convert the schedule into a serialized string representation for sending in web responses
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