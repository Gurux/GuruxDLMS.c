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

#if _MSC_VER > 1400
#include <stdio.h>
#include <crtdbg.h>
#endif
#include <string.h>
#include <math.h>
#include "../include/gxmem.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif //defined(_WIN32) || defined(_WIN64)

#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)
#include <stdio.h> //printf needs this or error is generated.
#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)

#include "../include/helpers.h"
#include "../include/errorcodes.h"

//Check byte order.
unsigned char hlp_isBigEndian(void)
{
    uint16_t a = 0x1234;
    return *((unsigned char*)&a) == 0x12;
}

const char* hlp_getErrorMessage(int error)
{
    const char* str;
    if ((error & DLMS_ERROR_TYPE_EXCEPTION_RESPONSE) != 0)
    {
        switch (error & 0xFF)
        {
        case DLMS_EXCEPTION_SERVICE_ERROR_OPERATION_NOT_POSSIBLE:
            str = GET_STR_FROM_EEPROM("OperationNotPossible");
        case DLMS_EXCEPTION_SERVICE_ERROR_SERVICE_NOT_SUPPORTED:
            str = GET_STR_FROM_EEPROM("ServiceNotSupported");
        case DLMS_EXCEPTION_SERVICE_ERROR_OTHER_REASON:
            str = GET_STR_FROM_EEPROM("OtherReason");
        case DLMS_EXCEPTION_SERVICE_ERROR_PDU_TOO_LONG:
            str = GET_STR_FROM_EEPROM("PduTooLong");
        case DLMS_EXCEPTION_SERVICE_ERROR_DECIPHERING_ERROR:
            str = GET_STR_FROM_EEPROM("DecipheringError");
        case DLMS_EXCEPTION_SERVICE_ERROR_INVOCATION_COUNTER_ERROR:
            str = GET_STR_FROM_EEPROM("InvocationCounterError");
        default:
            str = GET_STR_FROM_EEPROM("Unknown Exception response.");
        }
    }
    else if ((error & DLMS_ERROR_TYPE_CONFIRMED_SERVICE_ERROR) != 0)
    {
        str = GET_STR_FROM_EEPROM("Confirmed Service Error.");
    }
    else if ((error & DLMS_ERROR_TYPE_COMMUNICATION_ERROR) != 0)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if defined(_WIN32) || defined(_WIN64)
        wchar_t* s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error & ~DLMS_ERROR_TYPE_COMMUNICATION_ERROR,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, NULL);
        printf("%S\n", s);
        LocalFree(s);
        str = GET_STR_FROM_EEPROM("Connection error.");
#else
        str = strerror(error & ~DLMS_ERROR_TYPE_COMMUNICATION_ERROR);
#endif
        str = GET_STR_FROM_EEPROM("Connection error.");
#else
        str = GET_STR_FROM_EEPROM("Connection error.");
