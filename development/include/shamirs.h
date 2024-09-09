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

#ifndef SHAMIR_H
#define SHAMIR_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "curve.h"
#include "publickey.h"
#include "bytebuffer.h"
#include "bigInteger.h"    

    /// <summary>
    /// Count Shamir's trick.
    /// </summary>
    /// <param name="curve">Used curve.</param>
    /// <param name="pub">Public key.</param>
    /// <param name="ret">Result.</param>
    /// <param name="u1"></param>
    /// <param name="u2"></param>
    int shamirs_trick(gxCurve* curve,
        gxPublicKey* pub,
        gxEccPoint* ret,
        bigInteger* u1,
        bigInteger* u2);

    /// <summary>
    /// Add points.
    /// </summary>
    /// <param name="curve">Used curve.</param>
    /// <param name="ret">Result.</param>
    /// <param name="p1">Point 1.</param>
    /// <param name="p2">Point 2.</param>
    int shamirs_pointAdd(gxCurve* curve,
        gxEccPoint* ret,
        gxEccPoint* p1,
        gxEccPoint* p2);

    /// <summary>
    /// Double point.
    /// </summary>
    /// <param name="curve">Used curve.</param>
    /// <param name="ret">Result value.</param>
    /// <param name="p1">Point to double</param>
    int shamirs_pointDouble(gxCurve* curve,
        gxEccPoint* ret,
        gxEccPoint* p1);

    /// <summary>
    /// Multiply point with big integer value.
    /// </summary>
    /// <param name="curve">Used curve.</param>
    /// <param name="value">Return value.</param>
    /// <param name="point">Point.</param>
    /// <param name="scalar">Scaler.</param>
    int shamirs_pointMulti(gxCurve* curve,
        gxEccPoint* value,
        gxEccPoint* point,
        bigInteger* scalar);

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#ifdef  __cplusplus
}
#endif

#endif //SHAMIR_H
