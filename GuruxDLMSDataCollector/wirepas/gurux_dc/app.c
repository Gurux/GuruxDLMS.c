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
#include "dlms/include/client.h"
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

//Is new data received.
static bool received = false;

///////////////////////////////////////////////////////////////////////
#define USE_HDLC

#define PDU_BUFFER_SIZE 1024

#define HDLC_HEADER_SIZE 17
#define WRAPPER_FRAME_SIZE 8 + PDU_BUFFER_SIZE
#define HDLC_SIZE 256
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

//Space for password.
unsigned char PW[64];

///////////////////////////////////////////////////////////////////////
//Space for client challenge.
unsigned char C2S_CHALLENGE[16];
//Space for server challenge.
unsigned char S2C_CHALLENGE[16];

///////////////////////////////////////////////////////////////////////
// Define all COSEM objects here so they are not use stack.

unsigned char CTOS_CHALLENGE[16];
unsigned char STOC_CHALLENGE[16];

///////////////////////////////////////////////////////////////////////
// Define meter settings.
unsigned char CLIENT_ADDRESS = 0x20;
unsigned short SERVER_ADDRESS = 0x1;

#define AUTHENTICATION_LEVEL DLMS_AUTHENTICATION_HIGH
static char* PASSWORD = "Gurux";

unsigned char connecting = 0;

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
    char loopCount = 0;
    //Read reply data.
    frameData.size = 0;
    frameData.position = 0;
#ifdef USE_HDLC
    int pos;
    unsigned char eopFound = 0;
    unsigned short lastReadIndex = 0;
    do
    {
        ++loopCount;
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
        user_delay_ms(50);
    } while (eopFound == 0 && loopCount != 4);
    if (!eopFound)
    {
        return 1;
    }
#else
    do
    {
        ++loopCount;
        user_delay_ms(50);
    } while (!received && loopCount != 4);
    if (!received)
    {
        return 1;
    }
    received = false;
#endif //USE_HDLC
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
    unsigned char data[40];
    bb_attach(&rr, data, 0, sizeof(data));
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
#ifndef DLMS_IGNORE_HDLC
    if (meterSettings.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
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
    }
#endif //DLMS_IGNORE_HDLC
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
    // Get challenge Is HLS authentication is used.
    if (meterSettings.authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = cl_getApplicationAssociationRequest(&meterSettings, &messages)) != 0 ||
            (ret = com_readDataBlock(&messages, &reply)) != 0 ||
            (ret = cl_parseApplicationAssociationResponse(&meterSettings, &reply.data)) != 0)
        {
        }
        mes_clear(&messages);
        reply_clear(&reply);
    }
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
    // if ((settings->connected & DLMS_CONNECTION_STATE_DLMS) == 0)
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

