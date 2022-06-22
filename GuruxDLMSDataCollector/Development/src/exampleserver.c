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

#include "../include/objects.h"
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

//If HDLC framing is used.
#ifdef USE_HDLC
gxIecHdlcSetup iecHdlcSetup;
#else
//If WRAPPER framing is used.
gxTcpUdpSetup udpSetup;
gxIp4Setup ip4Setup;
#endif //USE_HDLC

gxAssociationLogicalName target;
gxAssociationLogicalName lowAssociation, associationNone, highAssociation;

static gxPushSetup pushSetup;
static gxPushSetup connectivityPushSetup;

/*Define all COSEM objects here so they are not use stack.*/
gxData ldn, id1, id2, fw;
//Meter serial number.
unsigned long SERIAL_NUMBER = 123456;
//Define Logical Device Name.
char LDN[17];
unsigned char CTOS_CHALLENGE[16];
unsigned char STOC_CHALLENGE[16];

//Client connection.
clientConnection clientCon;
//Meter reply is saved for this buffer.
char* serialPort;

unsigned char CLIENT_ADDRESS = 0x20;
unsigned short SERVER_ADDRESS = 0x1;
DLMS_AUTHENTICATION AUTHENTICATION_LEVEL = DLMS_AUTHENTICATION_LOW;
const static char* PASSWORD = "Gurux";

gxByteBuffer profileGenericData;
unsigned char PROFILE_GENERIC_DATA[600];

//All objects.
const gxObject* ALL_DC_OBJECTS[] = { BASE(associationNone), BASE(lowAssociation), BASE(highAssociation),
    BASE(ldn), BASE(id1), BASE(id2), BASE(fw),
    BASE(pushSetup),
    BASE(connectivityPushSetup),
#ifdef USE_HDLC
    BASE(iecHdlcSetup),
#else
    //If WRAPPER framing is used.
    BASE(udpSetup),
    BASE(ip4Setup),
    #endif //USE_HDLC
};

