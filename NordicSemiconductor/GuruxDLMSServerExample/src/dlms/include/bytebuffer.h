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
#include "gxignore.h"

#define VECTOR_CAPACITY 50

    typedef struct
    {
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        unsigned char* data;
        unsigned long capacity;
        unsigned long size;
        unsigned long position;
#else
        unsigned char* data;
        unsigned short capacity;
        unsigned short size;
        unsigned short position;
#endif
    } gxByteBuffer;

    /*
    * Is static buffer used.
    */
    char bb_isAttached(
        gxByteBuffer* arr);

    /*Get maximum buffer size.*/
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long bb_getCapacity(
        gxByteBuffer* arr);
#else
    unsigned short bb_getCapacity(
        gxByteBuffer* arr);
#endif

    /*
    * Initialize gxByteBuffer.
    */
    int bb_init(
        gxByteBuffer* bb);

    /*
    * Allocate new size for the array in bytes.
    */
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_capacity(
        gxByteBuffer* bb,
        unsigned long capacity);
#else
    int bb_capacity(
        gxByteBuffer* bb,
        unsigned short capacity);
#endif

 /*
 * Get size.
 */
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long bb_size(
        gxByteBuffer* bb);
#else
    unsigned short bb_size(
        gxByteBuffer* bb);
#endif


    /*
    * Fill buffer it with zeros.
    */
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    void bb_zero(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned long count);
#else
    void bb_zero(
        gxByteBuffer* bb,
        unsigned short index,
        unsigned short count);
#endif

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_insertUInt8(
        gxByteBuffer * arr,
        unsigned long index,
        unsigned char item);
#else
    int bb_insertUInt8(
        gxByteBuffer * arr,
        unsigned short index,
        unsigned char item);
#endif

    //Set new data to the gxByteBuffer.
    int bb_setUInt8(
        gxByteBuffer *bb,
        unsigned char item);

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_setUInt8ByIndex(
        gxByteBuffer *arr,
        unsigned long index,
        unsigned char item);
#else
    int bb_setUInt8ByIndex(
        gxByteBuffer *arr,
        unsigned short index,
        unsigned char item);
#endif

    int bb_setUInt16(
        gxByteBuffer* bb,
        unsigned short item);

    int bb_setUInt32(
        gxByteBuffer* bb,
        unsigned long item);

    int bb_setUInt32ByIndex(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned long item);

    int bb_setUInt64(
        gxByteBuffer* bb,
        unsigned long long item);

#ifndef GX_DLMS_MICROCONTROLLER
    int bb_setFloat(
        gxByteBuffer* arr,
        float value);

    int bb_setDouble(
        gxByteBuffer* arr,
        double value);
#endif //GX_DLMS_MICROCONTROLLER

    int bb_setInt8(
        gxByteBuffer* bb,
        char item);

    int bb_setInt16(
        gxByteBuffer* bb,
        short item);

    int bb_setInt32(
        gxByteBuffer* bb,
        long item);

    int bb_setInt64(
        gxByteBuffer* bb,
        long long item);

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_insert(
        const unsigned char* src,
        unsigned long count,
        gxByteBuffer* target,
        unsigned long index);

#else
    int bb_insert(
        const unsigned char* src,
        unsigned short count,
        gxByteBuffer* target,
        unsigned short index);
#endif

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_set(
        gxByteBuffer* bb,
        const unsigned char* pSource,
        unsigned long count);
#else
    int bb_set(
        gxByteBuffer* bb,
        const unsigned char* pSource,
        unsigned short count);
#endif

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_set2(
        gxByteBuffer* bb,
        gxByteBuffer* data,
        unsigned long index,
        unsigned long count);
#else
    int bb_set2(
        gxByteBuffer* bb,
        gxByteBuffer* data,
        unsigned short index,
        unsigned short count);
