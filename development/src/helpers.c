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
#include <math.h>

#ifndef GX_DLMS_MICROCONTROLLER
#include <stdio.h> //printf needs this or error is generated.
#endif //GX_DLMS_MICROCONTROLLER

#include "../include/helpers.h"
#include "../include/errorcodes.h"

//Check byte order.
unsigned char hlp_isBigEndian(void)
{
    unsigned short a = 0x1234;
    return *((unsigned char *)&a) == 0x12;
}

#ifndef GX_DLMS_MICROCONTROLLER
char* hlp_getErrorMessage(short err)
{
    switch (err)
    {
    case DLMS_ERROR_CODE_HARDWARE_FAULT:
        return "Access Error : Device reports a hardware fault.";
    case DLMS_ERROR_CODE_TEMPORARY_FAILURE:
        return "Access Error : Device reports a temporary failure.";
    case DLMS_ERROR_CODE_READ_WRITE_DENIED:
        return "Access Error : Device reports Read-Write denied.";
    case DLMS_ERROR_CODE_UNDEFINED_OBJECT:
        return "Access Error : Device reports a undefined object.";
    case DLMS_ERROR_CODE_INCONSISTENT_OBJECT:
        return "Access Error : Device reports a inconsistent Class or Object.";
    case DLMS_ERROR_CODE_UNAVAILABLE_OBJECT:
        return "Access Error : Device reports a unavailable object.";
    case DLMS_ERROR_CODE_UNMATCH_TYPE:
        return "Access Error : Device reports a unmatched type.";
    case DLMS_ERROR_CODE_VIOLATED:
        return "Access Error : Device reports scope of access violated.";
    case DLMS_ERROR_CODE_BLOCK_UNAVAILABLE:
        return "Access Error : Data Block Unavailable.";
    case DLMS_ERROR_CODE_READ_ABORTED:
        return "Access Error : Long Get Or Read Aborted.";
    case DLMS_ERROR_CODE_NO_LONG_GET_OR_READ_IN_PROGRESS:
        return "Access Error : No Long Get Or Read In Progress.";
    case DLMS_ERROR_CODE_WRITE_ABORTED:
        return "Access Error : Long Set Or Write Aborted.";
    case DLMS_ERROR_CODE_NO_LONG_SET_OR_WRITE_IN_PROGRESS:
        return "Access Error : No Long Set Or Write In Progress.";
    case DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID:
        return "Access Error : Data Block Number Invalid.";
    case DLMS_ERROR_CODE_OTHER_REASON:
        return "Access Error : Other Reason.";
    case DLMS_ERROR_CODE_OK:
        return "OK";
    case DLMS_ERROR_CODE_INVALID_PARAMETER:
        return "Invalid Parameter.";
    case DLMS_ERROR_CODE_NOT_INITIALIZED:
        return "Server is not initialized.";
    case DLMS_ERROR_CODE_OUTOFMEMORY:
        return "Not enough memory available.";
    case DLMS_ERROR_CODE_NOT_REPLY:
        return "Packet is not a reply for a send packet.";
    case DLMS_ERROR_CODE_REJECTED:
        return "Meter rejects send packet.";
    case DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS:
        return "Client HDLC Address is not set.";
    case DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS:
        return "Server HDLC Address is not set.";
    case DLMS_ERROR_CODE_INVALID_DATA_FORMAT:
        return "Not a HDLC frame.";
    case DLMS_ERROR_CODE_INVALID_VERSION_NUMBER:
        return "Invalid DLMS version number.";
    case DLMS_ERROR_CODE_CLIENT_ADDRESS_NO_NOT_MATCH:
        return "Client addresses do not match.";
    case DLMS_ERROR_CODE_SERVER_ADDRESS_NO_NOT_MATCH:
        return "Server addresses do not match.";
    case DLMS_ERROR_CODE_WRONG_CRC:
        return "CRC do not match.";
    case DLMS_ERROR_CODE_INVALID_RESPONSE:
        return "Invalid response.";
    case DLMS_ERROR_CODE_INVALID_TAG:
        return "Invalid Tag.";
    case DLMS_ERROR_CODE_ENCODING_FAILED:
        return "Encoding failed. Not enough data.";
    case DLMS_ERROR_CODE_REJECTED_PERMAMENT:
        return "Rejected permament.";
    case DLMS_ERROR_CODE_REJECTED_TRANSIENT:
        return "Rejected transient.";
    case DLMS_ERROR_CODE_NO_REASON_GIVEN:
        return "No reason given.";
    case DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED:
        return "Application context name not supported.";
    case DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED:
        return "Authentication mechanism name not recognised.";
    case DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_REQUIRED:
        return "Authentication mechanism name required.";
    case DLMS_ERROR_CODE_AUTHENTICATION_FAILURE:
        return "Authentication failure.";
    case DLMS_ERROR_CODE_AUTHENTICATION_REQUIRED:
        return "Authentication required.";
        //Data send failed.
    case DLMS_ERROR_CODE_SEND_FAILED:
        return "Send failed.";
        //Data receive failed.
    case DLMS_ERROR_CODE_RECEIVE_FAILED:
        return "Receive failed.";
    case DLMS_ERROR_CODE_NOT_IMPLEMENTED:
        return "Not implemented.";
    default:
        return "Unknown error.";
    }
}
#endif //GX_DLMS_MICROCONTROLLER

