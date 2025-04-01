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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#define GX_SWAP_UINT16(a)(((a & 0xFF) << 8) | ((a & 0xFF00) >> 8))
#define GX_SWAP_UINT32(a)(GX_SWAP_UINT16(a & 0xFFFF) << 16) | (GX_SWAP_UINT16(a >> 16) )

#define VARIANT_ARRAY_CAPACITY 10

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#define VERIFY(X, TYPE) (assert(X.vt == TYPE))
#else
#define VERIFY(X, TYPE)
#endif

#define V_VT(X)         ((X)->vt)
#define GX_UNION(X, Y, Z) V_VT(X)=Z;(X)->Y
#ifdef DLMS_IGNORE_MALLOC
#define GX_UNION2(X, Y, Z, S, C)  (X)->size=S;(X)->capacity=C;V_VT(X)=Z;(X)->Y
#endif //DLMS_IGNORE_MALLOC
#define GX_UINT8(X) GX_UNION(&X, bVal, DLMS_DATA_TYPE_UINT8)
#define GX_UINT16(X) GX_UNION(&X, uiVal, DLMS_DATA_TYPE_UINT16)
#define GX_UINT32(X) GX_UNION(&X, ulVal, DLMS_DATA_TYPE_UINT32)
#define GX_UINT64(X) GX_UNION(&X, ullVal, DLMS_DATA_TYPE_UINT64)
#define GX_INT8(X) GX_UNION(&X, cVal, DLMS_DATA_TYPE_INT8)
#define GX_INT16(X) GX_UNION(&X, iVal, DLMS_DATA_TYPE_INT16)
#define GX_INT32(X) GX_UNION(&X, lVal, DLMS_DATA_TYPE_INT32)
#define GX_INT64(X) GX_UNION(&X, llVal, DLMS_DATA_TYPE_INT64)
#define GX_FLOAT(X) GX_UNION(&X, fltVal, DLMS_DATA_TYPE_FLOAT32)
#define GX_DOUBLE(X) GX_UNION(&X, dblVal, DLMS_DATA_TYPE_FLOAT64)
#define GX_BOOL(X) GX_UNION(&X, bVal, DLMS_DATA_TYPE_BOOLEAN)
#define GX_ENUM(X) GX_UNION(&X, cVal, DLMS_DATA_TYPE_ENUM)
#ifdef DLMS_IGNORE_MALLOC
#define GX_DATETIME(X) X.size = 12; GX_UNION(&X, pVal, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATETIME))
#define GX_DATE(X) X.size = 5; GX_UNION(&X, pVal, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATE))
#define GX_TIME(X) X.size = 4; GX_UNION(&X, pVal, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_TIME))
#else
#define GX_DATETIME(X) GX_UNION(&X, pVal, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATETIME))
#define GX_DATE(X) GX_UNION(&X, pVal, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATE))
#define GX_TIME(X) GX_UNION(&X, pVal, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_TIME))
#endif //DLMS_IGNORE_MALLOC
#define GX_UINT8_BYREF(X, VALUE_) GX_UNION(&X, pbVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_UINT8))
#define GX_UINT16_BYREF(X, VALUE_) GX_UNION(&X, puiVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_UINT16))
#define GX_UINT32_BYREF(X, VALUE_) GX_UNION(&X, pulVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_UINT32))
#define GX_UINT64_BYREF(X, VALUE_) GX_UNION(&X, pullVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_UINT64))
#define GX_INT8_BYREF(X, VALUE_) GX_UNION(&X, pcVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_INT8))
#define GX_INT16_BYREF(X, VALUE_) GX_UNION(&X, piVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_INT16))
#define GX_INT32_BYREF(X, VALUE_) GX_UNION(&X, plVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_INT32))
#define GX_INT64_BYREF(X, VALUE_) GX_UNION(&X, pllVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_INT64))
#define GX_FLOAT_BYREF(X, VALUE_) GX_UNION(&X, pfltVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_FLOAT32))
#define GX_DOUBLE_BYREF(X, VALUE_) GX_UNION(&X, pdblVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_FLOAT64))

#ifdef DLMS_IGNORE_MALLOC
#define GX_OCTET_STRING(X, VALUE_, SIZE_) GX_UNION2(&X, pVal = VALUE_, (DLMS_DATA_TYPE) (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_OCTET_STRING), SIZE_, sizeof(VALUE_))
#define GX_BIT_STRING(X, VALUE_) GX_UNION(&X, pVal = &VALUE_, (DLMS_DATA_TYPE) (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_BIT_STRING))
#define GX_STRING(X, VALUE_, SIZE_) GX_UNION2(&X, pVal = VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_STRING), SIZE_, sizeof(VALUE_))
#define GX_ARRAY(X, VALUE_) GX_UNION2(&X, pVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_ARRAY), 0, 0)
#define GX_STRUCT(X, VALUE_) GX_UNION2(&X, pVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_STRUCTURE), 0, 0)
#endif //DLMS_IGNORE_MALLOC

