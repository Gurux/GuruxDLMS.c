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


//This attribute is not serialized.
#define IGNORE_ATTRIBUTE(OBJECT, INDEX) {OBJECT, INDEX, 0}

//This attribute is not serialized by object type.
#define IGNORE_ATTRIBUTE_BY_TYPE(TYPE, INDEX) {0, INDEX, TYPE}

#define ___COSEM_ATTRIBUTES(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,...) _16
//Visual Studio reguires this.
#define ___EXPAND(x) x
#define ___COUNT_ATTRIBUTES(...) ___EXPAND(___COSEM_ATTRIBUTES(__VA_ARGS__,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

#define GET_ALL_ATTRIBUTES() -1
#define GET_ATTRIBUTE1(A) 1 << (A - 1)
#define GET_ATTRIBUTE2(A, B) GET_ATTRIBUTE1(A) | GET_ATTRIBUTE1(B)
#define GET_ATTRIBUTE3(A, B, C) GET_ATTRIBUTE2(A, B) | GET_ATTRIBUTE1(C)
#define GET_ATTRIBUTE4(A, B, C, D) GET_ATTRIBUTE3(A, B, C) | GET_ATTRIBUTE1(D)
#define GET_ATTRIBUTE5(A, B, C, D, E) GET_ATTRIBUTE4(A, B, C, D) | GET_ATTRIBUTE1(E)
#define GET_ATTRIBUTE6(A, B, C, D, E, F) GET_ATTRIBUTE5(A, B, C, D, E) | GET_ATTRIBUTE1(F)
#define GET_ATTRIBUTE7(A, B, C, D, E, F, G) GET_ATTRIBUTE6(A, B, C, D, E, F) | GET_ATTRIBUTE1(G)
#define GET_ATTRIBUTE8(A, B, C, D, E, F, G, H) GET_ATTRIBUTE7(A, B, C, D, E, F, G) | GET_ATTRIBUTE1(H)
#define GET_ATTRIBUTE9(A, B, C, D, E, F, G, H, I) GET_ATTRIBUTE8(A, B, C, D, E, F, G, H) | GET_ATTRIBUTE1(I)
#define GET_ATTRIBUTE10(A, B, C, D, E, F, G, H, I, J)GET_ATTRIBUTE9(A, B, C, D, E, F, G, H, I) | GET_ATTRIBUTE1(J)
#define GET_ATTRIBUTE11(A, B, C, D, E, F, G, H, I, J, K) GET_ATTRIBUTE10(A, B, C, D, E, F, G, H, I, J) | GET_ATTRIBUTE1(K)
#define GET_ATTRIBUTE12(A, B, C, D, E, F, G, H, I, J, K, L) GET_ATTRIBUTE11(A, B, C, D, E, F, G, H, I, J, K) | GET_ATTRIBUTE1(L)
#define GET_ATTRIBUTE13(A, B, C, D, E, F, G, H, I, J, K, L, M) GET_ATTRIBUTE12(A, B, C, D, E, F, G, H, I, J, K, L) | GET_ATTRIBUTE1(M)
#define GET_ATTRIBUTE14(A, B, C, D, E, F, G, H, I, J, K, L, M, N) GET_ATTRIBUTE13(A, B, C, D, E, F, G, H, I, J, K, L, M) | GET_ATTRIBUTE1(N)
#define GET_ATTRIBUTE15(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O) GET_ATTRIBUTE14(A, B, C, D, E, F, G, H, I, J, K, L, M, N) | GET_ATTRIBUTE1(O)
#define GET_ATTRIBUTE16(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P) GET_ATTRIBUTE15(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O) | GET_ATTRIBUTE1(P)
#define CONC(A, B) CONC_(A, B)
#define CONC_(A, B) A##B
#define GET_ATTRIBUTE(...) CONC(GET_ATTRIBUTE, ___COUNT_ATTRIBUTES(__VA_ARGS__))(__VA_ARGS__)
#define GET_ATTRIBUTE_EXCEPT(...) ~(GET_ATTRIBUTE(__VA_ARGS__))
#define GET_ATTRIBUTE_ALL() -1

    typedef struct
    {
        /*Target to ignore*/
        gxObject* target;
        /*Bit enumerated attribute list from attributes that are not serialized.*/
        uint16_t attributes;
        /*Object type to ignore*/
        uint16_t objectType;
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
