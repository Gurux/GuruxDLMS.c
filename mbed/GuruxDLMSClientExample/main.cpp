//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
//
// Version:         $Revision: 10346 $,
//                  $Date: 2018-10-29 16:08:18 +0200 (Mon, 29 Oct 2018) $
//                  $Author: gurux01 $
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
// More information of Gurux DLMS/COSEM Director: http://www.gurux.org/GXDLMSDirector
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include <mbed.h>

//---------------------------------------------------------------------------
//Gurux framework includes
//---------------------------------------------------------------------------
#include "dlms/include/dlmssettings.h"
#include "dlms/include/variant.h"
#include "dlms/include/cosem.h"
#include "dlms/include/client.h"
#include "dlms/include/converters.h"
#include "dlms/include/gxobjects.h"
#include "dlms/include/gxobjects.h"

RawSerial      pc(USBTX, USBRX);

//Received data.
gxByteBuffer frameData;
dlmsSettings meterSettings;

//Client don't need this.
unsigned char svr_isTarget(
    dlmsSettings *settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    return 0;
}

//Client don't need this.
int svr_connected(
    dlmsServerSettings *settings) {
    return 0;
}

void time_now(
    gxtime* value)
{
    //Get local time somewhere.
    time_t tm1 = time(NULL);
    struct tm dt = *localtime(&tm1);
    time_init2(value, &dt);
}

int com_readSerialPort(
    unsigned char eop)
{
    //Read reply data.
    int pos;
    unsigned char eopFound = 0;
    unsigned short lastReadIndex = 0;
    int p = 0;
    frameData.size = 0;
    frameData.position = 0;
    do
    {
        frameData.data[frameData.size] = pc.getc();
        ++frameData.size;
        //Search eop.
        if (frameData.size > 5)
        {
            //Some optical strobes can return extra bytes.
            for (pos = frameData.size - 1; pos != lastReadIndex; --pos)
            {
                if (frameData.data[pos] == eop)
                {
                    eopFound = 1;
                    break;
                }
            }
            lastReadIndex = pos;
        }
    } while (eopFound == 0);
    return DLMS_ERROR_CODE_OK;
}

// Read DLMS Data frame from the device.
int readDLMSPacket(
    gxByteBuffer* data,
    gxReplyData* reply)
{
    int pos = 0, ret = DLMS_ERROR_CODE_OK;
    if (data->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    reply->complete = 0;
    frameData.size = 0;
    frameData.position = 0;
    //Send data.
    for (pos = 0; pos != data->size; ++pos)
    {
        pc.putc(data->data[pos]);
    }
    //Loop until packet is complete.
    do
    {
        if (com_readSerialPort(0x7E) != 0)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
        ret = cl_getData(&meterSettings, &frameData, reply);
        if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE)
        {
            break;
        }
    } while (reply->complete == 0);
    return ret;
}

