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

#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxint.h"
#include "gxignore.h"

//Arduino DOIT ESP32 uses bb_init. bb_Init is used instead.
#ifndef ESP_PLATFORM
#define BYTE_BUFFER_INIT bb_init
#else
#define BYTE_BUFFER_INIT bb_Init
#endif //DESP_PLATFORM

#define VECTOR_CAPACITY 50
    typedef struct
    {
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        unsigned char* data;
        uint32_t capacity;
        uint32_t size;
        uint32_t position;
#else
        unsigned char* data;
        uint16_t capacity;
        uint16_t size;
        uint16_t position;
#endif
    } gxByteBuffer;

    /*
    * Is static buffer used.
    */
    char bb_isAttached(
        gxByteBuffer* arr);

    /*Get maximum buffer size.*/
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t bb_getCapacity(
        gxByteBuffer* arr);
#else
    uint16_t bb_getCapacity(
        gxByteBuffer* arr);
#endif

    /*Returns amount of the available bytes.*/
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t bb_available(gxByteBuffer* arr);
#else
    uint16_t bb_available(gxByteBuffer* arr);
#endif
    /*
    * Initialize gxByteBuffer.
    */
    int BYTE_BUFFER_INIT(
        gxByteBuffer* bb);

    /*
    * Allocate new size for the array in bytes.
    */
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_capacity(
        gxByteBuffer* bb,
        uint32_t capacity);
#else
    int bb_capacity(
        gxByteBuffer* bb,
        uint16_t capacity);
#endif

    /*
    * Get size.
    */
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t bb_size(
        gxByteBuffer* bb);
#else
    uint16_t bb_size(
        gxByteBuffer* bb);
#endif


    /*
    * Fill buffer it with zeros.
    */
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_zero(
        gxByteBuffer* bb,
        uint32_t index,
        uint32_t count);
#else
    int bb_zero(
        gxByteBuffer* bb,
        uint16_t index,
        uint16_t count);
#endif

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_insertUInt8(
        gxByteBuffer* arr,
        uint32_t index,
        unsigned char item);
#else
    int bb_insertUInt8(
        gxByteBuffer* arr,
        uint16_t index,
        unsigned char item);
#endif

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_allocate(
        gxByteBuffer* arr,
        uint32_t index,
        uint32_t dataSize);
#else
    int bb_allocate(
        gxByteBuffer* arr,
        uint16_t index,
        uint16_t dataSize);
#endif
    //Set new data to the gxByteBuffer.
    int bb_setUInt8(
        gxByteBuffer* bb,
        unsigned char item);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_setUInt8ByIndex(
        gxByteBuffer* arr,
        uint32_t index,
        unsigned char item);
#else
    int bb_setUInt8ByIndex(
        gxByteBuffer* arr,
        uint16_t index,
        unsigned char item);
#endif

    int bb_setUInt16(
        gxByteBuffer* bb,
        uint16_t item);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_setUInt16ByIndex(
        gxByteBuffer* arr,
        uint32_t index,
        uint16_t item);
#else
    int bb_setUInt16ByIndex(
        gxByteBuffer* arr,
        uint16_t index,
        uint16_t item);
#endif

    int bb_setUInt32(
        gxByteBuffer* bb,
        uint32_t item);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_setUInt32ByIndex(
        gxByteBuffer* arr,
        uint32_t index,
        uint32_t item);
#else
    int bb_setUInt32ByIndex(
        gxByteBuffer* arr,
        uint16_t index,
        uint32_t item);
#endif //!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))

    int bb_setUInt64(
        gxByteBuffer* bb,
        uint64_t item);

#ifndef DLMS_IGNORE_FLOAT32
    int bb_setFloat(
        gxByteBuffer* arr,
        float value);
#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT64
    int bb_setDouble(
        gxByteBuffer* arr,
        double value);
#endif //DLMS_IGNORE_FLOAT64

    int bb_setInt8(
        gxByteBuffer* bb,
        char item);

    int bb_setInt16(
        gxByteBuffer* bb,
        int16_t item);

    int bb_setInt32(
        gxByteBuffer* bb,
        int32_t item);

    int bb_setInt64(
        gxByteBuffer* bb,
        int64_t item);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_insert(
        const unsigned char* src,
        uint32_t count,
        gxByteBuffer* target,
        uint32_t index);

#else
    int bb_insert(
        const unsigned char* src,
        uint16_t count,
        gxByteBuffer* target,
        uint16_t index);
#endif

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_set(
        gxByteBuffer* bb,
        const unsigned char* pSource,
        uint32_t count);
#else
    int bb_set(
        gxByteBuffer* bb,
        const unsigned char* pSource,
        uint16_t count);
#endif

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_set2(
        gxByteBuffer* bb,
        gxByteBuffer* data,
        uint32_t index,
        uint32_t count);
#else
    int bb_set2(
        gxByteBuffer* bb,
        gxByteBuffer* data,
        uint16_t index,
        uint16_t count);
#endif

    int bb_addString(
        gxByteBuffer* bb,
        const char* value);

    int bb_attachString(
        gxByteBuffer* bb,
        char* value);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_attach(
        gxByteBuffer* arr,
        unsigned char* value,
        uint32_t count,
        uint32_t capacity);
    int bb_attachString2(
        gxByteBuffer* arr,
        char* value,
        uint32_t count,
        uint32_t capacity);
#else
    int bb_attach(
        gxByteBuffer* arr,
        unsigned char* value,
        uint16_t count,
        uint16_t capacity);
    int bb_attachString2(
        gxByteBuffer* arr,
        char* value,
        uint16_t count,
        uint16_t capacity);
