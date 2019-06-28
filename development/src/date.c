//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include "../include/gxignore.h"
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif
#include <time.h>
#include <string.h>

#ifndef GX_DLMS_MICROCONTROLLER
#include <stdio.h> //printf needs this or error is generated.
#endif //GX_DLMS_MICROCONTROLLER

#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include "../include/date.h"

//Get UTC offset in minutes.
void time_getUtcOffset(short* hours, short* minutes)
{
    time_t zero = 24 * 60 * 60L;
    struct tm tm;

    // local time for Jan 2, 1900 00:00 UTC
#if _MSC_VER > 1000
    localtime_s(&tm, &zero);
#else
    tm = *localtime(&zero);
#endif
    * hours = (short)tm.tm_hour;

    //If the local time is the "day before" the UTC, subtract 24 hours from the hours to get the UTC offset
    if (tm.tm_mday < 2)
    {
        *hours -= 24;
    }
    *minutes = (short)tm.tm_min;
}

void time_init3(
    gxtime* time,
    int year,
    int month,
    int day,
    int hour,
    int minute,
    int second,
    int millisecond)
{
    short devitation, hours, minutes;
    time_getUtcOffset(&hours, &minutes);
    devitation = -(hours * 60 + minutes);
    time_init(time, year, month, day, hour, minute, second, millisecond, devitation);
}

// Constructor.
void time_init(
    gxtime * time,
    short year,
    short month,
    short day,
    short hour,
    short minute,
    short second,
    short millisecond,
    short devitation)
{
#ifdef DLMS_USE_EPOCH_TIME
    //January and February are counted as months 13 and 14 of the previous year
    if (month != -1 && month <= 2)
    {
        month += 12;
        year -= 1;
    }
    time->skip = DATETIME_SKIPS_NONE;
    //Convert years to days
    time->value = 0;
    if (year == -1)
    {
        time->skip = DATETIME_SKIPS_YEAR;
        year = 2000;
    }
    time->value = (365 * year) + (year / 4) - (year / 100) + (year / 400);
    //Convert months to days
    if (month == -1)
    {
        time->skip |= DATETIME_SKIPS_MONTH;
        month = 1;
    }
    time->value += (30 * month) + (3 * (month + 1) / 5) + day;
    //Unix time starts on January 1st, 1970
    time->value -= 719561;

    //Convert days to seconds
    if (time->value != 0)
    {
        time->value *= 86400;
    }
    //Add hours, minutes and seconds
    if (hour != -1)
    {
        time->value += (3600 * hour);
    }
    else
    {
        time->skip |= DATETIME_SKIPS_HOUR;
    }
    if (minute != -1)
    {
        time->value += (60 * minute);
    }
    else
    {
        time->skip |= DATETIME_SKIPS_MINUTE;
    }
    if (second != -1)
    {
        time->value += second;
    }
    else
    {
        time->skip |= DATETIME_SKIPS_SECOND;
    }
    time->deviation = devitation;
    time->status = DLMS_CLOCK_STATUS_OK;
    time->daylightSavingsBegin = time->daylightSavingsEnd = 0;
#else
    int skip = DATETIME_SKIPS_NONE;
    memset(&time->value, 0, sizeof(time->value));
    time->daylightSavingsBegin = time->daylightSavingsEnd = 0;
    time->status = DLMS_CLOCK_STATUS_OK;
    if (year < 1 || year == 0xFFFF)
    {
        skip |= DATETIME_SKIPS_YEAR;
        year = 2000;
    }
    if (month < 1 || month == 0xFF)
    {
        skip |= DATETIME_SKIPS_MONTH;
        month = 0;
    }
    else
    {
        --month;
    }
    if (day < 1 || day == 0xFF)
    {
        skip |= DATETIME_SKIPS_DAY;
        day = 1;
    }
    else if (day == 0xFD)
    {
        day = date_daysInMonth(year, (short)month) - 1;
    }
    else if (day == 0xFE)
    {
        day = date_daysInMonth(year, (short)month);
    }
    if (hour == -1 || hour == 0xFF)
    {
        skip |= DATETIME_SKIPS_HOUR;
        hour = 1;
    }
    if (minute == -1 || minute == 0xFF)
    {
        skip |= DATETIME_SKIPS_MINUTE;
        minute = 0;
    }
    if (second == -1 || second == 0xFF)
    {
        skip |= DATETIME_SKIPS_SECOND;
        second = 0;
    }
    if (millisecond < 1 || millisecond == 0xFF)
    {
        skip |= DATETIME_SKIPS_MS;
        millisecond = 0;
    }
    time->skip = (DATETIME_SKIPS)skip;
    if (year != 0)
    {
        time->value.tm_year = year - 1900;
    }
    time->value.tm_mon = month;
    time->value.tm_mday = day;
    time->value.tm_hour = hour;
    time->value.tm_min = minute;
    time->value.tm_sec = second;
    time->deviation = (short)devitation;
    if (mktime(&time->value) == (time_t) -1)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
    }
    //If summer time.
    if (time->value.tm_isdst)
    {
        --time->value.tm_hour;
        if (mktime(&time->value) == (time_t) -1)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
        }
    }
