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

#include "../include/shamirs.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#include "../include/gxmem.h"

#if _MSC_VER > 1400
#include <crtdbg.h>
#endif

int shamirs_trick(gxCurve* curve,
    gxPublicKey* pub,
    gxEccPoint* value,
    bigInteger* u1,
    bigInteger* u2)
{
    int ret;
    gxEccPoint sum;
    gxByteBuffer x, y;
    gxEccPoint op2;
    gxEccPoint tmp;
    if ((ret = BYTE_BUFFER_INIT(&x)) == 0 &&
        (ret = BYTE_BUFFER_INIT(&y)) == 0 &&
        (ret = pub_getX(pub, &x)) == 0 &&
        (ret = pub_getY(pub, &y)) == 0 &&
        (ret = eccPoint_init(&sum)) == 0 &&
        (ret = eccPoint_init(&op2)) == 0 &&
        (ret = eccPoint_init(&tmp)) == 0 &&
        (ret = bi_fromByteBuffer(&op2.x, &x)) == 0 &&
        (ret = bi_fromByteBuffer(&op2.y, &y)) == 0 &&
        (ret = shamirs_pointAdd(curve, &sum, &curve->g, &op2)) == 0)
    {
        uint16_t bits1 = bi_getUsedBits(u1);
        uint16_t bits2 = bi_getUsedBits(u2);
        uint16_t pos = bits1 > bits2 ? bits1 : bits2;
        --pos;
        if (bi_isBitSet(u1, pos) && bi_isBitSet(u2, pos))
        {
            bi_copy(&value->x, &sum.x);
            bi_copy(&value->y, &sum.y);
        }
        else if (bi_isBitSet(u1, pos))
        {
            bi_copy(&value->x, &curve->g.x);
            bi_copy(&value->y, &curve->g.y);
        }
        else if (bi_isBitSet(u2, pos))
        {
            bi_copy(&value->x, &op2.x);
            bi_copy(&value->y, &op2.y);
        }
        --pos;
        while (1)
        {
            bi_copy(&tmp.x, &value->x);
            bi_copy(&tmp.y, &value->y);
            ret = shamirs_pointDouble(curve, value, &tmp);
            if (ret == 0)
            {
                bi_copy(&tmp.x, &value->x);
                bi_copy(&tmp.y, &value->y);
                if (bi_isBitSet(u1, pos) && bi_isBitSet(u2, pos))
                {
                    ret = shamirs_pointAdd(curve, value, &tmp, &sum);
                }
                else if (bi_isBitSet(u1, pos))
                {
                    ret = shamirs_pointAdd(curve, value, &tmp, &curve->g);
                }
                else if (bi_isBitSet(u2, pos))
                {
                    ret = shamirs_pointAdd(curve, value, &tmp, &op2);
                }
            }
            if (ret != 0 || pos == 0)
            {
                break;
            }
            --pos;
        }
    }
    return ret;
}

int shamirs_pointAdd(gxCurve* curve,
    gxEccPoint* ret,
    gxEccPoint* p1,
    gxEccPoint* p2)
{
    bigInteger negy;
    bi_init(&negy);
    bi_copy(&negy, &curve->p);
    bi_sub(&negy, &p2->y);
    // Calculate lambda.
    bigInteger ydiff;
    bi_init(&ydiff);
    bi_copy(&ydiff, &p2->y);
    bi_sub(&ydiff, &p1->y);
    bigInteger xdiff;
    bi_init(&xdiff);
    bi_copy(&xdiff, &p2->x);
    bi_sub(&xdiff, &p1->x);
    bi_inv(&xdiff, &curve->p);
    bigInteger lambda;
    bi_init(&lambda);
    bi_copy(&lambda, &ydiff);
    bi_multiply(&lambda, &xdiff);
    bi_mod(&lambda, &curve->p);
    // calculate resulting x coord.
    bi_copy(&ret->x, &lambda);
    bi_multiply(&ret->x, &lambda);
    bi_sub(&ret->x, &p1->x);
    bi_sub(&ret->x, &p2->x);
    bi_mod(&ret->x, &curve->p);
    //calculate resulting y coord
    bi_copy(&ret->y, &p1->x);
    bi_sub(&ret->y, &ret->x);
    bi_multiply(&ret->y, &lambda);
    bi_sub(&ret->y, &p1->y);
    bi_mod(&ret->y, &curve->p);
    return 0;
}

