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
#include <userint.h>
#include <tcpsupp.h>
#include <stdlib.h> // malloc and free needs this or error is generated.
#include <stdio.h>
#include "../include/communication.h"
#include "../dlms/include/gxkey.h"
#include "../dlms/include/converters.h"
#include "../dlms/include/cosem.h"


//Client don't need this.
unsigned char svr_isTarget(
    dlmsSettings* settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    return 0;
}

//Client don't need this.
int svr_connected(
    dlmsServerSettings* settings) {
    return 0;

}

//Client don't need this.
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ_WRITE;
}

//Client don't need this.
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

//Client don't need this.
  void svr_preGet(
        dlmsSettings* settings,
        gxValueEventCollection* args)
  {
  }
//Client don't need this.
    void svr_postGet(
        dlmsSettings* settings,
        gxValueEventCollection* args)
{
}
//Client don't need this.  
void svr_preRead(
        dlmsSettings* settings,
        gxValueEventCollection* args)
		{
  }
//Client don't need this.
	void svr_preWrite(
        dlmsSettings* settings,
        gxValueEventCollection* args)
	{
  }
	//Client don't need this.    
   void svr_preAction(
        dlmsSettings* settings,
        gxValueEventCollection* args)
	{
  	}
	//Client don't need this.
	void svr_postRead(
        dlmsSettings* settings,
        gxValueEventCollection* args)
	{
  }
//Client don't need this.
	void svr_postWrite(
        dlmsSettings* settings,
        gxValueEventCollection* args)
	{
  }
	//Client don't need this.
	 void svr_postAction(
        dlmsSettings* settings,
        gxValueEventCollection* args)
	{
  }
	//Client don't need this.
    DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
        dlmsServerSettings* settings,
        DLMS_AUTHENTICATION authentication,
        gxByteBuffer* password)
	{
		return DLMS_SOURCE_DIAGNOSTIC_NONE;
  	}
	//Client don't need this.
    int svr_findObject(
        dlmsSettings* settings,
        DLMS_OBJECT_TYPE objectType,
        int sn,
        unsigned char* ln,
        gxValueEventArg* e)
	{
		return 0;
  	}
 	//Client don't need this.
    extern uint32_t time_elapsed(void)
	{
		return 0;
	}

    void svr_getDataType(
        dlmsSettings* settings,
        gxValueEventCollection* args)
	{
  }

    //Client don't need this.
    int svr_invalidConnection(dlmsServerSettings* settings)
	{
		return 0;
	}

    //Client don't need this.
    int svr_disconnected(dlmsServerSettings* settings)
	{
		return 0;
	}

	
//Returns current time.
//If you are not using operating system you have to implement this by yourself.
//Reason for this is that all compilers's or HWs don't support time at all.
void time_now(gxtime* value)
{
    time_initUnix(value, (unsigned long)time(NULL));
}

//Check is IP address IPv6 or IPv4 address.
unsigned char com_isIPv6Address(const char* pAddress)
{
    return strstr(pAddress, ":") != NULL;
}

int CVICALLBACK ClientTCPCB (unsigned handle, int event, int error,
                             void *callbackData)
{
    char receiveBuf[256] = {0};
    ssize_t dataSize         = sizeof (receiveBuf) - 1;
printf("\nMIKKO:\t");
    switch (event)
        {
        case TCP_DATAREADY:
            if ((dataSize = ClientTCPRead (handle, receiveBuf,
                                           dataSize, 1000)) < 0)
                {
                //MIKKO SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, "Receive Error\n");
                }
            else
            	{
            	receiveBuf[dataSize] = '\0';
				printf("\nMIKKO2:\t");
                }
            break;
        case TCP_DISCONNECT:
            //MessagePopup ("TCP Client", "Server has closed connection!");
            //MIKKO SetCtrlVal (handle, MAINPNL_CONNECTED, 0);
            // g_connected = 0;
            //MainPanelCB (0, EVENT_CLOSE, 0, 0, 0);
            break;
    }
    return 0;
}
//Make connection using TCP/IP connection.
int com_makeConnect(connection* connection, const char* address, int port, int waitTime)
{
    if (ConnectToTCPServer (&connection->socket, port, address, NULL,
                            NULL, waitTime) < 0)
	{
		  return DLMS_ERROR_CODE_INVALID_PARAMETER;
	}
    return DLMS_ERROR_CODE_OK;
}