#endif //DLMS_USE_EPOCH_TIME
}

void time_clearDate(
    gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    //Get hours, minutes and seconds
    unsigned long t = value->value / 60;
    unsigned char seconds = t % 60;
    t /= 60;
    unsigned char minutes = t % 60;
    t /= 60;
    unsigned char hours = t % 24;
    t /= 24;
    value->value = seconds;
    value->value += 60 * minutes;
    value->value += 3600 * hours;
#else
    value->value.tm_year = value->value.tm_yday =
        value->value.tm_wday = value->value.tm_mon =
        value->value.tm_mday = value->value.tm_isdst = 0;
#endif
}

void time_clearTime(
    gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    if (value->value != 0)
    {
        //Remove hours, minutes and seconds
        value->value /= 60;
        value->value /= 60;
        value->value /= 24;
    }
#else
    value->value.tm_sec = value->value.tm_min = value->value.tm_hour = 0;
#endif
}

//If OS is not used this must be defined on application.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
long time_current(void)
{
    return (long)time(NULL);
}

long time_elapsed(void)
{
    return (long)clock() * 1000;
}

void time_now(
    gxtime* value)
{
    time_t tm1 = time(NULL);
#ifdef DLMS_USE_EPOCH_TIME
    time_init4(value, (unsigned long) tm1);
#else
#if _MSC_VER > 1000
    struct tm dt;
    localtime_s(&dt, &tm1);
#else
    struct tm dt = *localtime(&tm1);
#endif
    time_init2(value, &dt);
#endif //DLMS_USE_EPOCH_TIME
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

unsigned char time_getYears(
    const gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    unsigned long t = value->value;
    //remove hours, minutes and seconds
    t /= 86400;
    //Convert Unix time to date
    unsigned long a = (unsigned long)((4 * t + 102032) / 146097 + 15);
    unsigned long b = (unsigned long)(t + 2442113 + a - (a / 4));
    unsigned long c = (20 * b - 2442) / 7305;
    unsigned long d = b - 365 * c - (c / 4);
    unsigned long e = d * 1000 / 30601;
    //January and February are counted as months 13 and 14 of the previous year
    if (e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }
    return (unsigned char)c;
#else
    return value->value.tm_year;
#endif // DLMS_USE_EPOCH_TIME
}

unsigned char time_getMonths(
    const gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    unsigned long t = value->value;
    //remove hours, minutes and seconds
    t /= 86400;
    //Convert Unix time to date
    unsigned long a = (unsigned long)((4 * t + 102032) / 146097 + 15);
    unsigned long b = (unsigned long)(t + 2442113 + a - (a / 4));
    unsigned long c = (20 * b - 2442) / 7305;
    unsigned long d = b - 365 * c - (c / 4);
    unsigned long e = d * 1000 / 30601;
    //January and February are counted as months 13 and 14 of the previous year
    if (e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }
    return (unsigned char)e;
#else
    return value->value.tm_mon;
#endif // DLMS_USE_EPOCH_TIME
}

unsigned char time_getDays(
    const gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    unsigned long t = value->value;
    //remove hours, minutes and seconds
    t /= 86400;
    //Convert Unix time to date
    unsigned long a = (unsigned long)((4 * t + 102032) / 146097 + 15);
    unsigned long b = (unsigned long)(t + 2442113 + a - (a / 4));
    unsigned long c = (20 * b - 2442) / 7305;
    unsigned long d = b - 365 * c - (c / 4);
    unsigned long e = d * 1000 / 30601;
    unsigned long f = d - e * 30 - e * 601 / 1000;
    return (unsigned char)f;
#else
    return value->value.tm_mday;
#endif // DLMS_USE_EPOCH_TIME
}

unsigned char time_getHours(
    const gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    return (unsigned char)((value->value % 86400) / 3600);
#else
    return value->value.tm_hour;
#endif // DLMS_USE_EPOCH_TIME
}

unsigned char time_getMinutes(
    const gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    return (unsigned char)((value->value % 3600) / 60);
#else
    return value->value.tm_min;
#endif // DLMS_USE_EPOCH_TIME
}

unsigned char time_getSeconds(
    const gxtime * value)
{
#ifdef DLMS_USE_EPOCH_TIME
    return (unsigned char)(value->value % 60);
#else
    return value->value.tm_sec;
#endif // DLMS_USE_EPOCH_TIME
}

void time_addDays(
    gxtime * value,
    int days)
{
#ifdef DLMS_USE_EPOCH_TIME
    time_addHours(value, 24 * days);
#else
    value->value.tm_mday += days;
    if (mktime(&value->value) == (time_t)-1)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
    }
#endif //DLMS_USE_EPOCH_TIME
}

