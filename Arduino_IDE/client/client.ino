
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
#include <EEPROM.h>
#include "GXDLMSClient.h"
//---------------------------------------------------------------------------
// Un-comment ignored objects from ArduinoIgnore.h to improve performance
// and memory usage.
//---------------------------------------------------------------------------

//Received data.
gxByteBuffer frameData;

//How long reply is waited from the meter.
const uint32_t WAIT_TIME = 2000;
//How many times message is try to resend to the meter.
const uint8_t RESEND_COUNT = 3;


uint16_t EEPROM_SIZE()
{
  return EEPROM.length();
}

int EEPROM_READ(uint16_t index, unsigned char* value)
{
  *value = EEPROM.read(index);
  return 0;
}

int EEPROM_WRITE(uint16_t index, unsigned char value)
{
  EEPROM.write(index, value);
  return 0;
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char* str, const char* data)
{
  //Send trace to the serial port.
  byte c;
  if (str != NULL)
  {
    Serial1.write("\t:");
    while ((c = pgm_read_byte(str++)) != 0)
    {
      Serial1.write(c);
    }
  }
  if (data != NULL)
  {
    Serial1.write(data);
  }
  if (str != NULL)
  {
    Serial1.write("\0");
  }
  delay(10);
}


///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_INT(const char* str, int32_t value)
{
  char data[10];
  sprintf(data, " %ld", value);
  GXTRACE(str, data);
}


///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_BB(const char* str, gxByteBuffer* value)
{
  int pos;
  char data[4];
  GXTRACE(str, NULL);
  for (pos = 0; pos < bb_size(value); ++pos)
  {
      sprintf(data, "%02X ", value->data[pos]);
      GXTRACE(NULL, data);
  }
  Serial1.write("\0");  
}

uint32_t runTime = 0;

///////////////////////////////////////////////////////////////////////
//Returns the approximate processor time in ms.
///////////////////////////////////////////////////////////////////////
uint32_t time_elapsed(void)
{
  return millis();
}

///////////////////////////////////////////////////////////////////////
// Returns current time.
// Get current time.
// Because there is no clock, clock object keeps base time and uptime is added to that.
///////////////////////////////////////////////////////////////////////
void time_now(gxtime* value)
{
  time_addSeconds(value, time_elapsed() / 1000);
}

int com_readSerialPort(
  unsigned char eop)
{
  //Read reply data.
  uint16_t pos;
  uint16_t available;
  unsigned char eopFound = 0;
  uint16_t lastReadIndex = frameData.position;
  uint32_t start = millis();
  do
  {
    available = Serial.available();
    if (available != 0)
    {
      if (frameData.size + available > frameData.capacity)
      {
        bb_capacity(&frameData, 20 + frameData.size + available);
      }
      Serial.readBytes((char*) (frameData.data + frameData.size), available);
      frameData.size += available;
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
    }
    else
    {
      delay(50);
    }
    //If the meter doesn't reply in given time.
    if (!(millis() - start < WAIT_TIME))
    {
      GXTRACE_INT(GET_STR_FROM_EEPROM("Received bytes: \n"), frameData.size);
      return DLMS_ERROR_CODE_RECEIVE_FAILED;
    }
  } while (eopFound == 0);
  return DLMS_ERROR_CODE_OK;
}

