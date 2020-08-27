
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
#include "GXDLMSClient.h"
//---------------------------------------------------------------------------
// Un-comment following lines from gxignore.h to improve perforamance 
// and memory usage.
// #define DLMS_IGNORE_SERVER
// #define DLMS_IGNORE_CLIENT
// #define GX_DLMS_MICROCONTROLLER
//---------------------------------------------------------------------------

//Received data.
gxByteBuffer frameData;

//How long reply is waited from the meter.
const uint32_t WAIT_TIME = 2000;
//How many times message is try to resend to the meter.
const uint8_t RESEND_COUNT = 3;

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char* str, const char* data)
{
  //Send trace to the serial port.
  byte c;
  Serial1.write("\t:", 2);
  while ((c = pgm_read_byte(str++)) != 0)
  {
    Serial1.write(c);
  }
  if (data != NULL)
  {
    Serial1.write(data, strlen(data));
  }
  Serial1.write("\0", 1);
  //Serial1.flush();
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
  uint16_t lastReadIndex = 0;
  uint32_t start = millis();
  frameData.size = 0;
  frameData.position = 0;
  do
  {
    available = Serial.available();
    if (available != 0)
    {
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
      delay(100);
    }
    //If the meter doesn't reply in given time.
    if (!(millis() - start < WAIT_TIME))
    {
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
  Serial.write(data->data, data->size);
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
      GXTRACE_INT(PSTR("Data send failed. Try to resend."), resend);
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

//Initialize connection to the meter.
int com_initializeConnection()
{
  int ret = DLMS_ERROR_CODE_OK;
  message messages;
  gxReplyData reply;
  mes_init(&messages);
  reply_init(&reply);

  //Get meter's send and receive buffers size.
  if ((ret = Client.SnrmRequest(&messages)) != 0 ||
      (ret = com_readDataBlock(&messages, &reply)) != 0 ||
      (ret = Client.ParseUAResponse(&reply.data)) != 0)
  {
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
  }
  mes_clear(&messages);
  reply_clear(&reply);
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

//Report error on output;
void com_reportError(
  const char* description,
  gxObject* object,
  unsigned char attributeOrdinal, 
  int ret)
{
  //char ln[25];
  //char type[30];
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
  if ((ret = Client.GetObjectsRequest(&data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = Client.ParseObjects(&reply.data)) != 0)
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
  if ((ret = Client.Read(object, attributeOrdinal, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = Client.UpdateValue(object, attributeOrdinal, &reply.dataValue)) != 0)
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
  if ((ret = Client.Write(object, attributeOrdinal, &data)) != 0 ||
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
  if ((ret = Client.Method(object, attributeOrdinal, params, &messages)) != 0 ||
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
    if ((ret = Client.ReadList(list, &messages)) != 0)
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
    //  printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
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
    ret = com_readRowsByRange(pg, &startTime, &endTime);
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
int com_readAllObjects()
{
  int ret;

  //Initialize connection.
  ret = com_initializeConnection();
  if (ret != DLMS_ERROR_CODE_OK)
  {
    GXTRACE_INT(PSTR("com_initializeConnection failed"), ret);
    return ret;
  }
  GXTRACE_INT(PSTR("com_initializeConnection SUCCEEDED"), ret);

  char* data = NULL;
  //Read Logical Device Name
  gxData ldn;
  cosem_init(BASE(ldn), DLMS_OBJECT_TYPE_DATA, "0.0.42.0.0.255");
  com_read(BASE(ldn), 2);
  obj_toString(BASE(ldn), &data);
  GXTRACE(PSTR("Logical Device Name"), data);
  obj_clear(BASE(ldn));
  free(data);

  //Read clock
  gxClock clock1;
  cosem_init(BASE(clock1), DLMS_OBJECT_TYPE_CLOCK, "0.0.1.0.0.255");
  com_read(BASE(clock1), 3);
  com_read(BASE(clock1), 2);
  obj_toString(BASE(clock1), &data);
  GXTRACE(PSTR("Clock"), data);
  obj_clear(BASE(clock1));
  free(data);

  //Read Profile generic.
  gxProfileGeneric pg;
  cosem_init(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, "1.0.99.1.0.255");
  com_read(BASE(pg), 3);
  com_readRowsByEntry(&pg, 1, 10);
  obj_toString(BASE(pg), &data);
  GXTRACE(PSTR("Load profile"), data);
  obj_clear(BASE(pg));
  free(data);
  //Release dynamically allocated objects.
  Client.ReleaseObjects();
  return ret;
}

void setup() {
  bb_init(&frameData);
  //Set frame size.
  bb_capacity(&frameData, 128);
  Client.init(true, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
  //Serial 1 is used to send trace.
  Serial1.begin(115200);

  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  int ret;
  if (millis() - runTime > 5000)
  {
    runTime = millis();
    GXTRACE(PSTR("Start reading"), NULL);
    //Initialize connection.
    ret = com_initializeConnection();
    if (ret != DLMS_ERROR_CODE_OK)
    {
      return;
    }
    ret = com_readAllObjects();
    com_close();
  }
}