void time_addHours(
    gxtime * value,
    int hours)
{
#ifdef DLMS_USE_EPOCH_TIME
    time_addMinutes(value, 60 * hours);
#else
    value->value.tm_hour += hours;
    if (mktime(&value->value) == (time_t)-1)
    {
        //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
    }
#endif //DLMS_USE_EPOCH_TIME
}

void time_addMinutes(
    gxtime * value,
    int minutes)
{
#ifdef DLMS_USE_EPOCH_TIME
    time_addSeconds(value, 60 * minutes);
#else
    value->value.tm_min += minutes;
    if (mktime(&value->value) == (time_t)-1)
    {
        //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
    }
#endif //DLMS_USE_EPOCH_TIME
}

void time_addSeconds(
    gxtime * value,
    int seconds)
{
#ifdef DLMS_USE_EPOCH_TIME
    value->value += seconds;
#else
    value->value.tm_sec += seconds;
    if (mktime(&value->value) == (time_t)-1)
    {
        //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
    }
#endif //DLMS_USE_EPOCH_TIME
}

#ifndef DLMS_USE_EPOCH_TIME
void time_init2(
    gxtime * time,
    struct tm* value)
{
#ifdef DLMS_USE_EPOCH_TIME
    unsigned short y;
    unsigned char m, d;
    //Year
    y = value->tm_year;
    //Month of year
    m = value->tm_mon;
    //Day of month
    d = value->tm_mday;

    //January and February are counted as months 13 and 14 of the previous year
    if (m <= 2)
    {
        m += 12;
        y -= 1;
    }

    //Convert years to days
    time->value = (365 * y) + (y / 4) - (y / 100) + (y / 400);
    //Convert months to days
    time->value += (30 * m) + (3 * (m + 1) / 5) + d;
    //Unix time starts on January 1st, 1970
    time->value -= 719561;
    //Convert days to seconds
    time->value *= 86400;
    //Add hours, minutes and seconds
    time->value += (3600 * value->tm_hour) + (60 * value->tm_min) + value->tm_sec;
#else
    short devitation, hours, minutes;
    time_getUtcOffset(&hours, &minutes);
    devitation = -(hours * 60 + minutes);
    time_init(time, value->tm_year + 1900, value->tm_mon + 1, value->tm_mday, value->tm_hour, value->tm_min,
        value->tm_sec, 0, devitation);
#endif //DLMS_USE_EPOCH_TIME
}
#endif //DLMS_USE_EPOCH_TIME

