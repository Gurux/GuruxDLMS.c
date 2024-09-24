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

#ifndef COSEM_H
#define COSEM_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxobjects.h"
#include "dlmssettings.h"

#ifndef DLMS_IGNORE_MALLOC
  int cosem_createObject(
      DLMS_OBJECT_TYPE type,
      gxObject** object);

  int cosem_createObject2(
      DLMS_OBJECT_TYPE type,
      const char* ln,
      gxObject** object);
#endif //DLMS_IGNORE_MALLOC

  int cosem_setLogicalName(
      gxObject* object,
      const unsigned char* value);

#ifndef DLMS_IGNORE_MALLOC
  int cosem_init(
      gxObject* object,
      DLMS_OBJECT_TYPE type,
      const char* logicalNameString);
#endif //DLMS_IGNORE_MALLOC

  int cosem_init2(
    gxObject* object,
    DLMS_OBJECT_TYPE type,
    const unsigned char* ln);

  //This initialize method will also check the size of the object type and compare it with the expected size.
  int cosem_init3(
      gxObject* object,
      const uint16_t expectedSize,
      DLMS_OBJECT_TYPE type,
      const unsigned char* ln);

  //This initialize method will also check the size of the object type and compare it with the expected size.
  int cosem_init4(
      void* object,
      const uint16_t expectedSize,
      DLMS_OBJECT_TYPE type,
      const unsigned char* ln);

  int cosem_checkStructure(gxByteBuffer* bb, uint16_t expectedItemCount);

  int cosem_getStructure(gxByteBuffer* bb, uint16_t* count);

  int cosem_checkArray(gxByteBuffer* bb, uint16_t* count);

  int cosem_checkArray2(gxByteBuffer* bb, uint16_t* count, unsigned char checkDataType);

  int cosem_getUInt8(gxByteBuffer* bb, unsigned char* value);

  int cosem_getUInt16(gxByteBuffer* bb, uint16_t* value);

  int cosem_getUInt32(gxByteBuffer* bb, uint32_t* value);

  int cosem_getInt8(gxByteBuffer* bb, signed char* value);

  int cosem_getInt16(gxByteBuffer* bb, int16_t* value);

  int cosem_getInt32(gxByteBuffer* bb, int32_t* value);

  int cosem_getOctetString(gxByteBuffer* bb, gxByteBuffer* value);


  int cosem_getString(gxByteBuffer* bb, gxByteBuffer* value);

  int cosem_getOctetString2(gxByteBuffer* bb, unsigned char* value, uint16_t capacity, uint16_t* size);

  int cosem_getOctetString3(gxByteBuffer* bb, gxByteBuffer* value, unsigned char exact);

  int cosem_getString2(gxByteBuffer* bb, char* value, uint16_t capacity);

  //Get date-time value from the octet-string.
  int cosem_getDateTimeFromOctetString(gxByteBuffer* bb, gxtime* value);

  //Get date-time value from the octet-string.
  int cosem_getDateFromOctetString(gxByteBuffer* bb, gxtime* value);

  //Get date-time value from the octet-string.
  int cosem_getTimeFromOctetString(gxByteBuffer* bb, gxtime* value);

  //Get date-time value from the octet-string.
  int cosem_getDateTimeFromOctetString2(gxByteBuffer* bb, gxtime* value, unsigned char checkDataType);

  //Get date-time value from the octet-string.
  int cosem_getDateFromOctetString2(gxByteBuffer* bb, gxtime* value, unsigned char checkDataType);

  //Get date-time value from the octet-string.
  int cosem_getTimeFromOctetString2(gxByteBuffer* bb, gxtime* value, unsigned char checkDataType);

  //Get date-time value from the byte array.
  int cosem_getDateTime(gxByteBuffer* bb, gxtime* value);

  //Get date value from the byte array.
  int cosem_getDate(gxByteBuffer* bb, gxtime* value);

  //Get time value from the byte array.
  int cosem_getTime(gxByteBuffer* bb, gxtime* value);

  //Get date-time value from the byte array.
  int cosem_getDateTime2(gxByteBuffer* bb, gxtime* value, unsigned char checkDataType);

  //Get date value from the byte array.
  int cosem_getDate2(gxByteBuffer* bb, gxtime* value, unsigned char checkDataType);

  //Get time value from the byte array.
  int cosem_getTime2(gxByteBuffer* bb, gxtime* value, unsigned char checkDataType);

  //Get get bit string from the byte array.
  int cosem_getBitString(gxByteBuffer* bb, bitArray* value);
  //Get get bit string from the byte array.
  int cosem_getBitString2(gxByteBuffer* bb, unsigned char* value, uint16_t capacity, uint16_t* size);

  //Get integer value from bit string.
  int cosem_getIntegerFromBitString(gxByteBuffer* bb, uint32_t* value);

  int cosem_getVariant(gxByteBuffer* bb, dlmsVARIANT* value);
  
  /*Updated variant data type must be the same as the existing value.
  E.g. limiter uses this.*/
  int cosem_getVariantExact(gxByteBuffer* bb, dlmsVARIANT* value);

  int cosem_getEnum(gxByteBuffer* bb, unsigned char* value);

  int cosem_getBoolean(gxByteBuffer* bb, unsigned char* value);

  int cosem_getUtf8String(gxByteBuffer* bb, gxByteBuffer* value);

  int cosem_getUtf8String2(gxByteBuffer* bb, char* value, uint16_t capacity, uint16_t* size);

  int cosem_setDateTimeAsOctetString(gxByteBuffer* bb, gxtime* value);

  int cosem_setDateAsOctetString(gxByteBuffer* bb, gxtime* value);

  int cosem_setBitString(gxByteBuffer* bb, uint32_t value, uint16_t count);

  int cosem_setTimeAsOctetString(gxByteBuffer* bb, gxtime* value);

  int cosem_setDateTime(gxByteBuffer* bb, gxtime* value);

  int cosem_setDate(gxByteBuffer* bb, gxtime* value);

  int cosem_setTime(gxByteBuffer* bb, gxtime* value);

  int cosem_setOctetString(gxByteBuffer* bb, gxByteBuffer* value);

  int cosem_setString(gxByteBuffer* bb, const char* value, uint16_t len);

  int cosem_setString2(gxByteBuffer* bb, gxByteBuffer* value);

  int cosem_setOctetString2(
      gxByteBuffer* bb,
      const unsigned char* value,
      uint16_t size);

  int cosem_setUInt8(gxByteBuffer* bb, unsigned char value);
  int cosem_setUInt16(gxByteBuffer* bb, uint16_t value);
  int cosem_setUInt32(gxByteBuffer* bb, uint32_t value);
  int cosem_setUInt64(gxByteBuffer* bb, uint64_t* value);

  int cosem_setInt8(gxByteBuffer* bb, char value);
  int cosem_setInt16(gxByteBuffer* bb, int16_t value);
  int cosem_setInt32(gxByteBuffer* bb, int32_t value);
  int cosem_setInt64(gxByteBuffer* bb, int64_t* value);
  int cosem_setVariant(gxByteBuffer* bb, dlmsVARIANT* value);

  int cosem_setStructure(gxByteBuffer* bb, uint16_t count);
  int cosem_setArray(gxByteBuffer* bb, uint16_t count);
  int cosem_setEnum(gxByteBuffer* bb, unsigned char value);
  int cosem_setBoolean(gxByteBuffer* bb, unsigned char value);