int shamirs_pointDouble(gxCurve* curve,
    gxEccPoint* ret,
    gxEccPoint* p1)
{
    bigInteger numer;
    bigInteger denom;
    bi_init(&numer);
    bi_init(&denom);
    bi_copy(&numer, &p1->x);
    bi_multiply(&numer, &p1->x);
    bi_multiplyInt32(&numer, 3);
    bi_addBigInteger(&numer, &curve->a);
    bi_copy(&denom, &p1->y);
    bi_multiplyInt32(&denom, 2);
    bi_inv(&denom, &curve->p);
    bigInteger lambda;
    bi_init(&lambda);
    bi_copy(&lambda, &numer);
    bi_multiply(&lambda, &denom);
    bi_mod(&lambda, &curve->p);
    //x coordinate.
    bi_copy(&ret->x, &lambda);
    bi_multiply(&ret->x, &lambda);
    bi_sub(&ret->x, &p1->x);
    bi_sub(&ret->x, &p1->x);
    bi_mod(&ret->x, &curve->p);
    //y coordinate.
    bi_copy(&ret->y, &p1->x);
    bi_sub(&ret->y, &ret->x);
    bi_multiply(&ret->y, &lambda);
    bi_sub(&ret->y, &p1->y);
    bi_mod(&ret->y, &curve->p);
    return 0;
}

int shamirs_pointMulti(gxCurve* curve,
    gxEccPoint* value,
    gxEccPoint* point,
    bigInteger* scalar)
{
    int ret;
    gxEccPoint R0;
    gxEccPoint R1;
    gxEccPoint tmp;
    eccPoint_init(&R0);
    eccPoint_init(&R1);
    eccPoint_init(&tmp);
    if ((ret = bi_copy(&R0.x, &point->x)) == 0 &&
        (ret = bi_copy(&R0.y, &point->y)) == 0)
    {
        if ((ret = shamirs_pointDouble(curve, &R1, point)) == 0)
        {
            uint16_t dbits = bi_getUsedBits(scalar);
            dbits -= 2;
            while (1)
            {
                if (bi_isBitSet(scalar, dbits))
                {
                    if ((ret = bi_copy(&tmp.x, &R0.x)) != 0 ||
                        (ret = bi_copy(&tmp.y, &R0.y)) != 0)
                    {
                        break;
                    }
                    if ((ret = shamirs_pointAdd(curve, &R0, &R1, &tmp)) != 0 ||
                        (ret = bi_copy(&tmp.x, &R1.x)) != 0 ||
                        (ret = bi_copy(&tmp.y, &R1.y)) != 0)
                    {
                        break;
                    }
                    if ((ret = shamirs_pointDouble(curve, &R1, &tmp)) != 0)
                    {
                        break;
                    }
                }
                else
                {
                    if ((ret = bi_copy(&tmp.x, &R1.x)) != 0 ||
                        (ret = bi_copy(&tmp.y, &R1.y)) != 0)
                    {
                        break;
                    }
                    if ((ret = shamirs_pointAdd(curve, &R1, &R0, &tmp)) != 0 ||
                        (ret = bi_copy(&tmp.x, &R0.x)) != 0 ||
                        (ret = bi_copy(&tmp.y, &R0.y)) != 0)
                    {
                        break;
                    }
                    if ((ret = shamirs_pointDouble(curve, &R0, &tmp)) != 0)
                    {
                        break;
                    }
                }
                if (dbits == 0)
                {
                    break;
                }
                --dbits;
            }
            if ((ret = bi_copy(&value->x, &R0.x)) == 0 &&
                (ret = bi_copy(&value->y, &R0.y)) == 0)
            {

            }
        }
    }
    return ret;
}

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