void time_init4(
    gxtime * time,
    unsigned long value)
{
    time->deviation = 0;
#ifdef DLMS_USE_EPOCH_TIME
    time->value = value;
#else
    time_fromUnixTime(value, &time->value);
    if (value != 0 && mktime(&time->value) == (time_t)-1)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    }

#endif //DLMS_USE_EPOCH_TIME
    time->daylightSavingsBegin = time->daylightSavingsEnd = 0;
#ifdef DLMS_USE_EPOCH_TIME
    time->status = DLMS_CLOCK_STATUS_OK;
#else
    if (time->value.tm_isdst)
    {
        time->status = DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE;
    }
    else
    {
        time->status = DLMS_CLOCK_STATUS_OK;
    }
#endif
    time->skip = DATETIME_SKIPS_NONE;
}

void time_clear(
    gxtime * time)
{
#ifdef DLMS_USE_EPOCH_TIME
    time->value = 0;
#else
    memset(&time->value, 0, sizeof(struct tm));
#endif //DLMS_USE_EPOCH_TIME
    time->skip = DATETIME_SKIPS_NONE;
    time->daylightSavingsBegin = time->daylightSavingsEnd = 0;
    time->status = DLMS_CLOCK_STATUS_OK;
}

unsigned char date_daysInMonth(
    int year,
    short month)
{
    if (month == 0 || month == 2 || month == 4 ||
        month == 6 || month == 7 || month == 9 || month == 11)
    {
        return 31;
    }
    else if (month == 3 || month == 5 || month == 8 || month == 10)
    {
        return 30;
    }
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
            {
                return 29;
            }
            return 28;
        }
        return 29;
    }
    return 28;
}

//Constants for different orders of date components.
typedef enum
{
    GXDLMS_DATE_FORMAT_INVALID = -1,
    GXDLMS_DATE_FORMAT_DMY = 0,
    GXDLMS_DATE_FORMAT_MDY = 1,
    GXDLMS_DATE_FORMAT_YMD = 2,
    GXDLMS_DATE_FORMAT_YDM = 3
} GXDLMS_DATE_FORMAT;

#ifndef GX_DLMS_MICROCONTROLLER
int getDateFormat(GXDLMS_DATE_FORMAT* format, char* separator)
{
    //If OS is used.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    int ret = 0, value, lastPos = 0, pos;
    char buff[10];
    struct tm order = { 0 };
    order.tm_year = 0;
    order.tm_mday = 1;
    order.tm_mon = 1;//Month is zero based.
    ret = (int)strftime(buff, 10, "%x", &order);
    if (ret > 0)
    {
        for (pos = 0; pos != ret; ++pos)
        {
            //If numeric value
            if (buff[pos] >= '0' && buff[pos] <= '9')
            {

            }
            else //If date time separator.
            {
                *separator = buff[pos];
#if _MSC_VER > 1000
                if (sscanf_s(buff + lastPos, "%d", &value) == 1)
#else
                if (sscanf(buff + lastPos, "%d", &value) == 1)
#endif
                {
                    if (value == 1)
                    {
                        *format = lastPos == 0 ? GXDLMS_DATE_FORMAT_DMY : GXDLMS_DATE_FORMAT_YDM;
                        break;
                    }
                    else if (value == 2)
                    {
                        *format = lastPos == 0 ? GXDLMS_DATE_FORMAT_MDY : GXDLMS_DATE_FORMAT_YMD;
                        break;
                    }
                }
                lastPos = pos + 1;
            }
        }
    }
    return 0;
#else
    * format = GXDLMS_DATE_FORMAT_MDY;
    *separator = '/';
    return 0;
#endif
}

int time_print(const char* format, gxtime* time)
{
    int ret;
    char buff[50];
    if ((ret = time_toString2(time, buff, 50)) != 0)
    {
        return ret;
    }
    if (format != NULL)
    {
        printf(format, buff);
    }
    else
    {
        printf(buff);
    }
    return 0;
}

