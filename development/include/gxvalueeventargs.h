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

#include "gxignore.h"
#include "gxobjects.h"
#include "errorcodes.h"

    typedef struct
    {
        /**
        * CGXDLMSVariant value.
        */
        dlmsVARIANT value;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        /**
        * Data type of the value.
        */
        DLMS_DATA_TYPE dataType;
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
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
        * Data index.
        */
        uint16_t dataIndex;        
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
        * Is value max PDU size skipped.
        */
        unsigned char skipMaxPduSize;

        /**
        *  Transaction begin index.
        */
        uint32_t transactionStartIndex;
        /**
        *  Transaction end index.
        */
        uint32_t transactionEndIndex;
        //It this transaction.
        uint16_t transaction;
    } gxValueEventArg;

    /**
    * Initialize value event arg.
    */
    void ve_init(
        gxValueEventArg* ve);

    /**
    * Clear value event arg.
    */
    void ve_clear(
        gxValueEventArg* ve);

    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        gxValueEventArg* data;
#else
        gxValueEventArg** data;
#endif //DLMS_IGNORE_MALLOC
        unsigned char capacity;
        unsigned char size;
        unsigned char position;
    } gxValueEventCollection;


    //Initialize value event collection.
    void vec_init(
        gxValueEventCollection* arr);

#ifdef DLMS_IGNORE_MALLOC
    //Attach value event collection.
    void vec_attach(
        gxValueEventCollection* arr,
        gxValueEventArg* value,
        unsigned char count,
        unsigned char capacity);
#endif //DLMS_IGNORE_MALLOC
    /*
    * Is static buffer used.
    */
    char vec_isAttached(
        gxValueEventCollection* arr);

    //Bit array capacity.
    unsigned char vec_getCapacity(
        gxValueEventCollection* arr);

#ifndef DLMS_IGNORE_MALLOC
    //Push new data to the value event collection.
    int vec_push(
        gxValueEventCollection* arr,
        gxValueEventArg* item);
#endif //DLMS_IGNORE_MALLOC

    //empty array, but items are not free.
    void vec_empty(
        gxValueEventCollection* arr);

    //Clear array. All items are automatically free.
    void vec_clear(
        gxValueEventCollection* arr);

    int vec_getByIndex(
        gxValueEventCollection* arr,
        int index,
        gxValueEventArg** value);

#ifdef  __cplusplus
}
#endif

#endif //VALUE_EVENT_ARG_H