#endif
    //Clean byte buffer and release allocated memory.
    int bb_clear(
        gxByteBuffer* bb);

    //Set size and position to zero. Allocated memory is not released.
    int bb_empty(
        gxByteBuffer* bb);

    int bb_getUInt8(
        gxByteBuffer* bb,
        unsigned char* value);

    int bb_getUInt8ByIndex(
        gxByteBuffer* bb,
        uint32_t index,
        unsigned char* value);

    int bb_getUInt16(
        gxByteBuffer* bb,
        uint16_t* value);

    int bb_getUInt24(
        gxByteBuffer* bb,
        uint32_t* value);

    int bb_getUInt32(
        gxByteBuffer* bb,
        uint32_t* value);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_get(
        gxByteBuffer* bb,
        unsigned char* value,
        uint32_t count);
#else
    int bb_get(
        gxByteBuffer* bb,
        unsigned char* value,
        uint16_t count);
#endif

    int bb_getInt8(
        gxByteBuffer* arr,
        signed char* value);

    int bb_getInt16(
        gxByteBuffer* bb,
        int16_t* value);

    int bb_getInt32(
        gxByteBuffer* bb,
        int32_t* value);

    int bb_getInt64(
        gxByteBuffer* bb,
        int64_t* value);

    int bb_getUInt64(
        gxByteBuffer* bb,
        uint64_t* value);

#ifndef DLMS_IGNORE_FLOAT32
    int bb_getFloat(
        gxByteBuffer* bb,
        float* value);
#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT64
    int bb_getDouble(
        gxByteBuffer* bb,
        double* value);
#endif //DLMS_IGNORE_FLOAT64

    int bb_getUInt16ByIndex(
        gxByteBuffer* bb,
        uint32_t index,
        uint16_t* value);

    int bb_getUInt24ByIndex(
        gxByteBuffer* bb,
        uint32_t index,
        uint32_t* value);

    int bb_getUInt32ByIndex(
        gxByteBuffer* bb,
        uint32_t index,
        uint32_t* value);

    int bb_getUInt64ByIndex(
        gxByteBuffer* bb,
        uint32_t index,
        uint64_t* value);

    int bb_getUInt128ByIndex(
        gxByteBuffer* bb,
        uint32_t index,
        unsigned char* value);

#ifndef DLMS_IGNORE_MALLOC
    //Add hex string to byte buffer.
    int bb_addHexString(
        gxByteBuffer* arr,
        const char* str);
#endif //DLMS_IGNORE_MALLOC

    //Add hex string to byte buffer.
    int bb_addHexString2(
        gxByteBuffer* arr,
        const char* str);

    //Add hex string to byte buffer.
    int bb_addLogicalName(
        gxByteBuffer* arr,
        const unsigned char* str);

    //Get byte array as hex string.
    int bb_toHexString2(
        gxByteBuffer* arr,
        char* buffer,
        uint16_t size);

#if !(defined(DLMS_IGNORE_STRING_CONVERTER) || defined(DLMS_IGNORE_MALLOC))
    //Get byte array as a string.
    char* bb_toString(
        gxByteBuffer* bb);

    //Get byte array as hex string.
    char* bb_toHexString(
        gxByteBuffer* bb);

    //Add double value to byte array as a string.
    int bb_addDoubleAsString(
        gxByteBuffer* ba,
        double value);
#endif //!(defined(DLMS_IGNORE_STRING_CONVERTER) || defined(DLMS_IGNORE_MALLOC))

    //Add integer value to byte array as a string.
    int bb_addIntAsString(
        gxByteBuffer* ba,
        int value);

    //Add integer value to byte array as a string.
    int bb_addIntAsString2(
        gxByteBuffer* ba,
        int value,
        unsigned char digits);

    /**
        * Returns data as byte array.
        *
        * @param bb Byte buffer as a byte array.
        */
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bb_subArray(
        gxByteBuffer* bb,
        uint32_t index,
        uint32_t count,
        gxByteBuffer* target);
#else
    int bb_subArray(
        gxByteBuffer* bb,
        uint16_t index,
        uint16_t count,
        gxByteBuffer* target);
#endif

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    //Move data insize byte array.
    int bb_move(
        gxByteBuffer* ba,
        uint32_t srcPos,
        uint32_t destPos,
        uint32_t count);
#else
    //Move data insize byte array.
    int bb_move(
        gxByteBuffer* ba,
        uint16_t srcPos,
        uint16_t destPos,
        uint16_t count);
#endif

    /**
     * Remove handled bytes. This can be used in debugging to remove handled
     * bytes.
     */
    int bb_trim(
        gxByteBuffer* bb);

    /**
       * Compares, whether two given arrays are similar starting from current
       * position.
       *
       * @param bb
       *            Bytebuffer to compare.
       * @param arr
       *            Array to compare.
       * @param length
       *            Array length.
       * @return True, if arrays are similar. False, if the arrays differ.
       */
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    unsigned char bb_compare(
        gxByteBuffer* bb,
        unsigned char* buff,
        uint32_t length);
#else
    unsigned char bb_compare(
        gxByteBuffer* bb,
        unsigned char* buff,
        uint16_t length);
#endif

    //Find index of given char.
    uint32_t bb_indexOf(
        gxByteBuffer* bb,
        char ch);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    void bb_reverse(gxByteBuffer* bb,
        uint32_t index,
        uint32_t count);
#else
    void bb_reverse(gxByteBuffer* bb,
        uint16_t index,
        uint16_t count);
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    //Print content of byte buffer to cout.
    int bb_print(gxByteBuffer* bb);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

#define BB_ATTACH(X, V, S) bb_attach(&X, V, S, sizeof(V))

#define BB_ATTACH_STR(X, V, S) bb_attachString2(&X, V, S, sizeof(V))

#ifdef  __cplusplus
}
#endif

#endif //BYTE_BUFFER_H
