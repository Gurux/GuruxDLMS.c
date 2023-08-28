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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "gxignore.h"
#include "bytebuffer.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define MESSAGE_CAPACITY 2

typedef struct
{
    gxByteBuffer** data;
    unsigned char capacity;
    unsigned char size;
#ifndef DLMS_IGNORE_MALLOC
    unsigned char attached;
#endif //DLMS_IGNORE_MALLOC
} message;

#ifndef DLMS_IGNORE_MALLOC
//Initialize gxByteBuffer.
void mes_init(message* mes);
//Push new message.
int mes_push(
    message* mes,
    gxByteBuffer* item);
#endif //DLMS_IGNORE_MALLOC

void mes_attach(message* mes, gxByteBuffer** data, unsigned char capacity);

//Clear message list.
void mes_clear(
    message* mes);

#ifdef  __cplusplus
}
#endif
#endif //MESSAGE_H
