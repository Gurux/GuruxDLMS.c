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

#ifndef GXECDSA_H
#define GXECDSA_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#include "gxignore.h"
#include "privateKey.h"

    /*Verify signature.
        signature: Generated signature.
        data: Data to valuate.
    */
    int gxecdsa_verify(gxPublicKey* key, gxByteBuffer* signature, gxByteBuffer* data);

    /*Sign data.
    data: Signed data.
    signature: Generated signature.
    */
    int gxecdsa_sign(gxPrivateKey* key, gxByteBuffer* data, gxByteBuffer* signature);

    /**
     Generates public and private key pair.
    */
    int gxecdsa_generateKeyPair(
        ECC scheme,
        gxPublicKey* pub, gxPrivateKey* priv);

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#ifdef  __cplusplus
}
#endif

#endif //GXECDSA_H