#ifndef DLMS_IGNORE_DELTA
  int cosem_setDeltaInt8(gxByteBuffer* bb, char value);
  int cosem_setDeltaInt16(gxByteBuffer* bb, int16_t value);
  int cosem_setDeltaInt32(gxByteBuffer* bb, int32_t value);
  int cosem_setDeltaUInt8(gxByteBuffer* bb, unsigned char value);
  int cosem_setDeltaUInt16(gxByteBuffer* bb, uint16_t value);
  int cosem_setDeltaUInt32(gxByteBuffer* bb, uint32_t value);

  int cosem_getDeltaUInt8(gxByteBuffer* bb, unsigned char* value);
  int cosem_getDeltaUInt16(gxByteBuffer* bb, uint16_t* value);
  int cosem_getDeltaUInt32(gxByteBuffer* bb, uint32_t* value);
  int cosem_getDeltaInt8(gxByteBuffer* bb, signed char* value);
  int cosem_getDeltaInt16(gxByteBuffer* bb, int16_t* value);
  int cosem_getDeltaInt32(gxByteBuffer* bb, int32_t* value);
#endif //DLMS_IGNORE_DELTA
#ifndef DLMS_IGNORE_PROFILE_GENERIC
  int cosem_getColumns(
      gxArray* captureObjects,
      unsigned char selector,
      dlmsVARIANT* parameters,
      gxArray* columns);
#endif //DLMS_IGNORE_PROFILE_GENERIC

  //Find object from settings object and create if not found.
  int cosem_findObjectByLN(
      dlmsSettings* settings,
      DLMS_OBJECT_TYPE ot,
      const unsigned char* ln,
      gxObject** object);

#ifdef  __cplusplus
}
#endif

#endif //COSEM_H
