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

#include "../include/gxignore.h"
#ifndef DLMS_IGNORE_MALLOC
#include "../include/gxmem.h"
#endif //DLMS_IGNORE_MALLOC

#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/message.h"
#include "../include/errorcodes.h"

#ifndef DLMS_IGNORE_MALLOC
//Initialize gxByteBuffer.
void mes_init(message* mes)
{
    mes->capacity = MESSAGE_CAPACITY;
    mes->data = (gxByteBuffer**) gxmalloc(mes->capacity * sizeof(gxByteBuffer*));
    mes->size = 0;
}
#else
void mes_attach(message* mes, gxByteBuffer **data, unsigned char capacity)
{
    mes->capacity = capacity;
    mes->data = data;
    mes->size = 0;
}
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
//Push new message.
int mes_push(message * mes, gxByteBuffer* item)
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
#ifdef gxrealloc
                //If compiler supports realloc.
                mes->data = (gxByteBuffer**) gxrealloc(mes->data, mes->capacity * sizeof(gxByteBuffer*));
 #else
                //If compiler doesn't support realloc.
                gxByteBuffer** old = mes->data;
                mes->data = (gxByteBuffer**) gxmalloc(mes->capacity * sizeof(gxByteBuffer*));
                //If not enought memory available.
                if (mes->data == NULL)
                {
                    mes->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(mes->data, old, sizeof(gxByteBuffer*) * mes->size);
                gxfree(old);
 #endif // gxrealloc      
        }
    }
    mes->data[mes->size] = item;
    ++mes->size;
    return 0;
}
#endif //DLMS_IGNORE_MALLOC

void mes_clear(message* mes)
{
    int pos;
#ifdef DLMS_IGNORE_MALLOC
    for (pos = 0; pos != mes->capacity; ++pos)
    {
        mes->data[pos]->size = mes->data[pos]->position = 0;
    }
#else
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
#endif //DLMS_IGNORE_MALLOC
    mes->size = 0;
}
