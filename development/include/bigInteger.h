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

#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#include "bytebuffer.h"

#ifdef DLMS_SECURITY_SUITE_2
#define BIG_INTERER_CAPACITY 80
#else
#define BIG_INTERER_CAPACITY 26
#endif //DLMS_SECURITY_SUITE_2

    typedef struct
    {
        //List of values. Least Significated is in the first item.
        uint32_t* data;

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        uint32_t capacity;
        //Bit array size.
        uint32_t size;
#else
        uint16_t capacity;
        //Bit array size.
        uint16_t size;
#endif
        /** Is value IsNegative.*/
        unsigned char isNegative;
        //Access big integer throught data. Don't use buff directly.
        uint32_t buff[BIG_INTERER_CAPACITY];
    } bigInteger;

    /*Initialize default value.*/
    void bi_init(bigInteger* arr);

    /*Append value.*/
    void bi_append(bigInteger* arr,
        uint32_t value);

    //Attach bit array.
    int bi_attach(
        bigInteger* arr,
        uint32_t* value,
        uint16_t count,
        uint16_t capacity);

    /*
    * Is static big integer buffer used.
    */
    char bi_isAttached(
        bigInteger* arr);

    int bi_clear(bigInteger* arr);

    //Copy big integer.
    int bi_copy(bigInteger* target, bigInteger* source);

    //Bit array capacity.
    uint16_t bi_getCapacity(
        bigInteger* arr);

    /*
  * Allocate new size for the bit array.
  */
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    int bi_capacity(
        bigInteger* arr,
        uint32_t capacity);
#else
    int bi_capacity(
        bigInteger* arr,
        uint16_t capacity);
#endif

    /*
    * Get size.
    */
    uint16_t bi_size(bigInteger* bb);

    //Invert value.
    int bi_inv(bigInteger* arr, bigInteger* value);
    //Multiply value.
    int bi_multiply(bigInteger* arr, bigInteger* value);
    //Multiply value.
    void bi_multiplyInt32(bigInteger* arr, int32_t value);
    //Modulus.
    int bi_mod(bigInteger* arr, bigInteger* mod);

    //Push new values to the bit array.
    int bi_addRange(
        bigInteger* arr,
        const uint32_t* values, uint16_t count);

    //Push new value to big integer.
    int bi_add(
        bigInteger* arr,
        const uint32_t item);

    //Push new value to the big integer.
    int bi_addBigInteger(bigInteger* arr, bigInteger* value);

    void bi_sub(bigInteger* arr, bigInteger* value);

#define BI_ATTACH(X, V, S) bi_attach(&X, V, S, 8 * sizeof(V)/sizeof(V[0]))

    unsigned char bi_isZero(bigInteger* arr);

    unsigned char bi_isEven(bigInteger* arr);

    unsigned char bi_isOne(bigInteger* arr);

    //Create biginteger from byte buffer.
    int bi_fromByteBuffer(bigInteger* value, gxByteBuffer* bb);

    //Convert big integer to byte buffer.
    int bi_toArray(bigInteger* arr, gxByteBuffer* data);

    //Used bits.
    uint16_t bi_getUsedBits(bigInteger* bi);

    //This method checks if the bit is set.
    unsigned char bi_isBitSet(bigInteger* bi, uint16_t index);

    /**
     Compare biginter value to other big integer value.

     Returns 1 is compared value is bigger, -1 if smaller and 0 if values are equals.     
    */
    int bi_compare(bigInteger* arr, bigInteger* value);


#ifdef  __cplusplus
}
#endif

#endif //BIG_INTEGER_H
