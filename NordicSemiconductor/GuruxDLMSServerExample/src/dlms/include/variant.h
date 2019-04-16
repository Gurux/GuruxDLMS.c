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

#ifndef VARIANT_H
#define VARIANT_H

#ifdef  __cplusplus
extern "C" {
#endif
#include "gxignore.h"
#include "date.h" 
#include "enums.h"
#include "errorcodes.h"
#include "bytebuffer.h"
#include "bitarray.h"
#if _MSC_VER > 1400
#pragma warning(disable : 4201)
#endif

#define VECTOR_ARRAY_CAPACITY 10

    typedef struct
    {
        void** data;
        unsigned short capacity;
        unsigned short size;
        unsigned short position;
    } variantArray;

    typedef struct tagdlmsVARIANT
    {
        DLMS_DATA_TYPE vt;
        union
        {
            unsigned char bVal;
            signed char cVal;
            short iVal;
            long lVal;
            long long llVal;
            float fltVal;
            double dblVal;
            unsigned char boolVal;
            unsigned short uiVal;
            unsigned long ulVal;
            unsigned long long ullVal;
            gxByteBuffer *strVal;
            bitArray *bitArr;
            gxByteBuffer *byteArr;
            gxByteBuffer *strUtfVal;
            gxtime* dateTime;
            variantArray *Arr; //<CGXDLMSVariant>
        };
    } dlmsVARIANT;

    //Initialize variantArray.
    void va_init(
        variantArray *arr);

    void va_attach(
        variantArray *trg,
        variantArray *src);

    //Is variant array attached.
    char va_isAttached(variantArray* arr);

    ///Get variant array capacity.
    unsigned short va_getCapacity(
        variantArray* arr);


    //Allocate new size for the array in bytes.
    void va_capacity(
        variantArray *arr,
        unsigned short capacity);

    //Push new data to the variantArray.
    int va_push(
        variantArray *arr,
        dlmsVARIANT *item);

    void va_clear(
        variantArray *arr);

    //Set byte value to variant.
    int var_setUInt8(
        dlmsVARIANT *data,
        unsigned char value);

    //Set UInt16 value to variant.
    int var_setUInt16(
        dlmsVARIANT *data,
        unsigned short value);

    //Set UInt32 value to variant.
    int var_setUInt32(dlmsVARIANT
        *data,
        unsigned long value);

    //Set UInt64 value to variant.
    int var_setUInt64(
        dlmsVARIANT *data,
        unsigned long long value);

    //Set signed byte value to variant.
    int var_setInt8(
        dlmsVARIANT *data,
        char value);

    //Set Int16 value to variant.
    int var_setInt16(
        dlmsVARIANT *data,
        short value);

    //Set Int32 value to variant.
    int var_setInt32(
        dlmsVARIANT *data,
        long value);

    //Set Int64 value to variant.
    int var_setInt64(
        dlmsVARIANT *data,
        long long value);

    int var_setDouble(
        dlmsVARIANT *data,
        double value);

    int var_setDateTime(
        dlmsVARIANT *target,
        gxtime* value);

    int var_setDate(
        dlmsVARIANT *target,
        gxtime* value);

    int var_setTime(
        dlmsVARIANT *target,
        gxtime* value);

    int var_setBoolean(
        dlmsVARIANT *target,
        char value);

    int var_getDateTime(
        dlmsVARIANT *target,
        gxtime* value);

    int var_getDateTime2(
        gxtime *dateTime,
        gxByteBuffer* ba);

    int var_getDate(
        gxtime * date,
        gxByteBuffer* ba);

    int var_getTime(
        gxtime * date,
        gxByteBuffer* ba);

    int var_setDateTimeAsOctetString(
        dlmsVARIANT *target,
        gxtime* value);

    int var_setDateAsOctetString(
        dlmsVARIANT *target,
        gxtime* value);

    int var_setTimeAsOctetString(
        dlmsVARIANT *target,
        gxtime* value);

    //Get byte value from variant.
    int var_getUInt8(
        dlmsVARIANT *data,
        unsigned char* value);

    //Get UInt16 value from variant.
    int var_getUInt16(
        dlmsVARIANT *data,
        unsigned short* value);

    //Get UInt32 value from variant.
    int var_getUInt32(
        dlmsVARIANT *data,
        unsigned long* value);

    //Get UInt64 value from variant.
    int var_getUInt64(
        dlmsVARIANT *data,
        unsigned long long* value);

    //Get signed byte value from variant.
    int var_getInt8(
        dlmsVARIANT *data,
        char* value);

    //Get Int16 value from variant.
    int var_getInt16(
        dlmsVARIANT *data,
        short* value);

    //Get Int32 value from variant.
    int var_getInt32(
        dlmsVARIANT *data,
        long* value);

    //Get Int64 value from variant.
    int var_getInt64(
        dlmsVARIANT *data,
        long long* value);

    int var_addBytes(
        dlmsVARIANT *data,
        const unsigned char* value,
        unsigned short count);

    int var_setString(
        dlmsVARIANT *data,
        const char* value,
        unsigned short count);

    void var_attach(
        dlmsVARIANT *target,
        gxByteBuffer * source);

    int var_addOctetString(
        dlmsVARIANT *data,
        gxByteBuffer* ba);

    int var_setEnum(dlmsVARIANT *data,
        unsigned char value);

    int var_addByteArray(
        dlmsVARIANT *data,
        gxByteBuffer* ba,
        unsigned short index,
        unsigned short count);

    //Initialize variant.
    int var_init(
        dlmsVARIANT *data);

    //attach static array.
    void var_attachArray(
        dlmsVARIANT * data,
        const dlmsVARIANT** arr,
        const unsigned short count);

    //attach static structure.
    void var_attachStructure(
        dlmsVARIANT * data,
        const dlmsVARIANT** arr,
        const unsigned short count);

    //copy variant.
    int var_copy(
        dlmsVARIANT *value,
        dlmsVARIANT *copy);

    //Clear variant.
    int var_clear(
        dlmsVARIANT *data);

    //Get bytes from variant value.
    int var_getBytes(
        dlmsVARIANT *data,
        gxByteBuffer* ba);

    //Get bytes from variant value.
    int var_getBytes2(
        dlmsVARIANT *data,
        DLMS_DATA_TYPE type,
        gxByteBuffer* ba);

    //Get size in bytes.
    int var_getSize(
        DLMS_DATA_TYPE vt);

    //Convert variant value to integer.
    int var_toInteger(
        dlmsVARIANT *data);

    //Get item from variant array.
    int va_get(
        variantArray* arr,
        dlmsVARIANT ** item);

    //Get item from variant array by index.
    int va_getByIndex(
        variantArray* arr,
        int index,
        dlmsVARIANT ** item);

    //copy variant array.
    int va_copyArray(
        variantArray* target,
        variantArray* source);

    int var_changeType(
        dlmsVARIANT *value,
        DLMS_DATA_TYPE newType);

    //Convert variant to string.
    //Note! toString do not clear existing byte array.
    int var_toString(
        dlmsVARIANT *item,
        gxByteBuffer* value);

    //Convert variant array to string.
    //Note! toString do not clear existing byte array.
    int va_toString(
        variantArray *items,
        gxByteBuffer* ba);

    //Convert variant to double.
    double var_toDouble(
        dlmsVARIANT *target);

#ifndef GX_DLMS_MICROCONTROLLER
    //Print content of the variant to cout.
    int var_print(
        //Format.
        const char* format,
        dlmsVARIANT *target);

    //Print content of the variant array to cout.
    int va_print(
        variantArray *target);

#endif //GX_DLMS_MICROCONTROLLER
    
#ifdef  __cplusplus
}
#endif

#endif //VARIANT_H