int sendData(connection* connection, gxByteBuffer* data)
{
    int ret = 0;
    if (connection->comPort != INVALID_HANDLE_VALUE)
    {
		//Serial port communication is not supported at the moment.
    }
    else
    {
        if ((ret = ClientTCPWrite(connection->socket, (const char*)data->data, data->size, connection->waitTime)) < 0)
        {
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        }
    }
    return 0;
}

int readData(connection* connection, gxByteBuffer* data, int* index)
{
    if (connection->comPort != INVALID_HANDLE_VALUE)
    {
       
    }
    else
    {
		uint32_t cnt = connection->data.capacity - connection->data.size;
        if (cnt < 1)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
		if ((cnt = ClientTCPRead (connection->socket, (char*)connection->data.data + connection->data.size,
                                           cnt, connection->waitTime)) < 0)
		{
			return DLMS_ERROR_CODE_RECEIVE_FAILED;
		}				
        connection->data.size += cnt;
    }
    if (connection->trace > GX_TRACE_LEVEL_INFO)
    {
        char* hex = hlp_bytesToHex(connection->data.data + *index, connection->data.size - *index);
        if (*index == 0)
        {
            printf("\nRX:\t %s", hex);
        }
        else
        {
            printf(" %s", hex);
        }
        free(hex);
        *index = connection->data.size;
    }
    return 0;
}

// Read DLMS Data frame from the device.
int readDLMSPacket(
    connection* connection,
    gxByteBuffer* data,
    gxReplyData* reply)
{
    char* hex;
    int index = 0, ret;
    if (data->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    reply->complete = 0;
    connection->data.size = 0;
    connection->data.position = 0;
    if (connection->trace == GX_TRACE_LEVEL_VERBOSE)
    {
        hex = bb_toHexString(data);
        printf("\nTX:\t%s\n", hex);
        free(hex);
    }
    if ((ret = sendData(connection, data)) != 0)
    {
        return ret;
    }
    //Loop until packet is complete.
    unsigned char pos = 0;
    do
    {
        if ((ret = readData(connection, &connection->data, &index)) != 0)
        {
            if (ret != DLMS_ERROR_CODE_RECEIVE_FAILED || pos == 3)
            {
                break;
            }
            ++pos;
            printf("\nData send failed. Try to resend %d/3\n", pos);
            if ((ret = sendData(connection, data)) != 0)
            {
                break;
            }
        }
        else
        {
            ret = cl_getData(&connection->settings, &connection->data, reply);
            if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE)
            {
                break;
            }
        }
    } while (reply->complete == 0);
    if (connection->trace == GX_TRACE_LEVEL_VERBOSE)
    {
        printf("\n");
    }
    return ret;
}