#endif
    }
    else
    {
        switch (error)
        {
        case DLMS_ERROR_CODE_OK:
            str = GET_STR_FROM_EEPROM("OK");
            break;
        case DLMS_ERROR_CODE_INVALID_PARAMETER:
            str = GET_STR_FROM_EEPROM("Invalid parameter.");
            break;
        case DLMS_ERROR_CODE_NOT_INITIALIZED:
            str = GET_STR_FROM_EEPROM("Server is not initialized.");
            break;
        case DLMS_ERROR_CODE_OUTOFMEMORY:
            str = GET_STR_FROM_EEPROM("Not enough memory available.");
            break;
        case DLMS_ERROR_CODE_NOT_REPLY:
            str = GET_STR_FROM_EEPROM("Packet is not a reply for a send packet.");
            break;
        case DLMS_ERROR_CODE_REJECTED:
            str = GET_STR_FROM_EEPROM("Meter rejects send packet.");
            break;
        case DLMS_ERROR_CODE_INVALID_LOGICAL_NAME:
            str = GET_STR_FROM_EEPROM("Invalid Logical Name.");
            break;
        case DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS:
            str = GET_STR_FROM_EEPROM("Client HDLC Address is not set.");
            break;
        case DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS:
            str = GET_STR_FROM_EEPROM("Server HDLC Address is not set.");
            break;
        case DLMS_ERROR_CODE_INVALID_DATA_FORMAT:
            str = GET_STR_FROM_EEPROM("Not a HDLC frame.");
            break;
        case DLMS_ERROR_CODE_INVALID_VERSION_NUMBER:
            str = GET_STR_FROM_EEPROM("Invalid DLMS version number.");
            break;
        case DLMS_ERROR_CODE_CLIENT_ADDRESS_NO_NOT_MATCH:
            str = GET_STR_FROM_EEPROM("Client addresses do not match.");
            break;
        case DLMS_ERROR_CODE_SERVER_ADDRESS_NO_NOT_MATCH:
            str = GET_STR_FROM_EEPROM("Server addresses do not match.");
            break;
        case DLMS_ERROR_CODE_WRONG_CRC:
            str = GET_STR_FROM_EEPROM("CRC do not match.");
            break;
        case DLMS_ERROR_CODE_INVALID_RESPONSE:
            str = GET_STR_FROM_EEPROM("Invalid response");
            break;
        case DLMS_ERROR_CODE_INVALID_TAG:
            str = GET_STR_FROM_EEPROM("Invalid Tag.");
            break;
        case DLMS_ERROR_CODE_ENCODING_FAILED:
            str = GET_STR_FROM_EEPROM("Encoding failed. Not enough data.");
            break;
        case DLMS_ERROR_CODE_REJECTED_PERMAMENT:
            str = GET_STR_FROM_EEPROM("Rejected permament.");
            break;
        case DLMS_ERROR_CODE_REJECTED_TRANSIENT:
            str = GET_STR_FROM_EEPROM("Rejected transient.");
            break;
        case DLMS_ERROR_CODE_NO_REASON_GIVEN:
            str = GET_STR_FROM_EEPROM("No reason given.");
            break;
        case DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED:
            str = GET_STR_FROM_EEPROM("Application context name not supported.");
            break;
        case DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED:
            str = GET_STR_FROM_EEPROM("Authentication mechanism name not recognised.");
            break;
        case DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_REQUIRED:
            str = GET_STR_FROM_EEPROM("Authentication mechanism name required.");
            break;
        case DLMS_ERROR_CODE_AUTHENTICATION_FAILURE:
            str = GET_STR_FROM_EEPROM("Authentication failure.");
            break;
        case DLMS_ERROR_CODE_AUTHENTICATION_REQUIRED:
            str = GET_STR_FROM_EEPROM("Authentication required.");
            break;
        case DLMS_ERROR_CODE_HARDWARE_FAULT:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports a hardware fault.");
            break;
        case DLMS_ERROR_CODE_TEMPORARY_FAILURE:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports a temporary failure.");
            break;
        case DLMS_ERROR_CODE_READ_WRITE_DENIED:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports Read-Write denied.");
            break;
        case DLMS_ERROR_CODE_UNDEFINED_OBJECT:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports a undefined object.");
            break;
        case DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports a inconsistent Class or Object.");
            break;
        case DLMS_ERROR_CODE_UNAVAILABLE_OBJECT:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports a unavailable object.");
            break;
        case DLMS_ERROR_CODE_UNMATCH_TYPE:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports a unmatched type.");
            break;
        case DLMS_ERROR_CODE_ACCESS_VIOLATED:
            str = GET_STR_FROM_EEPROM("Access Error : Device reports scope of access violated.");
            break;
        case DLMS_ERROR_CODE_DATA_BLOCK_UNAVAILABLE:
            str = GET_STR_FROM_EEPROM("Access Error : Data Block Unavailable.");
            break;
        case DLMS_ERROR_CODE_LONG_GET_OR_READ_ABORTED:
            str = GET_STR_FROM_EEPROM("Access Error : Long Get Or Read Aborted.");
            break;
        case DLMS_ERROR_CODE_NO_LONG_GET_OR_READ_IN_PROGRESS:
            str = GET_STR_FROM_EEPROM("Access Error : No Long Get Or Read In Progress.");
            break;
        case DLMS_ERROR_CODE_LONG_SET_OR_WRITE_ABORTED:
            str = GET_STR_FROM_EEPROM("Access Error : Long Set Or Write Aborted.");
            break;
        case DLMS_ERROR_CODE_NO_LONG_SET_OR_WRITE_IN_PROGRESS:
            str = GET_STR_FROM_EEPROM("Access Error : No Long Set Or Write In Progress.");
            break;
        case DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID:
            str = GET_STR_FROM_EEPROM("Access Error : Data Block Number Invalid.");
            break;
        case DLMS_ERROR_CODE_OTHER_REASON:
            str = GET_STR_FROM_EEPROM("Access Error : Other Reason.");
            break;
        case DLMS_ERROR_CODE_UNKNOWN:
            str = GET_STR_FROM_EEPROM("Unknown error.");
            break;
        case DLMS_ERROR_CODE_SEND_FAILED:
            str = GET_STR_FROM_EEPROM("Data send failed.");
            break;
        case DLMS_ERROR_CODE_RECEIVE_FAILED:
            str = GET_STR_FROM_EEPROM("Data receive failed.");
            break;
        case DLMS_ERROR_CODE_NOT_IMPLEMENTED:
            str = GET_STR_FROM_EEPROM("Not implemeted.");
            break;
        case DLMS_ERROR_CODE_INVALID_INVOKE_ID:
            str = GET_STR_FROM_EEPROM("Invalid Invoke ID.");
            break;
        case DLMS_ERROR_CODE_INVOCATION_COUNTER_TOO_SMALL:
            str = GET_STR_FROM_EEPROM("Invocation counter value is too small.");
            break;
        case DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR:
            str = GET_STR_FROM_EEPROM("Deciphering error.");
            break;
        case DLMS_ERROR_CODE_INVALID_SECURITY_SUITE:
            str = GET_STR_FROM_EEPROM("Client try to connect with wrong security suite.");
            break;
        default:
            str = GET_STR_FROM_EEPROM("Unknown error.");
            break;
        }
    }
    return str;
}

