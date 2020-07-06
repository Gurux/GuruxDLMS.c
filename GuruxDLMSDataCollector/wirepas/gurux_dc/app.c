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

#include <stdlib.h>
#include "api.h"
#include "node_configuration.h"
#include "hal_api.h"
#include "board.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "hw_delay.h"

//Gurux DLMS includes.
#include "dlms/include/server.h"
#include "dlms/include/bytebuffer.h"
#include "dlms/include/dlmssettings.h"
#include "dlms/include/variant.h"
#include "dlms/include/cosem.h"
#include "dlms/include/client.h"
#include "dlms/include/converters.h"
#include "dlms/include/gxobjects.h"
#include "dlms/include/notify.h"


/** Define baudrate for uart */
#define DLMS_BAUDRATE            9600

/** Read interval is 5 seconds. */
#define DEFAULT_PERIOD_S    10
#define DEFAULT_PERIOD_US   (DEFAULT_PERIOD_S*1000*1000)

/** Time needed to execute the periodic work, in us */
#define EXECUTION_TIME_US 500

/** Endpoint to change the sending period value */
#define SET_PERIOD_EP  10

/** Endpoint to send data */
#define DATA_EP        1

/** Period to send measurements, in us */
static uint32_t period_us;

//DLMS client settings.
static dlmsSettings meterSettings;
//DLMS server settings.
static dlmsServerSettings serverSettings;

//Is new data received.
static bool received = false;

///////////////////////////////////////////////////////////////////////
#define PDU_BUFFER_SIZE 1024

#define HDLC_HEADER_SIZE 17
#define WRAPPER_FRAME_SIZE 8 + PDU_BUFFER_SIZE
#define HDLC_SIZE 128
#define WRAPPER_SIZE 1024

#ifdef USE_HDLC
#define FRAME_SIZE HDLC_HEADER_SIZE + HDLC_SIZE
#else
#define FRAME_SIZE HDLC_HEADER_SIZE + WRAPPER_SIZE
#endif //USE_HDLC

unsigned char SERVER_PDU[10 + PDU_BUFFER_SIZE];
unsigned char SERVER_FRAME[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];
unsigned char SERVER_REPLY_PDU[FRAME_SIZE];

unsigned char CLIENT_PDU[PDU_BUFFER_SIZE];
unsigned char CLIENT_REPLY_PDU[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];
unsigned char CLIENT_FRAME[FRAME_SIZE];
gxByteBuffer frameData;
gxByteBuffer clientPdu;
gxByteBuffer rerverReply;

//Push needs own buffer so it don't cause problems with on-going communication.
unsigned char PUSH_PDU[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];
unsigned char PUSH_FRAME[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];

//Space for client challenge.
unsigned char C2S_CHALLENGE[16];
//Space for server challenge.
unsigned char S2C_CHALLENGE[16];

//Space for profile generic data.
gxByteBuffer profileGenericData;
unsigned char PROFILE_GENERIC_DATA[600];

///////////////////////////////////////////////////////////////////////
// Define all COSEM objects here so they are not use stack.

gxAssociationLogicalName target;
gxAssociationLogicalName associationNone;

//If HDLC framing is used.
#ifdef USE_HDLC
static gxIecHdlcSetup iecHdlcSetup;
#else
//If WRAPPER framing is used.
static gxTcpUdpSetup udpSetup;
#endif //USE_HDLC

static gxClock clock1;
static gxRegister activePowerL1;
static gxPushSetup push;
static gxData ldn, id1, id2, fw;
//Meter serial number.
unsigned long SERIAL_NUMBER = 123456;
//Define Logical Device Name.
char LDN[17];
unsigned char CTOS_CHALLENGE[16];
unsigned char STOC_CHALLENGE[16];
gxValueEventArg events[1];

///////////////////////////////////////////////////////////////////////
// Define meter settings.
unsigned char CLIENT_ADDRESS = 0x10;
unsigned short SERVER_ADDRESS = 0x1;
DLMS_AUTHENTICATION AUTHENTICATION_LEVEL = DLMS_AUTHENTICATION_NONE;
unsigned char PASSWORD[16];
static char* DEFAULT_PASSWORD = "Gurux";