// Read DLMS Data frame from the device.
int readDLMSPacket(
  gxByteBuffer* data,
  gxReplyData* reply)
{
  int resend = 0, ret = DLMS_ERROR_CODE_OK;
  if (data->size == 0)
  {
    return DLMS_ERROR_CODE_OK;
  }
  reply->complete = 0;
  frameData.size = 0;
  frameData.position = 0;
  //Send data.
  if ((ret = Serial.write(data->data, data->size)) != data->size)
  {
    //If failed to write all bytes.
    GXTRACE(GET_STR_FROM_EEPROM("Failed to write all data to the serial port.\n"), NULL);
  }
  //Loop until packet is complete.
  do
  {
    if ((ret = com_readSerialPort(0x7E)) != 0)
    {
      if (ret == DLMS_ERROR_CODE_RECEIVE_FAILED && resend == RESEND_COUNT)
      {
        return DLMS_ERROR_CODE_SEND_FAILED;
      }
      ++resend;
      GXTRACE_INT(GET_STR_FROM_EEPROM("Data send failed. Try to resend."), resend);
      if ((ret = Serial.write(data->data, data->size)) != data->size)
      {
        //If failed to write all bytes.
        GXTRACE(GET_STR_FROM_EEPROM("Failed to write all data to the serial port.\n"), NULL);
      }
    }
    ret = Client.GetData(&frameData, reply);
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
  BYTE_BUFFER_INIT(&rr);
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
      if ((ret = Client.ReceiverReady(reply->moreData, &rr)) != DLMS_ERROR_CODE_OK)
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
  if ((ret = Client.ReleaseRequest(true, &msg)) != 0 ||
      (ret = com_readDataBlock(&msg, &reply)) != 0)
  {
    //Show error but continue close.
  }
  reply_clear(&reply);
  mes_clear(&msg);

  if ((ret = Client.DisconnectRequest(&msg)) != 0 ||
      (ret = com_readDataBlock(&msg, &reply)) != 0)
  {
    //Show error but continue close.
  }
  reply_clear(&reply);
  mes_clear(&msg);
  return ret;
}

//Read Invocation counter (frame counter) from the meter and update it.
int com_updateInvocationCounter(const char* invocationCounter)
{
  int ret = DLMS_ERROR_CODE_OK;
  //Read frame counter if security is used.
  if (invocationCounter != NULL && Client.GetSecurity() != DLMS_SECURITY_NONE)
  {
    uint32_t ic = 0;
    message messages;
    gxReplyData reply;
    unsigned short add = Client.GetClientAddress();
    DLMS_AUTHENTICATION auth = Client.GetAuthentication();
    DLMS_SECURITY security = Client.GetSecurity();
    Client.SetClientAddress(16);
    Client.SetAuthentication(DLMS_AUTHENTICATION_NONE);
    Client.SetSecurity(DLMS_SECURITY_NONE);
    if ((ret = com_initializeConnection()) == DLMS_ERROR_CODE_OK)
    {
      gxData d;
      cosem_init(BASE(d), DLMS_OBJECT_TYPE_DATA, invocationCounter);
      if ((ret = com_read(BASE(d), 2)) == 0)
      {
        GXTRACE_INT(GET_STR_FROM_EEPROM("Invocation Counter:"), var_toInteger(&d.value));
        ic = 1 + var_toInteger(&d.value);
      }
      obj_clear(BASE(d));
    }
    //Close connection. It's OK if this fails.
    com_close();
    //Return original settings.
    Client.SetClientAddress(add);
    Client.SetAuthentication(auth);
    Client.SetSecurity(security);
    Client.SetInvocationCounter(ic);
  }
  return ret;
}

int com_initializeOpticalHead()
{
  int baudRate;
  if (Client.GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
  {
    // Optical probes work with 300 bps 7E1:
    Serial.begin(300, SERIAL_7E1);
    static char DATA[6] = "/?!\r\n";
    //Send data.
    if (Serial.write(DATA, sizeof(DATA)) != sizeof(DATA))
    {
      //If failed to write all bytes.
      GXTRACE(GET_STR_FROM_EEPROM("Failed to write all data to the serial port.\n"), NULL);
    }
    String data = Serial.readStringUntil('\n');
    if (data[0] != '/')
    {
      GXTRACE(GET_STR_FROM_EEPROM("Failed to to receive IEC reply from serial port.\n"), NULL);
      return DLMS_ERROR_CODE_SEND_FAILED;
    }
    //Get used baud rate.
    switch (data[4])
    {
      case '0':
        baudRate = 300;
        break;
      case '1':
        baudRate = 600;
        break;
      case '2':
        baudRate = 1200;
        break;
      case '3':
        baudRate = 2400;
        break;
      case '4':
        baudRate = 4800;
        break;
      case '5':
        baudRate = 9600;
        break;
      case '6':
        baudRate = 19200;
        break;
      default:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    //Send ACK
    unsigned char buff[6];
    buff[0] = 0x06;
    //Send Protocol control character
    buff[1] = '2';// "2" HDLC protocol procedure (Mode E)
    buff[2] = (unsigned char)data[4];
    buff[3] = '2';
    buff[4] = (char)0x0D;
    buff[5] = 0x0A;
    if (Serial.write(buff, sizeof(buff)) != sizeof(buff))
    {
      //If failed to write all bytes.
      GXTRACE(GET_STR_FROM_EEPROM("Failed to write all data to the serial port.\n"), NULL);
    }
    Serial.flush();
    //This sleep is in standard. Do not remove.
    delay(300);
    Serial.begin(baudRate, SERIAL_8N1);
    Serial.readBytes(buff, sizeof(buff));
    //Some meters need this sleep. Do not remove.
    delay(800);
  }
  return 0;
}

//Initialize connection to the meter.
int com_initializeConnection()
{
  int ret = DLMS_ERROR_CODE_OK;
  message messages;
  gxReplyData reply;
  mes_init(&messages);
  reply_init(&reply);


#ifndef DLMS_IGNORE_HDLC
  //Get meter's send and receive buffers size.
  if ((ret = com_initializeOpticalHead()) != 0 ||
      (ret = Client.SnrmRequest(&messages)) != 0 ||
      (ret = com_readDataBlock(&messages, &reply)) != 0 ||
      (ret = Client.ParseUAResponse(&reply.data)) != 0)
  {
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
  }
  mes_clear(&messages);
  reply_clear(&reply);
#endif //DLMS_IGNORE_HDLC

  if ((ret = Client.AarqRequest(&messages)) != 0 ||
      (ret = com_readDataBlock(&messages, &reply)) != 0 ||
      (ret = Client.ParseAAREResponse(&reply.data)) != 0)
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
  if (Client.GetAuthentication() > DLMS_AUTHENTICATION_LOW)
  {
    if ((ret = Client.GetApplicationAssociationRequest(&messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0 ||
        (ret = Client.ParseApplicationAssociationResponse(&reply.data)) != 0)
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

//Get Association view.
int com_getAssociationView()
{
  int ret;
  message data;
  gxReplyData reply;
  mes_init(&data);
  reply_init(&reply);
  if ((ret = Client.GetObjectsRequest(&data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = Client.ParseObjects(&reply.data)) != 0)
  {
  }
  //Parse object one at the time. This can be used if there is a limited amount of the memory available.
  /*
    //Value is not parsed for decrease the memory usage.
    reply.ignoreValue = 1;
    if ((ret = Client.GetObjectsRequest(&data)) == 0 &&
      (ret = com_readDataBlock(&data, &reply)) == 0)
    {
    uint16_t pos, count;
    if ((ret = Client.ParseObjectCount(&reply.data, &count)) != 0)
    {
      GXTRACE_INT("cl_parseObjectCount failed", ret);
    }
    gxObject obj;
    for (pos = 0; pos != count; ++pos)
    {
      memset(&obj, 0, sizeof(gxObject));
      if ((ret = Client.ParseNextObject(&reply.data, &obj)) != 0)
      {
        break;
      }
      //Only data and register objects are added to the association view.
      if (obj.objectType == DLMS_OBJECT_TYPE_DATA)
      {
        gxData* data = (gxData*) malloc(sizeof(gxData));
        INIT_OBJECT((*data), (DLMS_OBJECT_TYPE) obj.objectType, obj.logicalName);
        data->base.shortName = obj.shortName;
        oa_push(Client.GetObjects(), BASE((*data)));
      }
      else if (obj.objectType == DLMS_OBJECT_TYPE_REGISTER)
      {
        gxRegister* r = (gxRegister*) malloc(sizeof(gxRegister));
        INIT_OBJECT((*r), (DLMS_OBJECT_TYPE) obj.objectType, obj.logicalName);
        r->base.shortName = obj.shortName;
        oa_push(Client.GetObjects(), BASE((*r)));
      }
    }
    }
  */
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
  if ((ret = Client.Read(object, attributeOrdinal, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = Client.UpdateValue(object, attributeOrdinal, &reply.dataValue)) != 0)
  {
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_read failed."), ret);
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
  if ((ret = Client.Write(object, attributeOrdinal, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0)
  {
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_write failed."), ret);
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
  if ((ret = Client.Method(object, attributeOrdinal, params, &messages)) != 0 ||
      (ret = com_readDataBlock(&messages, &reply)) != 0)
  {
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_method failed."), ret);
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
    if ((ret = Client.ReadList(list, &messages)) != 0)
    {
      GXTRACE_INT(GET_STR_FROM_EEPROM("com_readList failed."), ret);
    }
    else
    {
      reply_init(&reply);
      BYTE_BUFFER_INIT(&rr);
      BYTE_BUFFER_INIT(&bb);
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
          if ((ret = Client.ReceiverReady(reply.moreData, &rr)) != DLMS_ERROR_CODE_OK ||
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
        ret = Client.UpdateValues(list, &bb);
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
  if ((ret = Client.ReadRowsByEntry(object, index, count, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = Client.UpdateValue((gxObject*)object, 2, &reply.dataValue)) != 0)
  {
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_readRowsByEntry failed."), ret);
  }
  mes_clear(&data);
  reply_clear(&reply);
  return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange(
  gxProfileGeneric* object,
  gxtime* start,
  gxtime* end)
{
  int ret;
  message data;
  gxReplyData reply;
  mes_init(&data);
  reply_init(&reply);
  if ((ret = Client.ReadRowsByRange(object, start, end, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = Client.UpdateValue((gxObject*)object, 2, &reply.dataValue)) != 0)
  {
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_readRowsByRange failed."), ret);
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
  ret = oa_getObjects2(Client.GetObjects(), types, 3, &objects);
  if (ret != DLMS_ERROR_CODE_OK)
  {
    return ret;
  }
  if ((Client.GetNegotiatedConformance() & DLMS_CONFORMANCE_MULTIPLE_REFERENCES) != 0)
  {
    arr_init(&list);
    //Try to read with list first. All meters do not support it.
    for (pos = 0; pos != Client.GetObjects()->size; ++pos)
    {
      ret = oa_getByIndex(Client.GetObjects(), pos, &obj);
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
  ret = oa_getObjects(Client.GetObjects(), DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
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
  ret = oa_getObjects(Client.GetObjects(), DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
  if (ret != DLMS_ERROR_CODE_OK)
  {
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
  }
  BYTE_BUFFER_INIT(&ba);
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
    ret = com_readRowsByRange(pg, &startTime, &endTime);
  }
  //Do not clear objects list because it will free also objects from association view list.
  oa_empty(&objects);
  return ret;
}

// This function reads ALL objects that meter have excluded profile generic objects.
// It will loop all object's attributes.
int com_readValues()
{
  gxByteBuffer attributes;
  unsigned char ch;
  gxObject* object;
  unsigned long index;
  int ret, pos;
  BYTE_BUFFER_INIT(&attributes);

  for (pos = 0; pos != Client.GetObjects()->size; ++pos)
  {
    ret = oa_getByIndex(Client.GetObjects(), pos, &object);
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
int com_readAllObjects(const char* invocationCounter)
{
  int ret;
  if (invocationCounter != NULL)
  {
    if ((ret = com_updateInvocationCounter(invocationCounter)) != 0)
    {
      return ret;
    }
  }
  //Initialize connection.
  ret = com_initializeConnection();
  if (ret != DLMS_ERROR_CODE_OK)
  {
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_initializeConnection failed"), ret);
    return ret;
  }
  GXTRACE_INT(GET_STR_FROM_EEPROM("com_initializeConnection SUCCEEDED"), ret);
  ///////////////////////////////////////////////////////////////////////////////////
  // Un-commnent this if association view is read and serialized.
  // Note! Modify com_getAssociationView to returns only wanted objects.
  ///////////////////////////////////////////////////////////////////////////////////
  /*
    //Only current association view (attribute #2) is serialized.
    gxObject* obj = NULL;
    gxAssociationLogicalName ln;
    gxAssociationShortName sn;
    static unsigned char CURRENT_LN[] = { 0, 0, 40, 0, 0, 0xFF };
    gxSerializerSettings serializerSettings;
    ser_init(&serializerSettings);
    gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = { IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, GET_ATTRIBUTE_EXCEPT(2)) };
    serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
    serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
    gxObject* CURRENT_ASSOCIATION[1];
    if (Client.UseLogicalNameReferencing())
    {
    if ((ret = INIT_OBJECT(ln, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, CURRENT_LN)) != 0)
    {
      return ret;
    }
    CURRENT_ASSOCIATION[0] = BASE(ln);
    }
    else
    {
    if ((ret = INIT_OBJECT(sn, DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME, CURRENT_LN)) != 0)
    {
      return ret;
    }
    NON_SERIALIZED_OBJECTS[0].objectType = DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME;
    CURRENT_ASSOCIATION[0] = BASE(sn);
    }
    if ((ret = Client.LoadObjects(&serializerSettings, CURRENT_ASSOCIATION, sizeof(CURRENT_ASSOCIATION) / sizeof(CURRENT_ASSOCIATION[0]))) != 0)
    {
    GXTRACE_INT(GET_STR_FROM_EEPROM("LoadObjects FAILED:"), ret);
    //Read the association view from the meter if reading from the EEPROM fails.
    //Get objects from the meter and read them.
    ret = com_getAssociationView();
    if (ret != DLMS_ERROR_CODE_OK)
    {
      //If error code is 260 association view is too big and can't fit to memory.
      GXTRACE_INT(GET_STR_FROM_EEPROM("Association View read FAILED:"), ret);
      return ret;
    }
    ret = com_readScalerAndUnits();
    if (ret != DLMS_ERROR_CODE_OK)
    {
      GXTRACE_INT(GET_STR_FROM_EEPROM("Scaler and units read FAILED:"), ret);
      return ret;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Read Profile Generic columns.
    ret = com_readProfileGenericColumns();
    if (ret != DLMS_ERROR_CODE_OK)
    {
      GXTRACE_INT(GET_STR_FROM_EEPROM("Profile Generic columns read FAILED:"), ret);
      return ret;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Only current association view is serialized.
    if (Client.UseLogicalNameReferencing())
    {
      oa_copy(&ln.objectList, Client.GetObjects());
    }
    else
    {
      oa_copy(&sn.objectList, Client.GetObjects());
    }
    if ((ret = Client.SaveObjects(&serializerSettings, CURRENT_ASSOCIATION, sizeof(CURRENT_ASSOCIATION) / sizeof(CURRENT_ASSOCIATION[0]))) != 0)
    {
      GXTRACE_INT(GET_STR_FROM_EEPROM("SaveObjects FAILED:"), ret);
      return ret;
    }
    if (Client.UseLogicalNameReferencing())
    {
      obj_clear(BASE(ln));
    }
    else
    {
      obj_clear(BASE(sn));
    }
    }
    else
    {
    if (Client.UseLogicalNameReferencing())
    {
      oa_attach(Client.GetObjects(), ln.objectList.data, ln.objectList.size);
    }
    else
    {
      oa_attach(Client.GetObjects(), sn.objectList.data, sn.objectList.size);
    }
    GXTRACE_INT(GET_STR_FROM_EEPROM("LoadObjects Items loaded:"), Client.GetObjects()->size);
    }
  */
  //Read just wanted objects withour serializating the association view.
  char* data = NULL;
  //Read Logical Device Name
  gxData ldn;
  cosem_init(BASE(ldn), DLMS_OBJECT_TYPE_DATA, "0.0.42.0.0.255");
  com_read(BASE(ldn), 2);
  obj_toString(BASE(ldn), &data);
  GXTRACE(GET_STR_FROM_EEPROM("Logical Device Name"), data);
  obj_clear(BASE(ldn));
  free(data);
  //Read clock
  gxClock clock1;
  cosem_init(BASE(clock1), DLMS_OBJECT_TYPE_CLOCK, "0.0.1.0.0.255");
  com_read(BASE(clock1), 3);
  com_read(BASE(clock1), 2);
  obj_toString(BASE(clock1), &data);
  GXTRACE(GET_STR_FROM_EEPROM("Clock"), data);
  obj_clear(BASE(clock1));
  free(data);
  /*
     TODO: This is an example how to read profile generic.
    //Read Profile generic.
    gxProfileGeneric pg;
    cosem_init(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, "1.0.99.1.0.255");
    com_read(BASE(pg), 3);
    com_readRowsByEntry(&pg, 1, 2);
    obj_toString(BASE(pg), &data);
    GXTRACE(GET_STR_FROM_EEPROM("Load profile"), data);
    obj_clear(BASE(pg));
    free(data);
  */
  //Release dynamically allocated objects.
  Client.ReleaseObjects();
  return ret;
}

void setup() {
  GXTRACE(GET_STR_FROM_EEPROM("Start application"), NULL);
  BYTE_BUFFER_INIT(&frameData);
  //Set frame capacity.
  bb_capacity(&frameData, 128);
  Client.init(true, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
  //Un-comment this if you want to set system title, block cipher key or authentication key.
  /*
    Client.SetSecurity(DLMS_SECURITY_AUTHENTICATION_ENCRYPTION);
    //TODO: Change logical name of the frame counter if it's used to com_readAllObjects.
    gxByteBuffer bb;
    bb_init(&bb);
    bb_addHexString(&bb, "3132333435363738");
    Client.SetSystemTitle(&bb);
    bb_clear(&bb);
    bb_addHexString(&bb, "D0 D1 D2 D3 D4 D5 D6 D7D8 D9 DA DB DC DD DE DF");
    Client.SetAuthenticationKey(&bb);
    bb_clear(&bb);
    bb_addHexString(&bb, "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    Client.SetBlockCipherKey(&bb);
    bb_clear(&bb);
  */
  //Serial 1 is used to send trace.
  Serial1.begin(115200);

  // start serial port at 9600 bps:
 if (Client.GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
  {
    // Optical probes work with 300 bps 7E1:
    Serial.begin(300, SERIAL_7E1);
  }
  else
  {
    Serial.begin(9600, SERIAL_8N1);    
  }
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  int ret;
  if (millis() - runTime > 5000)
  {
    runTime = millis();
    GXTRACE(GET_STR_FROM_EEPROM("Start reading"), NULL);
    //TODO: Change logical name of the frame counter if it's used.
    ret = com_readAllObjects("0.0.43.1.0.255");
    com_close();
  }
}

#ifdef DLMS_DEBUG
//Print detailed information to the serial port.
void svr_trace(const char* str, const char* data)
{
  GXTRACE(str, data);
}
#endif //DLMS_DEBUG
