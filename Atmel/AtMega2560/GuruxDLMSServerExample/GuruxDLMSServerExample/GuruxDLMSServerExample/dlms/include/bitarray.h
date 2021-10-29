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
        uint16_t capacity;
        uint16_t size;
#ifndef GX_DLMS_MICROCONTROLLER
        uint16_t position;
#endif //GX_DLMS_MICROCONTROLLER
    } bitArray;

    //Initialize gxByteBuffer.
    void ba_init(
        bitArray* arr);

    //Attach bit array.
    void ba_attach(
        bitArray* arr,
        unsigned char* value,
        uint16_t count,
        uint16_t capacity);

    /*
    * Is static buffer used.
    */
    char ba_isAttached(
        bitArray* arr);

    //Bit array capacity.
    uint16_t ba_getCapacity(
        bitArray* arr);

    /*
    * Get size.
    */
    uint16_t ba_size(bitArray* bb);

    //How many bytes bit array will take.
    uint16_t ba_getByteCount(
        uint16_t bitCount);

    //Allocate new size for the array in bits.
    int ba_capacity(
        bitArray* arr,
        uint16_t capacity);

    //Push new data to the bit array.
    int ba_set(
        bitArray* arr,
        unsigned char item);

    //Push new data to the bit array.
    int ba_setByIndex(
        bitArray* arr,
        uint16_t index,
        unsigned char item);

    //Copy bit array.
    int ba_copy(
        bitArray* target,
        unsigned char* source,
        uint16_t count);

    //Clear bit array.
    void ba_clear(
        bitArray* arr);

    //Get bit value.
    int ba_get(
        bitArray* arr,
        unsigned char* value);

    //Get bit value by index.
    int ba_getByIndex(
        bitArray* arr,
        int index,
        unsigned char* value);

    //Convert bit array to integer.
    int ba_toInteger(
        bitArray* arr,
        uint32_t* value);

#ifndef DLMS_IGNORE_MALLOC
    //Get bit array as a string.
    char* ba_toString(
        bitArray* arr);
#endif //DLMS_IGNORE_MALLOC

    //Add bit array as a string.
    int ba_toString2(
        gxByteBuffer* bb,
        bitArray* ba);

#define BIT_ATTACH(X, V, S) ba_attach(&X, V, S, 8 * sizeof(V)/sizeof(V[0]))

#ifdef  __cplusplus
}
#endif

#endif //BYTE_ARRAY_H