//All objects.
const gxObject* ALL_OBJECTS[] = { BASE(associationNone),
    BASE(ldn), BASE(id1), BASE(id2), BASE(fw),
    BASE(push),
#ifdef USE_HDLC
    BASE(iecHdlcSetup),
#else
    //If WRAPPER framing is used.
    BASE(udpSetup),
#endif //USE_HDLC
};

//Returns the approximate processor time in ms.
uint32_t time_elapsed(void)
{
    return lib_time->getTimestampS() * 1000;
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
        OA_ATTACH(associationNone.objectList, ALL_OBJECTS);
        associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        associationNone.clientSAP = 0x10;
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
		iecHdlcSetup.interCharachterTimeout = 2000;
        iecHdlcSetup.inactivityTimeout = 120;
        iecHdlcSetup.deviceAddress = 0x10;
    }
    return ret;
}
#else
///////////////////////////////////////////////////////////////////////
//This method adds example TCP/UDP setup object.
///////////////////////////////////////////////////////////////////////
int addTcpUdpSetup()
{
    int ret;
    //Add Tcp/Udp setup. Default Logical Name is 0.0.25.0.0.255.
    const unsigned char ln[6] = { 0,0,25,0,0,255 };
    if ((ret = INIT_OBJECT(udpSetup, DLMS_OBJECT_TYPE_TCP_UDP_SETUP, ln)) == 0)
    {
        udpSetup.port = 4059;
        udpSetup.maximumSimultaneousConnections = 1;
        udpSetup.maximumSegmentSize = WRAPPER_FRAME_SIZE;
        udpSetup.inactivityTimeout = 180;
        udpSetup.ipSetup = NULL;
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
            GX_OCTECT_STRING(fw.value, FW, (unsigned short)strlen(FW));
        }
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
    int ret;
    //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
    const unsigned char ln[6] = { 0,0,1,0,0,255 };
    if ((ret = INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
    {
        time_init(&clock1.begin, -1, 9, 1, -1, -1, -1, -1, 0x8000);
        time_init(&clock1.end, -1, 3, 1, -1, -1, -1, -1, 0x8000);
        //Meter is using UTC time zone.
        clock1.timeZone = 0;
        //Deviation is 60 minutes.
        clock1.deviation = 60;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
    int ret;
    const unsigned char ln[6] = { 1,1,21,25,0,255 };
    if ((ret = INIT_OBJECT(activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        //10 ^ 3 =  1000
        activePowerL1.scaler = -2;
        activePowerL1.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add push setup object.
///////////////////////////////////////////////////////////////////////
int addPushSetup()
{
    int ret = 0;
    const unsigned char ln[6] = { 0,0,25,9,0,255 };
    INIT_OBJECT(push, DLMS_OBJECT_TYPE_PUSH_SETUP, ln);
    static gxTarget PUSH_OBJECTS[5];
    //Add Logical Device Name so we can identify the meter.
    PUSH_OBJECTS[0].target = &ldn.base;
    PUSH_OBJECTS[0].attributeIndex = 2;
    PUSH_OBJECTS[0].dataIndex = 0;
    //Add OBIS code of Push object so we can know what push message is sent.
    PUSH_OBJECTS[1].target = &push.base;
    PUSH_OBJECTS[1].attributeIndex = 1;
    PUSH_OBJECTS[1].dataIndex = 0;
    //Read time.
    PUSH_OBJECTS[2].target = &clock1.base;
    PUSH_OBJECTS[2].attributeIndex = 2;
    PUSH_OBJECTS[2].dataIndex = 0;
    //Register value.
    PUSH_OBJECTS[3].target = &activePowerL1.base;
    PUSH_OBJECTS[3].attributeIndex = 2;
    PUSH_OBJECTS[3].dataIndex = 0;
    //Register scaler and unit.
    PUSH_OBJECTS[4].target = &activePowerL1.base;
    PUSH_OBJECTS[4].attributeIndex = 3;
    PUSH_OBJECTS[4].dataIndex = 0;
    ARR_ATTACH(push.pushObjectList, PUSH_OBJECTS, 5);
    return ret;
}


int svr_InitObjects(
    dlmsServerSettings* settings)
{
    int ret;
    //Get buffer for serialized data.
	bb_attach(&clientPdu, CLIENT_PDU, 0, sizeof(CLIENT_PDU));
    meterSettings.serializedPdu = &clientPdu;
    bb_attach(&meterSettings.ctoSChallenge, CTOS_CHALLENGE, 0, sizeof(CTOS_CHALLENGE));
    bb_attach(&meterSettings.stoCChallenge, STOC_CHALLENGE, 0, sizeof(STOC_CHALLENGE));


    OA_ATTACH(settings->base.objects, ALL_OBJECTS);

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
            GX_OCTECT_STRING(ldn.value, LDN, sizeof(LDN));
        }
    }
    //Electricity ID 1
    {
        const unsigned char ln[6] = { 1,1,0,0,0,255 };
        if ((ret = INIT_OBJECT(id1, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTECT_STRING(id1.value, LDN, sizeof(LDN));
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
    if ((ret = addTcpUdpSetup()) != 0)
    {
        return ret;
    }
#endif //USE_HDLC

    if ((ret = addPushSetup()) != 0 ||
        (ret = addClockObject()) != 0 ||
        (ret = addRegisterObject()) != 0)
    {
        return ret;
    }

    if ((ret = addNoneAssociation()) != 0)
    {
        return ret;
    }
    return oa_verify(&settings->base.objects);
}


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
        (ret = cosem_getOctectString2(e->parameters.byteArr, ln, 6, NULL)) == 0 &&
        (ret = cosem_getInt8(e->parameters.byteArr, &aIndex)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &dIndex)) == 0)
    {

    }
    return ret;
}

static unsigned char preEstablished = 0;

//Send data to the sink.
static void SendToSink(bool tx, unsigned char* data, unsigned short len)
{
    // Create a data packet to send
    app_lib_data_to_send_t data_to_send;
    data_to_send.bytes = (const uint8_t*)data;
    data_to_send.num_bytes = len;
    data_to_send.dest_address = APP_ADDR_ANYSINK;
    data_to_send.src_endpoint = 110;
    data_to_send.dest_endpoint = 102;
    data_to_send.qos = APP_LIB_DATA_QOS_HIGH;
    data_to_send.delay = 0;
    data_to_send.flags = APP_LIB_DATA_SEND_FLAG_NONE;
    data_to_send.tracking_id = APP_LIB_DATA_NO_TRACKING_ID;
    // Send the data packet
    lib_data->sendData(&data_to_send);
}

/**
* \brief   Blocking wait for given amount of time
* \param   period
*          time to wait in ms
* \note    Lib doesn't use it a lot and for only short period
*          mainly at init stage executed from app_init
*/
static void user_delay_ms(uint32_t period)
{
    app_lib_time_timestamp_hp_t end;
    end = lib_time->addUsToHpTimestamp(lib_time->getTimestampHp(), period * 1000);
    // Active wait until period is elapsed
    while (lib_time->isHpTimestampBefore(lib_time->getTimestampHp(), end));
}

//Read data from the serial port and save it to the buffer.
static int com_readSerialPort(
    unsigned char eop)
{
    //Read reply data.
    int pos;
    unsigned char eopFound = 0;
    unsigned short lastReadIndex = 0;
    frameData.size = 0;
    frameData.position = 0;
    do
    {
        if (received)
        {
            //Search eop.
            if (frameData.size > 5)
            {
                //Some optical strobes can return extra bytes.
                for (pos = frameData.size - 1; pos != lastReadIndex; --pos)
                {
                    if (frameData.data[pos] == eop)
                    {
                        eopFound = 1;
                        received = false;
                        break;
                    }
                }
                lastReadIndex = pos;
            }
        }
        else if (frameData.size > 0)
        {
            user_delay_ms(10);
        }
    } while (eopFound == 0);
    return DLMS_ERROR_CODE_OK;
}


//Write data to the serial port.
static size_t gxUart_write(const unsigned char* ptr, unsigned short len)
{
    // Check that size is not too large for the ring buffer in usart-driver
    if (len > Usart_getMTUSize())
    {
        return 0;
    }
    // Write data to the usart port and return number of bytes written
    return Usart_sendBuffer(ptr, len);
}

// Read DLMS Data frame from the device.
static int readDLMSPacket(
    gxByteBuffer* data,
    gxReplyData* reply)
{
    int ret = DLMS_ERROR_CODE_OK;
    if (data->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    reply->complete = 0;
    //Send data.
    // SendToSink(true, data->data, data->size);
    gxUart_write(data->data, data->size);
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
    // SendToSink(false, connection->data, connection->size);
    return ret;
}

static int com_readDataBlock(
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

//Initialize connection to the meter.
int com_initializeConnection()
{
    int ret = DLMS_ERROR_CODE_OK;
    message messages;
    gxReplyData reply;
    gxByteBuffer bb;
    BB_ATTACH(bb, CLIENT_REPLY_PDU, 0);
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
    reply_init(&reply);
    reply.data = *meterSettings.serializedPdu;
    bb_clear(&reply.data);
    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&meterSettings, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&meterSettings, &reply.data)) != 0)
    {
        static char* str = "SNRM failed";
        SendToSink(false, (unsigned char*)str, 11);
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if ((ret = cl_aarqRequest(&meterSettings, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0 ||
        (ret = cl_parseAAREResponse(&meterSettings, &reply.data)) != 0)
    {
        static char* str = "AARQ failed";
        SendToSink(false, (unsigned char*)str, 11);
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    /*
    // Get challenge Is HLS authentication is used.
    if (meterSettings.authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = cl_getApplicationAssociationRequest(&meterSettings, &messages)) != 0 ||
            (ret = com_readDataBlock(&messages, &reply)) != 0 ||
            (ret = cl_parseApplicationAssociationResponse(&meterSettings, &reply.data)) != 0)
        {
            return ret;
        }
    }
    */
    return DLMS_ERROR_CODE_OK;
}

//Close connection to the meter.
int com_close()
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    reply_init(&reply);

#ifdef DLMS_IGNORE_MALLOC
        gxByteBuffer bb;
        bb_attach(&bb, CLIENT_PDU, 0, sizeof(CLIENT_PDU));
        gxByteBuffer* tmp[] = { &bb };
        mes_attach(&msg, tmp, 1);
        bb_attach(&reply.data, CLIENT_REPLY_PDU, 0, sizeof(CLIENT_REPLY_PDU));

#else
        mes_init(&msg);
#endif //DLMS_IGNORE_MALLOC

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
        meterSettings.clientAddress = settings->clientAddress;
        meterSettings.serverAddress = settings->serverAddress;
        meterSettings.authentication = settings->authentication;
        meterSettings.password = settings->password;
        if ((ret = com_initializeConnection()) != 0)
        {
            com_close();
            return ret;
        }
    }
    return 0;
}


//Report error on output;
void com_reportError(char* description,
    gxObject* object,
    unsigned char attributeOrdinal, int ret)
{
    SendToSink(0, (unsigned char*)description, strlen(description));
}

//Read object.
static int com_read(
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    reply_init(&reply);
    unsigned char buff[PDU_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, buff, 0, sizeof(buff));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&data, tmp, 1);

    //TODO: Size of reply data depends from the amount of COSEM objects.
    //Here we expect that receive data fits to one PDU.
    unsigned char replydata[PDU_BUFFER_SIZE];
    bb_attach(&reply.data, replydata, 0, sizeof(replydata));

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
    unsigned char attributeOrdinal,
    dlmsVARIANT* value)
{
    int ret;
    message data;
    gxReplyData reply;
    reply_init(&reply);
    unsigned char buff[PDU_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, buff, 0, sizeof(buff));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&data, tmp, 1);

    //TODO: Size of reply data depends from the amount of COSEM objects.
    //Here we expect that receive data fits to one PDU.
    unsigned char replydata[PDU_BUFFER_SIZE];
    bb_attach(&reply.data, replydata, 0, sizeof(replydata));

    if ((ret = cl_write(&meterSettings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0)
    {
        com_reportError("Write failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
    dlmsServerSettings* settings)
{
    //If client wants to read data from the meter server address is 1.
    //If client wants to read data from the data collector server address is 2.
    if (settings->base.serverAddress == 1)
    {
        //Close connection to the meter.
        return com_close();
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
        //read(settings, args, 1);
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
            if ((ret = com_write(e->target, e->index, &e->value)) != 0)
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

static int sendPush(
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
    serverSettings.base.serializedPdu = &pushPdu;
    gxTarget* t;
    gxValueEventArg list[10];
    gxValueEventCollection args;
    vec_attach(&args, list, (unsigned char)push->pushObjectList.size, (unsigned char) sizeof(list));
    for (pos = 0; pos != push->pushObjectList.size; ++pos)
    {
        if ((ret = arr_getByIndex2(&push->pushObjectList, pos, (void**)& t, sizeof(gxTarget))) != 0)
        {
            break;
        }
        list[pos].target = t->target;
        list[pos].index = t->attributeIndex;
    }
    if (ret == 0)
    {
        unsigned char connected = (meterSettings.connected & DLMS_CONNECTION_STATE_DLMS) != 0;
        if (!connected)
        {
            //Connect to the device after client has made a connection to the server.
            meterSettings.clientAddress = CLIENT_ADDRESS;
            meterSettings.serverAddress = SERVER_ADDRESS;
            meterSettings.authentication = AUTHENTICATION_LEVEL;
            bb_clear(&meterSettings.password);
            bb_set(&meterSettings.password, (unsigned char*)DEFAULT_PASSWORD, strlen(DEFAULT_PASSWORD));
            if ((ret = com_initializeConnection()) != 0)
            {
                com_close();
                return ret;
            }
        }
        for (pos = 0; pos != push->pushObjectList.size; ++pos)
        {
            if ((ret = com_read(list[pos].target, list[pos].index)) != 0)
            {
                break;
            }
        }
        if (!connected)
        {
            com_close();
        }
        bb_clear(&bb);
        if ((ret = notify_generatePushSetupMessages(&serverSettings.base, 0, push, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                SendToSink(false, bb.data, bb.size);
                {
                    break;
                }
            }
        }
    }
    mes_clear(&messages);
    return ret;
}


int com_method(
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params)
{
    int ret;
    message data;
    gxReplyData reply;
    reply_init(&reply);
    unsigned char buff[PDU_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, buff, 0, sizeof(buff));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&data, tmp, 1);

    //TODO: Size of reply data depends from the amount of COSEM objects.
    //Here we expect that receive data fits to one PDU.
    unsigned char replydata[PDU_BUFFER_SIZE];
    bb_attach(&reply.data, replydata, 0, sizeof(replydata));
    if ((ret = cl_method(&meterSettings, object, attributeOrdinal, params, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0)
    {
        com_reportError("Method failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
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
            if ((ret = com_method(e->target, e->index, &e->parameters)) != 0)
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
    gxValueEventArg * e;
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
        if (authentication != DLMS_AUTHENTICATION_NONE)
        {
            return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
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


//Serial port data received.
static void dataReceivedSp(uint8_t* ch, size_t n)
{
    if (bb_set(&frameData, ch, n) == 0)
    {
        if (ch[n - 1] == 0x7E)
        {
            received = true;
        }
    }
    else
    {
        frameData.size = frameData.position = 0;
    }
}

//Initialize UART.
void gxUart_init()
{
    // Initialize the hardware module
    Usart_init(DLMS_BAUDRATE, UART_FLOW_CONTROL_NONE);
    Usart_setEnabled(true);
    Usart_enableReceiver(dataReceivedSp);
    Usart_receiverOn();
}


void time_now(
    gxtime* value)
{
}

/**
 * \brief   Make connection to the meter and read needed values.
 */
 
static uint32_t readMeter(void)
{
    static char* str = "Start read!";
    SendToSink(false, (unsigned char*)str, strlen(str));
    int ret;
	ret = sendPush(&serverSettings.base, &push);
	if (ret != 0)
	{
		static char* READ_FAILED = "Read failed";
        SendToSink(false, (unsigned char*)READ_FAILED, 11);
	}
    // Inform the stack that this function should be called again in
    // period_us microseconds. By returning APP_LIB_SYSTEM_STOP_PERIODIC,
    // the stack won't call this function again.
    return period_us;
}

/**
 * \brief   Data reception callback
 * \param   data
 *          Received data, \ref app_lib_data_received_t
 * \return  Result code, \ref app_lib_data_receive_res_e
 */
static app_lib_data_receive_res_e dataReceivedCb(
    const app_lib_data_received_t* data)
{
    if ((data->num_bytes < 1) || (data->dest_endpoint != 0x66))
    {
        // Data was not for this application
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
    }
	if (svr_handleRequest2(&serverSettings, (unsigned char*)data->bytes, data->num_bytes, &rerverReply) != 0)
	{
        // Data was not for this application
		bb_clear(&rerverReply);
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
	}
	if (rerverReply.size != 0)
    {
		SendToSink(false, (unsigned char*) rerverReply.data, rerverReply.size);
		bb_clear(&rerverReply);
    }
    // Data handled successfully
    return APP_LIB_DATA_RECEIVE_RES_HANDLED;
}

/**
 * \brief   Initialization callback for application
 *
 * This function is called after hardware has been initialized but the
 * stack is not yet running.
 *
 */
void App_init(const app_global_functions_t* functions)
{
    cl_init(&meterSettings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
    bb_attach(&frameData, CLIENT_FRAME, 0, sizeof(CLIENT_FRAME));
    bb_attach(&rerverReply, SERVER_REPLY_PDU, 0, sizeof(SERVER_REPLY_PDU));
#ifdef USE_HDLC
    svr_init(&serverSettings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_SIZE, PDU_BUFFER_SIZE, SERVER_FRAME, sizeof(SERVER_FRAME), SERVER_PDU, sizeof(SERVER_PDU));
#else
    svr_init(&serverSettings, 1, DLMS_INTERFACE_TYPE_WRAPPER, HDLC_SIZE, PDU_BUFFER_SIZE, SERVER_FRAME, sizeof(SERVER_FRAME), SERVER_PDU, sizeof(SERVER_PDU));
#endif //USE_HDLC
    //Allow non-ciphered pre-established connections.
    serverSettings.info.preEstablished = 1;
    //Readlist and write list are not supported.
    serverSettings.base.proposedConformance &= ~DLMS_CONFORMANCE_MULTIPLE_REFERENCES;
    //Allocate space for client password.
	strcpy((char*)PASSWORD, DEFAULT_PASSWORD);
    BB_ATTACH(serverSettings.base.password, PASSWORD, strlen(DEFAULT_PASSWORD));
    //Allocate space for client challenge.
    BB_ATTACH(serverSettings.base.ctoSChallenge, C2S_CHALLENGE, 0);
    //Allocate space for server challenge.
    BB_ATTACH(serverSettings.base.stoCChallenge, S2C_CHALLENGE, 0);

    //Allocate space for read list.
    vec_attach(&serverSettings.transaction.targets, events, 0, sizeof(events) / sizeof(events[0]));
    // Open Wirepas public API
    API_Open(functions);

    // Open HAL
    HAL_Open();
    int ret;

    // Basic configuration of the node with a unique node address
    if (configureNode(getUniqueAddress(),
        NETWORK_ADDRESS,
        NETWORK_CHANNEL) != APP_RES_OK)
    {
        // Could not configure the node
        // It should not happen except if one of the config value is invalid
        return;
    }

#ifdef USE_HDLC
    serverSettings.hdlc = &iecHdlcSetup;
#else
    serverSettings.wrapper = &udpSetup;
#endif //USE_HDLC
    // Set a periodic callback to be called after DEFAULT_PERIOD_US
    period_us = DEFAULT_PERIOD_US;
    lib_system->setPeriodicCb(readMeter,
        period_us,
        EXECUTION_TIME_US);

    // Set callback for received unicast messages
    lib_data->setDataReceivedCb(dataReceivedCb);

    // Set callback for received broadcast messages
    lib_data->setBcastDataReceivedCb(dataReceivedCb);

    //Open serial port.
    gxUart_init();
    // Start the stack
    lib_state->startStack();
    //Add COSEM objects.
    if ((ret = svr_InitObjects(&serverSettings)) != 0)
    {
		static char* str = "svr_InitObjects failed!";
		gxUart_write((const unsigned char*) str, strlen(str));
		SendToSink(false, (unsigned char*)str, strlen(str));
    }
    ///////////////////////////////////////////////////////////////////////
    //Server must initialize after all objects are added.
    ret = svr_initialize(&serverSettings);
    if (ret != DLMS_ERROR_CODE_OK)
    {
		static char* str = "svr_initialize failed!";
		gxUart_write((const unsigned char*) str, strlen(str));
		SendToSink(false, (unsigned char*)str, strlen(str));
    }
	static char* str = "Meter started!";
	gxUart_write((const unsigned char*) str, strlen(str));
    SendToSink(false, (unsigned char*)str, strlen(str));
}
