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

#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/message.h"

//Initialize gxByteBuffer.
void mes_init(message* mes)
{
    mes->capacity = MESSAGE_CAPACITY;
    mes->data = (gxByteBuffer**) gxmalloc(mes->capacity * sizeof(gxByteBuffer*));
    mes->size = 0;
}

//Push new message.
void mes_push(message * mes, gxByteBuffer* item)
{

    if(mes->size == mes->capacity)
    {
        mes->capacity += MESSAGE_CAPACITY;
        if (mes->data == NULL)
        {
            mes->data = (gxByteBuffer**) gxmalloc(mes->capacity * sizeof(gxByteBuffer*));
        }
        else
        {
            mes->data = (gxByteBuffer**) gxrealloc(mes->data, mes->capacity * sizeof(gxByteBuffer*));
        }
    }
    mes->data[mes->size] = item;
    mes->size++;
}

void mes_clear(message* mes)
{
    int pos;
    if (mes->size != 0)
    {
        for(pos = 0; pos != mes->size; ++pos)
        {
            gxfree(mes->data[pos]->data);
            gxfree(mes->data[pos]);
        }
    }
    if (mes->data != NULL)
    {
        gxfree(mes->data);
        mes->data = NULL;
    }
    mes->capacity = 0;
    mes->size = 0;
}