#define GX_BOOL_BYREF(X, VALUE_) GX_UNION(&X, pcVal = &VALUE_, (DLMS_DATA_TYPE)(DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_BOOLEAN))

    /*Get UInt8 value from variant.*/
#define GX_GET_UINT8(X)  (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.bVal : *X.pbVal

/*Get UInt16 value from variant.*/
#define GX_GET_UINT16(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.uiVal : *X.puiVal

/*Get UInt32 value from variant.*/
#define GX_GET_UINT32(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.culVal : *X.pulVal

/*Get UInt64 value from variant.*/
#define GX_GET_UINT64(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.cullVal : *X.pullVal

/*Get Int8 value from variant.*/
#define GX_GET_INT8(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.cVal : *X.pcVal
/*Get Int16 value from variant.*/
#define GX_GET_INT16(X)(X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.iVal : *X.piVal
/*Get Int32 value from variant.*/
#define GX_GET_INT32(X)(X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.lVal : *X.plVal
/*Get Int64 value from variant.*/
#define GX_GET_INT64(X)(X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.llVal : *X.pllVal
/*Get float value from variant.*/
#define GX_GET_FLOAT(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.fltVal : *X.pfltVal
/*Get double value from variant.*/
#define GX_GET_DOUBLE(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? X.dblVal : *X.pdblVal
/*Get boolean value from variant.*/
#define GX_GET_BOOL(X) (X.vt & DLMS_DATA_TYPE_BYREF) == 0 ? (X.bVal != 0) : (*X.pbVal != 0)

#define GX_ADD(X, VALUE_) GX_UNION(&X, pbVal += VALUE_, X.vt)

    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        void* data;
#else
        void** data;
#endif //DLMS_IGNORE_MALLOC
        uint16_t capacity;
        uint16_t size;
    } variantArray;

    typedef struct tagdlmsVARIANT
    {
        DLMS_DATA_TYPE vt;
        union
        {
            unsigned char bVal;
            signed char cVal;
            int16_t iVal;
            int32_t lVal;
            int64_t llVal;
#ifndef DLMS_IGNORE_FLOAT32
            float fltVal;
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
            double dblVal;
#endif //DLMS_IGNORE_FLOAT64
            unsigned char boolVal;
            uint16_t uiVal;
            uint32_t ulVal;
            uint64_t ullVal;
#ifndef DLMS_IGNORE_MALLOC
            gxtime* dateTime;
            gxByteBuffer* strVal;
            gxByteBuffer* strUtfVal;
#endif //DLMS_IGNORE_MALLOC
            variantArray* Arr;
            gxByteBuffer* byteArr;
            bitArray* bitArr;
            unsigned char* pbVal;
            signed char* pcVal;
            int16_t* piVal;
            int32_t* plVal;
            int64_t* pllVal;
#ifndef DLMS_IGNORE_FLOAT32
            float* pfltVal;
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
            double* pdblVal;
#endif //DLMS_IGNORE_FLOAT64
            unsigned char* pboolVal;
            uint16_t* puiVal;
            uint32_t* pulVal;
            uint64_t* pullVal;
            void* pVal;
        };
#ifdef DLMS_IGNORE_MALLOC
        uint16_t size;
        uint16_t capacity;
#endif //DLMS_IGNORE_MALLOC
    } dlmsVARIANT;


#ifdef DLMS_IGNORE_MALLOC
    typedef dlmsVARIANT* dlmsVARIANT_PTR;
#else
    typedef dlmsVARIANT* dlmsVARIANT_PTR;
#endif //DLMS_IGNORE_MALLOC

    //Initialize variantArray.
    void va_init(
        variantArray* arr);

#ifdef DLMS_IGNORE_MALLOC
    void va_attach(
        variantArray* trg,
        dlmsVARIANT* src,
        uint16_t size,
        uint16_t capacity);
