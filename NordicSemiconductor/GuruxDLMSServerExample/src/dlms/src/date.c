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
    *hours = (short)tm.tm_hour;

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
    gxtime* time,
    int year,
    int month,
    int day,
    int hour,
    int minute,
    int second,
    int millisecond,
    int devitation)
{
    gxtime now;
    int skip = DATETIME_SKIPS_NONE;
    memset(&time->value, 0, sizeof(time->value));
    time->daylightSavingsBegin = time->daylightSavingsEnd = 0;
    time->status = DLMS_CLOCK_STATUS_OK;
    if (year < 1 || year == 0xFFFF)
    {
        skip |= DATETIME_SKIPS_YEAR;
        time_now(&now);
        year = 1900 + now.value.tm_year;
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
    if (mktime(&time->value) == -1)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
    }
    //If summer time.
    if (time->value.tm_isdst)
    {
        --time->value.tm_hour;
        if (mktime(&time->value) == -1)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
        }
    }
}

void time_clearDate(
    gxtime* value)
{
    value->value.tm_year = value->value.tm_yday =
        value->value.tm_wday = value->value.tm_mon =
        value->value.tm_mday = value->value.tm_isdst = 0;
}

void time_clearTime(
    gxtime* value)
{
    value->value.tm_sec = value->value.tm_min = value->value.tm_hour = 0;
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
#if _MSC_VER > 1000
    struct tm dt;
    localtime_s(&dt, &tm1);
#else
    struct tm dt = *localtime(&tm1);
#endif
    time_init2(value, &dt);
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

void time_addDays(
    gxtime* value,
    int days)
{
    value->value.tm_mday += days;
    //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if (mktime(&value->value) == -1)
    {
        assert(0);
    }
#endif
}

void time_addHours(
    gxtime* value,
    int hours)
{
    value->value.tm_hour += hours;
    //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if (mktime(&value->value) == -1)
    {
        assert(0);
    }
#endif
}

void time_addMinutes(
    gxtime* value,
    int minutes)
{
    value->value.tm_min += minutes;
    //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if (mktime(&value->value) == -1)
    {
        assert(0);
    }
#endif
}

void time_addSeconds(
    gxtime* value,
    int seconds)
{
    value->value.tm_sec += seconds;
    //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if (mktime(&value->value) == -1)
    {
        assert(0);
    }
#endif
}

void time_init2(
    gxtime* time,
    struct tm* value)
{
    short devitation, hours, minutes;
    time_getUtcOffset(&hours, &minutes);
    devitation = -(hours * 60 + minutes);
    time_init(time, value->tm_year + 1900, value->tm_mon + 1, value->tm_mday, value->tm_hour, value->tm_min,
        value->tm_sec, 0, devitation);
}

void time_init4(
    gxtime* time,
    time_t value)
{
    time->deviation = 0;
    time_fromUnixTime(value, &time->value);
    time->daylightSavingsBegin = time->daylightSavingsEnd = 0;
    time->status = DLMS_CLOCK_STATUS_OK;
    time->skip = DATETIME_SKIPS_NONE;
}

void time_clear(
    gxtime* time)
{
    time->skip = DATETIME_SKIPS_NONE;
    memset(&time->value, 0, sizeof(struct tm));
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
    *format = GXDLMS_DATE_FORMAT_MDY;
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
    gxtime* time,
    char* buff,
    unsigned short len)
{
    gxByteBuffer bb;
    bb_attach(&bb, (unsigned char*)buff, 0, len);
    return time_toString(time, &bb);
}

int time_toString(
    gxtime* time,
    gxByteBuffer* ba)
{
    int ret = 0;
    char buff[50];
    GXDLMS_DATE_FORMAT format;
    char separator;
    if (time->skip != DATETIME_SKIPS_NONE)
    {
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
                if (time->value.tm_mday != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
                {
                    bb_addIntAsString(ba, time->value.tm_mday);
                }
                if (time->value.tm_mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, 1 + time->value.tm_mon);
                }
                if (time->value.tm_year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, time->value.tm_year);
                }
            }
            break;
            case GXDLMS_DATE_FORMAT_MDY:
            {
                if (time->value.tm_mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    bb_addIntAsString(ba, 1 + time->value.tm_mon);
                }
                if (time->value.tm_mday != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, time->value.tm_mday);
                }
                if (time->value.tm_year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, 1900 + time->value.tm_year);
                }
            }
            break;
            case GXDLMS_DATE_FORMAT_YMD:
            {
                if (time->value.tm_year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    bb_addIntAsString(ba, 1900 + time->value.tm_year);
                }
                if (time->value.tm_mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, 1 + time->value.tm_mon);
                }
                if (time->value.tm_mday != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, time->value.tm_mday);
                }
            }
            break;
            case GXDLMS_DATE_FORMAT_YDM:
            {
                if (time->value.tm_year != -1 && (time->skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    bb_addIntAsString(ba, 1900 + time->value.tm_year);
                }
                if (time->value.tm_mday != -1 && (time->skip & DATETIME_SKIPS_DAY) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, time->value.tm_mday);
                }
                if (time->value.tm_mon != -1 && (time->skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    if (ba->size != 0)
                    {
                        bb_setUInt8(ba, separator);
                    }
                    bb_addIntAsString(ba, 1 + time->value.tm_mon);
                }
            }
            break;
            default:
            {
                //If OS is used.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
                ret = (int)strftime(buff, 50, "%X", &time->value);
#else
                sprintf(buff, "%.2d/%.2d/%.4d %.2d:%.2d",
                    time->value.tm_mon,
                    time->value.tm_mday,
                    1900 + time->value.tm_year,
                    time->value.tm_hour,
                    time->value.tm_min);
#endif
                bb_setUInt8(ba, ' ');
                bb_set(ba, (unsigned char*)buff, (unsigned short)ret);
                return 0;
            }
        }
    }
        //Add hours.
        if (time->value.tm_hour != -1 && (time->skip & DATETIME_SKIPS_HOUR) == 0)
        {
            if (ba->size != 0)
            {
                bb_setUInt8(ba, ' ');
            }
            bb_addIntAsString(ba, time->value.tm_hour);
        }
        //Add minutes.
        if (time->value.tm_min != -1 && (time->skip & DATETIME_SKIPS_MINUTE) == 0)
        {
            if (ba->size != 0)
            {
                bb_setUInt8(ba, ':');
            }
            bb_addIntAsString(ba, time->value.tm_min);
        }
        //Add seconds.
        if (time->value.tm_sec != -1 && (time->skip & DATETIME_SKIPS_SECOND) == 0)
        {
            if (ba->size != 0)
            {
                bb_setUInt8(ba, ':');
            }
            bb_addIntAsString(ba, time->value.tm_sec);
        }
        //Add end of string, but that is not added to the length.
        bb_setUInt8(ba, '\0');
        --ba->size;
        return 0;
}
    //If value is not set return empty string.
    if (time->value.tm_year == -1 || time->value.tm_mday == 0)
    {
        return 0;
    }
    //If OS is used.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    ret = (int)strftime(buff, 50, "%x %X", &time->value);
    buff[ret] = '\0';
    bb_addString(ba, buff);
#else
    //TODO:
#endif
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
    time->value.tm_hour += hours;
    time->value.tm_min += minutes;
    time->value.tm_sec += seconds;
    //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    mktime(&time->value);
#endif
}

int time_compare(
    gxtime* value1,
    gxtime* value2)
{
    struct tm tm1 = value1->value;
    //0x8000 == -32768
    if (value1->deviation != -32768)
    {
        tm1.tm_min -= value1->deviation;
    }
    struct tm tm2 = value2->value;
    if (value2->deviation != -32768)
    {
        tm2.tm_min -= value2->deviation;
    }
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);
    if (time1 < time2)
    {
        return -1;
    }
    if (time1 > time2)
    {
        return 1;
    }
    return 0;
}

//Get date time from Epoch time.
int time_fromUnixTime(const time_t epoch, struct tm* time)
{
#if _MSC_VER > 1000
    return localtime_s(time, &epoch);
#else
    *time = *localtime(&epoch);
#endif
    return 0;
}

// Convert date time to Epoch time.
time_t time_toUnixTime(struct tm* time)
{
    return mktime(time);
}