unsigned char hlp_getObjectCountSizeInBytes(uint32_t count)
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
            uint16_t value;
            ret = bb_getUInt16(buff, &value);
            if (ret != 0)
            {
                return -1;
            }
            return value;
        }
        else if (ch == 0x83)
        {
            uint32_t value;
            ret = bb_getUInt24(buff, &value);
            if (ret != 0)
            {
                return -1;
            }
            return value;
        }
        else if (ch == 0x84)
        {
            uint32_t value;
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

int hlp_getObjectCount2(gxByteBuffer* buff, uint16_t* count)
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
        else if (ch == 0x83)
        {
            uint32_t value;
            ret = bb_getUInt24(buff, &value);
            *count = (uint16_t)value;
        }
        else if (ch == 0x84)
        {
            uint32_t value;
            ret = bb_getUInt32(buff, &value);
            *count = (uint16_t)value;
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
int hlp_setObjectCount(uint32_t count, gxByteBuffer* buff)
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
            ret = bb_setUInt16(buff, (uint16_t)count);
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

#ifndef DLMS_IGNORE_MALLOC
char* hlp_bytesToHex(const unsigned char* bytes, int count)
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
                tmp = bytes[pos] & 0xFF;
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
#endif //DLMS_IGNORE_MALLOC

int hlp_bytesToHex2(const unsigned char* bytes, uint16_t count, char* buff, uint16_t size)
{
    const char hexArray[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    unsigned char tmp;
    int pos;
    if (3 * count > size)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (count != 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            tmp = bytes[pos] & 0xFF;
            buff[pos * 3] = hexArray[tmp >> 4];
            buff[pos * 3 + 1] = hexArray[tmp & 0x0F];
            buff[pos * 3 + 2] = ' ';
        }
        buff[(3 * count) - 1] = '\0';
    }
    else
    {
        buff[0] = '\0';
    }
    return 0;
}

unsigned char hlp_getValue(char c)
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

#ifndef DLMS_IGNORE_MALLOC
int hlp_hexToBytes(
    const char* str,
    unsigned char** buffer,
    uint16_t* count)
{
    *count = 0;
    if (buffer != NULL && *buffer != NULL)
    {
        gxfree(*buffer);
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
                lastValue = hlp_getValue(*str);
            }
            else if (lastValue != -1)
            {
                tmp[*count] = (unsigned char)(lastValue << 4 | hlp_getValue(*str));
                lastValue = -1;
                ++* count;
            }
        }
        else if (lastValue != -1)
        {
            tmp[*count] = hlp_getValue(*str);
            lastValue = -1;
            ++* count;
        }
        ++str;
    }
    if (len / 2 != *count)
    {
#ifdef gxrealloc
        //If compiler supports realloc.
        *buffer = gxrealloc(*buffer, *count);
 #else
        //A few extra bytes are returned if compiler doesn't support realloc.
 #endif // gxrealloc  
    }
    return 0;
}
#endif //DLMS_IGNORE_MALLOC