int time_toString2(
    const gxtime* time,
    char* buff,
    unsigned short len)
{
    gxByteBuffer bb;
    bb_attach(&bb, (unsigned char*)buff, 0, len);
    return time_toString(time, &bb);
}

int time_toString(
    const gxtime* time,
    gxByteBuffer* ba)
{
    int ret = 0;
    GXDLMS_DATE_FORMAT format;
    char separator;
    unsigned short year = 0;
    unsigned char mon = 0, day = 0, hour = 0, min = 0, sec = 0;
#ifdef DLMS_USE_EPOCH_TIME
    time_fromUnixTime2(time->value, &year, &mon, &day, &hour, &min, &sec, NULL);
#else
    year = time->value.tm_year;
    if (year != -1)
    {
        year += 1900;
    }
    mon = time->value.tm_mon;
    if (year != -1)
    {
        mon += 1;
    }
    day = time->value.tm_mday;
    hour = time->value.tm_hour;
    min = time->value.tm_min;
    sec = time->value.tm_sec;
#endif //DLMS_USE_EPOCH_TIME
    //Add year, month and date if used.
    if ((time->skip & (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY)) != (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY))
    {
        if ((ret = getDateFormat(&format, &separator)) != 0)
        {
            return ret;
        }
        switch (format)
        {
        case GXDLMS_DATE_FORMAT_DMY:
        {
            if (day != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
            {
                bb_addIntAsString(ba, day);
            }
            if (mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, mon);
            }
            if (year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, year);
            }
        }
        break;
        case GXDLMS_DATE_FORMAT_YMD:
        {
            if (year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
            {
                bb_addIntAsString(ba, year);
            }
            if (mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, mon);
            }
            if (day != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, day);
            }
        }
        break;
        case GXDLMS_DATE_FORMAT_YDM:
        {
            if (year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
            {
                bb_addIntAsString(ba, year);
            }
            if (day != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, day);
            }
            if (mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, mon);
            }
        }
        break;
        default: //GXDLMS_DATE_FORMAT_MDY
        {
            if (mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
            {
                bb_addIntAsString(ba, mon);
            }
            if (day != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, day);
            }
            if (year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
            {
                if (ba->size != 0)
                {
                    bb_setUInt8(ba, separator);
                }
                bb_addIntAsString(ba, year);
            }
        }
        break;
        }
    }
    //Add hours.
    if (hour != -1 && (time->skip & DATETIME_SKIPS_HOUR) == 0)
    {
        if (ba->size != 0)
        {
            bb_setUInt8(ba, ' ');
        }
        bb_addIntAsString(ba, hour);
    }
    //Add minutes.
    if (min != -1 && (time->skip & DATETIME_SKIPS_MINUTE) == 0)
    {
        if (ba->size != 0)
        {
            bb_setUInt8(ba, ':');
        }
        bb_addIntAsString(ba, min);
    }
    //Add seconds.
    if (sec != -1 && (time->skip & DATETIME_SKIPS_SECOND) == 0)
    {
        if (ba->size != 0)
        {
            bb_setUInt8(ba, ':');
        }
        bb_addIntAsString(ba, sec);
    }
    //Add end of string, but that is not added to the length.
    bb_setUInt8(ba, '\0');
    --ba->size;
    return 0;
}
#endif //GX_DLMS_MICROCONTROLLER;

void time_copy(
    gxtime* trg,
    gxtime* src)
{
    trg->daylightSavingsBegin = src->daylightSavingsBegin;
    trg->daylightSavingsEnd = src->daylightSavingsEnd;
    trg->skip = src->skip;
    trg->status = src->status;
    trg->value = src->value;
    trg->deviation = src->deviation;
}

void time_addTime(
    gxtime* time,
    int hours,
    int minutes,
    int seconds)
{
#ifdef DLMS_USE_EPOCH_TIME
    time_addHours(time, hours);
    time_addMinutes(time, minutes);
    time_addSeconds(time, seconds);
#else
    time->value.tm_hour += hours;
    time->value.tm_min += minutes;
    time->value.tm_sec += seconds;
    mktime(&time->value);
#endif // DLMS_USE_EPOCH_TIME
}