int com_readDataBlock(
    message* messages,
    gxReplyData* reply)
{
    gxByteBuffer rr;
    int pos, ret = DLMS_ERROR_CODE_OK;
    //If there is no data to send.
    if (messages->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    bb_init(&rr);
    //Send data.
    for (pos = 0; pos != messages->size; ++pos)
    {
        //Send data.
        if ((ret = readDLMSPacket(messages->data[pos], reply)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        //Check is there errors or more data from server
        while (reply_isMoreData(reply))
        {
            if ((ret = cl_receiverReady(&meterSettings, reply->moreData, &rr)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            if ((ret = readDLMSPacket(&rr, reply)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            bb_clear(&rr);
        }
    }
    return ret;
}

//Close connection to the meter.
int com_close()
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    reply_init(&reply);
    mes_init(&msg);
    if ((ret = cl_releaseRequest(&meterSettings, &msg)) != 0 ||
        (ret = com_readDataBlock(&msg, &reply)) != 0)
    {
        //Show error but continue close.
    }
    reply_clear(&reply);
    mes_clear(&msg);

    if ((ret = cl_disconnectRequest(&meterSettings, &msg)) != 0 ||
        (ret = com_readDataBlock(&msg, &reply)) != 0)
    {
        //Show error but continue close.
    }
    reply_clear(&reply);
    mes_clear(&msg);
    cl_clear(&meterSettings);
    return ret;
}

//Initialize connection to the meter.
int com_initializeConnection()
{
    int ret = DLMS_ERROR_CODE_OK;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);

    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&meterSettings, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&meterSettings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if ((ret = cl_aarqRequest(&meterSettings, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0 ||
        (ret = cl_parseAAREResponse(&meterSettings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        if (ret == DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
        {
            return ret;
        }
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    // Get challenge Is HLS authentication is used.
    if (meterSettings.authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = cl_getApplicationAssociationRequest(&meterSettings, &messages)) != 0 ||
            (ret = com_readDataBlock(&messages, &reply)) != 0 ||
            (ret = cl_parseApplicationAssociationResponse(&meterSettings, &reply.data)) != 0)
        {
            mes_clear(&messages);
            reply_clear(&reply);
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
    }
    return DLMS_ERROR_CODE_OK;
}

//Report error on output;
void com_reportError(const char* description,
    gxObject* object,
    unsigned char attributeOrdinal, int ret)
{
    char ln[25];
    char type[30];
    /*
      hlp_getLogicalNameToString(object->logicalName, ln);
      obj_typeToString(object->objectType, type);
      printf("%s %s %s:%d %s\r\n", description, type, ln, attributeOrdinal, hlp_getErrorMessage(ret));
    */
}

//Get Association view.
int com_getAssociationView()
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_getObjectsRequest(&meterSettings, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0 ||
        (ret = cl_parseObjects(&meterSettings, &reply.data)) != 0)
    {
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

//Read object.
int com_read(
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_read(&meterSettings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0 ||
        (ret = cl_updateValue(&meterSettings, object, attributeOrdinal, &reply.dataValue)) != 0)
    {
        com_reportError("ReadObject failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_write(
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_write(&meterSettings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0)
    {
        com_reportError("Write failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_method(
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params)
{
    int ret;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);
    if ((ret = cl_method(&meterSettings, object, attributeOrdinal, params, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0)
    {
        //    printf("Method failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}

//Read objects.
int com_readList(
    gxArray* list)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer bb, rr;
    message messages;
    gxReplyData reply;
    if (list->size != 0)
    {
        mes_init(&messages);
        if ((ret = cl_readList(&meterSettings, list, &messages)) != 0)
        {
            //  printf("ReadList failed %s\r\n", hlp_getErrorMessage(ret));
        }
        else
        {
            reply_init(&reply);
            bb_init(&rr);
            bb_init(&bb);
            //Send data.
            for (pos = 0; pos != messages.size; ++pos)
            {
                //Send data.
                reply_clear(&reply);
                if ((ret = readDLMSPacket(messages.data[pos], &reply)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                //Check is there errors or more data from server
                while (reply_isMoreData(&reply))
                {
                    if ((ret = cl_receiverReady(&meterSettings, reply.moreData, &rr)) != DLMS_ERROR_CODE_OK ||
                        (ret = readDLMSPacket(&rr, &reply)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    bb_clear(&rr);
                }
                bb_set2(&bb, &reply.data, reply.data.position, -1);
            }
            if (ret == 0)
            {
                ret = cl_updateValues(&meterSettings, list, &bb);
            }
            bb_clear(&bb);
            bb_clear(&rr);
            reply_clear(&reply);
        }
        mes_clear(&messages);
    }
    return ret;
}

int com_readRowsByEntry(
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByEntry(&meterSettings, object, index, count, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0 ||
        (ret = cl_updateValue(&meterSettings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        //  printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange(
    gxProfileGeneric* object,
    struct tm* start,
    struct tm* end)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByRange(&meterSettings, object, start, end, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0 ||
        (ret = cl_updateValue(&meterSettings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        //    printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
//Read scalers and units. They are static so they are read only once.
int com_readScalerAndUnits()
{
    gxObject* obj;
    int ret, pos;
    objectArray objects;
    gxArray list;
    gxObject* object;
    DLMS_OBJECT_TYPE types[] = { DLMS_OBJECT_TYPE_EXTENDED_REGISTER, DLMS_OBJECT_TYPE_REGISTER, DLMS_OBJECT_TYPE_DEMAND_REGISTER };
    oa_init(&objects);
    //Find registers and demand registers and read them.
    ret = oa_getObjects2(&meterSettings.objects, types, 3, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    if ((meterSettings.negotiatedConformance & DLMS_CONFORMANCE_MULTIPLE_REFERENCES) != 0)
    {
        arr_init(&list);
        //Try to read with list first. All meters do not support it.
        for (pos = 0; pos != meterSettings.objects.size; ++pos)
        {
            ret = oa_getByIndex(&meterSettings.objects, pos, &obj);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                oa_empty(&objects);
                arr_clear(&list);
                return ret;
            }
            if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER ||
                obj->objectType == DLMS_OBJECT_TYPE_EXTENDED_REGISTER)
            {
                arr_push(&list, key_init(obj, (void*)3));
            }
            else if (obj->objectType == DLMS_OBJECT_TYPE_DEMAND_REGISTER)
            {
                arr_push(&list, key_init(obj, (void*)4));
            }
        }
        ret = com_readList(&list);
        arr_clear(&list);
    }
    //If read list failed read items one by one.
    if (ret != 0)
    {
        for (pos = 0; pos != objects.size; ++pos)
        {
            ret = oa_getByIndex(&objects, pos, &object);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                oa_empty(&objects);
                return ret;
            }
            ret = com_read(object, object->objectType == DLMS_OBJECT_TYPE_DEMAND_REGISTER ? 4 : 3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                oa_empty(&objects);
                return ret;
            }
        }
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
//Read profile generic columns. They are static so they are read only once.
int com_readProfileGenericColumns()
{
    int ret, pos;
    objectArray objects;
    gxObject* object;
    oa_init(&objects);
    ret = oa_getObjects(&meterSettings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        oa_empty(&objects);
        return ret;
    }
    for (pos = 0; pos != objects.size; ++pos)
    {
        ret = oa_getByIndex(&objects, pos, &object);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ret = com_read(object, 3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
//Read profile generics rows.
int com_readProfileGenerics()
{
    gxtime startTime, endTime;
    int ret, pos;
    char str[50];
    char ln[25];
    char* data = NULL;
    gxByteBuffer ba;
    objectArray objects;
    gxProfileGeneric* pg;
    oa_init(&objects);
    ret = oa_getObjects(&meterSettings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        //Do not clear objects list because it will free also objects from association view list.
        oa_empty(&objects);
        return ret;
    }
    bb_init(&ba);
    for (pos = 0; pos != objects.size; ++pos)
    {
        ret = oa_getByIndex(&objects, pos, (gxObject**)&pg);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read entries in use.
        ret = com_read((gxObject*)pg, 7);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 7);
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read entries.
        ret = com_read((gxObject*)pg, 8);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 8);
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        printf("Entries: %ld/%ld\r\n", pg->entriesInUse, pg->profileEntries);
        //If there are no columns or rows.
        if (pg->entriesInUse == 0 || pg->captureObjects.size == 0)
        {
            continue;
        }
        //Read first row from Profile Generic.
        ret = com_readRowsByEntry(pg, 1, 1);
        //Read last day from Profile Generic.
        time_now(&startTime);
        endTime = startTime;
        time_clearTime(&startTime);
        ret = com_readRowsByRange(pg, &startTime.value, &endTime.value);
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

int com_readValue(gxObject* object, unsigned char index)
{
    int ret;
    char* data = NULL;
    ret = com_read(object, index);
    return ret;
}

// This function reads ALL objects that meter have excluded profile generic objects.
// It will loop all object's attributes.
int com_readValues()
{
    gxByteBuffer attributes;
    unsigned char ch;
    char* data = NULL;
    gxObject* object;
    unsigned long index;
    int ret, pos;
    bb_init(&attributes);

    for (pos = 0; pos != meterSettings.objects.size; ++pos)
    {
        ret = oa_getByIndex(&meterSettings.objects, pos, &object);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            bb_clear(&attributes);
            return ret;
        }
        ///////////////////////////////////////////////////////////////////////////////////
        // Profile generics are read later because they are special cases.
        // (There might be so lots of data and we so not want waste time to read all the data.)
        if (object->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            continue;
        }
        ret = obj_getAttributeIndexToRead(object, &attributes);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            bb_clear(&attributes);
            return ret;
        }
        for (index = 0; index < attributes.size; ++index)
        {
            ret = bb_getUInt8ByIndex(&attributes, index, &ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&attributes);
                return ret;
            }
            ret = com_read(object, ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                //Return error if not DLMS error.
                if (ret != DLMS_ERROR_CODE_READ_WRITE_DENIED)
                {
                    bb_clear(&attributes);
                    return ret;
                }
                ret = 0;
            }
        }
        bb_clear(&attributes);
    }
    bb_clear(&attributes);
    return ret;
}

//This function reads ALL objects that meter have. It will loop all object's attributes.
int com_readAllObjects()
{
    int ret;

    //Initialize connection.
    ret = com_initializeConnection();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Get objects from the meter and read them.
    ret = com_getAssociationView();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readScalerAndUnits();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Read Profile Generic columns.
    ret = com_readProfileGenericColumns();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readValues();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readProfileGenerics();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    return ret;
}

gxClock clock1;

int main(void) {
    int ret;
    bb_init(&frameData);
    //Set frame size.
    bb_capacity(&frameData, 128);
    cl_init(&meterSettings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);

    cosem_init(&clock1.base, DLMS_OBJECT_TYPE_CLOCK, "0.0.1.0.0.255");

    //Initialize connection.
    ret = com_initializeConnection();
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Read clock.
    ret = com_read(&clock1.base, 2);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    com_close();
    return 0;
}