int hlp_hexToBytes2(
    const char* str,
    unsigned char* buffer,
    uint16_t* count)
{
    uint16_t max = *count;
    *count = 0;
    if (buffer == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
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
    int lastValue = -1;
    for (int pos = 0; pos != len; ++pos)
    {
        if (*str >= '0' && *str < 'g')
        {
            if (lastValue == -1)
            {
                lastValue = hlp_getValue(*str);
            }
            else if (lastValue != -1)
            {
                buffer[*count] = (unsigned char)(lastValue << 4 | hlp_getValue(*str));
                lastValue = -1;
                ++* count;
            }
        }
        else if (lastValue != -1)
        {
            buffer[*count] = hlp_getValue(*str);
            lastValue = -1;
            ++* count;
        }
        if (max < *count)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        ++str;
    }
    return 0;
}

#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)
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
    //AVR compiler can't handle this if casting to char* is removed.
    while ((ch = (char*)strchr(pBuff, '.')) != NULL)
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
        printf("%s", ln);
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


#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)

int hlp_getLogicalNameToString(const unsigned char value[6], char* ln)
{
    int ret;
#if defined(_WIN32) || defined(_WIN64)
	#if _MSC_VER > 1000
        ret = sprintf_s(ln, 25, "%d.%d.%d.%d.%d.%d", value[0], value[1], value[2], value[3], value[4], value[5]);
	#else
	    ret = sprintf(ln, "%d.%d.%d.%d.%d.%d", value[0], value[1], value[2], value[3], value[4], value[5]);
	#endif  
    if (ret != -1)
    {
        ret = 0;
    }
#else
    ret = hlp_intToString(ln, 25, value[0], 0, 1);
    if (ret != -1)
    {
        ln += ret;
        *ln = '.';
        ++ln;
        ret = hlp_intToString(ln, 25, value[1], 0, 1);
        if (ret != -1)
        {
            ln += ret;
            *ln = '.';
            ++ln;
            ret = hlp_intToString(ln, 25, value[2], 0, 1);
            if (ret != -1)
            {
                ln += ret;
                *ln = '.';
                ++ln;
                ret = hlp_intToString(ln, 25, value[3], 0, 1);
                if (ret != -1)
                {
                    ln += ret;
                    *ln = '.';
                    ++ln;
                    ret = hlp_intToString(ln, 25, value[4], 0, 1);
                    if (ret != -1)
                    {
                        ln += ret;
                        *ln = '.';
                        ++ln;
                        ret = hlp_intToString(ln, 25, value[5], 0, 1);
                    }
                }
            }
        }
    }
    if (ret != -1)
    {
        ret = 0;
    }
#endif //WIN32
    return ret;
}

#if !defined(DLMS_IGNORE_MALLOC)
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
    //AVR compiler can't handle this if casting to char* is removed.
    while ((ch = (char*) strchr(pBuff, '.')) != NULL)
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
        memset(ln, 0, 6);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //!defined(DLMS_IGNORE_MALLOC)

void hlp_replace(gxByteBuffer* str, char oldCh, char newCh)
{
    uint32_t pos;
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
    double ret = 1;
    if (scaler > 0) 
    {
        while (scaler--)
        {
            ret *= 10;
        }
    }
    else if (scaler < 0)
    {
        while (scaler++)
        {
            ret /= 10;
        }
    }
    return ret; 
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

int hlp_intToString(char* str, int bufsize, int32_t value, unsigned char isSigned, unsigned char digits)
{
    int cnt = 0;
    int32_t val = value;
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
        val = value;
        ++cnt;
    }
    if (digits != 0)
    {
        --digits;
    }
    //Find length.
    while ((val = (val / 10)) > 0)
    {
        ++str;
        if (digits != 0)
        {
            --digits;
        }
    }
    *(str + digits + 1) = '\0';
    while (digits != 0)
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = '0';
        --digits;
        --bufsize;
        ++str;
        ++cnt;
    }
    do
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = (value % 10) + '0';
        value /= 10;
		if (value != 0)
		{
        	--str;
		}
        --bufsize;
        ++cnt;
    } while (value != 0);
    return cnt;
}


