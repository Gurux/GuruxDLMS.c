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

#include "../include/publicKey.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#include "../include/errorcodes.h"

void pub_init(gxPublicKey *key)
{
    key->scheme = ECC_P256;
    BB_ATTACH(key->rawValue, key->buff, 0);
}

void pub_clear(gxPublicKey* key)
{
    key->scheme = ECC_P256;
    bb_clear(&key->rawValue);
}

int pub_fromRawBytes(
    gxPublicKey* key,
    gxByteBuffer* value)
{
    int ret = 0;
    bb_clear(&key->rawValue);
    if (bb_size(value) == 65)
    {
        key->scheme = ECC_P256;
    }
    else if (bb_size(value) == 97)
    {
        key->scheme = ECC_P384;
    }
    else if (bb_size(value) == 64)
    {
        //Compression tag is not send in DLMS messages.
        key->scheme = ECC_P256;
        ret = bb_setUInt8(&key->rawValue, 4);
    }
    else if (bb_size(value) == 96)
    {
        //Compression tag is not send in DLMS messages.
        key->scheme = ECC_P384;
        ret = bb_setUInt8(&key->rawValue, 4);
    }
    else
    {
        //Invalid key.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (ret == 0)
    {
        ret = bb_set(&key->rawValue, value->data, value->size);
    }
    return ret;
}


int pub_getX(gxPublicKey* key, gxByteBuffer* value)
{
    bb_clear(value);
    int size = bb_size(&key->rawValue) / 2;
    int ret = bb_subArray(&key->rawValue, 1, size, value);
    key->rawValue.position = 0;
    return ret;
}

int pub_getY(gxPublicKey* key, gxByteBuffer* value)
{
    bb_clear(value);
    int size = bb_size(&key->rawValue) / 2;
    int ret = bb_subArray(&key->rawValue, 1 + size, size, value);
    key->rawValue.position = 0;
    return ret;
}

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
