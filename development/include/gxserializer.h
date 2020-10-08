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

#ifndef GXSERIALIZER_H
#define GXSERIALIZER_H

#include "gxignore.h"
#include "gxobjects.h"
#include "dlmssettings.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif


    //Don't serialize this attribute.
#define IGNORE_ATTRIBUTE(a, x) {a, x}

    typedef struct
    {
        /*Target to ignore*/
        gxObject* target;
        /*Bit-string attribute list from attributes that are not serialized.*/
        uint16_t attributes;
    } gxSerializerIgnore;

    typedef struct
    {
        //List of attributes that are not serialized.
        gxSerializerIgnore* ignoredAttributes;
        //Count of ignored objects.
        uint16_t count;
    } gxSerializerSettings;

    //Serialize object to bytebuffer.
    int ser_saveObject(
        gxSerializerSettings* serializeSettings,
        gxObject* object,
        gxByteBuffer* serializer);

    //Serialize objects to bytebuffer.
    int ser_saveObjects(
        gxSerializerSettings* serializeSettings,
        gxObject** object,
        uint16_t count,
        gxByteBuffer* serializer);

    //Serialize object from bytebuffer.
    int ser_loadObject(
        dlmsSettings* settings,
        gxSerializerSettings* serializeSettings,
        gxObject* object,
        gxByteBuffer* serializer);

    //Serialize objects from the bytebuffer.
    int ser_loadObjects(
        dlmsSettings* settings,
        gxSerializerSettings* serializeSettings,
        gxObject** object,
        uint16_t count,
        gxByteBuffer* serializer);

#ifdef  __cplusplus
}
#endif

#endif //GXSERIALIZER_H
