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
#ifndef DATE_H
#define DATE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "enums.h"

#ifndef GX_DLMS_MICROCONTROLLER
#include "bytebuffer.h"
#endif //GX_DLMS_MICROCONTROLLER

    //If OS
#include <time.h>

    //Get UTC offset in minutes.
    void time_getUtcOffset(short* hours, short* minutes);

    // DataType enumerates skipped fields from date time.
    typedef enum
    {
        // Nothing is skipped from date time.
        DATETIME_SKIPS_NONE = 0x0,
        // Year part of date time is skipped.
        DATETIME_SKIPS_YEAR = 0x1,
        // Month part of date time is skipped.
        DATETIME_SKIPS_MONTH = 0x2,
        // Day part is skipped.
        DATETIME_SKIPS_DAY = 0x4,
        // Day of week part of date time is skipped.
        DATETIME_SKIPS_DAYOFWEEK = 0x8,
        // Hours part of date time is skipped.
        DATETIME_SKIPS_HOUR = 0x10,
        // Minute part of date time is skipped.
        DATETIME_SKIPS_MINUTE = 0x20,
        // Seconds part of date time is skipped.
        DATETIME_SKIPS_SECOND = 0x40,
        // Hundreds of seconds part of date time is skipped.
        DATETIME_SKIPS_MS = 0x80,
        //Devitation is skipped.
        DATETIME_SKIPS_DEVITATION = 0x100
    } DATETIME_SKIPS;

    typedef struct
    {
        unsigned char skip; //DATETIME_SKIPS
        struct tm value;
        short deviation;
        unsigned char daylightSavingsBegin;
        unsigned char daylightSavingsEnd;
        unsigned char status;//DLMS_CLOCK_STATUS
    } gxtime;

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
        int devitation);

    void time_init2(
        gxtime* time,
        struct tm* value);

    // Constructor.
    void time_init3(
        gxtime* time,
        int year,
        int month,
        int day,
        int hour,
        int minute,
        int second,
        int millisecond);

    //Constructor from Unix time.
    void time_init4(
        gxtime* time,
        time_t value);


    void time_clear(
        gxtime* time);

    void time_copy(
        gxtime* trg,
        gxtime* src);

    //Returns current time in number of seconds from epoch.
    //If you are not using operating system you have to implement this by yourself.
    //Reason for this is that all compilers's don't support time at all.
    extern long time_current(void);

    //Returns the approximate processor time in ms.
    extern long time_elapsed(void);

    //Returns current time.
    //If you are not using operating system you have to implement this by yourself.
    //Reason for this is that all compilers's or HWs don't support time at all.
    extern void time_now(
        gxtime* value);

    /*
    Adds amount of days to current time.
    */
    void time_addDays(
        gxtime* value,
        int days);

    /*
    Adds amount of hours to current time.
    */
    void time_addHours(
        gxtime* value,
        int hours);

    /*
    Adds amount of minutes to current time.
    */
    void time_addMinutes(
        gxtime* value,
        int minutes);

    /*
    Adds amount of seconds to current time.
    */
    void time_addSeconds(
        gxtime* value,
        int seconds);

    /*
    Clears date part.
    */
    void time_clearDate(
        gxtime* value);

    /*
    Clears time part.
    */
    void time_clearTime(
        gxtime* value);

    unsigned char date_daysInMonth(
        int year,
        short month);

#ifndef GX_DLMS_MICROCONTROLLER
    //Print time to cout.
    int time_print(
        //Format.
        const char* format,
        gxtime* time);

    //Save time to char buffer.
    int time_toString2(
        gxtime* time,
        char* buff,
        unsigned short len);

    //Save time to bytebuffer.
    int time_toString(
        gxtime* time,
        gxByteBuffer* arr);
#endif //GX_DLMS_MICROCONTROLLER

    void time_addTime(
        gxtime* time,
        int hours,
        int minutes,
        int seconds);

    int time_compare(
        gxtime* value1,
        gxtime* value2);

    //Get date time from Epoch time.
    int time_fromUnixTime(
        const time_t epoch, 
        struct tm* time);

    // Convert date time to Epoch time.
    time_t time_toUnixTime(
        struct tm* time);

#ifdef  __cplusplus
}
#endif

#endif //DATE_H
