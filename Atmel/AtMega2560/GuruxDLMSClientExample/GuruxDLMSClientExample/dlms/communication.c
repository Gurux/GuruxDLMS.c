//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL:  $
//
// Version:         $Revision:  $,
//                  $Date:  $
//                  $Author: $
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
#include <atmel_start.h>
#include "include/errorcodes.h"
#include "communication.h"
#include "include/gxkey.h"
#include "include/converters.h"
//#include "driver_init.h"
//#include "utils.h"

int com_readSerialPort(
    connection *connection,
    unsigned char eop)
{
    //Read reply data.
    int pos;
    unsigned char eopFound = 0;
    unsigned short lastReadIndex = 0;
    do
    {
		io_read(connection->io, connection->data.data, 1);
        connection->data.size += 1;
        //Search eop.
        if (connection->data.size > 5)
        {
            //Some optical strobes can return extra bytes.
            for (pos = connection->data.size - 1; pos != lastReadIndex; --pos)
            {
                if (connection->data.data[pos] == eop)
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
    connection *connection,
    gxByteBuffer* data,
    gxReplyData* reply)
{
    int index = 0, ret = DLMS_ERROR_CODE_OK;
    if (data->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    reply->complete = 0;
    connection->data.size = 0;
    connection->data.position = 0;
	io_write(connection->io, data->data, data->size);
    //Loop until packet is complete.
    do
    {
        if (com_readSerialPort(connection, 0x7E) != 0)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
        ret = cl_getData(&connection->settings, &connection->data, reply);
        if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE)
        {
            break;
        }
    } while (reply->complete == 0);
    return ret;
}

int com_readDataBlock(
    connection *connection,
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
        if ((ret = readDLMSPacket(connection, messages->data[pos], reply)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        //Check is there errors or more data from server
        while (reply_isMoreData(reply))
        {
            if ((ret = cl_receiverReady(&connection->settings, reply->moreData, &rr)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            if ((ret = readDLMSPacket(connection, &rr, reply)) != DLMS_ERROR_CODE_OK)
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
int com_close(
    connection *connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    reply_init(&reply);
    mes_init(&msg);
    if ((ret = cl_releaseRequest(&connection->settings, &msg)) != 0 ||
        (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
    {
        //Show error but continue close.
    }
    reply_clear(&reply);
    mes_clear(&msg);

    if ((ret = cl_disconnectRequest(&connection->settings, &msg)) != 0 ||
        (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
    {
        //Show error but continue close.
    }
    reply_clear(&reply);
    mes_clear(&msg);
    cl_clear(&connection->settings);
    return ret;
}

//Initialize connection to the meter.
int com_initializeConnection(
    connection *connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);

    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&connection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&connection->settings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if ((ret = cl_aarqRequest(&connection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
        (ret = cl_parseAAREResponse(&connection->settings, &reply.data)) != 0)
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
    if (connection->settings.maxPduSize == 0xFFFF)
    {
        con_initializeBuffers(connection, connection->settings.maxPduSize);
    }
    else
    {
        //Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
        con_initializeBuffers(connection, 50 + connection->settings.maxPduSize);
    }

    // Get challenge Is HLS authentication is used.
    if (connection->settings.authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = cl_getApplicationAssociationRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseApplicationAssociationResponse(&connection->settings, &reply.data)) != 0)
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
int com_getAssociationView(connection *connection)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_getObjectsRequest(&connection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_parseObjects(&connection->settings, &reply.data)) != 0)
    {
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

//Read object.
int com_read(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_read(&connection->settings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, object, attributeOrdinal, &reply.dataValue)) != 0)
    {
        com_reportError("ReadObject failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_write(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_write(&connection->settings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
    {
        com_reportError("Write failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_method(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params)
{
    int ret;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);
    if ((ret = cl_method(&connection->settings, object, attributeOrdinal, params, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0)
    {
        printf("Method failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}

//Read objects.
int com_readList(
    connection *connection,
    gxArray* list)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer bb, rr;
    message messages;
    gxReplyData reply;
    if (list->size != 0)
    {
        mes_init(&messages);
        if ((ret = cl_readList(&connection->settings, list, &messages)) != 0)
        {
            printf("ReadList failed %s\r\n", hlp_getErrorMessage(ret));
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
                if ((ret = readDLMSPacket(connection, messages.data[pos], &reply)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                //Check is there errors or more data from server
                while (reply_isMoreData(&reply))
                {
                    if ((ret = cl_receiverReady(&connection->settings, reply.moreData, &rr)) != DLMS_ERROR_CODE_OK ||
                        (ret = readDLMSPacket(connection, &rr, &reply)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    bb_clear(&rr);
                }
                bb_set2(&bb, &reply.data, reply.data.position, -1);
            }
            if (ret == 0)
            {
                ret = cl_updateValues(&connection->settings, list, &bb);
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
    connection *connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByEntry(&connection->settings, object, index, count, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange(
    connection *connection,
    gxProfileGeneric* object,
    struct tm* start,
    struct tm* end)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByRange(&connection->settings, object, start, end, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
//Read scalers and units. They are static so they are read only once.
int com_readScalerAndUnits(
    connection *connection)
{
    gxObject* obj;
    int ret, pos;
    objectArray objects;
    gxArray list;
    gxObject* object;
    DLMS_OBJECT_TYPE types[] = { DLMS_OBJECT_TYPE_EXTENDED_REGISTER, DLMS_OBJECT_TYPE_REGISTER, DLMS_OBJECT_TYPE_DEMAND_REGISTER };
    oa_init(&objects);
    //Find registers and demand registers and read them.
    ret = oa_getObjects2(&connection->settings.objects, types, 3, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    if ((connection->settings.negotiatedConformance & DLMS_CONFORMANCE_MULTIPLE_REFERENCES) != 0)
    {
        arr_init(&list);
        //Try to read with list first. All meters do not support it.
        for (pos = 0; pos != connection->settings.objects.size; ++pos)
        {
            ret = oa_getByIndex(&connection->settings.objects, pos, &obj);
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
        ret = com_readList(connection, &list);
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
            ret = com_read(connection, object, object->objectType == DLMS_OBJECT_TYPE_DEMAND_REGISTER ? 4 : 3);
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
int com_readProfileGenericColumns(
    connection *connection)
{
    int ret, pos;
    objectArray objects;
    gxObject* object;
    oa_init(&objects);
    ret = oa_getObjects(&connection->settings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
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
        ret = com_read(connection, object, 3);
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
int com_readProfileGenerics(
    connection *connection)
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
    ret = oa_getObjects(&connection->settings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
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
        ret = com_read(connection, (gxObject*)pg, 7);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 7);
            }
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read entries.
        ret = com_read(connection, (gxObject*)pg, 8);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 8);
            }
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("Entries: %ld/%ld\r\n", pg->entriesInUse, pg->profileEntries);
        }
        //If there are no columns or rows.
        if (pg->entriesInUse == 0 || pg->captureObjects.size == 0)
        {
            continue;
        }
        //Read first row from Profile Generic.
        ret = com_readRowsByEntry(connection, pg, 1, 1);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s rows by entry\r\n", str, ln);
            }
        }
        else
        {
            if (connection->trace > GX_TRACE_LEVEL_WARNING)
            {
                bb_init(&ba);
                obj_rowsToString(&ba, &pg->buffer);
                data = bb_toString(&ba);
                bb_clear(&ba);
                printf("%s\r\n", data);
                free(data);
            }
        }
        //Read last day from Profile Generic.
        time_now(&startTime);
        endTime = startTime;
        time_clearTime(&startTime);
        ret = com_readRowsByRange(connection, pg, &startTime.value, &endTime.value);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s rows by entry\r\n", str, ln);
            }
        }
        else
        {
            if (connection->trace > GX_TRACE_LEVEL_WARNING)
            {
                bb_init(&ba);
                obj_rowsToString(&ba, &pg->buffer);
                data = bb_toString(&ba);
                bb_clear(&ba);
                printf("%s\r\n", data);
                free(data);
            }
        }
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

int com_readValue(connection *connection, gxObject* object, unsigned char index)
{
    int ret;
    char* data = NULL;
    char str[50];
    char ln[25];
    ret = obj_typeToString(object->objectType, str);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = hlp_getLogicalNameToString(object->logicalName, ln);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("-------- Reading Object %s %s\r\n", str, ln);
    }
    ret = com_read(connection, object, index);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        if (connection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("Failed to read object %s %s attribute index %d\r\n", str, ln, index);
        }
        //Return error if not DLMS error.
        if (ret != DLMS_ERROR_CODE_READ_WRITE_DENIED)
        {
            return ret;
        }
    }
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        ret = obj_toString(object, &data);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (data != NULL)
        {
            printf("%s", data);
            free(data);
            data = NULL;
        }
    }
    return 0;
}

// This function reads ALL objects that meter have excluded profile generic objects.
// It will loop all object's attributes.
int com_readValues(connection *connection)
{
    gxByteBuffer attributes;
    unsigned char ch;
    char* data = NULL;
    char str[50];
    char ln[25];
    gxObject* object;
    unsigned long index;
    int ret, pos;
    bb_init(&attributes);

    for (pos = 0; pos != connection->settings.objects.size; ++pos)
    {
        ret = oa_getByIndex(&connection->settings.objects, pos, &object);
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
        ret = obj_typeToString(object->objectType, str);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            bb_clear(&attributes);
            return ret;
        }
        ret = hlp_getLogicalNameToString(object->logicalName, ln);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            bb_clear(&attributes);
            return ret;
        }
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("-------- Reading Object %s %s\r\n", str, ln);
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
            ret = com_read(connection, object, ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                if (connection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Failed to read object %s %s attribute index %d\r\n", str, ln, ch);
                }
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
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            ret = obj_toString(object, &data);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&attributes);
                return ret;
            }
            if (data != NULL)
            {
                printf("%s", data);
                free(data);
                data = NULL;
            }
        }
    }
    bb_clear(&attributes);
    return ret;
}

//This function reads ALL objects that meter have. It will loop all object's attributes.
int com_readAllObjects(connection *connection)
{
    int ret;

    //Initialize connection.
    ret = com_initializeConnection(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Get objects from the meter and read them.
    ret = com_getAssociationView(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readScalerAndUnits(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Read Profile Generic columns.
    ret = com_readProfileGenericColumns(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readValues(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readProfileGenerics(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    return ret;
}