#endif

    int bb_addString(
        gxByteBuffer* bb,
        const char* value);

    void bb_attachString(
        gxByteBuffer* bb,
        char* value);

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    void bb_attach(
        gxByteBuffer *arr,
        unsigned char * value,
        unsigned long count,
        unsigned long capacity);
#else
    void bb_attach(
        gxByteBuffer *arr,
        unsigned char * value,
        unsigned short count,
        unsigned short capacity);
#endif

    int bb_clear(
        gxByteBuffer* bb);

    int bb_getUInt8(
        gxByteBuffer* bb,
        unsigned char* value);

    int bb_getUInt8ByIndex(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned char* value);

    int bb_getUInt16(
        gxByteBuffer* bb,
        unsigned short* value);

    int bb_getUInt32(
        gxByteBuffer* bb,
        unsigned long* value);

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_get(
        gxByteBuffer* bb,
        unsigned char* value,
        unsigned long count);
#else
    int bb_get(
        gxByteBuffer* bb,
        unsigned char* value,
        unsigned short count);
#endif

    int bb_getInt8(
        gxByteBuffer* arr,
        signed char* value);

    int bb_getInt16(
        gxByteBuffer* bb,
        short* value);

    int bb_getInt32(
        gxByteBuffer* bb,
        long* value);

    int bb_getInt64(
        gxByteBuffer* bb,
        long long* value);

    int bb_getUInt64(
        gxByteBuffer* bb,
        unsigned long long* value);

#ifndef GX_DLMS_MICROCONTROLLER
    int bb_getFloat(
        gxByteBuffer* bb,
        float* value);

    int bb_getDouble(
        gxByteBuffer* bb,
        double* value);
#endif //GX_DLMS_MICROCONTROLLER

    int bb_getUInt16ByIndex(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned short* value);

    int bb_getUInt32ByIndex(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned long* value);

    int bb_getUInt64ByIndex(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned long long* value);

    int bb_getUInt128ByIndex(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned char* value);

    //Add hex string to byte buffer.
    int bb_addHexString(
        gxByteBuffer* arr,
        const char* str);

#ifndef GX_DLMS_MICROCONTROLLER
    //Get byte array as a string.
    char* bb_toString(
        gxByteBuffer* bb);

    //Get byte array as hex string.
    char* bb_toHexString(
        gxByteBuffer* bb);

    //Add integer value to byte array as a string.
    void bb_addIntAsString(
        gxByteBuffer* ba,
        int value);

    //Add double value to byte array as a string.
    void bb_addDoubleAsString(
        gxByteBuffer* ba,
        double value);
#endif //GX_DLMS_MICROCONTROLLER
    /**
        * Returns data as byte array.
        *
        * @param bb Byte buffer as a byte array.
        */
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    int bb_subArray(
        gxByteBuffer* bb,
        unsigned long index,
        unsigned long count,
        gxByteBuffer* target);
#else
    int bb_subArray(
        gxByteBuffer* bb,
        unsigned short index,
        unsigned short count,
        gxByteBuffer* target);
#endif

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    //Move data insize byte array.
    int bb_move(
        gxByteBuffer* ba,
        unsigned long srcPos,
        unsigned long destPos,
        unsigned long count);
#else
    //Move data insize byte array.
    int bb_move(
        gxByteBuffer* ba,
        unsigned short srcPos,
        unsigned short destPos,
        unsigned short count);
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
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned char bb_compare(
        gxByteBuffer* bb,
        unsigned char* buff,
        unsigned long length);
#else
    unsigned char bb_compare(
        gxByteBuffer* bb,
        unsigned char* buff,
        unsigned short length);
#endif

    //Find index of given char.
    unsigned long bb_indexOf(
        gxByteBuffer* bb,
        char ch);

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    //Print content of byte buffer to cout.
    void bb_print(gxByteBuffer* bb);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)


#ifdef  __cplusplus
}
#endif

#endif //BYTE_BUFFER_H