int com_method(
    DLMS_OBJECT_TYPE objectType,
    unsigned char* name,
    unsigned char index,
    dlmsVARIANT* params)
{
    int ret;
    message messages;
    gxReplyData reply;
    reply_init(&reply);
    unsigned char buff[PDU_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, buff, 0, sizeof(buff));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);

    //TODO: Size of reply data depends from the amount of COSEM objects.
    //Here we expect that receive data fits to one PDU.
    unsigned char replydata[PDU_BUFFER_SIZE];
    bb_attach(&reply.data, replydata, 0, sizeof(replydata));
    if ((ret = cl_methodLN(&meterSettings, name, objectType, index, params, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0)
    {
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}

//Serial port data received.
#ifdef TARGET_BOARDS_PCA10056
static void dataReceivedSp(uint8_t* ch, size_t n)
#else
static void dataReceivedSp(uint8_t ch)
#endif // TARGET_BOARDS_PCA10056
{
#ifdef TARGET_BOARDS_PCA10056
    if (bb_set(&frameData, ch, n) == 0)
    {
#ifdef USE_HDLC
        if (ch[n - 1] == 0x7E)
        {
            received = true;
        }
#else
        //There is no end of char in HDLC. For this reason all received bytes must be handled.
        received = true;
#endif //USE_HDLC
    }
    else
    {
        frameData.size = frameData.position = 0;
    }
#else
    if (bb_setUInt8(&frameData, ch) == 0)
    {
#ifdef USE_HDLC
        if (ch == 0x7E)
        {
            received = true;
        }
#else
        //There is no end of char in HDLC. For this reason all received bytes must be handled.
        received = true;
#endif //USE_HDLC
    }
    else
    {
        frameData.size = frameData.position = 0;
    }
#endif //TARGET_BOARDS_PCA10056
}

//Initialize UART.
void gxUart_init()
{
    // Initialize the hardware module
    Usart_init(DLMS_BAUDRATE, UART_FLOW_CONTROL_NONE);
    Usart_enableReceiver(dataReceivedSp);
    Usart_setEnabled(true);
    Usart_receiverOn();
}

//Read object and send data to the sink.
int com_read(
    unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
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
    if ((ret = cl_readLN(&meterSettings, name, objectType, attributeOrdinal, NULL, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0)
    {
        return ret;
    }
    //Compressed data.
    unsigned char COMPRESSED[100];
    gxByteBuffer compressed;
    bb_attach(&compressed, COMPRESSED, 0, sizeof(COMPRESSED));
    //Octet string
    unsigned char OCTET[12];
    gxByteBuffer octet;
    bb_attach(&octet, OCTET, 0, sizeof(OCTET));
    dlmsVARIANT var;
    var_init(&var);
    if ((ret = cosem_getVariant(&reply.data, &var)) == 0)
    {
        if (var.vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            if ((ret = cosem_getOctetString(&reply.data, &octet)) != 0 ||
                (ret = bb_set(&compressed, octet.data, octet.size)) != 0)
            {
            }
            bb_clear(&octet);
        }
        else
        {
            if ((ret = var_getBytes3(&var, var.vt, &compressed, 1)) != 0)
            {
            }
        }
        var_clear(&var);
        if (ret == 0)
        {
            if (compressed.size > 100)
            {
                char* err = "Error! Too much data in payload.";
                SendToSink(true, (unsigned char*)err, strlen(err));
            }
            else
            {
                SendToSink(true, compressed.data, compressed.size);
            }
        }
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

//Read profile generic object.
int com_readProfileGeneric(
    unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
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
    if ((ret = cl_readLN(&meterSettings, name, objectType, attributeOrdinal, NULL, &data)) != 0 ||
        (ret = com_readDataBlock(&data, &reply)) != 0)
    {
        return ret;
    }
    uint16_t pos, count = 0xFFFF, columns = 0xFFFF;
    unsigned char pos2;
    //Compressed data.
    unsigned char COMPRESSED[256];
    gxByteBuffer compressed;
    bb_attach(&compressed, COMPRESSED, 0, sizeof(COMPRESSED));
    //Octet string
    unsigned char OCTET[12];
    gxByteBuffer octet;
    bb_attach(&octet, OCTET, 0, sizeof(OCTET));
    dlmsVARIANT var;
    var_init(&var);
    if ((ret = cosem_checkArray(&reply.data, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getStructure(&reply.data, &columns)) == 0)
            {
                for (pos2 = 0; pos2 != columns; ++pos2)
                {
                    if ((ret = cosem_getVariant(&reply.data, &var)) != 0)
                    {
                        break;
                    }
                    if (var.vt == DLMS_DATA_TYPE_OCTET_STRING)
                    {
                        if ((ret = cosem_getOctetString(&reply.data, &octet)) != 0 ||
                            (ret = bb_set(&compressed, octet.data, octet.size)) != 0)
                        {
                            break;
                        }
                        bb_clear(&octet);
                    }
                    else
                    {
                        if ((ret = var_getBytes3(&var, var.vt, &compressed, 0)) != 0)
                        {
                            break;
                        }
                    }
                    var_clear(&var);
                }
            }
        }
    }
    if (compressed.size > 100)
    {
        char* err = "Error! Too much data in payload.";
        SendToSink(true, (unsigned char*)err, strlen(err));
    }
    else
    {
        SendToSink(true, compressed.data, compressed.size);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

//Read object and send data to the sink.
int com_read2(
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
        return ret;
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

/**
* Update firmware of the meter.
*
* In image update following steps are made:
1. Image_transfer_enabled is read.
2. Image block size is read.
3. image_transferred_blocks_status is read to check is image try to update before.
4. image_transfer_initiate
5. image_transfer_status is read.
6. image_block_transfer
7. image_transfer_status is read.
8. image_transfer_status is read.
9. image_verify is called.
10. image_transfer_status is read.
11. image_activate is called.
*/
int imageUpdate(unsigned char* identification, uint16_t identificationSize, unsigned char* image, uint32_t imageSize)
{
    int ret;
    gxByteBuffer bb;
    bb_init(&bb);
    dlmsVARIANT param;
    var_init(&param);
    gxImageTransfer im;
    unsigned char ln[] = { 0,0,44,0,0,255 };
    INIT_OBJECT(im, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln);
    //1. Image_transfer_enabled is read.
    if ((ret = com_read2(&im.base, 5)) == 0 &&
        //2. Image block size is read.
        (ret = com_read2(&im.base, 2)) == 0 &&
        //3. image_transferred_blocks_status is read to check is image try to update before.
        (ret = com_read2(&im.base, 3)) == 0 &&
        //4. image_transfer_initiate
        (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_STRUCTURE)) == 0 &&
        (ret = bb_setInt8(&bb, 2)) == 0 &&
        (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_OCTET_STRING)) == 0 &&
        (ret = hlp_setObjectCount(identificationSize, &bb)) == 0 &&
        (ret = bb_set(&bb, identification, identificationSize)) == 0 &&
        (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_UINT32)) == 0 &&
        (ret = bb_setInt32(&bb, imageSize)) == 0 &&
        (ret = var_addOctetString(&param, &bb)) == 0 &&
        (ret = com_method(DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln, 1, &param)) == 0)
    {
        //5. image_transfer_status is read.
        if ((ret = com_read2(&im.base, 6)) == 0)
        {
            // 6. image_block_transfer
            uint32_t count = im.imageBlockSize;
            uint32_t blockNumber = 0;
            while (imageSize != 0)
            {
                if (imageSize < im.imageBlockSize)
                {
                    count = imageSize;
                }
                bb_clear(&bb);
                if ((ret = bb_setInt8(&bb, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setInt8(&bb, 2)) != 0 ||
                    (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_UINT32)) != 0 ||
                    (ret = bb_setInt32(&bb, blockNumber)) != 0 ||
                    (ret = bb_setInt8(&bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(count, &bb)) != 0 ||
                    (ret = bb_set(&bb, image, count)) != 0 ||
                    (ret = var_addOctetString(&param, &bb)) != 0 ||
                    (ret = com_method(DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln, 2, &param)) != 0)
                {
                    break;
                }
                imageSize -= count;
                ++blockNumber;
            }
            if (ret == 0)
            {
                //7. image_transfer_status is read.
                ret = com_read2(&im.base, 6);
                if (ret == 0)
                {
                    //9. image_verify is called.
                    var_clear(&param);
                    GX_INT8(param) = 0;
                    if ((ret = com_method(DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln, 3, &param)) == 0 ||
                        ret == DLMS_ERROR_CODE_TEMPORARY_FAILURE)
                    {
                        while (1)
                        {
                            //10. image_transfer_status is read.
                            ret = com_read2(&im.base, 6);
                            if (im.imageTransferStatus == DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL)
                            {
                                break;
                            }
                            if (im.imageTransferStatus == DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED)
                            {
                                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                                break;
                            }
                            //Wait until image is activated.
                           // Sleep(10000);
                        }
                        if (ret == 0)
                        {
                            ret = com_read2(&im.base, 6);
                            //11. image_activate is called.
                            ret = com_method(DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln, 4, &param);
                        }
                    }
                }
            }
        }
    }
    bb_clear(&bb);
    var_clear(&param);
    return ret;
}

/**
 * \brief   Make connection to the meter and read needed values.
 */
static uint32_t readMeter(void)
{
    //Check that there is no read ongoing...
    if (connecting != 0)
    {
        static char* READ_FAILED = "Read on progress!";
        SendToSink(false, (unsigned char*)READ_FAILED, 13);
    }
    connecting = 1;
    static char* str = "Start read!";
    SendToSink(false, (unsigned char*)str, strlen(str));
    int ret = 0;
    if ((ret = connect2Client(&meterSettings)) == 0)
    {
        //Read clock.
        unsigned char ln3[6] = { 0, 0, 1, 0, 0, 255 };
        ret = com_read(ln3, DLMS_OBJECT_TYPE_CLOCK, 2);

        //Read Instant profile.
        unsigned char ln2[6] = { 1, 0, 94, 91, 0, 255 };
        ret = com_readProfileGeneric(ln2, DLMS_OBJECT_TYPE_PROFILE_GENERIC, 2);
        com_close();
    }
    connecting = 0;
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
 * \brief   Client sends byte value 1 or 2. This value is used to invoke disconnect control and open or close the valve. 
 * \param   data
 *          Received data, \ref app_lib_data_received_t
 * \return  Result code, \ref app_lib_data_receive_res_e
 */
static app_lib_data_receive_res_e dataReceivedCb(
    const app_lib_data_received_t* data)
{
    static char* READ_FAILED = "Action start!";
    SendToSink(false, (unsigned char*)READ_FAILED, 13);
    if (data->num_bytes != 1)
    {
        // Data was not for this application
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
    }
    //Check that there is no read ongoing...
    if (connecting != 0)
    {
        static char* READ_FAILED = "Read on progress!";
        SendToSink(false, (unsigned char*)READ_FAILED, 13);
    }
    connecting = 1;
    if (data->bytes[0] == 1 || data->bytes[0] == 2)
    {
        int ret;
#ifdef USE_HDLC
        cl_init(&meterSettings, 1, SERVER_ADDRESS, CLIENT_ADDRESS, AUTHENTICATION_LEVEL, NULL, DLMS_INTERFACE_TYPE_HDLC);
#else
        cl_init(&meterSettings, 1, SERVER_ADDRESS, CLIENT_ADDRESS, AUTHENTICATION_LEVEL, NULL, DLMS_INTERFACE_TYPE_WRAPPER);
#endif //USE_HDLC
        bb_attach(&frameData, CLIENT_FRAME, 0, sizeof(CLIENT_FRAME));
        bb_attach(&rerverReply, SERVER_REPLY_PDU, 0, sizeof(SERVER_REPLY_PDU));
        //Get buffer for serialized data.
        bb_attach(&clientPdu, CLIENT_PDU, 0, sizeof(CLIENT_PDU));
        meterSettings.serializedPdu = &clientPdu;
        //Allocate space for client password.
        BB_ATTACH(meterSettings.password, PW, 0);
        bb_addString(&meterSettings.password, PASSWORD);

        //Allocate space for client challenge.
        BB_ATTACH(meterSettings.ctoSChallenge, C2S_CHALLENGE, 0);
        //Allocate space for server challenge.
        BB_ATTACH(meterSettings.stoCChallenge, S2C_CHALLENGE, 0);
        if ((ret = connect2Client(&meterSettings)) == 0)
        {
            dlmsVARIANT param;
            var_init(&param);
            unsigned char ln[6] = { 0, 0, 96, 3, 10, 255 };
            if ((ret = com_method(DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln, data->bytes[0], &param)) != 0)
            {
                static char* READ_FAILED = "Action failed";
                SendToSink(false, (unsigned char*)READ_FAILED, 13);
            }
            var_clear(&param);
            com_close();
        }
    }
    else
    {
        static char* READ_FAILED = "Action failed!";
        SendToSink(false, (unsigned char*)READ_FAILED, 14);
        // Data was not for this application
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
    }
    connecting = 0;
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
    connecting = 0;
    // Open Wirepas public API
    API_Open(functions);

    // Open HAL
    HAL_Open();
    //Open serial port.
    gxUart_init();
    Usart_sendBuffer("Started", sizeof("Started") - 1);
    // Basic configuration of the node with a unique node address
    if (configureNode(getUniqueAddress(),
        NETWORK_ADDRESS,
        NETWORK_CHANNEL) != APP_RES_OK)
    {
        // Could not configure the node
        // It should not happen except if one of the config value is invalid
        static char* FAILED = "configureNode FAILED!";
        gxUart_write((const unsigned char*)FAILED, strlen(FAILED));
        return;
    }

#ifdef USE_HDLC
    cl_init(&meterSettings, SERVER_ADDRESS, CLIENT_ADDRESS, 1, AUTHENTICATION_LEVEL, NULL, DLMS_INTERFACE_TYPE_HDLC);
#else
    cl_init(&meterSettings, SERVER_ADDRESS, CLIENT_ADDRESS, 1, AUTHENTICATION_LEVEL, NULL, DLMS_INTERFACE_TYPE_WRAPPER);
#endif //USE_HDLC
    bb_attach(&frameData, CLIENT_FRAME, 0, sizeof(CLIENT_FRAME));
    bb_attach(&rerverReply, SERVER_REPLY_PDU, 0, sizeof(SERVER_REPLY_PDU));
    //Get buffer for serialized data.
    bb_attach(&clientPdu, CLIENT_PDU, 0, sizeof(CLIENT_PDU));
    meterSettings.serializedPdu = &clientPdu;
    //Allocate space for client password.
    BB_ATTACH(meterSettings.password, PW, 0);
    bb_addString(&meterSettings.password, PASSWORD);

    //Allocate space for client challenge.
    BB_ATTACH(meterSettings.ctoSChallenge, C2S_CHALLENGE, 0);
    //Allocate space for server challenge.
    BB_ATTACH(meterSettings.stoCChallenge, S2C_CHALLENGE, 0);
    // Set a periodic callback to be called after DEFAULT_PERIOD_US
    period_us = DEFAULT_PERIOD_US;
    lib_system->setPeriodicCb(readMeter,
        period_us,
        EXECUTION_TIME_US);

    // Set callback for received unicast messages
    lib_data->setDataReceivedCb(dataReceivedCb);

    // Set callback for received broadcast messages
    lib_data->setBcastDataReceivedCb(dataReceivedCb);

    // Start the stack
    lib_state->startStack();

    static char* STARTED = "Client started!";
    gxUart_write((const unsigned char*)STARTED, strlen(STARTED));
}
