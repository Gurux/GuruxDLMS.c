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

#include <stdio.h>
#include <stdlib.h> // malloc and free needs this or error is generated.

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#if _MSC_VER > 1400
#include <crtdbg.h>
#include <malloc.h>
#endif
#include <tchar.h>
#include <conio.h>
#include <ws2tcpip.h>//Add support for sockets
#include <time.h>
#include <process.h>//Add support for threads
#endif
#if defined(__linux__) //Linux includes.
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h> //Add support for sockets
#include <unistd.h> //Add support for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#endif

#include "../include/connection.h"
#include "../include/communication.h"
#include "../dlms/include/converters.h"
#include "../dlms/include/helpers.h"
#include "../dlms/include/date.h"

#include "../include/exampleserver.h"
#include "../dlms/include/cosem.h"
#include "../dlms/include/gxkey.h"
#include "../dlms/include/gxobjects.h"
#include "../dlms/include/gxget.h"
#include "../dlms/include/notify.h"
#include "../dlms/include/gxset.h"
#include "../dlms/include/gxinvoke.h"

//Client connection.
clientConnection clientCon;
//Meter reply is saved for this buffer.
char* serialPort;

//If HDLC framing is used.
gxIecHdlcSetup iecHdlcSetup;
//If WRAPPER framing is used.
gxTcpUdpSetup udpSetup;

gxAssociationLogicalName target;

//All objects.
const gxObject* ALL_DC_OBJECTS[] = {
    BASE(iecHdlcSetup),
    //If WRAPPER framing is used.
    BASE(udpSetup)
};

//Returns the approximate processor time in ms.
uint32_t time_elapsed(void)
{
    return (uint32_t)clock() * 1000;
}

