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

#include "../include/gxmem.h"
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/errorcodes.h"
#include "../include/bigInteger.h"
#include "../include/helpers.h"

char bi_isAttached(bigInteger* arr)
{
    if (arr == NULL)
    {
        return 0;
    }
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    //If byte buffer is attached.
    return (arr->capacity & 0x80000000) == 0x80000000;
#else
    return (arr->capacity & 0x8000) == 0x8000;
#endif
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bi_capacity(
    bigInteger* arr,
    uint32_t capacity)
#else
int bi_capacity(
    bigInteger* arr,
    uint16_t capacity)
#endif
{
#ifndef DLMS_IGNORE_MALLOC
    //Capacity can't change if it's attached.
    if (!bi_isAttached(arr))
    {
        if (capacity == 0)
        {
            if (arr->data != NULL)
            {
                gxfree(arr->data);
                arr->data = NULL;
                arr->size = 0;
            }
        }
        else
        {
            if (arr->capacity == 0)
            {
                arr->data = (uint32_t*)gxmalloc(capacity);
                if (arr->data == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
            }
            else
            {
#ifdef gxrealloc
                //If compiler supports realloc.
                uint32_t* tmp = (uint32_t*)gxrealloc(arr->data, capacity);
                //If not enought memory available.
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr->data = tmp;
#else
                //If compiler doesn't support realloc.
                uint32_t* old = arr->data;
                arr->data = (uint32_t*)gxmalloc(capacity);
                //If not enought memory available.
                if (arr->data == NULL)
                {
                    arr->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(arr->data, old, arr->size);
                gxfree(old);
#endif // gxrealloc       
            }
            if (arr->size > capacity)
            {
                arr->size = capacity;
            }
        }
        arr->capacity = capacity;
    }
#endif //DLMS_IGNORE_MALLOC
    if (bi_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return DLMS_ERROR_CODE_OK;
}

uint16_t bi_getCapacity(bigInteger* arr)
{
    return arr->capacity & 0x7FFF;
}

uint16_t bi_size(bigInteger* bb)
{
    return bb != NULL ? bb->size : 0;
}

void bi_init(bigInteger* arr)
{
    bi_attach(arr, arr->buff, 0, sizeof(arr->buff));
    arr->isNegative = 0;
}

void bi_append(bigInteger* arr, uint32_t value)
{
    arr->data[arr->size] = value;
    ++arr->size;
}

int bi_attach(
    bigInteger* arr,
    uint32_t* value,
    uint16_t count,
    uint16_t capacity)
{
    arr->data = value;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    arr->capacity = 0x80000000 | capacity;
#else
    if (capacity >= 0x8000)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    arr->capacity = (uint16_t)(0x8000 | capacity);
#endif
    arr->size = count;
    arr->isNegative = 0;
    return 0;
}

int bi_addRange(
    bigInteger* arr,
    const uint32_t* values, uint16_t count)
{
    if ((arr->size + count) > bi_getCapacity(arr))
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    memcpy(arr->data + arr->size, values, sizeof(uint32_t) * count);
    arr->size += count;
    return 0;
}

int bi_add(bigInteger* arr, const uint32_t value)
{
    return bi_addRange(arr, &value, 1);
}

int bi_addValue2(uint32_t* list,
    uint16_t length,
    uint32_t value,
    uint16_t index)
{
    uint64_t tmp;
    if (index < length)
    {
        tmp = list[index];
        tmp += value;
        list[index] = (uint32_t)tmp;
        uint32_t reminer = (uint32_t)(tmp >> 32);
        if (reminer > 0)
        {
            bi_addValue2(list, length, reminer, index + 1);
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return 0;
}

unsigned char bi_isZero(bigInteger* arr)
{
    return arr->size == 0 ||
        (arr->size == 1 && arr->data[0] == 0);
}

unsigned char bi_isEven(bigInteger* arr)
{
    return arr->size != 0 &&
        arr->data[0] % 2 == 0;
}

unsigned char bi_isOne(bigInteger* arr)
{
    return arr->size == 1 && arr->data[0] == 1;
}

int bi_toArray(bigInteger* arr, gxByteBuffer* data)
{
    int ret = 0;
    uint32_t pos;
    uint32_t value;
    for (pos = 0; pos != arr->size; ++pos)
    {
        value = arr->data[pos];
        if ((ret = bb_setUInt8(data, value)) != 0 ||
            (ret = bb_setUInt8(data, value >> 8)) != 0 ||
            (ret = bb_setUInt8(data, value >> 16)) != 0 ||
            (ret = bb_setUInt8(data, value >> 24)) != 0)
        {
            break;
        }
    }
    if (ret == 0)
    {
        bb_reverse(data, 0, data->size);
    }
    return ret;
}

void bi_or(bigInteger* arr, bigInteger* value)
{
    uint32_t pos;
    if (arr->size < value->size)
    {
        arr->size = value->size;
    }
    for (pos = 0; pos < value->size; ++pos)
    {
        arr->data[pos] |= value->data[pos];
    }
}

int bi_compare(bigInteger* arr, bigInteger* value)
{
    int ret = 0;
    if (arr->isNegative != value->isNegative)
    {
        //If other value is isNegative.
        if (arr->isNegative)
        {
            ret = -1;
        }
        else
        {
            ret = 1;
        }
    }
    else if (bi_isZero(arr) && bi_isZero(value))
    {
        ret = 0;
    }
    else
    {
        int cntA = (int)arr->size;
        cntA -= 1;
        //Skip zero values.
        while (cntA != -1 && arr->data[cntA] == 0)
        {
            --cntA;
        }
        int cntB = (int)value->size;
        cntB -= 1;
        //Skip zero values.
        while (cntB != -1 && value->data[cntB] == 0)
        {
            --cntB;
        }
        if (cntA > cntB)
        {
            ret = 1;
        }
        else if (cntA < cntB)
        {
            ret = -1;
        }
        else
        {
            do
            {
                if (arr->data[cntA] > value->data[cntA])
                {
                    ret = 1;
                    break;
                }
                else if (arr->data[cntA] < value->data[cntA])
                {
                    ret = -1;
                    break;
                }
                cntA -= 1;
            } while (cntA != -1);
        }
    }
    return ret;
}

int bi_compareValue(bigInteger* arr, uint32_t value)
{
    if (arr->size == 0)
    {
        return -1;
    }
    if (arr->data[0] == value)
    {
        return 0;
    }
    return arr->data[0] < value ? -1 : 1;
}

int bi_addBigInteger(bigInteger* arr, bigInteger* value)
{
    if (value->isNegative)
    {
        value->isNegative = 0;
        bi_sub(arr, value);
        value->isNegative = 1;
    }
    else
    {
        while (arr->size < value->size)
        {
            bi_add(arr, 0);
        }
        uint64_t overflow = 0;
        for (uint32_t pos = 0; pos != arr->size; ++pos)
        {
            uint64_t tmp = arr->data[pos];
            if (pos < value->size)
            {
                tmp += value->data[pos];
            }
            tmp += overflow;
            arr->data[pos] = (uint32_t)tmp;
            overflow = tmp >> 32;
        }
        if (overflow != 0)
        {
            bi_add(arr, (uint32_t)overflow);
        }
    }
    return 0;
}

int bi_fromByteBuffer(bigInteger* value, gxByteBuffer* bb)
{
    int ret = 0;
    uint32_t tmp;
    unsigned char ch;
    uint16_t ival;
    int pos;
    if ((ret = bi_clear(value)) == 0 &&
        (ret = bi_capacity(value, bb_size(bb) / 4)) == 0)
    {
        for (pos = bb_size(bb) - 4; pos > -1; pos = pos - 4)
        {
            if ((ret = bb_getUInt32ByIndex(bb, pos, &tmp)) != 0 ||
                (ret = bi_add(value, tmp)) != 0)
            {
                break;
            }
        }
        if (ret == 0)
        {
            switch (bb_size(bb) % 4)
            {
            case 1:
                if ((ret = bb_getUInt8(bb, &ch)) != 0 ||
                    (ret = bi_add(value, ch)) != 0)
                {
                    break;
                }
                break;
            case 2:
                if ((ret = bb_getUInt16(bb, &ival)) != 0 ||
                    (ret = bi_add(value, ival)) != 0)
                {
                    break;
                }
                break;
            case 3:
                if ((ret = bb_getUInt8(bb, &ch)) != 0 ||
                    (ret = bb_getUInt16(bb, &ival)) != 0)
                {
                    break;
                }
                tmp = ch;
                tmp <<= 16;
                tmp |= ival;
                ret = bi_add(value, tmp);
                break;
            default:
                break;
            }
        }
    }
    return ret;
}

void bi_sub(bigInteger* arr, bigInteger* value)
{
    int c = bi_compare(arr, value);
    if (c == 0)
    {
        bi_clear(arr);
    }
    else if (value->isNegative || c == -1)
    {
        if (!value->isNegative && !arr->isNegative)
        {
            //If biger value is decreased from smaller value.
            bigInteger tmp;
            bi_init(&tmp);
            bi_addBigInteger(&tmp, value);
            bi_sub(&tmp, arr);
            bi_clear(arr);
            bi_addRange(arr, tmp.data, tmp.size);
            arr->size = tmp.size;
            arr->isNegative = 1;
        }
        else
        {
            //If isNegative value is decreased from the value.
            unsigned char ret = value->isNegative;
            value->isNegative = 0;
            bi_addBigInteger(arr, value);
            value->isNegative = ret;
            arr->isNegative = !ret;
        }
    }
    else
    {
        if (!bi_isZero(value))
        {
            if (bi_isZero(arr))
            {
                arr->isNegative = 1;
                bi_clear(arr);
                bi_addRange(arr, value->data, value->size);
                arr->size = value->size;
            }
            else
            {
                while (arr->size < value->size)
                {
                    bi_add(arr, (uint32_t)0);
                }
                unsigned char borrow = 0;
                uint64_t tmp;
                uint32_t pos;
                for (pos = 0; pos != value->size; ++pos)
                {
                    tmp = arr->data[pos];
                    tmp += 0x100000000;
                    tmp -= value->data[pos];
                    tmp -= borrow;
                    arr->data[pos] = (uint32_t)tmp;
                    borrow = (unsigned char)((tmp < 0x100000000) ? 1 : 0);
                }
                if (borrow != 0)
                {
                    for (; pos != arr->size; ++pos)
                    {
                        tmp = arr->data[pos];
                        tmp += 0x100000000;
                        tmp -= borrow;
                        arr->data[pos] = (uint32_t)tmp;
                        borrow = (unsigned char)((tmp < 0x100000000) ? 1 : 0);
                        if (borrow == 0)
                        {
                            break;
                        }
                    }
                }
                //Remove empty last item(s).
                while (arr->size != 1 && arr->data[arr->size - 1] == 0)
                {
                    --arr->size;
                }
            }
        }
    }
}

int bi_multiply(bigInteger* arr, bigInteger* value)
{
    if (bi_isZero(value) || bi_isZero(arr))
    {
        arr->size = 0;
    }
    else if (!bi_isOne(value))
    {
        int ret2;
        uint16_t lenght = 1 + value->size + arr->size;
        bi_capacity(arr, lenght);
        uint32_t ret[BIG_INTERER_CAPACITY];
        memset(ret, 0, BIG_INTERER_CAPACITY * sizeof(uint32_t));
        uint32_t overflow = 0;
        uint32_t index = 0;
        for (uint32_t i = 0; i != value->size; ++i)
        {
            overflow = 0;
            for (uint32_t j = 0; j != arr->size; ++j)
            {
                uint64_t result = value->data[i];
                result *= arr->data[j];
                result += overflow;
                overflow = (uint32_t)(result >> 32);
                index = i + j;
                if ((ret2 = bi_addValue2(ret, lenght, (uint32_t)result, index)) != 0)
                {
                    return ret2;
                }
            }
            if (overflow > 0)
            {
                if ((ret2 = bi_addValue2(ret, lenght, overflow, 1 + index)) != 0)
                {
                    return ret2;
                }
            }
        }
        index = lenght - 1;
        while (index != 0 && ret[index] == 0)
        {
            --index;
        }
        ++index;
        memcpy(arr->data, ret, index * sizeof(uint32_t));
        arr->size = index;
    }
    if (value->isNegative != arr->isNegative)
    {
        if (!arr->isNegative)
        {
            arr->isNegative = 1;
        }
    }
    else if (arr->isNegative)
    {
        //If both values are isNegative.
        arr->isNegative = 0;
    }
    return 0;
}

void bi_multiplyInt32(bigInteger* arr, int32_t value)
{
    if (value == 0 || bi_isZero(arr))
    {
        arr->size = 0;
    }
    else if (value != 1)
    {
        uint16_t lenght = 1 + 1 + arr->size;
        bi_capacity(arr, lenght);
        uint32_t ret[BIG_INTERER_CAPACITY];
        memset(ret, 0, BIG_INTERER_CAPACITY * sizeof(uint32_t));
        uint32_t overflow = 0;
        uint32_t index;
        for (index = 0; index != arr->size; ++index)
        {
            uint64_t result = value;
            result *= arr->data[index];
            result += overflow;
            overflow = (uint32_t)(result >> 32);
            bi_addValue2(ret, lenght, (uint32_t)result, index);
        }
        if (overflow > 0)
        {
            bi_addValue2(ret, lenght, overflow, index);
        }
        index = lenght - 1;
        while (index != 0 && ret[index] == 0)
        {
            --index;
        }
        ++index;
        memcpy(arr->data, ret, index * sizeof(uint32_t));
        arr->size = index;
    }
    if (value < 0 && !arr->isNegative)
    {
        arr->isNegative = 1;
    }
    else if (value < 0 && arr->isNegative)
    {
        //If both values are isNegative.
        arr->isNegative = 0;
    }
}

void bi_lshift(bigInteger* arr, uint32_t amount)
{
    //Move dwords.
    uint16_t dwshift = (uint16_t)(amount / (8 * sizeof(uint32_t)));
    if (dwshift != 0)
    {
        memcpy(arr->buff + dwshift, arr->buff, sizeof(uint32_t) * arr->size);
        memset(arr->buff, 0, sizeof(uint32_t) * dwshift);
        arr->size += dwshift;
        amount -= dwshift * 8 * sizeof(uint32_t);
    }
    if (amount != 0)
    {
        uint32_t overflow = 0;
        for (uint32_t pos = 0; pos != arr->size; ++pos)
        {
            uint64_t tmp = arr->buff[pos];
            tmp <<= amount;
            tmp |= overflow;
            arr->buff[pos] = (uint32_t)tmp;
            overflow = (uint32_t)(tmp >> 32);
        }
        if (overflow != 0)
        {
            bi_add(arr, overflow);
        }
    }
}

void bi_rshift(bigInteger* arr, uint32_t amount)
{
    while (amount > 8 * sizeof(uint32_t))
    {
        bi_rshift(arr, 8 * sizeof(uint32_t));
        //--m_Count;
        amount -= 8 * sizeof(uint32_t);
    }
    if (amount != 0)
    {
        int pos;
        uint64_t overflow = 0;
        uint32_t mask = (uint32_t)0xFFFFFFFF;
        mask = mask >> (32 - amount);
        uint32_t cnt = arr->size - 1;
        for (pos = cnt; pos != -1; --pos)
        {
            uint64_t tmp = arr->data[pos];
            arr->data[pos] = (uint32_t)((tmp >> amount) | overflow);
            overflow = (tmp & mask) << (32 - amount);
        }
        //Remove last item if it's empty.
        while (arr->size != 1 && arr->data[cnt] == 0)
        {
            --arr->size;
            --cnt;
        }
    }
}

int bi_clear(bigInteger* arr)
{
    if (arr == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    arr->size = 0;
    arr->isNegative = 0;
    return 0;
}

int bi_copy(bigInteger* target, bigInteger* source)
{
    if (target->capacity < source->capacity)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    memcpy(target->data, source->data, sizeof(uint32_t) * source->size);
    target->size = source->size;
    target->isNegative = source->isNegative;
    return 0;
}

void bi_pow(bigInteger* arr, uint32_t exponent)
{
    if (exponent != 1)
    {
        uint32_t pos = 1;
        bigInteger tmp;
        bi_copy(&tmp, arr);
        while (pos <= exponent / 2)
        {
            bi_multiply(arr, arr);
            pos <<= 1;
        }
        while (pos < exponent)
        {
            bi_multiply(arr, &tmp);
            ++pos;
        }
    }
}

void bi_div(bigInteger* arr, bigInteger* value)
{
    bigInteger current;
    bi_init(&current);
    bi_append(&current, 1);
    bigInteger denom;
    bi_init(&denom);
    bi_copy(&denom, value);
    bigInteger tmp;
    bi_init(&tmp);
    bi_copy(&tmp, arr);
    unsigned char neq = arr->isNegative;
    arr->isNegative = 0;
    // while denom < this.
    while (bi_compare(&denom, arr) == -1)
    {
        uint16_t bits = bi_getUsedBits(arr) - bi_getUsedBits(&denom);
        if (bits != 0)
        {
            bi_lshift(&current, bits);
            bi_lshift(&denom, bits);
        }
        else
        {
            bi_lshift(&current, 1);
            bi_lshift(&denom, 1);
        }
    }
    //If overflow.
    if (bi_compare(&denom, arr) == 1)
    {
        if (bi_isOne(&current))
        {
            bi_clear(arr);
            return;
        }
        bi_clear(arr);
        bi_rshift(&current, 1);
        bi_rshift(&denom, 1);
        while (!bi_isZero(&current))
        {
            uint32_t r = bi_compare(&tmp, &denom);
            if (r == 1)
            {
                bi_sub(&tmp, &denom);
                bi_addBigInteger(arr, &current);
            }
            else if (r == 0)
            {
                bi_addBigInteger(arr, &current);
                break;
            }
            uint16_t bits = bi_getUsedBits(&current);
            bi_rshift(&current, bits);
            bi_rshift(&denom, bits);
        }
        memcpy(current.data, arr->data, sizeof(uint32_t) * arr->size);
    }
    arr->isNegative = neq;
    memcpy(arr->data, current.data, sizeof(uint32_t) * arr->size);
}

int bi_mod(bigInteger* arr, bigInteger* mod)
{
    bigInteger current;
    bi_init(&current);
    bi_append(&current, 1);
    bigInteger denom;
    bi_init(&denom);
    bi_copy(&denom, mod);
    unsigned char neq = arr->isNegative;
    arr->isNegative = 0;
    // while denom < this.
    while (bi_compare(&denom, arr) == -1)
    {
        uint16_t bits = bi_getUsedBits(arr) - bi_getUsedBits(&denom);
        bi_lshift(&current, bits);
        bi_lshift(&denom, bits);
    }
    //If overflow.
    if (bi_compare(&denom, arr) == 1)
    {
        if (bi_isOne(&current))
        {
            if (neq)
            {
                bi_sub(arr, mod);
                arr->isNegative = 0;
            }
            return 0;
        }
        bi_rshift(&current, 1);
        bi_rshift(&denom, 1);
        while (!bi_isZero(&current))
        {
            uint32_t r = bi_compare(arr, &denom);
            if (r == 1)
            {
                bi_sub(arr, &denom);
            }
            else if (r == 0)
            {
                break;
            }
            bi_rshift(&current, 1);
            bi_rshift(&denom, 1);
        }
    }
    else
    {
        bi_clear(arr);
    }
    if (neq)
    {
        bi_sub(arr, mod);
        arr->isNegative = 0;
    }
    return 0;
}

int bi_inv(bigInteger* arr, bigInteger* value)
{
    if (!bi_isZero(arr))
    {
        bigInteger lm, hm, low, high, r, tmp, nm;
        bi_init(&lm);
        bi_init(&hm);
        bi_init(&low);
        bi_init(&high);
        bi_init(&r);
        bi_init(&tmp);
        bi_init(&nm);
        bi_append(&lm, 1);
        bi_append(&hm, 0);
        bi_copy(&low, arr);
        bi_mod(&low, value);
        bi_copy(&high, value);
        while (!(bi_isZero(&low) || bi_isOne(&low)))
        {
            bi_copy(&r, &high);
            bi_div(&r, &low);
            bi_copy(&tmp, &lm);
            bi_multiply(&tmp, &r);
            bi_copy(&nm, &hm);
            bi_sub(&nm, &tmp);
            bi_copy(&tmp, &low);
            bi_multiply(&tmp, &r);
            bi_sub(&high, &tmp);

            bi_copy(&tmp, &low);
            bi_copy(&low, &high);
            bi_copy(&high, &tmp);
            bi_copy(&hm, &lm);
            bi_copy(&lm, &nm);
        }
        memcpy(arr->data, lm.data, sizeof(uint32_t) * lm.size);
        arr->size = lm.size;
        arr->isNegative = lm.isNegative;
        bi_mod(arr, value);
    }
    return 0;
}

uint16_t bi_getUsedBits(bigInteger* bi)
{
    uint16_t count = (uint16_t)(32 * bi->size);
    if (bi->size != 0)
    {
        uint32_t tmp = bi->buff[bi->size - 1];
        uint32_t mask = 0x80000000;
        while ((tmp & mask) == 0)
        {
            mask >>= 1;
            --count;
        }
    }
    return count;
}

unsigned char bi_isBitSet(bigInteger* bi, uint16_t index)
{
    uint16_t pos = (uint16_t)(index / 32);
    if (index > 32)
    {
        index -= (uint16_t)(32 * index);
    }
    return (bi->buff[pos] & (1 << index)) != 0;
}