int time_compare2(
    gxtime* value1,
    unsigned long value2)
{
    unsigned short year1;
    unsigned char month1, day1, hour1, minute1, second1;
    unsigned short year2;
    unsigned char month2, day2, hour2, minute2, second2;
#ifdef DLMS_USE_EPOCH_TIME
    time_fromUnixTime2(value1->value, &year1, &month1,
        &day1, &hour1, &minute1, &second1, NULL);
#else
    year1 = value1->value.tm_year;
    month1 = value1->value.tm_mon;
    day1 = value1->value.tm_mday;
    hour1 = value1->value.tm_hour;
    minute1 = value1->value.tm_min;
    second1 = value1->value.tm_sec;
#endif //DLMS_USE_EPOCH_TIME
    time_fromUnixTime2(value2, &year2, &month2,
        &day2, &hour2, &minute2, &second2, NULL);

    if ((value1->skip & DATETIME_SKIPS_YEAR) == 0 && year1 != year2)
    {
        if (year1 < year2)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_MONTH) == 0 && month1 != month2)
    {
        if (month1 < month2)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_DAY) == 0 && day1 != day2)
    {
        if (day1 < day2)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_HOUR) == 0 && hour1 != hour2)
    {
        if (hour1 < hour2)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_MINUTE) == 0 && minute1 != minute2)
    {
        if (minute1 < minute2)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_SECOND) == 0 && second1 != second2)
    {
        if (second1 < second2)
        {
            return -1;
        }
        return 1;
    }
    return 0;
}

int time_compare(
    gxtime* value1,
    gxtime* value2)
{
#ifdef DLMS_USE_EPOCH_TIME
    return time_compare2(value1, value2->value);
#else
    if ((value1->skip & DATETIME_SKIPS_YEAR) == 0 && value1->value.tm_year != value2->value.tm_year)
    {
        if (value1->value.tm_year < value2->value.tm_year)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_MONTH) == 0 && value1->value.tm_mon != value2->value.tm_mon)
    {
        if (value1->value.tm_mon < value2->value.tm_mon)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_DAY) == 0 && value1->value.tm_mday != value2->value.tm_mday)
    {
        if (value1->value.tm_mday < value2->value.tm_mday)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_HOUR) == 0 && value1->value.tm_hour != value2->value.tm_hour)
    {
        if (value1->value.tm_hour < value2->value.tm_hour)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_MINUTE) == 0 && value1->value.tm_min != value2->value.tm_min)
    {
        if (value1->value.tm_min < value2->value.tm_min)
        {
            return -1;
        }
        return 1;
    }
    if ((value1->skip & DATETIME_SKIPS_SECOND) == 0 && value1->value.tm_sec != value2->value.tm_sec)
    {
        if (value1->value.tm_sec < value2->value.tm_sec)
        {
            return -1;
        }
        return 1;
    }
    return 0;
#endif // DLMS_USE_EPOCH_TIME
}

int time_fromUnixTime2(
    unsigned long epoch,
    unsigned short* year,
    unsigned char* month,
    unsigned char* day,
    unsigned char* hour,
    unsigned char* minute,
    unsigned char* second,
    unsigned char* dayOfWeek)
{
    //Retrieve hours, minutes and seconds
    if (second != NULL)
    {
        *second = epoch % 60;
    }
    epoch /= 60;
    if (minute != NULL)
    {
        *minute = epoch % 60;
    }
    epoch /= 60;
    if (hour != NULL)
    {
        *hour = epoch % 24;
    }
    epoch /= 24;
    //Convert Unix time to date
    unsigned long a = (unsigned long)((4 * epoch + 102032) / 146097 + 15);
    unsigned long b = (unsigned long)(epoch + 2442113 + a - (a / 4));
    unsigned long c = (20 * b - 2442) / 7305;
    unsigned long d = b - 365 * c - (c / 4);
    unsigned long e = d * 1000 / 30601;
    unsigned long f = d - e * 30 - e * 601 / 1000;
    //January and February are counted as months 13 and 14 of the previous year
    if (e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }
    //Retrieve year, month and day
    if (year != NULL)
    {
        *year = (unsigned short)c;
    }
    if (month != NULL)
    {
        *month = (unsigned char)e;
    }
    if (day != NULL)
    {
        *day = (unsigned char)f;
    }
    if (dayOfWeek != NULL && year != NULL && month != NULL && day != NULL)
    {
        //Calculate day of week
        *dayOfWeek = time_dayOfWeek(*year, *month, *day);
    }
    return 0;
}

