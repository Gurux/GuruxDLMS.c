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

#ifndef OBJECTARRAY_H
#define OBJECTARRAY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#include "gxobjects.h"

#define OBJECT_ARRAY_CAPACITY 10

    //Initialize variantArray.
    void oa_init(
        objectArray* arr);

    //Attach objects to objectArray.
    void oa_attach(
        objectArray * arr,
        gxObject** item,
        uint16_t count);

    //Verify that all objects are called init2. This is used for developing purposes.
    int oa_verify(
        objectArray * arr);



    char oa_isAttached(objectArray* arr);

    uint16_t oa_getCapacity(objectArray* arr);

    //Allocate new size for the array in bytes.
    int oa_capacity(
        objectArray* arr,
        const uint16_t capacity);

#ifndef DLMS_IGNORE_MALLOC
    //Push new data to the variantArray.
    int oa_push(
        objectArray * arr,
        gxObject* item);
#endif //DLMS_IGNORE_MALLOC

    //Copy content of object array.
    void oa_copy(
        objectArray *target,
        objectArray* source);

    //Move content of object array.
    void oa_move(
        objectArray* target,
        objectArray* source);

    //Clear object array. Clear will free objects as well.
    void oa_clear2(
        objectArray* arr,
        uint16_t index,
        uint16_t count);

    //Clear object array. Clear will free objects as well.
    void oa_clear(
        objectArray* arr,
        unsigned char releaseObjects);

    //Empty object array. Empty do not free objects.
    void oa_empty(
        objectArray* arr);

    //Get item from object array by index.
    int oa_getByIndex(
        const objectArray* arr,
        uint16_t index,
        gxObject** item);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    //Find object using Short Name.
    int oa_findBySN(
        objectArray* objects,
        uint16_t sn,
        gxObject** object);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    //Find object using Logical Name.
    int oa_findByLN(
        objectArray* objects,
        DLMS_OBJECT_TYPE type,
        const unsigned char* ln,
        gxObject** object);

    //Get object by object type.
    int oa_getObjects(
        objectArray* src,
        DLMS_OBJECT_TYPE type,
        objectArray* objects);

    //Get object by array of object type.
    int oa_getObjects2(
        objectArray* src,
        DLMS_OBJECT_TYPE* types,
        unsigned char typeCount,
        objectArray* objects);

#define OA_ATTACH(X, V) oa_attach(&X, (gxObject**) V, sizeof(V)/sizeof(V[0]))
#ifdef  __cplusplus
}
#endif

#endif //OBJECTARRAY_H