unsigned char hlp_getObjectCountSizeInBytes(unsigned long count)
{
    if (count < 0x80)
    {
        return 1;
    }
    else if (count < 0x100)
    {
        return 2;
    }
    else if (count < 0x10000)
    {
        return 3;
    }
    else
    {
        return 5;
    }
}

int hlp_getObjectCount(gxByteBuffer* buff)
{
    int ret;
    unsigned char ch;
    ret = bb_getUInt8(buff, &ch);
    if (ret != 0)
    {
        return -1;
    }
    if (ch > 0x80)
    {
        if (ch == 0x81)
        {
            ret = bb_getUInt8(buff, &ch);
        }
        else if (ch == 0x82)
        {
            unsigned short value;
            ret = bb_getUInt16(buff, &value);
            if (ret != 0)
            {
                return -1;
            }
            return value;
        }
        else if (ch == 0x84)
        {
            unsigned long value;
            ret = bb_getUInt32(buff, &value);
            if (ret != 0)
            {
                return -1;
            }
            return value;
        }
        else
        {
            return -1;
        }
    }
    return ch;
}

int hlp_getObjectCount2(gxByteBuffer* buff, unsigned short* count)
{
    int ret;
    unsigned char ch;
    ret = bb_getUInt8(buff, &ch);
    if (ret != 0)
    {
        return ret;
    }
    if (ch > 0x80)
    {
        if (ch == 0x81)
        {
            ret = bb_getUInt8(buff, &ch);
            *count = ch;
        }
        else if (ch == 0x82)
        {
            ret = bb_getUInt16(buff, count);
        }
        else if (ch == 0x84)
        {
            unsigned long value;
            ret = bb_getUInt32(buff, &value);
            *count = (unsigned short)value;
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else
    {
        *count = ch;
    }
    return ret;
}

// Set count of items.
int hlp_setObjectCount(unsigned long count, gxByteBuffer* buff)
{
    int ret;
    if (count < 0x80)
    {
        ret = bb_setUInt8(buff, (unsigned char)count);
    }
    else if (count < 0x100)
    {
        if ((ret = bb_setUInt8(buff, 0x81)) == 0)
        {
            ret = bb_setUInt8(buff, (unsigned char)count);
        }
    }
    else if (count < 0x10000)
    {
        if ((ret = bb_setUInt8(buff, 0x82)) == 0)
        {
            ret = bb_setUInt16(buff, (unsigned short)count);
        }
    }
    else
    {
        if ((ret = bb_setUInt8(buff, 0x84)) == 0)
        {
            ret = bb_setUInt32(buff, count);
        }
    }
    return ret;
}

char* hlp_bytesToHex(const unsigned char* pBytes, int count)
{
    const char hexArray[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    unsigned char tmp;
    int pos;
    char* hexChars;
    if (count != 0)
    {
        hexChars = (char*)gxmalloc(3 * count);
        if (hexChars != NULL)
        {
            for (pos = 0; pos != count; ++pos)
            {
                tmp = pBytes[pos] & 0xFF;
                hexChars[pos * 3] = hexArray[tmp >> 4];
                hexChars[pos * 3 + 1] = hexArray[tmp & 0x0F];
                hexChars[pos * 3 + 2] = ' ';
            }
            hexChars[(3 * count) - 1] = '\0';
        }
    }
    else
    {
        hexChars = (char*)gxmalloc(1);
        hexChars[0] = '\0';
    }
    return hexChars;
}

unsigned char getValue(char c)
{
    unsigned char value;
    if (c > '9')
    {
        if (c > 'Z')
        {
            value = (c - 'a' + 10);
        }
        else
        {
            value = (c - 'A' + 10);
        }
    }
    else
    {
        value = (c - '0');
    }
    return value;
}

int hlp_hexToBytes(const char* str, unsigned char** buffer, unsigned char* count)
{
    *count = 0;
    if (buffer != NULL) 
    {
        if (*buffer != NULL)
        {
            gxfree(*buffer);
        }
        
    }
    if (str == NULL)
    {
        return 0;
    }
    int len = (int)strlen(str);
    if (len == 0)
    {
        return 0;
    }
    unsigned char* tmp = (unsigned char*)gxmalloc(len / 2);
    if (tmp == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *buffer = tmp;
    int lastValue = -1;
    for (int pos = 0; pos != len; ++pos)
    {
        if (*str >= '0' && *str < 'g')
        {
            if (lastValue == -1)
            {
                lastValue = getValue(*str);
            }
            else if (lastValue != -1)
            {
                tmp[*count] = (unsigned char)(lastValue << 4 | getValue(*str));
                lastValue = -1;
                ++*count;
            }
        }
        else if (lastValue != -1)
        {
            tmp[*count] = getValue(*str);
            lastValue = -1;
            ++*count;
        }
        ++str;
    }
    if (len / 2 != *count)
    {
        *buffer = gxrealloc(*buffer, *count);
    }
    return 0;
}

#ifndef GX_DLMS_MICROCONTROLLER
void hlp_trace(unsigned char* data, int index, int count, unsigned char send)
{
    char* buff = hlp_bytesToHex(data + index, count);
    if (send)
    {
        printf("<- %s\r\n", buff);
    }
    else
    {
        printf("-> %s\r\n", buff);
    }
    gxfree(buff);
}

int hlp_parseLogicalName(gxByteBuffer* value, unsigned char ln[6])
{
    char* ch;
    char* pOriginalBuff;
    char* pBuff;
    int val = 0, count = 0, size = value->size;
    if (size < 11)
    {
        return -1;
    }
    pBuff = (char*)gxmalloc(size + 1);
    pOriginalBuff = pBuff;
    memcpy(pBuff, value->data, size);
    pBuff[value->size] = 0;
    while ((ch = strchr(pBuff, '.')) != NULL)
    {
        *ch = '\0';
        val = hlp_stringToInt(pBuff);
        if (val == -1)
        {
            return -1;
        }
        ln[count] = (unsigned char)val;
        pBuff = ch + sizeof(char);
        ++count;
    }
    if (count == 5)
    {
        val = hlp_stringToInt(pBuff);
        if (val == -1)
        {
            return -1;
        }
        ln[count] = (unsigned char)val;
        pBuff = ch + sizeof(char);
        ++count;
    }
    gxfree(pOriginalBuff);
    if (count != 6)
    {
        memset(&ln, 0, 6);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

//Set logical name from string.
int hlp_setLogicalName2(dlmsVARIANT* ln, const char* name)
{
    int ret;
    unsigned char tmp[6];
    var_clear(ln);
    if ((ret = hlp_setLogicalName(tmp, name)) != 0)
    {
        return ret;
    }
    var_addBytes(ln, tmp, 6);
    return DLMS_ERROR_CODE_OK;
}

int hlp_printLogicalName(const char* format, const unsigned char value[6])
{
    char ln[25];
    int ret = hlp_getLogicalNameToString(value, ln);
    if (ret != 0)
    {
        return ret;
    }
    if (format != NULL)
    {
        printf(format, ln);
    }
    else
    {
        printf(ln);
    }
    return 0;
}

int hlp_appendLogicalName(gxByteBuffer* bb, const unsigned char value[6])
{
    char ln[25];
    int ret = hlp_getLogicalNameToString(value, ln);
    if (ret != 0)
    {
        return ret;
    }
    return bb_addString(bb, ln);
}

int hlp_getLogicalNameToString(const unsigned char value[6], char* ln)
{
    int ret;
    ret = hlp_intToString(ln, 25, value[0], 0);
    if (ret != -1)
    {
        ln += ret;
        *ln = '.';
        ++ln;
        ret = hlp_intToString(ln, 25, value[1], 0);
        if (ret != -1)
        {
            ln += ret;
            *ln = '.';
            ++ln;
            ret = hlp_intToString(ln, 25, value[2], 0);
            if (ret != -1)
            {
                ln += ret;
                *ln = '.';
                ++ln;
                ret = hlp_intToString(ln, 25, value[3], 0);
                if (ret != -1)
                {
                    ln += ret;
                    *ln = '.';
                    ++ln;
                    ret = hlp_intToString(ln, 25, value[4], 0);
                    if (ret != -1)
                    {
                        ln += ret;
                        *ln = '.';
                        ++ln;
                        ret = hlp_intToString(ln, 25, value[5], 0);
                    }
                }
            }
        }
    }
    if (ret != -1)
    {
        ret = 0;
    }
    return ret;
}

#endif //GX_DLMS_MICROCONTROLLER

//Set logical name from string.
int hlp_setLogicalName(unsigned char ln[6], const char* name)
{
    char* ch;
    char* pOriginalBuff;
    char* pBuff;
    int val = 0, count = 0, size = (int)strlen(name);
    if (size < 11)
    {
        return -1;
    }
    pBuff = (char*)gxmalloc(size + 1);
    pOriginalBuff = pBuff;
    memcpy(pBuff, name, size);
    pBuff[size] = 0;
    while ((ch = strchr(pBuff, '.')) != NULL)
    {
        *ch = '\0';
        val = hlp_stringToInt(pBuff);
        if (val == -1)
        {
            gxfree(pOriginalBuff);
            return -1;
        }
        ln[count] = (unsigned char)val;
        pBuff = ch + sizeof(char);
        ++count;
    }
    if (count == 5)
    {
        val = hlp_stringToInt(pBuff);
        if (val == -1)
        {
            gxfree(pOriginalBuff);
            return -1;
        }
        ln[count] = (unsigned char)val;
        pBuff = ch + sizeof(char);
        ++count;
    }
    gxfree(pOriginalBuff);
    if (count != 6)
    {
        memset(&ln, 0, 6);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

void hlp_replace(gxByteBuffer* str, char oldCh, char newCh)
{
    unsigned long pos;
    for (pos = 0; pos != str->size; ++pos)
    {
        if (str->data[pos] == oldCh)
        {
            str->data[pos] = newCh;
        }
    }
}

double hlp_getScaler(int scaler)
{
    //If OS
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return pow((float)10, scaler);
#else
    return 1;
    //TODO:
#endif
}

int hlp_getDataTypeSize(DLMS_DATA_TYPE type)
{
    int size = -1;
    switch (type)
    {
    case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
        size = 1;
        break;
    case DLMS_DATA_TYPE_BOOLEAN:
        size = 1;
        break;
    case DLMS_DATA_TYPE_DATE:
        size = 5;
        break;
    case DLMS_DATA_TYPE_DATETIME:
        size = 12;
        break;
    case DLMS_DATA_TYPE_ENUM:
        size = 1;
        break;
#ifndef GX_DLMS_MICROCONTROLLER
    case DLMS_DATA_TYPE_FLOAT32:
        size = 4;
        break;
    case DLMS_DATA_TYPE_FLOAT64:
        size = 8;
        break;
#endif //GX_DLMS_MICROCONTROLLER
    case DLMS_DATA_TYPE_INT16:
        size = 2;
        break;
    case DLMS_DATA_TYPE_INT32:
        size = 4;
        break;
    case DLMS_DATA_TYPE_INT64:
        size = 8;
        break;
    case DLMS_DATA_TYPE_INT8:
        size = 1;
        break;
    case DLMS_DATA_TYPE_NONE:
        size = 0;
        break;
    case DLMS_DATA_TYPE_TIME:
        size = 4;
        break;
    case DLMS_DATA_TYPE_UINT16:
        size = 2;
        break;
    case DLMS_DATA_TYPE_UINT32:
        size = 4;
        break;
    case DLMS_DATA_TYPE_UINT64:
        size = 8;
        break;
    case DLMS_DATA_TYPE_UINT8:
        size = 1;
        break;
    default:
        break;
    }
    return size;
}

int hlp_intToString(char *str, int bufsize, long value, unsigned char isSigned)
{
    int cnt = 0;
    long val = value;
    if (isSigned && value < 0)
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = '-';
        ++str;
        --bufsize;
        value = -value;
        ++cnt;
    }
    //Find length.
    while ((val = (val / 10)))
    {
        ++str;
    }
    *(str + 1) = '\0';
    do
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = (value % 10) + '0';
        --str;
        value /= 10;
        --bufsize;
        ++cnt;
    } while (value != 0);
    return cnt;
}

long hlp_stringToInt(const char *str)
{
    if (str == NULL)
    {
        return -1;
    }
    long value = 0;
    unsigned char minus = 0;
    if (*str == '-')
    {
        minus = 1;
        ++str;
    }
    while (*str != '\0')
    {
        if (*str < '0' || *str > '9')
        {
            return -1;
        }
        value *= 10;
        value += *str - '0';
        ++str;
    }
    if (minus)
    {
        return -value;
    }
    return value;
}

int hlp_int64ToString(char *str, int bufsize, long long value, unsigned char isSigned)
{
    int cnt = 0;
    long long val = value;
    if (isSigned && value < 0)
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = '-';
        ++str;
        --bufsize;
        value = -value;
        ++cnt;
    }
    //Find length.
    while ((val = (val / 10)))
    {
        ++str;
    }
    *(str + 1) = '\0';
    do
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = (value % 10) + '0';
        --str;
        value /= 10;
        --bufsize;
        ++cnt;
    } while (value != 0);
    return cnt;
}

long long hlp_stringToInt64(const char *str)
{
    if (str == NULL)
    {
        return -1;
    }
    long value = 0;
    unsigned char minus = 0;
    if (*str == '-')
    {
        minus = 1;
        ++str;
    }
    while (*str != '\0')
    {
        if (*str < '0' || *str > '9')
        {
            return -1;
        }
        value *= 10;
        value += *str - '0';
        ++str;
    }
    if (minus)
    {
        return -value;
    }
    return value;
}
unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned char hlp_rand(void)
{
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    return (unsigned char)(lfsr = (unsigned short)((lfsr >> 1) | (bit << 15)));
}
