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

#ifndef GXARRAY_H
#define GXARRAY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <string.h> /* memset */
#include "gxint.h"

#define GXARRAY_CAPACITY 10

    typedef struct
    {
#ifndef DLMS_IGNORE_MALLOC
        void** data;
#else
        void* data;
#endif //DLMS_IGNORE_MALLOC

        uint16_t capacity;
        uint16_t size;
#ifndef DLMS_IGNORE_MALLOC
        int position;
#endif //DLMS_IGNORE_MALLOC
    } gxArray;

    //Initialize gxArray.
    void arr_init(gxArray* arr);

    void arr_attach(
        gxArray* arr,
        void* value,
        uint16_t count,
        uint16_t capacity);
    /*
       * Is static buffer used.
       */
    char arr_isAttached(
        gxArray* arr);

    /*Get maximum buffer size.*/
    uint16_t arr_getCapacity(
        gxArray* arr);

    //Allocate new size for the array in bytes.
    int arr_capacity(gxArray* arr, int capacity);


#ifndef DLMS_IGNORE_MALLOC
    //Push new data to the gxArray.
    int arr_push(gxArray* arr, void* item);
#endif //DLMS_IGNORE_MALLOC

    //Clear array. All items are automatically free.
    void arr_clear(gxArray* arr);

    //Empty array but is not free items.
    void arr_empty(
        gxArray* arr);

    //Clear key value pair array. All items are automatically free.
    void arr_clearKeyValuePair(gxArray* arr);

    //Clear string array.
    void arr_clearStrings(gxArray* arr);

#ifndef DLMS_IGNORE_MALLOC
    int arr_get(gxArray* arr, void** value);

    int arr_getByIndex(
        gxArray* arr,
        uint16_t index,
        void** value);

    int arr_getByIndex3(
        gxArray* arr,
        uint16_t index,
        void** value,
        unsigned char checkSize);

    int arr_removeByIndex(
        gxArray* arr,
        uint16_t index,
        void** value);

    //Swap two array items.
    int arr_swap(gxArray* arr, uint16_t index1, uint16_t index2);
#else
    int arr_getByIndex(
        gxArray* arr,
        uint16_t index,
        void** value,
        uint16_t itemSize);

    int arr_getByIndex3(
        gxArray* arr,
        uint16_t index,
        void** value,
        uint16_t itemSize,
        unsigned char checkSize);

    int arr_removeByIndex(
        gxArray* arr,
        uint16_t index,
        uint16_t itemSize);

    //Swap two array items.
    int arr_swap(
        gxArray* arr,
        uint16_t index1,
        uint16_t index2,
        uint16_t itemSize,
        void* tmp);

    //This method is used to access array where items are saved with pointer.
    //Example: registerAssignment.
    int arr_getByIndexRef(gxArray* arr, uint16_t index, void** value);

    int arr_setByIndexRef(gxArray* arr, void* value);

#endif //DLMS_IGNORE_MALLOC

    int arr_getByIndex2(gxArray* arr, uint16_t index, void** value, uint16_t itemSize);
    int arr_getByIndex4(gxArray* arr, uint16_t index, void** value, uint16_t itemSize, unsigned char checkSize);

#define ARR_ATTACH(X, V, S) arr_attach(&X, V, S, sizeof(V)/sizeof(V[0]))

#ifdef  __cplusplus
}
#endif

#endif //GXARRAY_H