int com_readDataBlock(
    connection* connection,
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
int com_disconnect(
    connection* connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    reply_init(&reply);
    mes_init(&msg);
    if ((ret = cl_disconnectRequest(&connection->settings, &msg)) != 0 ||
        (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
    {
        //Show error but continue close.
        printf("Close failed.");
    }
    reply_clear(&reply);
    mes_clear(&msg);
    return ret;
}

//Close connection to the meter.
int com_close(
    connection* connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    //If client is closed.
    if (!connection->settings.server)
    {
        reply_init(&reply);
        mes_init(&msg);
        if ((ret = cl_releaseRequest(&connection->settings, &msg)) != 0 ||
            (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
        {
            //Show error but continue close.
            printf("Release failed.");
        }
        reply_clear(&reply);
        mes_clear(&msg);

        if ((ret = cl_disconnectRequest(&connection->settings, &msg)) != 0 ||
            (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
        {
            //Show error but continue close.
            printf("Close failed.");
        }
        reply_clear(&reply);
        mes_clear(&msg);
    }
    if (connection->socket != -1)
    {
        connection->closing = 1;
        connection->socket = -1;
    }
    else if (connection->comPort != INVALID_HANDLE_VALUE)
    {
        connection->comPort = INVALID_HANDLE_VALUE;
    }
    cl_clear(&connection->settings);
    return ret;
}

//Read Invocation counter (frame counter) from the meter and update it.
int com_updateInvocationCounter(
    connection* connection,
    const char* invocationCounter)
{
    int ret = DLMS_ERROR_CODE_OK;
    //Read frame counter if GeneralProtection is used.
    if (invocationCounter != NULL && connection->settings.cipher.security != DLMS_SECURITY_NONE)
    {
        message messages;
        gxReplyData reply;
        unsigned short add = connection->settings.clientAddress;
        DLMS_AUTHENTICATION auth = connection->settings.authentication;
        DLMS_SECURITY security = connection->settings.cipher.security;
        gxByteBuffer challenge;
        bb_init(&challenge);
        bb_set(&challenge, connection->settings.ctoSChallenge.data, connection->settings.ctoSChallenge.size);
        connection->settings.clientAddress = 16;
        connection->settings.authentication = DLMS_AUTHENTICATION_NONE;
        connection->settings.cipher.security = DLMS_SECURITY_NONE;
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("updateInvocationCounter\r\n");
        }
        mes_init(&messages);
        reply_init(&reply);
        //Get meter's send and receive buffers size.
        if ((ret = cl_snrmRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseUAResponse(&connection->settings, &reply.data)) != 0)
        {
            bb_clear(&challenge);
            mes_clear(&messages);
            reply_clear(&reply);
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("SNRMRequest failed %s\r\n", hlp_getErrorMessage(ret));
            }
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
        if ((ret = cl_aarqRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseAAREResponse(&connection->settings, &reply.data)) != 0)
        {
            bb_clear(&challenge);
            mes_clear(&messages);
            reply_clear(&reply);
            if (ret == DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
            {
                if (connection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Use Logical Name referencing is wrong. Change it!\r\n");
                }
                return ret;
            }
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("AARQRequest failed %s\r\n", hlp_getErrorMessage(ret));
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
        gxData d;
        cosem_init(BASE(d), DLMS_OBJECT_TYPE_DATA, invocationCounter);
        if ((ret = com_read(connection, BASE(d), 2)) == 0)
        {
            connection->settings.cipher.invocationCounter = 1 + var_toInteger(&d.value);
            if (connection->trace > GX_TRACE_LEVEL_WARNING)
            {
                printf("Invocation counter: %ld (0x%lX)\r\n",
                    connection->settings.cipher.invocationCounter,
                    connection->settings.cipher.invocationCounter);
            }
            //It's OK if this fails.
            com_disconnect(connection);
            connection->settings.clientAddress = add;
            connection->settings.authentication = auth;
            connection->settings.cipher.security = security;
            bb_clear(&connection->settings.ctoSChallenge);
            bb_set(&connection->settings.ctoSChallenge, challenge.data, challenge.size);
            bb_clear(&challenge);
        }
    }
    return ret;
}

//Initialize connection to the meter.
int com_initializeConnection(
    connection* connection)
{
    message messages;
    gxReplyData reply;
    int ret = 0;
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("InitializeConnection\r\n");
    }

    mes_init(&messages);
    reply_init(&reply);
    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&connection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&connection->settings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        if (connection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("SNRMRequest failed %s\r\n", hlp_getErrorMessage(ret));
        }
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
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Use Logical Name referencing is wrong. Change it!\r\n");
            }
            return ret;
        }
        if (connection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("AARQRequest failed %s\r\n", hlp_getErrorMessage(ret));
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
    hlp_getLogicalNameToString(object->logicalName, ln);
    printf("%s %s %s:%d %s\r\n", description, obj_typeToString2(object->objectType), ln, attributeOrdinal, hlp_getErrorMessage(ret));
}

//Get Association view.
int com_getAssociationView(connection* connection)
{
    int ret;
    message data;
    gxReplyData reply;
    printf("GetAssociationView\r\n");
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_getObjectsRequest(&connection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_parseObjects(&connection->settings, &reply.data)) != 0)
    {
        printf("GetObjects failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}


//Read object.
int com_read(
    connection* connection,
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

int com_getKeepAlive(
    connection* connection)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_getKeepAlive(&connection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
    {
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_write(
    connection* connection,
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
    connection* connection,
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
    connection* connection,
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
            //NOTE! Set ignore value to true because list is parsed differently than normal read.
            reply.ignoreValue = 1;
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
                    //NOTE! Set ignore value to true because list is parsed differently than normal read.
                    reply.ignoreValue = 1;
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
    connection* connection,
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

int com_readRowsByEntry2(
    connection* connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count,
    unsigned short colStartIndex,
    unsigned short colEndIndex)
{
    int ret;
    message data;
    gxReplyData reply;
    gxByteBuffer bb;
    char* str;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByEntry2(&connection->settings, object, index, count, colStartIndex, colEndIndex, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    else
    {
        bb_init(&bb);
        var_toString(&reply.dataValue, &bb);
        str = bb_toString(&bb);
        printf(str);
        bb_clear(&bb);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange(
    connection* connection,
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
int com_readRowsByRange2(
    connection* connection,
    gxProfileGeneric* object,
    gxtime* start,
    gxtime* end)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByRange2(&connection->settings, object, start, end, &data)) != 0 ||
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
    connection* connection)
{
    gxObject* obj;
    int ret;
	uint16_t pos;
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
    connection* connection)
{
    int ret;
	uint16_t pos;
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
    connection* connection)
{
    gxtime startTime, endTime;
    int ret;
	uint16_t pos;
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
        ret = com_readRowsByRange2(connection, pg, &startTime, &endTime);
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

int com_readValue(connection* connection, gxObject* object, unsigned char index)
{
    int ret;
    char* data = NULL;
    char ln[25];
    ret = hlp_getLogicalNameToString(object->logicalName, ln);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("-------- Reading Object %s %s\r\n", obj_typeToString2(object->objectType), ln);
    }
    ret = com_read(connection, object, index);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        if (connection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("Failed to read object %s %s attribute index %d\r\n", obj_typeToString2(object->objectType), ln, index);
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
int com_readValues(connection* connection)
{
    gxByteBuffer attributes;
    unsigned char ch;
    char* data = NULL;
    char ln[25];
    gxObject* object;
    unsigned long index;
    int ret = 0;
	uint16_t pos;
    bb_init(&attributes);
    for (pos = 0; pos != connection->settings.objects.size; ++pos)
    {
        ret = oa_getByIndex(&connection->settings.objects, pos, &object);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ///////////////////////////////////////////////////////////////////////////////////
        // Profile generics are read later because they are special cases.
        // (There might be so lots of data and we so not want waste time to read all the data.)
        if (object->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            continue;
        }
        ret = hlp_getLogicalNameToString(object->logicalName, ln);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("-------- Reading Object %s %s\n", obj_typeToString2(object->objectType), ln);
        }
        ret = obj_getAttributeIndexToRead(object, &attributes);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        for (index = 0; index < attributes.size; ++index)
        {
            ret = bb_getUInt8ByIndex(&attributes, index, &ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            ret = com_read(connection, object, ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                if (connection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Failed to read object %s %s attribute index %d\r\n", obj_typeToString2(object->objectType), ln, ch);
                }
                //Return error if not DLMS error.
                if (ret != DLMS_ERROR_CODE_READ_WRITE_DENIED)
                {
                    continue;
                }
                ret = 0;
            }
        }
        if (ret != 0)
        {
            break;
        }
        bb_clear(&attributes);
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            ret = obj_toString(object, &data);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
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

int com_readAllObjects(connection* connection)
{
    int ret;
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