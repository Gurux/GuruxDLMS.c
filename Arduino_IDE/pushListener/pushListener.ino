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
// More information of Gurux DLMS products: https://www.gurux.fi/
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------
#include "GXDLMSNotify.h"
//---------------------------------------------------------------------------
// Un-comment ignored objects from ArduinoIgnore.h to improve performance
// and memory usage.
//---------------------------------------------------------------------------

//Received data.
gxByteBuffer frameData;
gxReplyData data;

//TODO: Add push objects.
gxPushSetup pushSetup;
gxClock clock1;

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char* str, const char* data)
{
  //Send trace to the serial port.
  byte c;
  Serial1.write("\t:");
  while ((c = pgm_read_byte(str++)) != 0)
  {
    Serial1.write(c);
  }
  if (data != NULL)
  {
    Serial1.write(data);
  }
  Serial1.write("\0");
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

void setup() {
  BYTE_BUFFER_INIT(&frameData);
  //Set frame capacity.
  bb_capacity(&frameData, 512);
  reply_init(&data);
  //TODO: Parameters depend from the meter.
  Notify.init(true, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
  //Serial 1 is used to send trace.
  Serial1.begin(115200);

  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //TODO: Configure push objects to match for the meter objects.
  {
    const unsigned char ln[6] = { 0, 7, 25, 9, 0, 255 };
    INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln);
  }
  {
    const unsigned char ln[6] = { 0, 0, 1, 0, 0, 255 };
    INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln);
  }
  //MIKKO arr_push(&pushSetup.pushObjectList, key_init(&pushSetup, co_init(2, 0)));
  arr_push(&pushSetup.pushObjectList, key_init(&clock1, co_init(2, 0)));
  GXTRACE(PSTR("Start application."), NULL);
}

void loop() {
  int pos, ret;
  dlmsVARIANT* it;
  int available = Serial.available();
  if (available > 0)
  {
    if (frameData.size + available > frameData.capacity)
    {
      available = frameData.capacity - frameData.size;
      if (available == 0)
      {
        GXTRACE(PSTR("Buffer is full."), NULL);
        bb_empty(&frameData);
        return;
      }
    }
    frameData.size += Serial.readBytes(frameData.data + frameData.size, available);
    if (Notify.GetData(&frameData, &data) != 0)
    {
      GXTRACE(PSTR("Invalid data received."), NULL);
      reply_clear(&data);
    }
    else
    {
      // If all data is received.
      if (data.complete)
      {
        bb_empty(&frameData);
        if (data.moreData == DLMS_DATA_REQUEST_TYPES_NONE)
        {
          gxArray items;
          arr_init(&items);
          if (Notify.GetPushValues(&pushSetup, data.dataValue.Arr, &items) == 0)
          {
            gxKey* k;
            char* str = NULL;
            for (pos = 0; pos != pushSetup.pushObjectList.size; ++pos)
            {
              if ((ret = arr_getByIndex(&pushSetup.pushObjectList, pos, (void**)&k)) != 0 ||
                  (ret = obj_toString((gxObject*)k->key, &str)) != 0)
              {
                break;
              }
              GXTRACE(PSTR("Received Push item:"), str);
              free(str);
            }
          }
          obj_clearPushObjectList(&items);
          bb_empty(&frameData);
          reply_clear(&data);
        }
      }
    }
  }
}
