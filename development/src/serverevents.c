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
#include "../include/serverevents.h"
#include "../include/bytebuffer.h"

#ifdef DLMS_DEBUG
#include <stdio.h>
#endif //DLMS_DEBUG

void svr_notifyTrace(const char* str, int err)
{
#ifdef DLMS_DEBUG
    if (err != 0)
    {
        char tmp[20];
        sprintf(tmp, " Error: %d", err);
        svr_trace(str, tmp);
    }
    else
    {
        svr_trace(str, " succeeded.");
    }
#endif// DLMS_DEBUG
}

void svr_notifyTrace2(const char* str, const short ot, const unsigned char* ln, int err)
{
#ifdef DLMS_DEBUG
    if (err != 0)
    {
        char tmp[20];
        sprintf(tmp, " %d:%d.%d.%d.%d.%d.%d Error: %d", ot, ln[0], ln[1], ln[2], ln[3], ln[4], ln[5], err);
        svr_trace(str, tmp);
    }
    else
    {
        svr_trace(str, " succeeded.");
    }
#endif// DLMS_DEBUG
}


void svr_notifyTrace3(const char* str, const char* value)
{
#ifdef DLMS_DEBUG
    svr_trace(str, value);
#endif// DLMS_DEBUG
}

void svr_notifyTrace4(const char* str, gxByteBuffer* value)
{
#ifdef DLMS_DEBUG
    svr_notifyTrace5(str, value->data, value->size);
#endif// DLMS_DEBUG
}

//Server uses notify trace if DLMS_DEBUG is defined.
void svr_notifyTrace5(const char* str, const unsigned char* value, uint16_t length)
{
#ifdef DLMS_DEBUG
    int pos, index = 0;
#define COUNT 20
    static char tmp[3 * COUNT];
    for (pos = 0; pos != length; ++pos)
    {
        sprintf(tmp + (3 * index), "%02X ", value[pos]);
        ++index;
        if (index == COUNT)
        {
            tmp[sizeof(tmp) - 1] = '\0';
            svr_trace(str, tmp);
            index = 0;
        }
    }
    if (index != 0)
    {
        tmp[(3 * index) - 1] = '\0';
        svr_trace(str, tmp);
    }
#endif// DLMS_DEBUG
}

