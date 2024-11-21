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

#include "bytebuffer.h"

#ifndef DLMS_USE_EPOCH_TIME
#include <time.h>
#endif

#ifndef DLMS_USE_EPOCH_TIME
    //Get UTC offset in minutes.
    void time_getUtcOffset(short* hours, short* minutes);
#endif //DLMS_USE_EPOCH_TIME

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
        DATETIME_SKIPS_DEVITATION = 0x100,
        //Status is skipped.
        DATETIME_SKIPS_STATUS = 0x200
    } DATETIME_SKIPS;

    typedef struct
    {
#ifdef DLMS_USE_EPOCH_TIME
        uint32_t value;
#else
        struct tm value;
#endif
        /*Milliseconds.*/
        uint16_t millisecond;
        int16_t deviation;
        DATETIME_SKIPS skip : 16;
        DLMS_DATE_TIME_EXTRA_INFO extraInfo : 8;
        DLMS_CLOCK_STATUS status : 8;
    } gxtime;

    // Constructor.
    void time_init(
        gxtime* time,
        uint16_t year,
        unsigned char month,
        unsigned char day,
        unsigned char hour,
        unsigned char minute,
        unsigned char second,
        uint16_t millisecond,
        int16_t devitation);

#ifndef DLMS_USE_EPOCH_TIME
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
#endif //DLMS_USE_EPOCH_TIME

    //Constructor from Unix time.
    void time_initUnix(
        gxtime* time,
        uint32_t value);

    void time_clear(
        gxtime* time);

    void time_copy(
        gxtime* trg,
        gxtime* src);

    //Returns the approximate processor time in ms.
    extern uint32_t time_elapsed(void);

    /*
    Get years from time.
    */
    uint16_t time_getYears(
        const gxtime* value);
    /*
    Get months from time.
    */
    unsigned char time_getMonths(
        const gxtime* value);
    /*
    Get days from time.
    */
    unsigned char time_getDays(
        const gxtime* value);

    /*
    Get hours from time.
    */
    unsigned char time_getHours(
        const gxtime* value);

    /*
    Get minutes from time.
    */
    unsigned char time_getMinutes(
        const gxtime* value);

    /*
    Get seconds from time.
    */
    unsigned char time_getSeconds(
        const gxtime* value);

    /*
Get years from time.
*/
    uint16_t time_getYears2(
        uint32_t value);
    /*
    Get months from time.
    */
    unsigned char time_getMonths2(
        uint32_t value);
    /*
    Get days from time.
    */
    unsigned char time_getDays2(
        uint32_t value);

    /*
    Get hours from time.
    */
    unsigned char time_getHours2(
        uint32_t value);

    /*
    Get minutes from time.
    */
    unsigned char time_getMinutes2(
        uint32_t value);

    /*
    Get seconds from time.
    */
    unsigned char time_getSeconds2(
        uint32_t value);

    /*
    Adds amount of years to current time.
    */
    void time_addYears(
        gxtime* value,
        int years);

    /*
    Adds amount of months to current time.
    */
        void time_addMonths(
            gxtime* value,
            int months);

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
    Returns the date.
*/
    uint32_t time_getDate(uint32_t value);

    /*
    Clears time part.
    */
    void time_clearTime(
        gxtime* value);

    /*
    Returns the time.
    */
    uint32_t time_getTime(uint32_t value);


    /*
    Clears hours.
    */
    void time_clearHours(
        gxtime* value);

    /*
    Clears minutes.
    */
    void time_clearMinutes(
        gxtime* value);

    /*
    Clears seconds.
    */
    void time_clearSeconds(
        gxtime* value);

    unsigned char date_daysInMonth(
        uint16_t year,
        uint8_t month);

#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)
    //Print time to cout.
    int time_print(
        //Format.
        const char* format,
        gxtime* time);
#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)

    //Save time to char buffer.
    int time_toString2(
        const gxtime* time,
        char* buff,
        uint16_t len);

    //Save time to bytebuffer.
    int time_toString(
        const gxtime* time,
        gxByteBuffer* arr);

    void time_addTime(
        gxtime* time,
        int hours,
        int minutes,
        int seconds);

    /////////////////////////////////////////////////////////////////////////
    // Compare times.
    //
    // if Return value < 0 then it indicates value1 is less than value2.
    // if Return value > 0 then it indicates value2 is less than value1.
    // if Return value = 0 then it indicates value1 is equal to value2.
    int time_compare(
        gxtime* value1,
        gxtime* value2);

    /////////////////////////////////////////////////////////////////////////
    // Compare time and EPOCH time.
    //
    // if Return value < 0 then it indicates value1 is less than value2.
    // if Return value > 0 then it indicates value2 is less than value1.
    // if Return value = 0 then it indicates value1 is equal to value2.
    int time_compare2(
        gxtime* value1,
        uint32_t value2);


    int time_compareWithDiff(
        gxtime* value1,
        uint32_t value2,
        short deviationDiff);

#ifndef DLMS_USE_EPOCH_TIME
    //Get date-time from EPOCH time.
    int time_fromUnixTime(
        uint32_t epoch,
        struct tm* time);
#endif //DLMS_USE_EPOCH_TIME

    //Get date-time from EPOCH time.
    int time_fromUnixTime2(
        uint32_t epoch,
        uint16_t* year,
        unsigned char* month,
        unsigned char* day,
        unsigned char* hour,
        unsigned char* minute,
        unsigned char* second,
        unsigned char* dayOfWeek);

    //Get date-time from EPOCH time.
    int time_fromUnixTime3(
        const gxtime* time,
        uint16_t* year,
        unsigned char* month,
        unsigned char* day,
        unsigned char* hour,
        unsigned char* minute,
        unsigned char* second,
        unsigned char* dayOfWeek);

#ifndef DLMS_USE_EPOCH_TIME
    // Convert date time to Epoch time.
    uint32_t time_toUnixTime(
        struct tm* time);
#endif //DLMS_USE_EPOCH_TIME

    // Convert date time to Epoch time.
    uint32_t time_toUnixTime2(
        gxtime* time);

    //Get day of week.
    unsigned char time_dayOfWeek(
        uint16_t year,
        unsigned char month,
        unsigned char day);

    //Get deviation.
    int time_getDeviation(
        gxtime* value1);

    //Convert date time to UTC date time.
    int time_toUTC(gxtime* value);

    // Get next scheduled date in UTC time.
    uint32_t time_getNextScheduledDate(
        //Start time.
        uint32_t start,
        //Compared time.
        gxtime* value);

#ifdef  __cplusplus
}
#endif

#endif //DATE_H
