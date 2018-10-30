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

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#include "bytebuffer.h"

#define BIT_ARRAY_CAPACITY 8

    typedef struct
    {
        unsigned char* data;
        unsigned short capacity;
        unsigned short size;
#ifndef GX_DLMS_MICROCONTROLLER
        unsigned short position;
#endif //GX_DLMS_MICROCONTROLLER
    } bitArray;

    //Initialize gxByteBuffer.
    void bit_init(
        bitArray* arr);

    //How many bytes bit array will take.
    unsigned short bit_getByteCount(
        unsigned short bitCount);

    //Allocate new size for the array in bits.
    void bit_capacity(
        bitArray* arr,
        unsigned short capacity);

    //Push new data to the bit array.
    void bit_set(
        bitArray * arr,
        unsigned char item);

    //Push new data to the bit array.
    void bit_setByIndex(
        bitArray *arr,
        int index,
        unsigned char item);

    //Add bits from byte array to bit array.
    int bit_add(
        bitArray *arr,
        gxByteBuffer *bytes,
        unsigned short count,
        unsigned char intelByteOrder);

    //Copy bit array.
    int bit_copy(
        bitArray *target,
        unsigned char *source,
        unsigned short count);

    //Clear bit array.
    void bit_clear(
        bitArray* arr);

    //Get bit value.
    int bit_get(
        bitArray* arr,
        unsigned char* value);

    //Get bit value by index.
    int bit_getByIndex(
        bitArray* arr,
        int index,
        unsigned char* value);

    //Convert bit array to integer.
    int bit_toInteger(
        bitArray *arr,
        int *value);

    //Get bit array as a string.
    char* bit_toString(
        bitArray* arr);

#ifdef  __cplusplus
}
#endif

#endif //BYTE_ARRAY_H