#endif //DLMS_IGNORE_MALLOC

    void va_attach2(
        variantArray* trg,
        variantArray* src);

    //Is variant array attached.
    char va_isAttached(variantArray* arr);

    //Get variant array capacity.
    uint16_t va_size(
        variantArray* arr);

    //Get variant array capacity.
    uint16_t va_getCapacity(
        variantArray* arr);


    //Allocate new size for the array in bytes.
    int va_capacity(
        variantArray* arr,
        uint16_t capacity);

    //Push new data to the variantArray.
    int va_push(
        variantArray* arr,
        dlmsVARIANT* item);

    void va_clear(
        variantArray* arr);

    //Set byte value to variant.
    int var_setUInt8(
        dlmsVARIANT* data,
        unsigned char value);

    //Set UInt16 value to variant.
    int var_setUInt16(
        dlmsVARIANT* data,
        uint16_t value);

    //Set UInt32 value to variant.
    int var_setUInt32(dlmsVARIANT
        * data,
        uint32_t value);

    //Set UInt64 value to variant.
    int var_setUInt64(
        dlmsVARIANT* data,
        uint64_t value);

    //Set signed byte value to variant.
    int var_setInt8(
        dlmsVARIANT* data,
        char value);

    //Set Int16 value to variant.
    int var_setInt16(
        dlmsVARIANT* data,
        short value);

    //Set Int32 value to variant.
    int var_setInt32(
        dlmsVARIANT* data,
        int32_t value);

    //Set Int64 value to variant.
    int var_setInt64(
        dlmsVARIANT* data,
        int64_t value);

#ifndef DLMS_IGNORE_FLOAT64
    int var_setDouble(
        dlmsVARIANT* data,
        double value);
#endif //DLMS_IGNORE_FLOAT64

#ifndef DLMS_IGNORE_MALLOC
    int var_setDateTime(
        dlmsVARIANT* target,
        gxtime* value);

    int var_setDate(
        dlmsVARIANT* target,
        gxtime* value);

    int var_setTime(
        dlmsVARIANT* target,
        gxtime* value);

#endif //DLMS_IGNORE_MALLOC
    int var_setBoolean(
        dlmsVARIANT* target,
        char value);

    int var_getDateTime(
        dlmsVARIANT* target,
        gxtime* value);

    int var_getDateTime2(
        gxtime* dateTime,
        gxByteBuffer* ba);

    int var_getDate(
        gxtime* date,
        gxByteBuffer* ba);

    int var_getTime(
        gxtime* date,
        gxByteBuffer* ba);

    int var_setDateTimeAsOctetString(
        dlmsVARIANT* target,
        gxtime* value);

    int var_setDateAsOctetString(
        dlmsVARIANT* target,
        gxtime* value);

    int var_setTimeAsOctetString(
        dlmsVARIANT* target,
        gxtime* value);

    //Get byte value from variant.
    int var_getUInt8(
        dlmsVARIANT* data,
        unsigned char* value);

    //Get UInt16 value from variant.
    int var_getUInt16(
        dlmsVARIANT* data,
        uint16_t* value);

    //Get UInt32 value from variant.
    int var_getUInt32(
        dlmsVARIANT* data,
        uint32_t* value);

    //Get UInt64 value from variant.
    int var_getUInt64(
        dlmsVARIANT* data,
        uint64_t* value);

    //Get signed byte value from variant.
    int var_getInt8(
        dlmsVARIANT* data,
        char* value);

    //Get Int16 value from variant.
    int var_getInt16(
        dlmsVARIANT* data,
        short* value);

    //Get Int32 value from variant.
    int var_getInt32(
        dlmsVARIANT* data,
        int32_t* value);

    //Get Int64 value from variant.
    int var_getInt64(
        dlmsVARIANT* data,
        int64_t* value);

#ifndef DLMS_IGNORE_MALLOC
    int var_addBytes(
        dlmsVARIANT* data,
        const unsigned char* value,
        uint16_t count);
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
    int var_setString(
        dlmsVARIANT* data,
        const char* value,
        uint16_t count);
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
    void var_attach(
        dlmsVARIANT* target,
        gxByteBuffer* source);
#endif //DLMS_IGNORE_MALLOC

    int var_addOctetString(
        dlmsVARIANT* data,
        gxByteBuffer* ba);

    int var_setEnum(dlmsVARIANT* data,
        unsigned char value);

    int var_addByteArray(
        dlmsVARIANT* data,
        gxByteBuffer* ba,
        uint16_t index,
        uint16_t count);

    //Initialize variant.
    int var_init(
        dlmsVARIANT* data);

#ifndef DLMS_IGNORE_MALLOC
    //attach static array.
    void var_attachArray(
        dlmsVARIANT* data,
        const variantArray* arr,
        const uint16_t count);
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
    //attach static structure.
    void var_attachStructure(
        dlmsVARIANT* data,
        const dlmsVARIANT** arr,
        const uint16_t count);
