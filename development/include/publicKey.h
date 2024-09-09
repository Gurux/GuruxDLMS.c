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

#ifndef GXPUBLICKEY_H
#define GXPUBLICKEY_H

#include "gxignore.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "enums.h"
#include "byteBuffer.h"

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef DLMS_SECURITY_SUITE_1
#define PUBLIC_KEY_SIZE 65
#endif //DLMS_SECURITY_SUITE_1
#ifdef DLMS_SECURITY_SUITE_2
#define PUBLIC_KEY_SIZE 80
#endif //DLMS_SECURITY_SUITE_2

    typedef struct
    {
        /**
            Used scheme.
        */
        ECC scheme;
        /**
         Public key raw value.
        */
        gxByteBuffer rawValue;
        //Access public key throught rawValue. Don't use buff directly.
        unsigned char buff[PUBLIC_KEY_SIZE];
    } gxPublicKey;

    void pub_init(gxPublicKey* key);

    /*Clear the public key.*/
    void pub_clear(gxPublicKey* key);

    /**
    Create the public key from raw bytes.
    key: Public key.
    value: Raw data
    */
    int pub_fromRawBytes(
        gxPublicKey* key, 
        gxByteBuffer* value);

    /**
     X Coordinate.
    */
    int pub_getX(gxPublicKey* key,
        gxByteBuffer* value);

    /**
        Y Coordinate.
    */
    int pub_getY(gxPublicKey* key,
        gxByteBuffer* value);

#ifdef  __cplusplus
}
#endif
#endif//defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#endif //GXPUBLICKEY_H
