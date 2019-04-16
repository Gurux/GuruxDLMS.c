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
#ifndef VALUE_EVENT_ARG_H
#define VALUE_EVENT_ARG_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxobjects.h"
#include "errorcodes.h"

    typedef struct
    {
        /**
        * CGXDLMSVariant value.
        */
        dlmsVARIANT value;
        /**
        * Is request handled.
        */
        unsigned char handled;
        /**
        * Target DLMS object
        */
        gxObject* target;
        /**
        * Attribute index.
        */
        unsigned char index;
        /**
        * Data type of the value.
        */
        DLMS_DATA_TYPE dataType;
        /**
        * Optional selector.
        */
        unsigned char selector;
        /**
        * Optional parameters.
        */
        dlmsVARIANT parameters;

        /**
         * Occurred error.
         */
        DLMS_ERROR_CODE error;
        /**
        * Is action. This is reserved for internal use.
        */
        unsigned char action;

        /**
        * Is value added as byte array.
        */
        unsigned char byteArray;

        /**
        * List of objects that are released after use.
        */
        objectArray releasedObjects;

        /**
        * Is value max PDU size skipped.
        */
        unsigned char skipMaxPduSize;

        /**
        *  Transaction begin index.
        */
        unsigned short transactionStartIndex;
        /**
        *  Transaction end index.
        */
        unsigned short transactionEndIndex;
        //It this transaction.
        unsigned short transaction;
    } gxValueEventArg;

    /**
    * Initialize value event arg.
    */
    void ve_init(
        gxValueEventArg * ve);

    /**
    * Clear value event arg.
    */
    void ve_clear(
        gxValueEventArg * ve);

    typedef struct
    {
        gxValueEventArg** data;
        unsigned char capacity;
        unsigned char size;
        unsigned char position;
    } gxValueEventCollection;


    //Initialize value event collection.
    void vec_init(
        gxValueEventCollection* arr);

    //Push new data to the value event collection.
    void vec_push(
        gxValueEventCollection *arr,
        gxValueEventArg *item);

    //empty array, but items are not free.
    void vec_empty(
        gxValueEventCollection* arr);

    //Clear array. All items are automatically free.
    void vec_clear(
        gxValueEventCollection* arr);

    int vec_get(
        gxValueEventCollection* arr,
        gxValueEventArg** value);

    int vec_getByIndex(
        gxValueEventCollection* arr,
        int index,
        gxValueEventArg** value);


#ifdef  __cplusplus
}
#endif

#endif //VALUE_EVENT_ARG_H
