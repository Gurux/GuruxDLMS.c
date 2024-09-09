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

#ifndef GXHELPERS_H
#define GXHELPERS_H

#ifdef  __cplusplus
extern "C" {
#endif
#include "gxignore.h"
#ifdef USE_AVR
    //If AVR is used.
#include <avr/pgmspace.h>
#endif //USE_AVR

#include "bytebuffer.h"
#include "variant.h"

static const unsigned char EMPTY_SYSTEM_TITLE[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static const unsigned char EMPTY_LN[6] = { 0, 0, 0, 0, 0, 0 };
static const unsigned char DEFAULT_ASSOCIATION[6] = { 0, 0, 40, 0, 0, 255 };
static const unsigned char EMPTY_KEY[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


//Get error message directly from EEPROM to save RAM.
#if defined(ARDUINO) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
//If AVR is used.
   #ifdef IDF_VER//espressif is using the different folder.
      #include <pgmspace.h>
   #else
      #include <avr/pgmspace.h>
   #endif//ESP_PLATFORM
#define GET_STR_FROM_EEPROM(x) PSTR(x)
#else
#define GET_STR_FROM_EEPROM(x) (const char*)x
#endif//defined(ARDUINO) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)


//Get UInt32.
#define GETU32(pt) (((uint32_t)(pt)[0] << 24) | \
                    ((uint32_t)(pt)[1] << 16) | \
                    ((uint32_t)(pt)[2] <<  8) | \
                    ((uint32_t)(pt)[3]))

//Set Int32 as Big Endian value.
#define PUT32(ct, st) { \
                    (ct)[0] = (unsigned char)((st) >> 24); \
                    (ct)[1] = (unsigned char)((st) >> 16); \
                    (ct)[2] = (unsigned char)((st) >>  8); \
                    (ct)[3] = (unsigned char)(st); }

    //Check byte order.
    unsigned char hlp_isBigEndian(void);

    //Convert ASCII value to numeric unsigned char value.
    unsigned char hlp_getValue(char c);

    const char* hlp_getErrorMessage(int error);

    //Returns items count. Use hlp_getObjectCount2.
    int hlp_getObjectCount(gxByteBuffer* buff);

    //Returns items count.
    int hlp_getObjectCount2(
        gxByteBuffer* buff,
        uint16_t* count);

    //Get count size in bytes.
    unsigned char hlp_getObjectCountSizeInBytes(uint32_t count);

    // Set count of items.
    int hlp_setObjectCount(
        uint32_t count,
        gxByteBuffer* buff);

#ifndef DLMS_IGNORE_MALLOC
    /**
    * Convert byte array to hex string. This method use malloc to allocate enough memory.
    */
    char* hlp_bytesToHex(const unsigned char* pBytes, int count);
#endif //DLMS_IGNORE_MALLOC

    /**
    * Convert byte array to hex string.
    */
    int hlp_bytesToHex2(const unsigned char* bytes, uint16_t count, char* buff, uint16_t size);

    /**
    * Convert byte array to hex string.
    */
    int hlp_bytesToHex3(const unsigned char* bytes, uint16_t count, char* buff, uint16_t size, unsigned char addSpace);

#ifndef DLMS_IGNORE_MALLOC
    /**
    * Convert hex string to byte array. This method use malloc to allocate enough memory.
    */
    int hlp_hexToBytes(
        const char* str,
        unsigned char** arr,
        uint16_t* count);
#endif //DLMS_IGNORE_MALLOC

    /**
    * Convert hex string to byte array.
    */
    int hlp_hexToBytes2(
        const char* str,
        unsigned char* arr,
        uint16_t* count);

#if !defined(DLMS_IGNORE_MALLOC)
    //Set logical name from string.
    int hlp_setLogicalName(unsigned char ln[6], const char* name);
#endif //!defined(DLMS_IGNORE_MALLOC)

#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)
    void hlp_trace(unsigned char* data, int index, int count, unsigned char send);
    //Get Logical Name from string.
    int hlp_parseLogicalName(gxByteBuffer* value, unsigned char ln[6]);

    //Set logical name from string.
    int hlp_setLogicalName2(dlmsVARIANT* ln, const char* name);

    int hlp_appendLogicalName(gxByteBuffer* bb, const unsigned char value[6]);

    //Print logical name to cout.
    int hlp_printLogicalName(
        //Format.
        const char* format,
        //Logical name.
        const unsigned char value[6]);
#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)

    int hlp_getLogicalNameToString(const unsigned char value[6], char* ln);

    void hlp_replace(gxByteBuffer* str, char oldCh, char newCh);

    double hlp_getScaler(int scaler);

    /**
    * Get data type in bytes.
    *
    * @param type
    *            Data type.
    * @return Size of data type in bytes.
    */
    int hlp_getDataTypeSize(DLMS_DATA_TYPE type);

    /**
    * Convert integer to string.
    *
    * @param str
    *            Parsed string.
    * @param strsize
    *            String size.
    * @param value
    *            Integer value.
    * @param isSigned
    *            Is value signed number.
    * @param digits
    *            number of digits in string.
    * @return Length of the string or -1 if error has occurred.
    */
    int hlp_intToString(
        char* str,
        int bufsize,
        int32_t value,
        unsigned char isSigned,
        unsigned char digits);

    /**
    * Convert integer to string.
    *
    * @param str
    *            Parsed string.
    * @param strsize
    *            String size.
    * @param value
    *            Integer value.
    * @param digits
    *            number of digits in string.
    * @return Length of the string or -1 if error has occurred.
    */
    int hlp_uint64ToString(
        char* str,
        int bufsize,
        uint64_t value,
        unsigned char digits);

    /**
    * Convert string to integer.
    *
    * @param str
    *            Parsed string.
    * @return Value of string as integer.
    */
    int32_t hlp_stringToInt(
        const char* str);

    /**
    * Convert string to integer.
    *
    * @param str
    *            Parsed string.
    * @return Value of string as integer.
    */
    int32_t hlp_stringToInt2(
        const char* str, const char* end);

    /**
    * Convert integer to string.
    *
    * @param str
    *            Parsed string.
    * @param strsize
    *            String size.
    * @param value
    *            Integer value.
    * @param isSigned
    *            Is value signed number.
    * @return Length of the string or -1 if error has occurred.
    */
    int hlp_int64ToString(
        char* str,
        int bufsize,
        int64_t value,
        unsigned char isSigned);

    /**
    * Convert string to integer64.
    *
    * @param str
    *            Parsed string.
    * @return Value of string as integer.
    */
    int64_t hlp_stringToInt64(
        const char* str);

    /**
    * Random generator using Linear-feedback shift register.
    * https://en.wikipedia.org/wiki/Linear-feedback_shift_register
    *
    * @return Random number.
    */
    unsigned char hlp_rand(void);

    //Add bits from byte array to bit array.
    int hlp_add(
        bitArray* arr,
        gxByteBuffer* bytes,
        uint16_t count);


    //Swap bits. Reserved for internal use.
    unsigned char hlp_swapBits(unsigned char value);


    /*Load file.*/
    int hlp_load(const char* path, char* value, uint16_t* length);

    /*Save file.*/
    int hlp_save(const char* path, const char* value);

    /*Create new directory.*/
    int hlp_createDir(const char* path);
    
    /*Check if the directory exists.*/
    unsigned char hlp_directoryExists(const char* path);

    /**
     * Convert Base64 string to byte array.
     *
     * @param input
     *            Base64 string.
     * @return Converted byte array.
     */
    int hlp_fromBase64(const char* input, uint16_t length, gxByteBuffer* decoded);

#ifdef  __cplusplus
}
#endif

#endif //GXHELPERS_H