//Returns the approximate processor time in ms.
uint32_t time_elapsed(void)
{
    return (uint32_t)clock() * 1000;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for None authentication.
// PC in Indian standard.
///////////////////////////////////////////////////////////////////////
int addNoneAssociation()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
    if ((ret = INIT_OBJECT(associationNone, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        //Only Logical Device Name is add to this Association View.
        //Use this if you  need to save heap.
        OA_ATTACH(associationNone.objectList, ALL_DC_OBJECTS);
        associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        associationNone.clientSAP = 0x10;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for Low authentication.
// MR in Indian standard.
///////////////////////////////////////////////////////////////////////
int addLowAssociation()
{
    int ret;
    //Define low level password.
    static char LLS_PASSWORD[20] = "Gurux";
    const unsigned char ln[6] = { 0, 0, 40, 0, 2, 255 };
    if ((ret = INIT_OBJECT(lowAssociation, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        lowAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_LOW;
        OA_ATTACH(lowAssociation.objectList, ALL_DC_OBJECTS);
        lowAssociation.clientSAP = 0x20;
        BB_ATTACH(lowAssociation.secret, LLS_PASSWORD, (unsigned short)strlen(LLS_PASSWORD));
        //All objects are add for this Association View later.
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for High authentication.
// UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addHighAssociation()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 3, 255 };
    if ((ret = INIT_OBJECT(highAssociation, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        highAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
        OA_ATTACH(highAssociation.objectList, ALL_DC_OBJECTS);
        highAssociation.clientSAP = 0x30;
        static char HLS_PASSWORD[20] = "Gurux";
        BB_ATTACH(highAssociation.secret, HLS_PASSWORD, (unsigned short)strlen(HLS_PASSWORD));
    }
    return ret;
}

#ifdef USE_HDLC
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
#else
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
        udpSetup.ipSetup = BASE(ip4Setup);
    }
    return ret;
}


unsigned long getIpAddress()
{
    int ret = -1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    struct hostent* phe;
    char ac[80];
    if ((ret = gethostname(ac, sizeof(ac))) != -1)
    {
        phe = gethostbyname(ac);
        if (phe == 0)
        {
            ret = 0;
        }
        else
        {
            struct in_addr* addr = (struct in_addr*)phe->h_addr_list[0];
#if defined(_WIN32) || defined(_WIN64)//If Windows
            return addr->S_un.S_addr;
#else //or Linux
            return addr->s_addr;
#endif
        }
    }
#else
    //If no OS get IP.
#endif
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add IP4 Setup object.
///////////////////////////////////////////////////////////////////////
int addIP4Setup()
{
    int ret;
    static unsigned long MULTICAST_IP_ADDRESS[10] = { 0 };
    const unsigned char ln[6] = { 0,0,25,1,0,255 };
    if ((ret = INIT_OBJECT(ip4Setup, DLMS_OBJECT_TYPE_IP4_SETUP, ln)) == 0)
    {
        ip4Setup.ipAddress = getIpAddress();
        ARR_ATTACH(ip4Setup.multicastIPAddress, MULTICAST_IP_ADDRESS, 0);
    }
    return ret;
}

#endif //USE_HDLC

///////////////////////////////////////////////////////////////////////
//Add firmware version object.
///////////////////////////////////////////////////////////////////////
int addFirmwareVersion()
{
    int ret;
    static char FW[] = "Gurux Data Collector FW 0.0.1";
    //Firmware version.
    {
        const unsigned char ln[6] = { 1,0,0,2,0,255 };
        if ((ret = INIT_OBJECT(fw, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTET_STRING(fw.value, FW, (unsigned short)strlen(FW));
        }
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add instantaneous profile push setup object.
///////////////////////////////////////////////////////////////////////
int addPushSetup()
{
    int ret;
    const char* DEST = "";
    static gxTarget PUSH_OBJECTS[3] = { 0 };
    const unsigned char ln[6] = { 0,1,25,9,0,255 };
    if ((ret = INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln)) == 0)
    {
        pushSetup.service = DLMS_SERVICE_TYPE_HDLC;
        bb_set(&pushSetup.destination, DEST, 0);
        // Add push object itself. This is needed to tell structure of data to
        // the Push listener.
        PUSH_OBJECTS[0].target = &pushSetup.base;
        PUSH_OBJECTS[0].attributeIndex = 2;
        PUSH_OBJECTS[0].dataIndex = 0;

        // Add logical device name.
        PUSH_OBJECTS[1].target = &ldn.base;
        PUSH_OBJECTS[1].attributeIndex = 2;
        PUSH_OBJECTS[1].dataIndex = 0;

        // Add instantaneous profile value.
        PUSH_OBJECTS[2].target = &instantaneousProfile.base;
        PUSH_OBJECTS[2].attributeIndex = 2;
        PUSH_OBJECTS[2].dataIndex = 0;
        ARR_ATTACH(pushSetup.pushObjectList, PUSH_OBJECTS, (sizeof(PUSH_OBJECTS) / sizeof(PUSH_OBJECTS[0])));
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addOnConnectivityPushSetup()
{
    int ret;
    const char* DEST = "";
    static gxTarget CAPTURE_OBJECTS[2] = { 0 };

    const unsigned char ln[6] = { 0,0,25,9,0,255 };
    if ((ret = INIT_OBJECT(connectivityPushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln)) == 0)
    {
        connectivityPushSetup.service = DLMS_SERVICE_TYPE_HDLC;
        bb_set(&connectivityPushSetup.destination, DEST, (unsigned short)strlen(DEST));

        // Add push object itself. This is needed to tell structure of data to
        // the Push listener.
        CAPTURE_OBJECTS[0].target = &connectivityPushSetup.base;
        CAPTURE_OBJECTS[0].attributeIndex = 2;
        CAPTURE_OBJECTS[0].dataIndex = 0;

        // Add logical device name.
        CAPTURE_OBJECTS[1].target = &ldn.base;
        CAPTURE_OBJECTS[1].attributeIndex = 2;
        CAPTURE_OBJECTS[1].dataIndex = 0;

        // Add 0.0.25.1.0.255 Ch. 0 IPv4 setup IP address.
        CAPTURE_OBJECTS[2].target = &ip4Setup.base;
        CAPTURE_OBJECTS[2].attributeIndex = 3;
        CAPTURE_OBJECTS[2].dataIndex = 0;
        ARR_ATTACH(connectivityPushSetup.pushObjectList, CAPTURE_OBJECTS, 3);
    }
    return ret;
}

int svr_InitObjects(
    dlmsServerSettings* settings,
    char* meterPort,
    int serverPort)
{
    int ret;
    serialPort = meterPort;
    conCl_init(&clientCon, 0);
    //Update correct settings.
    cl_init(&clientCon.settings, 1, 16, 1, DLMS_AUTHENTICATION_HIGH, "Gurux", DLMS_INTERFACE_TYPE_HDLC);
    bb_attach(&clientCon.data, CLIENT_PDU, 0, sizeof(CLIENT_PDU));

    //Get buffer for serialized data.
    clientCon.settings.serializedPdu = &clientCon.data;
    bb_attach(&clientCon.settings.ctoSChallenge, CTOS_CHALLENGE, 0, sizeof(CTOS_CHALLENGE));
    bb_attach(&clientCon.settings.stoCChallenge, STOC_CHALLENGE, 0, sizeof(STOC_CHALLENGE));

    bb_attach(&profileGenericData, PROFILE_GENERIC_DATA, 0, sizeof(PROFILE_GENERIC_DATA));


    OA_ATTACH(settings->base.objects, ALL_DC_OBJECTS);
    ret = obj_InitObjects(settings);

    sprintf(LDN, "GRX%.13lu", SERIAL_NUMBER);
    ///////////////////////////////////////////////////////////////////////
    //Add Logical Device Name. 123456 is meter serial number.
    ///////////////////////////////////////////////////////////////////////
    // COSEM Logical Device Name is defined as an octet-string of 16 octets.
    // The first three octets uniquely identify the manufacturer of the device and it corresponds
    // to the manufacturer's identification in IEC 62056-21.
    // The following 13 octets are assigned by the manufacturer.
    //The manufacturer is responsible for guaranteeing the uniqueness of these octets.
    {
        const unsigned char ln[6] = { 0,0,42,0,0,255 };
        if ((ret = INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTET_STRING(ldn.value, LDN, sizeof(LDN));
        }
    }
    //Electricity ID 1
    {
        const unsigned char ln[6] = { 1,1,0,0,0,255 };
        if ((ret = INIT_OBJECT(id1, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTET_STRING(id1.value, LDN, sizeof(LDN));
        }
    }

    //Electricity ID 2.
    {
        const unsigned char ln[6] = { 1,1,0,0,1,255 };
        if ((ret = INIT_OBJECT(id2, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_UINT32(id2.value) = SERIAL_NUMBER;
        }
    }
    if ((ret = addFirmwareVersion()) != 0)
    {
        return ret;
    }

#ifdef USE_HDLC
    if ((ret = addIecHdlcSetup()) != 0)
    {
        return ret;
    }
#else
    if ((ret = addTcpUdpSetup(serverPort)) != 0 ||
        (ret = addIP4Setup()) != 0)
    {
        return ret;
    }
#endif //USE_HDLC
    if ((ret = addPushSetup()) != 0 ||
        (ret = addOnConnectivityPushSetup()) != 0)
    {
        return ret;
    }

    if ((ret = addNoneAssociation()) != 0 ||
        (ret = addLowAssociation()) != 0 ||
        (ret = addHighAssociation()) != 0)
    {
        return ret;
    }
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
#ifdef USE_HDLC
    con->settings.hdlc = &iecHdlcSetup;
#else
    con->settings.wrapper = &udpSetup;
#endif //USE_HDLC
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


int svr_findObject(
    dlmsSettings* settings,
    DLMS_OBJECT_TYPE objectType,
    int sn,
    unsigned char* ln,
    gxValueEventArg* e)
{
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->serverAddress == 1)
    {
        e->target = BASE(target);
        memcpy(target.base.logicalName, ln, 6);
        target.base.objectType = objectType;
    }
    else
    {
        if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
        {
            if (settings->authentication == DLMS_AUTHENTICATION_NONE)
            {
                e->target = &associationNone.base;
                return 0;
            }
            if (settings->authentication == DLMS_AUTHENTICATION_LOW)
            {
                e->target = &lowAssociation.base;
                return 0;
            }
            if (settings->authentication == DLMS_AUTHENTICATION_HIGH ||
                settings->authentication == DLMS_AUTHENTICATION_HIGH_MD5 ||
                settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA1 ||
                settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC ||
                settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA256)
            {
                e->target = &highAssociation.base;
                return 0;
            }
            e->target = NULL;
            return 0;
        }
    }
    return 0;
}


/**
* Find restricting object.
*/
int getRestrictingObject(gxValueEventArg* e)
{
    int ret;
    unsigned char ln[6];
    unsigned short ot;
    signed char aIndex;
    unsigned short dIndex;
    if ((ret = cosem_checkStructure(e->parameters.byteArr, 4)) == 0 &&
        (ret = cosem_checkStructure(e->parameters.byteArr, 4)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &ot)) == 0 &&
        (ret = cosem_getOctetString2(e->parameters.byteArr, ln, 6, NULL)) == 0 &&
        (ret = cosem_getInt8(e->parameters.byteArr, &aIndex)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &dIndex)) == 0)
    {

    }
    return ret;
}

static unsigned char preEstablished = 0;


void read(dlmsSettings* settings, gxValueEventCollection* args, unsigned char ignoreValue)
{
    int ret, pos;
    gxValueEventArg* e;
    gxReplyData reply;
    reply_init(&reply);
    reply.data = *clientCon.settings.serializedPdu;
    bb_clear(&reply.data);
    //Don't try to get the value from DLMS data.
    reply.ignoreValue = ignoreValue;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            e->error = ret;
            break;
        }
        e->byteArray = 1;
        e->handled = 1;
        if (e->selector == 1)
        {
            gxtime start, end;
            if ((ret = getRestrictingObject(e)) != 0 ||
                (ret = cosem_getDateTimeFromOctetString(e->parameters.byteArr, &start)) != 0 ||
                (ret = cosem_getDateTimeFromOctetString(e->parameters.byteArr, &end)) != 0)
            {
                e->error = ret;
                break;
            }
            //Clear parameters because data is serialized for this buffer.
            bb_clear(e->parameters.byteArr);
            gxProfileGeneric pg;
            cosem_init2(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, e->target->logicalName);
            if ((ret = com_readRowsByRange(&clientCon, &pg, start.value, end.value, &reply)) != 0)
            {
                e->error = ret;
                break;
            }
        }
        else if (e->selector == 2)
        {
            unsigned long index, count;
            //Read by range (start and end index).
            if ((ret = cosem_checkStructure(e->parameters.byteArr, 4)) != 0 ||
                (ret = cosem_getUInt32(e->parameters.byteArr, &index)) != 0 ||
                (ret = cosem_getUInt32(e->parameters.byteArr, &count)) != 0)
            {
                e->error = ret;
                break;
            }
            //Clear parameters because data is serialized for this buffer.
            bb_clear(e->parameters.byteArr);
            if (count != 0)
            {
                count -= index - 1;
            }
            gxProfileGeneric pg;
            cosem_init2(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, e->target->logicalName);
            if ((ret = com_readRowsByEntry(&clientCon, &pg, index, count, &reply)) != 0)
            {
                e->error = ret;
                break;
            }
        }
        else
        {
            //Read data from the meter.
            if (e->transactionEndIndex == 0)
            {
                if ((ret = com_read(&clientCon, e->target, e->index, &reply)) != 0)
                {
                    e->error = ret;
                    break;
                }
            }
            else
            {
                e->transactionEndIndex = 0;
                if ((ret = com_readNextBlock(&clientCon, &reply)) != 0)
                {
                    e->error = ret;
                    break;
                }
            }
        }
        if (ignoreValue)
        {
            if ((ret = bb_set2(e->value.byteArr, &reply.data, 0, reply.data.size)) != 0)
            {
                e->error = ret;
                break;
            }
        }
        else
        {
            if ((ret = cl_updateValue(&clientCon.settings, e->target, e->index, &reply.dataValue)) != 0)
            {
                e->error = ret;
                break;
            }
        }
        //We have more blocks to read.
        if (reply.moreData != 0)
        {
            e->transactionEndIndex = 1;
        }
        else
        {
            //All blocks are read.
            e->transactionEndIndex = 0;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//Connect to the client.
/////////////////////////////////////////////////////////////////////////////
int connect2Client(dlmsSettings* settings)
{
    if ((settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0 ||
        //If pre-established connection.
        preEstablished)
    {
        int ret;
        //Connect to the device after client has made a connection to the server.
        if ((ret = com_open(&clientCon, serialPort, 0)) != 0)
        {
            return ret;
        }
        clientCon.settings.clientAddress = settings->clientAddress;
        clientCon.settings.serverAddress = settings->serverAddress;
        clientCon.settings.authentication = settings->authentication;
        clientCon.settings.password = settings->password;
        clientCon.settings.ctoSChallenge = settings->ctoSChallenge;
        if ((ret = com_initializeConnection(&clientCon)) != 0)
        {
            com_close(&clientCon);
            return ret;
        }
        settings->stoCChallenge = clientCon.settings.stoCChallenge;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->serverAddress == 1)
    {
        int ret;
        //If this is pre-established connection.
        unsigned char connected = (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0;
        if (!connected)
        {
            preEstablished = 1;
            ret = connect2Client(settings);
            preEstablished = 0;
            if (ret != 0)
            {
                return;
            }
        }
        read(settings, args, 1);
        if (!connected)
        {
            if ((ret = svr_disconnected((dlmsServerSettings*)settings)) != 0)
            {
                return;
            }
        }
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
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->serverAddress == 1)
    {
        //If this is pre-established connection.
        unsigned char connected = (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0;
        if (!connected)
        {
            preEstablished = 1;
            ret = connect2Client(settings);
            preEstablished = 0;
            if (ret != 0)
            {
                return;
            }
        }
        for (pos = 0; pos != args->size; ++pos)
        {
            if ((ret = vec_getByIndex(args, pos, &e)) != 0)
            {
                e->error = ret;
                break;
            }
            e->byteArray = 1;
            e->handled = 1;
            if (e->value.vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                bb_trim(e->value.byteArr);
            }
            if ((ret = com_write(&clientCon, e->target, e->index, &e->value)) != 0)
            {
                e->error = ret;
                break;
            }
        }
        if (!connected)
        {
            if ((ret = svr_disconnected((dlmsServerSettings*)settings)) != 0)
            {
                return;
            }
        }
    }
}

int sendPush(
    dlmsSettings* settings,
    gxPushSetup* push)
{
    int ret = 0, pos;
    message messages;
    gxByteBuffer bb, pushPdu;
    bb_attach(&bb, PUSH_FRAME, 0, sizeof(PUSH_FRAME));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
    bb_attach(&pushPdu, PUSH_PDU, 0, sizeof(PUSH_PDU));
    settings->serializedPdu = &pushPdu;

    if (pushSocket != -1)
    {
        gxTarget* t;
        gxValueEventArg list[10] = { 0 };
        gxValueEventCollection args;
        vec_attach(&args, list, 0, (unsigned char) sizeof(list));
        int index = 0;
        //MIKKO
        for (pos = 2; pos != push->pushObjectList.size; ++pos)
        {
            if ((ret = arr_getByIndex2(&push->pushObjectList, pos, &t, sizeof(gxTarget))) != 0)
            {
                break;
            }
            //Don't read push object from the meter.
            if (t->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
            {
                list[index].value.vt = DLMS_DATA_TYPE_OCTET_STRING;
                list[index].value.byteArr = &profileGenericData;
                list[index].target = t->target;
                list[index].index = t->attributeIndex;
                ++index;
            }
        }
        args.size = index;
        if (ret == 0)
        {
            //Connect to the device after client has made a connection to the server.
            clientCon.settings.clientAddress = CLIENT_ADDRESS;
            clientCon.settings.serverAddress = SERVER_ADDRESS;
            clientCon.settings.authentication = AUTHENTICATION_LEVEL;
            bb_attach(&clientCon.settings.password, (unsigned char*)PASSWORD, strlen(PASSWORD), strlen(PASSWORD));
            if ((ret = com_open(&clientCon, serialPort, 0)) != 0)
            {
                com_close(&clientCon);
                return ret;
            }
            if ((ret = com_initializeConnection(&clientCon)) != 0)
            {
                com_close(&clientCon);
                return ret;
            }
            read(&clientCon.settings, &args, 1);
            com_close(&clientCon);
            if ((ret = notify_generatePushSetupMessages(settings, 0, push, &messages)) == 0)
            {
                for (pos = 0; pos != messages.size; ++pos)
                {
                    if (send(pushSocket, (char*)bb.data, bb.size, 0) == -1)
                    {
                        mes_clear(&messages);
                        break;
                    }
                }
            }
        }
    }
    mes_clear(&messages);
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->serverAddress == 1)
    {
        //If this is pre-established connection.
        unsigned char connected = (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0;
        if (!connected)
        {
            preEstablished = 1;
            ret = connect2Client(settings);
            preEstablished = 0;
            if (ret != 0)
            {
                return;
            }
        }
        for (pos = 0; pos != args->size; ++pos)
        {
            if ((ret = vec_getByIndex(args, pos, &e)) != 0)
            {
                break;
            }
            e->byteArray = 1;
            e->handled = 1;
            if (e->value.vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                bb_trim(e->value.byteArr);
            }
            if ((ret = com_method(&clientCon, e->target, e->index, &e->parameters)) != 0)
            {
                e->error = ret;
                break;
            }
        }
        if (!connected)
        {
            if ((ret = svr_disconnected((dlmsServerSettings*)settings)) != 0)
            {
                return;
            }
        }
    }
    else
    {
        for (pos = 0; pos != args->size; ++pos)
        {
            if ((ret = vec_getByIndex(args, pos, &e)) != 0)
            {
                break;
            }
            if (e->target->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP && e->index == 1)
            {
                e->error = sendPush(settings, (gxPushSetup*)e->target);
                e->handled = 1;
            }
        }
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
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    return 1;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings* settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer* password)
{
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->base.serverAddress == 2)
    {
        //Check used security level.
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (settings->base.cipher.security == DLMS_SECURITY_NONE)
        {
            //Uncomment this if you want that only Get service is available with authentication level None.
            //settings->base.negotiatedConformance = DLMS_CONFORMANCE_GET;
             //Uncomment this if secured (ciphered) connection is always required.
             //return DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
        }
#endif //DLMS_IGNORE_HIGH_GMAC

        if (authentication == DLMS_AUTHENTICATION_NONE)
        {
            //Uncomment this if authentication is always required.
            //return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
        }
        //Check Low Level security..
        if (authentication == DLMS_AUTHENTICATION_LOW)
        {
            gxByteBuffer expected;
            if (settings->base.useLogicalNameReferencing)
            {
                expected = lowAssociation.secret;
            }
            if (bb_compare(&expected, password->data, password->size) == 0)
            {
                return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
            }
        }
    }
    // Other authentication levels are check on phase two.
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->serverAddress == 1)
    {
        // All writes are allowed for the meter.
        return DLMS_ACCESS_MODE_READ_WRITE;
    }
    // Only read is allowed for the data collector.
    return DLMS_ACCESS_MODE_READ;
}

/**
* Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    //If client wants to read data from the meter server address is 1.
   //If client wants to read data from the data collector server address is 2.
    if (settings->serverAddress == 1)
    {
        // All actions are allowed for the meter.
        return DLMS_METHOD_ACCESS_MODE_ACCESS;
    }
    // Only push action is allowed for the meter.
    if (obj->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP)
    {
        return DLMS_METHOD_ACCESS_MODE_ACCESS;
    }
    return DLMS_METHOD_ACCESS_MODE_NONE;

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
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->base.serverAddress == 1 && settings->base.authentication < DLMS_AUTHENTICATION_HIGH)
    {
        //Allow non-ciphered pre-established connections.
        settings->info.preEstablished = 1;
        connect2Client(&settings->base);
    }
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
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->base.serverAddress == 1)
    {
        //Close connection to the meter.
        return com_close(&clientCon);
    }
    return 0;
}

void svr_preGet(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    //Profile generic data is read and save as raw data before sending in push message.
    args->data[0].value.vt = DLMS_DATA_TYPE_OCTET_STRING;
    bb_set2(args->data[0].value.byteArr, &profileGenericData, 0, profileGenericData.size);
    bb_clear(&profileGenericData);
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