///////////////////////////////////////////////////////////////////////
//Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup()
{
    int ret;
    unsigned char ln[6] = { 0,0,22,0,0,255 };
    if ((ret = INIT_OBJECT(iecHdlcSetup, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
    {
        iecHdlcSetup.communicationSpeed = DLMS_BAUD_RATE_9600;
        iecHdlcSetup.windowSizeReceive = iecHdlcSetup.windowSizeTransmit = 1;
        iecHdlcSetup.maximumInfoLengthTransmit = iecHdlcSetup.maximumInfoLengthReceive = 128;
        iecHdlcSetup.inactivityTimeout = 120;
        iecHdlcSetup.deviceAddress = 0x10;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example TCP/UDP setup object.
///////////////////////////////////////////////////////////////////////
int addTcpUdpSetup(int port)
{
    int ret;
    //Add Tcp/Udp setup. Default Logical Name is 0.0.25.0.0.255.
    const unsigned char ln[6] = { 0,0,25,0,0,255 };
    if ((ret = INIT_OBJECT(udpSetup, DLMS_OBJECT_TYPE_TCP_UDP_SETUP, ln)) == 0)
    {
        udpSetup.port = port;
        udpSetup.maximumSimultaneousConnections = 1;
        udpSetup.maximumSegmentSize = WRAPPER_FRAME_SIZE;
        udpSetup.inactivityTimeout = 180;
    }
    return ret;
}

int svr_InitObjects(
    dlmsServerSettings* settings,
    char* meterPort,
    int serverPort,
    GX_TRACE_LEVEL trace)
{
    //GW uses pre-established connection.
    settings->info.preEstablished = 1;
    serialPort = meterPort;
    conCl_init(&clientCon, trace);
    //Initial setting for the client connection.
    cl_init(&clientCon.settings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
    return oa_verify(&settings->base.objects);
}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
/**
* Start server.
*/
int svr_start(
    connection* con,
    unsigned short port)
{
    int ret;
    if ((ret = svr_listen(con, port)) != 0)
    {
        return ret;
    }
    con->settings.hdlc = &iecHdlcSetup;
    con->settings.wrapper = &udpSetup;
    ///////////////////////////////////////////////////////////////////////
    //Server must initialize after all objects are added.
    ret = svr_initialize(&con->settings);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

//Target object that is read or write at the moment.
gxObject* targetObject = NULL;

int svr_findObject(
    dlmsSettings* settings,
    DLMS_OBJECT_TYPE objectType,
    int sn,
    unsigned char* ln,
    gxValueEventArg* e)
{
    if (targetObject != NULL && targetObject->objectType == objectType && memcmp(targetObject->logicalName, ln, 6) == 0)
    {
        e->target = targetObject;
        return 0;
    }
    if (targetObject != NULL)
    {
        obj_clear(targetObject);
        targetObject = NULL;       
    }
    int ret = cosem_createObject(objectType, &targetObject);
    if (ret == 0)
    {
        e->target = targetObject;
        memcpy(e->target->logicalName, ln, 6);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
//Connect to the client.
/////////////////////////////////////////////////////////////////////////////
int connect2Client(dlmsSettings* settings)
{
    int ret;
    //Connect to the device after client has made a connection to the server.
    if ((ret = com_open(&clientCon, serialPort)) != 0)
    {
        return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
    }
    //Update HES connection settings for the meter connection.
    clientCon.settings.useLogicalNameReferencing = settings->useLogicalNameReferencing;
    clientCon.settings.maxPduSize = settings->maxPduSize;
    clientCon.settings.maxInfoTX = 128;
    clientCon.settings.maxInfoRX = 128;
    clientCon.settings.windowSizeRX = 1;
    clientCon.settings.windowSizeTX = 1;
    clientCon.settings.proposedConformance = settings->negotiatedConformance;
    clientCon.settings.clientAddress = settings->clientAddress;
    clientCon.settings.serverAddress = settings->serverAddress;
    clientCon.settings.authentication = settings->authentication;
    bb_clear(&clientCon.settings.password);
    bb_set(&clientCon.settings.password, settings->password.data, settings->password.size);

    clientCon.settings.customChallenges = 1;
    bb_clear(&clientCon.settings.ctoSChallenge);
    bb_set(&clientCon.settings.ctoSChallenge, settings->ctoSChallenge.data, settings->ctoSChallenge.size);

    bb_empty(&clientCon.data);
    if ((ret = com_initializeConnection(&clientCon)) != 0)
    {
        com_close(&clientCon);
        return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
    }
    bb_empty(&clientCon.data);
    bb_clear(&settings->stoCChallenge);
    bb_set(&settings->stoCChallenge, clientCon.settings.stoCChallenge.data, clientCon.settings.stoCChallenge.size);
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

///////////////////////////////////////////////////////////////////////
// Write trace to the console.
//
///////////////////////////////////////////////////////////////////////
void GXTRACE_LN(const char* str, uint16_t type, unsigned char* ln)
{
    printf("%d %d.%d.%d.%d.%d.%d\r\n", type, ln[0], ln[1], ln[2], ln[3], ln[4], ln[5]);
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN("svr_preRead: ", e->target->objectType, e->target->logicalName);
        bb_empty(&clientCon.data);
        e->error = com_read2(&clientCon, e->target, e->index, &e->value);
        e->dataType = e->value.vt;
        //Data is in byte array.
        e->byteArray = 1;
        e->handled = 1;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN("svr_preWrite: ", e->target->objectType, e->target->logicalName);
        bb_empty(&clientCon.data);
        e->error = com_write(&clientCon, e->target, e->index);
        e->handled = 1;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxReplyData reply;
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN("svr_preAction: ", e->target->objectType, e->target->logicalName);
        bb_empty(&clientCon.data);
        e->error = com_method(&clientCon, e->target, e->index, &e->parameters, &e->value);
        reply_clear(&reply);
        //Data is in byte array.
        e->byteArray = 1;
        e->handled = 1;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    //Show updated values.
#if !(defined(DLMS_IGNORE_MALLOC) ||defined(GX_DLMS_MICROCONTROLLER))
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    gxValueEventArg* e;
    int ret, pos;
    char* buff;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) == 0)
        {
            if ((ret = obj_toString(e->target, &buff)) != 0)
            {
                printf("svr_postWrite::obj_toString failed %d.", ret);
                break;
            }
            printf(buff);
            free(buff);
        }
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#endif //!(defined(GX_DLMS_MICROCONTROLLER) ||defined(GX_DLMS_MICROCONTROLLER))
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postAction(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            break;
        }
    }
}

unsigned char svr_isTarget(
    dlmsSettings* settings,
    uint32_t serverAddress,
    uint32_t clientAddress)
{
    //In this example gateway there is only one meter after the GW.
    return 1;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings* settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer* password)
{
    //Establish the connection for the meter.
    return connect2Client(&settings->base);
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    // The meter will tell are read and write allowd.
    return DLMS_ACCESS_MODE_READ_WRITE;
}

/**
* Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    // The meter will tell are actions allowd.
    return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

/////////////////////////////////////////////////////////////////////////////
//Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
    dlmsServerSettings* settings)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    printf("Connected %d.\r\n", settings->base.connected);
#endif
    return 0;
}

/**
    * Client has try to made invalid connection. Password is incorrect.
    *
    * @param connectionInfo
    *            Connection information.
    */
int svr_invalidConnection(dlmsServerSettings* settings)
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
    dlmsServerSettings* settings)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    printf("Disconnected %d.\r\n", settings->base.connected);
#endif
    //Close connection to the meter.
    return com_close(&clientCon);
}

void svr_preGet(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
}

void svr_postGet(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{

}

/**
* This is reserved for future use. Do not use it.
*
* @param args
*            Handled data type requests.
*/
void svr_getDataType(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{

}