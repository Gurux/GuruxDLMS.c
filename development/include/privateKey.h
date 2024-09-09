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

#ifndef GXPRIVATEKEY_H
#define GXPRIVATEKEY_H

#include "gxignore.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "enums.h"
#include "byteBuffer.h"
#include "publicKey.h"

#ifdef  __cplusplus
extern "C" {
#endif
#ifdef DLMS_SECURITY_SUITE_1
#define PRIVATE_KEY_SIZE 32
#endif //DLMS_SECURITY_SUITE_1
#ifdef DLMS_SECURITY_SUITE_2
#define PRIVATE_KEY_SIZE 80
#endif //DLMS_SECURITY_SUITE_2
    typedef struct
    {
        /**
            Used scheme.
        */
        ECC scheme;
        /**
         Private key raw value.
        */
        gxByteBuffer rawValue;
        gxPublicKey publicKey;
        //Access private key throught rawValue. Don't use buff directly.
        unsigned char buff[PRIVATE_KEY_SIZE];
    } gxPrivateKey;

    void priv_init(gxPrivateKey* key);

    /*Clear the private key.*/
    void priv_clear(gxPrivateKey* key);

    /**
        Create the private key from raw bytes.
        value: Raw data
        key: Private key.
    */
    int priv_fromRawBytes(
        gxByteBuffer* value,
        gxPrivateKey* key);

    /**
        Get public key from private key.
        Returns Public key.
    */
    int priv_getPublicKey(gxPrivateKey* priv,
        gxPublicKey* pub);

#ifdef  __cplusplus
}
#endif
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#endif //GXPRIVATEKEY_H
