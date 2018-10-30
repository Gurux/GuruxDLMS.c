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

#define GXARRAY_CAPACITY 10

    typedef struct
    {
        void** data;
        int capacity;
        unsigned short size;
        int position;
    } gxArray;

    //Initialize gxArray.
    void arr_init(gxArray* arr);

    //Allocate new size for the array in bytes.
    int arr_capacity(gxArray* arr, int capacity);

    //Push new data to the gxArray.
    int arr_push(gxArray * arr, void* item);

    //Clear array. All items are automatically free.
    void arr_clear(gxArray* arr);

    //Empty array but is not free items.
    void arr_empty(
        gxArray* arr);

    //Clear key value pair array. All items are automatically free.
    void arr_clearKeyValuePair(gxArray* arr);

    //Clear string array.
    void arr_clearStrings(gxArray* arr);

    int arr_get(gxArray* arr, void** value);

    int arr_getByIndex(
        gxArray* arr,
        int index,
        void** value);

    int arr_removeByIndex(
        gxArray* arr,
        int index,
        void** value);

#ifdef  __cplusplus
}
#endif

#endif //GXARRAY_H