int hlp_uint64ToString(char* str, int bufsize, uint64_t value, unsigned char digits)
{
    int cnt = 0;
    uint64_t val = value;
    if (digits != 0)
    {
        --digits;
    }
    //Find length.
    while ((val = (val / 10)) > 0)
    {
        ++str;
        if (digits != 0)
        {
            --digits;
        }
    }
    *(str + digits + 1) = '\0';
    while (digits != 0)
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = '0';
        --digits;
        --bufsize;
        ++str;
        ++cnt;
    }
    do
    {
        if (bufsize < 1)
        {
            return -1;
        }
        *str = (value % 10) + '0';
        value /= 10;
        if (value != 0)
        {
            --str;
        }
        --bufsize;
        ++cnt;
    } while (value != 0);
    return cnt;
}

int32_t hlp_stringToInt(const char* str)
{
    if (str == NULL)
    {
        return -1;
    }
    int32_t value = 0;
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

int hlp_int64ToString(char* str, int bufsize, int64_t value, unsigned char isSigned)
{
    int cnt = 0;
    int64_t val = value;
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
    while ((val = (val / 10)) > 0)
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

int64_t hlp_stringToInt64(const char* str)
{
    if (str == NULL)
    {
        return -1;
    }
    int32_t value = 0;
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
uint16_t lfsr = 0xACE1u;
unsigned bit;

unsigned char hlp_rand(void)
{
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    return (unsigned char)(lfsr = (uint16_t)((lfsr >> 1) | (bit << 15)));
}

unsigned char hlp_swapBits(unsigned char value)
{
    unsigned char ret = 0, pos;
    for (pos = 0; pos != 8; ++pos)
    {
        ret = (unsigned char)((ret << 1) | (value & 0x01));
        value = (unsigned char)(value >> 1);
    }
    return ret;
}

int hlp_add(bitArray* arr, gxByteBuffer* bytes, uint16_t count)
{
    uint16_t pos, bytePos = 0;
    int ret;
    unsigned char ch = 0;
    if (count == 0xFFFF)
    {
        count = (uint16_t)(bytes->size - bytes->position);
    }
    for (pos = 0; pos != count; ++pos)
    {
        //Get next byte.
        if ((pos % 8) == 0)
        {
            bytePos = 7;
            ret = bb_getUInt8ByIndex(bytes, bytes->position, &ch);
            if (ret != 0)
            {
                return ret;
            }
            ++bytes->position;
        }
        if ((ret = ba_setByIndex(arr, pos, (unsigned char)(ch & (1 << bytePos)))) != 0)
        {
            return ret;
        }
        --bytePos;
        ++arr->size;
    }
    return 0;
}