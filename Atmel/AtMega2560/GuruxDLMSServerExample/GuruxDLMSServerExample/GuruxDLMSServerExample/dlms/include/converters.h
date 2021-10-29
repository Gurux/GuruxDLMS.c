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

#ifndef CONVERTERRS_H
#define CONVERTERRS_H
#include "gxignore.h"

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef DLMS_IGNORE_MALLOC
#include "errorcodes.h"
#include "variant.h"
#include "gxarray.h"
#include "gxobjects.h"
#endif //DLMS_IGNORE_MALLOC
#include "enums.h"

    //Get object type as string.
    int obj_typeToString(
        DLMS_OBJECT_TYPE type,
        char* buff);

    //Get object type as string.
    const char* obj_typeToString2(
        DLMS_OBJECT_TYPE type);

    //Get object type as string.
    const char* obj_typeToString2(
        DLMS_OBJECT_TYPE type);

#ifndef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_STRING_CONVERTER

    const char* obj_getUnitAsString(
        unsigned char unit);

    int obj_rowsToString(
        gxByteBuffer* ba,
        gxArray* buffer);

    //Convert object's attributes to string.
    //This can be used in debugging purposes.
    int obj_toString(
        gxObject* object,
        char** buff);
#endif //DLMS_IGNORE_STRING_CONVERTER
    const char* err_toString(int err);

#endif //DLMS_IGNORE_MALLOC
#ifdef  __cplusplus
}
#endif
#endif //CONVERTERRS_H
