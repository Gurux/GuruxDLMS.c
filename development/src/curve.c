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

#include "../include/curve.h"
#include "../include/errorcodes.h"

int curve_init(gxCurve* curve, ECC scheme)
{
    if (scheme == ECC_P256)
    {
        //Table A. 1 – ECC_P256_Domain_Parameters
        static const uint32_t INIT_A[] = {
            0xFFFFFFFC, 0xFFFFFFFF,
            0xFFFFFFFF, 0x00000000,
            0x00000000, 0x00000000,
            0x00000001, 0xFFFFFFFF,
        };
        static const uint32_t INIT_G1[] = {
            0xD898C296, 0xF4A13945,
            0x2DEB33A0,0x77037D81,
            0x63A440F2,0xF8BCE6E5,
            0xE12C4247,0x6B17D1F2,
        };
        static const uint32_t INIT_G2[] = {
            0x37BF51F5, 0xCBB64068,
            0x6B315ECE,0x2BCE3357,
            0x7C0F9E16,0x8EE7EB4A,
            0xFE1A7F9B,0x4FE342E2,
        };
        static const uint32_t INIT_N[] = {
            0xFC632551,0xF3B9CAC2,
            0xA7179E84,0xBCE6FAAD,
            0xFFFFFFFF,0xFFFFFFFF,
            0x00000000,0xFFFFFFFF,
        };
        static const uint32_t INIT_P[] = {
            0xFFFFFFFF,0xFFFFFFFF,
            0xFFFFFFFF,0x00000000,
            0x00000000,0x00000000,
            0x00000001,0xFFFFFFFF,
        };
        static const uint32_t INIT_B[] = {
            0x27D2604B, 0x3BCE3C3E,
            0xCC53B0F6,0x651D06B0,
            0x769886BC,0xB3EBBD55,
            0xAA3A93E7,0x5AC635D8,
        };
        uint16_t size = sizeof(INIT_A) / sizeof(uint32_t);
        bi_attach(&curve->a, (uint32_t*) INIT_A, size, size);
        bi_attach(&curve->g.x, (uint32_t*) INIT_G1, size, size);
        bi_attach(&curve->g.y, (uint32_t*)INIT_G2, size, size);
        bi_attach(&curve->n, (uint32_t*)INIT_N, size, size);
        bi_attach(&curve->p, (uint32_t*)INIT_P, size, size);
        bi_attach(&curve->b, (uint32_t*)INIT_B, size, size);
    }
    else if (scheme == ECC_P384)
    {
        //Table A. 2 – ECC_P384_Domain_Parameters
        static const uint32_t INIT_A[] = {
            0xFFFFFFFC, 0x00000000,
            0x00000000, 0xFFFFFFFF,
            0xFFFFFFFE, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
        };
        static  const uint32_t INIT_G1[] = {
            0x72760AB7, 0x3A545E38,
            0xBF55296C, 0x5502F25D,
            0x82542A38, 0x59F741E0,
            0x8BA79B98, 0x6E1D3B62,
            0xF320AD74, 0x8EB1C71E,
            0xBE8B0537, 0xAA87CA22,
        };
        static const uint32_t INIT_G2[] = {
            0x90EA0E5F, 0x7A431D7C,
            0x1D7E819D, 0x0A60B1CE,
            0xB5F0B8C0, 0xE9DA3113,
            0x289A147C, 0xF8F41DBD,
            0x9292DC29, 0x5D9E98BF,
            0x96262C6F, 0x3617DE4A,
        };
        static const uint32_t INIT_N[] = {
            0xCCC52973, 0xECEC196A,
            0x48B0A77A, 0x581A0DB2,
            0xF4372DDF, 0xC7634D81,
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
        };
        static const uint32_t INIT_P[] = {
            0xFFFFFFFF, 0x00000000,
            0x00000000, 0xFFFFFFFF,
            0xFFFFFFFE, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
        };
        static const uint32_t INIT_B[] = {
            0xD3EC2AEF, 0x2A85C8ED,
            0x8A2ED19D, 0xC656398D,
            0x5013875A, 0x0314088F,
            0xFE814112, 0x181D9C6E,
            0xE3F82D19, 0x988E056B,
            0xE23EE7E4, 0xB3312FA7,
        };
        uint16_t size = sizeof(INIT_A) / sizeof(uint32_t);
        bi_attach(&curve->a, (uint32_t*)INIT_A, size, size);
        bi_attach(&curve->g.x, (uint32_t*)INIT_G1, size, size);
        bi_attach(&curve->g.y, (uint32_t*)INIT_G2, size, size);
        bi_attach(&curve->n, (uint32_t*)INIT_N, size, size);
        bi_attach(&curve->p, (uint32_t*)INIT_P, size, size);
        bi_attach(&curve->b, (uint32_t*)INIT_B, size, size);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return 0;
}