int time_fromUnixTime3(
    const gxtime* time,
    unsigned short* year,
    unsigned char* month,
    unsigned char* day,
    unsigned char* hour,
    unsigned char* minute,
    unsigned char* second,
    unsigned char* dayOfWeek)
{
#ifdef DLMS_USE_EPOCH_TIME
    return time_fromUnixTime2(time->value, year, month,
        day, hour, minute, second, dayOfWeek);
#else
    if (year != NULL)
    {
        *year = 1900 + time->value.tm_year;
    }
    if (month != NULL)
    {
        *month = 1 + time->value.tm_mon;
    }
    if (day != NULL)
    {
        *day = time->value.tm_mday;
    }
    if (hour != NULL)
    {
        *hour = time->value.tm_hour;
    }
    if (minute != NULL)
    {
        *minute = time->value.tm_min;
    }
    if (second != NULL)
    {
        *second = time->value.tm_sec;
    }
    if (dayOfWeek != NULL)
    {
        *dayOfWeek = time->value.tm_year;
    }
    return 0;
#endif //DLMS_USE_EPOCH_TIME
}

unsigned char time_dayOfWeek(
    unsigned short year,
    unsigned char month,
    unsigned char day)
{
    unsigned short h, j, k;
    //January and February are counted as months 13 and 14 of the previous year
    if (month <= 2)
    {
        month += 12;
        year -= 1;
    }
    //J is century
    j = year / 100;
    //K is year of the century
    k = year % 100;
    h = day + (26 * (month + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);
    //Return the day of the week
    return ((h + 5) % 7) + 1;
}

#ifndef DLMS_USE_EPOCH_TIME
//Get date time from Epoch time.
int time_fromUnixTime(unsigned long epoch, struct tm* time)
{
#ifdef DLMS_USE_EPOCH_TIME
    unsigned short year;
    unsigned char month, day, hour, minute, second, dayOfWeek;
    time_fromUnixTime2(epoch, &year, &month,
        &day, &hour, &minute, &second, &dayOfWeek);
    time->tm_sec = second;
    time->tm_min = minute;
    time->tm_hour = hour;
    time->tm_year = year;
    time->tm_mon = month;
    time->tm_mday = day;
    time->tm_wday = dayOfWeek;
#else
    time_t tmp = epoch;
#if _MSC_VER > 1000
    return gmtime_s(time, &tmp);
#else
    *time = *gmtime(&tmp);
#endif
#endif //DLMS_USE_EPOCH_TIME
    return 0;
}

// Convert date time to Epoch time.
unsigned long time_toUnixTime(struct tm* time)
{
    return (unsigned long)mktime(time);
}
#endif //DLMS_USE_EPOCH_TIME

// Convert date time to Epoch time.
unsigned long time_toUnixTime2(gxtime * time)
{
#ifdef DLMS_USE_EPOCH_TIME
    return time->value;
#else
    return (unsigned long)mktime(&time->value);
#endif //DLMS_USE_UTC_TIME_ZONE
}


int time_getDeviation(gxtime * value1)
{
#ifdef DLMS_USE_UTC_TIME_ZONE
    return -value1->deviation;
#else
    return value1->deviation;
#endif //DLMS_USE_UTC_TIME_ZONE
}

int time_toUTC(gxtime* value)
{
    //Convert time to UCT if time zone is given.
    if (value->deviation != 0 && value->deviation != -32768)
    {
        time_addMinutes(value, time_getDeviation(value));
        value->deviation = 0;
        //DST deviation is included to deviation. Remove status.
        value->status &= ~DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE;
    }
    return 0;
}