#endif //DLMS_IGNORE_MALLOC

    //copy variant.
    int var_copy(
        dlmsVARIANT* target,
        dlmsVARIANT* source);

    //Clear variant.
    int var_clear(
        dlmsVARIANT* data);

    //Get bytes from variant value.
    int var_getBytes(
        dlmsVARIANT* data,
        gxByteBuffer* ba);

    //Get bytes from variant value.
    int var_getBytes2(
        dlmsVARIANT* data,
        DLMS_DATA_TYPE type,
        gxByteBuffer* ba);

    //Get bytes from variant value without data type.
    int var_getBytes3(
        dlmsVARIANT* data,
        DLMS_DATA_TYPE type,
        gxByteBuffer* ba,
        unsigned char addType);

    //Get bytes from variant value.
    int var_getBytes4(
        dlmsVARIANT* data,
        DLMS_DATA_TYPE type,
        gxByteBuffer* ba,
        unsigned char addType,
        unsigned char addArraySize,
        unsigned char addStructureSize);

    //Get size in bytes.
    int var_getSize(
        DLMS_DATA_TYPE vt);

    //Convert variant value to integer.
    int var_toInteger(
        dlmsVARIANT* data);

    //Get item from variant array by index.
    int va_getByIndex(
        variantArray* arr,
        int index,
        dlmsVARIANT_PTR* item);

#ifndef DLMS_IGNORE_MALLOC
    //copy variant array.
    int va_copyArray(
        variantArray* target,
        variantArray* source);

    //Add given value to the array N times.
    int va_addValue(
        variantArray* target,
        dlmsVARIANT* value,
        uint16_t count);
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
    int var_changeType(
        dlmsVARIANT* value,
        DLMS_DATA_TYPE newType);
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
    //Convert variant to string.
    //Note! toString do not clear existing byte array.
    int var_toString(
        dlmsVARIANT* item,
        gxByteBuffer* value);

    //Convert variant array to string.
    //Note! toString do not clear existing byte array.
    int va_toString(
        variantArray* items,
        gxByteBuffer* ba);

#endif //DLMS_IGNORE_MALLOC
    //Convert variant to double.
    double var_toDouble(
        dlmsVARIANT* target);

#ifdef DLMS_IGNORE_MALLOC
    //Attach value by ref.
    int var_byRef(
        dlmsVARIANT* target,
        DLMS_DATA_TYPE type,
        void* value);
#endif //DLMS_IGNORE_MALLOC

#ifndef GX_DLMS_MICROCONTROLLER
    //Print content of the variant to cout.
    int var_print(
        //Format.
        const char* format,
        dlmsVARIANT* target);

    //Print content of the variant array to cout.
    int va_print(
        variantArray* target);

#endif //GX_DLMS_MICROCONTROLLER

#ifndef DLMS_IGNORE_DELTA
    //Set byte delta value to variant.
    int var_setDeltaUInt8(
        dlmsVARIANT* data,
        unsigned char value);

    //Set UInt16 delta value to variant.
    int var_setDeltaUInt16(
        dlmsVARIANT* data,
        uint16_t value);

    //Set UInt32 delta value to variant.
    int var_setDeltaUInt32(dlmsVARIANT
        * data,
        uint32_t value);

    //Set signed byte delta value to variant.
    int var_setDeltaInt8(
        dlmsVARIANT* data,
        char value);

    //Set Int16 delta value to variant.
    int var_setDeltaInt16(
        dlmsVARIANT* data,
        short value);

    //Set Int32 delta value to variant.
    int var_setDeltaInt32(
        dlmsVARIANT* data,
        int32_t value);

    //Get byte value from variant.
    int var_getDeltaUInt8(
        dlmsVARIANT* data,
        unsigned char* value);

    //Get UInt16 value from variant.
    int var_getDeltaUInt16(
        dlmsVARIANT* data,
        uint16_t* value);

    //Get UInt32 value from variant.
    int var_getDeltaUInt32(
        dlmsVARIANT* data,
        uint32_t* value);

    //Get signed byte value from variant.
    int var_getDeltaInt8(
        dlmsVARIANT* data,
        char* value);

    //Get Int16 value from variant.
    int var_getDeltaInt16(
        dlmsVARIANT* data,
        short* value);

    //Get Int32 value from variant.
    int var_getDeltaInt32(
        dlmsVARIANT* data,
        int32_t* value);
#endif //DLMS_IGNORE_DELTA
#define VA_ATTACH(X, V, S) va_attach(&X, V, S, sizeof(V)/sizeof(V[0]))

#ifdef  __cplusplus
}
#endif

#endif //VARIANT_H
