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

#if defined(__SAMD51J20A__)
#define __ASF__
#endif //defined(__SAMD51J20A__)
#ifdef __ASF__
#include <sam.h>
#include <atmel_start.h>
#endif //__ASF__

#ifdef __AVR__
#include <xc.h>
#include <avr/io.h>
#endif //__AVR__

#include <time.h>
#include "dlms/include/gxignore.h"
#include "dlms/include/client.h"
#include "dlms/include/serverevents.h"
#include "dlms/include/dlmssettings.h"
#include "dlms/include/gxvalueeventargs.h"
#include "dlms/include/gxObjects.h"
#include "dlms/include/bytebuffer.h"
#include "dlms/include/variant.h"
#include "dlms/include/objectarray.h"
#include "dlms/include/cosem.h"
//obj_toString requires this
#include "dlms/include/converters.h"
#include "dlms/include/helpers.h"

#ifdef __ASF__
struct io_descriptor *io;

struct io_descriptor *traceIo;
#endif //__ASF__

#ifdef __AVR__
// TODO: This depends from the CPU. 
// Verify this from the documentation. If it's wrong the serial port is not work correctly.
#define F_CPU	16000000UL
/**
 * \brief Set the desired baud rate value
 *
 * set the desired baudrate, the define is used by the util/setbaud.h to calculate the
 * register values.
 */
#define BAUD 9600

//set the baud rate tolerance to 2%
#define BAUD_TOL 2

//define the UART data buffer ready interrupt vector
#define UART0_DATA_EMPTY_IRQ USART0_UDRE_vect

#ifdef UBRR1H
//define the UART data buffer ready interrupt vector.
//This is used for debugging.
#define UART1_DATA_EMPTY_IRQ USART1_UDRE_vect
#endif //UBRR1H

//! define the UART data
#define UART0_RX_IRQ USART0_RX_vect
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif // __AVR__

#include <stdlib.h>
#include <stdio.h>
//DLMS settings.
dlmsSettings settings;

//How long reply is waited from the meter.
const uint32_t WAIT_TIME = 2000;
//How many times message is try to resend to the meter.
const uint8_t RESEND_COUNT = 3;

//Is trace used.
static GX_TRACE_LEVEL trace = GX_TRACE_LEVEL_VERBOSE;
//Received data.
static gxByteBuffer frameData;

#ifdef __AVR__
//Is new data available for read in the UART.
volatile unsigned char available = 0;
volatile unsigned long milliseconds;

ISR(TIMER1_COMPA_vect)
{
	++milliseconds;
	if (milliseconds % 1000 == 0)
	{
		system_tick();
	}
}

unsigned long millis(void)
{
	return milliseconds;
}

#endif //__AVR__

#ifdef __ASF__
unsigned long millis(void)
{
	return SysTick->VAL;
}
#endif //__ASF__


#ifdef UBRR1H
/**
 * \brief Function for putting a char in the trace UART buffer
 *
 * \param data the data to add to the trace UART buffer and send
 *
 */
static inline void uart_putTraceChar(uint8_t data)
{
#ifdef __AVR__
	//Wait until data is written.
	while ((UCSR1B & (1 << UDRIE1)) != 0)
	{
		_delay_ms(10);		
	}
	/* Disable interrupts */
	cli();
	UDR1 = data;
	// First data in buffer, enable data ready interrupt
	UCSR1B |= (1 << UDRIE1);
	/* Enable interrupts */
	sei();
#endif //__AVR__
}
#endif //UBRR1H

/**
 * \brief Function for putting a char in the UART buffer
 *
 * \param data the data to add to the UART buffer and send.
 *
 */
static inline int uart_traceWrite(const char* data)
{
#ifdef UBRR1H	
	if (data != NULL)
	{		
		int pos, len = strlen(data);
		for(pos = 0; pos < len; ++pos)
		{
			uart_putTraceChar(data[pos]);
		}
	}
#endif //UBRR1H
	return 0;
}


///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char* str, const char* data)
{
	if (trace == GX_TRACE_LEVEL_VERBOSE)
	{
#ifdef __AVR__
		time_t tm = time(NULL);
		uart_traceWrite((const char*) "\t:");
		uart_traceWrite(asctime(gmtime(&tm)));
		uart_traceWrite((const char*) "\t");
		uart_traceWrite(str);
		uart_traceWrite(data);
		uart_traceWrite((const char*) "\0");
		_delay_ms(10);		
#endif //__AVR__
#ifdef __ASF__
		delay_ms(10);
#endif //__ASF__
	}
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

int com_readSerialPort(
  unsigned char eop)
{
  //Read reply data.
  uint16_t pos;
  unsigned char eopFound = 0;
  uint16_t lastReadIndex = frameData.position;
  uint32_t start = millis();
  do
  {
#ifdef __ASF__	  
	io_read(io, frameData.data, 1);
	frameData.size += 1;
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
#endif //__ASF__	  
#ifdef __AVR__
    if (available)
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
            break;
          }
        }
        lastReadIndex = pos;
      }
    }
#endif //__AVR__
    else
    {
#ifdef __AVR__
      _delay_ms(50);
#endif //__AVR__
#ifdef __ASF__
	  delay_ms(50);
#endif //__ASF__
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


#ifdef __AVR__
/**
 * \brief Function for putting a char in the UART buffer
 *
 * \param data the data to add to the UART buffer and send
 *
 */
static inline void uart_putchar(uint8_t data)
{
	//Wait until data is written.
	while ((UCSR0B & (1 << UDRIE0)) != 0)
	{
		_delay_ms(10);		
	}
	UDR0 = data;
	// First data in buffer, enable data ready interrupt
	UCSR0B |= (1 << UDRIE0);
}
#endif //__AVR__

/**
 * \brief Function for putting a char in the UART buffer
 *
 * \param data the data to add to the UART buffer and send.
 *
 */
static inline int uart_write(gxByteBuffer* data)
{
#ifdef __AVR__
	while (data->position < data->size)
	{
		uart_putchar(data->data[data->position]);
		++data->position;
	}
#endif //__AVR__
#ifdef __ASF__
	int ret;
	while (bb_available(data) > 0)
	{
		ret = io_write(io, data->data + data->position, bb_available(data));
		if (ret > 0)
		{			
			data->position += ret;
		}
	}
#endif //__ASF__
	return 0;
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
  if ((ret = uart_write(data)) != 0)
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
	  if ((ret = uart_write(data)) != 0)
      {
        //If failed to write all bytes.
        GXTRACE(GET_STR_FROM_EEPROM("Failed to write all data to the serial port.\n"), NULL);
      }
    }
    ret = cl_getData(&settings, &frameData, reply);
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
      if ((ret = cl_receiverReady(&settings, reply->moreData, &rr)) != DLMS_ERROR_CODE_OK)
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
  if ((ret = cl_releaseRequest2(&settings, &msg, 1)) != 0 ||
      (ret = com_readDataBlock(&msg, &reply)) != 0)
  {
    //Show error but continue close.
  }
  reply_clear(&reply);
  mes_clear(&msg);

  if ((ret = cl_disconnectRequest(&settings, &msg)) != 0 ||
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
  if ((ret = cl_snrmRequest(&settings, &messages)) != 0 ||
      (ret = com_readDataBlock(&messages, &reply)) != 0 ||
      (ret = cl_parseUAResponse(&settings, &reply.data)) != 0)
  {
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
  }
  mes_clear(&messages);
  reply_clear(&reply);
  if ((ret = cl_aarqRequest(&settings, &messages)) != 0 ||
      (ret = com_readDataBlock(&messages, &reply)) != 0 ||
      (ret = cl_parseAAREResponse(&settings, &reply.data)) != 0)
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
  if (settings.authentication > DLMS_AUTHENTICATION_LOW)
  {
    if ((ret = cl_getApplicationAssociationRequest(&settings, &messages)) != 0 ||
        (ret = com_readDataBlock(&messages, &reply)) != 0 ||
        (ret = cl_parseApplicationAssociationResponse(&settings, &reply.data)) != 0)
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
  if ((ret = cl_getObjectsRequest(&settings, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = cl_parseObjects(&settings, &reply.data)) != 0)
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
  if ((ret = cl_read(&settings, object, attributeOrdinal, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = cl_updateValue(&settings, object, attributeOrdinal, &reply.dataValue)) != 0)
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
  if ((ret = cl_write(&settings, object, attributeOrdinal, &data)) != 0 ||
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
  if ((ret = cl_method(&settings, object, attributeOrdinal, params, &messages)) != 0 ||
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
    if ((ret = cl_readList(&settings, list, &messages)) != 0)
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
          if ((ret = cl_receiverReady(&settings, reply.moreData, &rr)) != DLMS_ERROR_CODE_OK ||
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
        ret = cl_updateValues(&settings, list, &bb);
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
  if ((ret = cl_readRowsByEntry(&settings, object, index, count, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = cl_updateValue(&settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
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
  if ((ret = cl_readRowsByRange2(&settings, object, start, end, &data)) != 0 ||
      (ret = com_readDataBlock(&data, &reply)) != 0 ||
      (ret = cl_updateValue(&settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
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
  ret = oa_getObjects2(&settings.objects, types, 3, &objects);
  if (ret != DLMS_ERROR_CODE_OK)
  {
    return ret;
  }
  if ((settings.negotiatedConformance & DLMS_CONFORMANCE_MULTIPLE_REFERENCES) != 0)
  {
    arr_init(&list);
    //Try to read with list first. All meters do not support it.
    for (pos = 0; pos != settings.objects.size; ++pos)
    {
      ret = oa_getByIndex(&settings.objects, pos, &obj);
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
  ret = oa_getObjects(&settings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
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
  gxByteBuffer ba;
  objectArray objects;
  gxProfileGeneric* pg;
  oa_init(&objects);
  ret = oa_getObjects(&settings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
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
    time_initUnix(&startTime, time(NULL));
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
  int pos;
  int ret = BYTE_BUFFER_INIT(&attributes);
  for (pos = 0; pos != settings.objects.size; ++pos)
  {
    ret = oa_getByIndex(&settings.objects, pos, &object);
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
    GXTRACE_INT(GET_STR_FROM_EEPROM("com_initializeConnection failed"), ret);
    return ret;
  }
  GXTRACE_INT(GET_STR_FROM_EEPROM("com_initializeConnection SUCCEEDED"), ret);

  char* data = NULL;
  //Read Logical Device Name
  gxData ldn;
  cosem_init(BASE(ldn), DLMS_OBJECT_TYPE_DATA, "0.0.42.0.0.255");
  com_read(BASE(ldn), 2);
  //obj_toString(BASE(ldn), &data);
  GXTRACE(GET_STR_FROM_EEPROM("Logical Device Name"), data);
  obj_clear(BASE(ldn));
  free(data);
#ifdef __AVR__
  if (time_getYears2(time(NULL)) == 1970)
  {
	  //Read clock and set micro controller to use meter time if it's not set.
	  gxClock clock1;
	  cosem_init(BASE(clock1), DLMS_OBJECT_TYPE_CLOCK, "0.0.1.0.0.255");
	  com_read(BASE(clock1), 3);
	  com_read(BASE(clock1), 2);
	  set_system_time(time_toUnixTime2(&clock1.time));
	  // obj_toString(BASE(clock1), &data);
	  GXTRACE(GET_STR_FROM_EEPROM("Clock"), data);
	  obj_clear(BASE(clock1));
	  free(data);
  }
#endif // __AVR__

/* TODO: This is an example how to read profile generic by range (start and end index).
  //Read Profile generic.
  gxProfileGeneric pg;
  cosem_init(BASE(pg), DLMS_OBJECT_TYPE_PROFILE_GENERIC, "1.0.99.1.0.255");
  com_read(BASE(pg), 3);
  com_readRowsByEntry(&pg, 1, 2);
 //  obj_toString(BASE(pg), &data);
  GXTRACE(GET_STR_FROM_EEPROM("Load profile"), data);
  obj_clear(BASE(pg));
  free(data);
  */
  //Release dynamically allocated objects.
  oa_clear(&settings.releasedObjects, 1);
  com_close();
  return ret;
}

#ifdef __AVR__
/**
 * \brief UART data register empty interrupt handler
 *
 * This handler is called each time the UART data register is available for
 * sending data.
 */
ISR(UART0_DATA_EMPTY_IRQ)
{
	/* Disable interrupts */
	cli();
	// no more data to send, turn off data ready interrupt
	UCSR0B &= ~(1 << UDRIE0);
	/* Enable interrupts */
	sei();
}

/**
 * \brief Data RX interrupt handler
 *
 * This is the handler for UART receive data
 */
ISR(UART0_RX_IRQ)
{
	/* Disable interrupts */
	cli();
	if (bb_setUInt8(&frameData, UDR0) != 0)
	{		
		//Clear buffer if it's full.
		frameData.position = frameData.size = 0;
	}
	available = 1;
	/* Enable interrupts */
	sei();
}

//If debug serial port is available.
#ifdef UBRR1H
/**
 * \brief UART data register empty interrupt handler
 *
 * This handler is called each time the UART data register is available for
 * sending data.
 */
ISR(UART1_DATA_EMPTY_IRQ)
{
	// no more data to send, turn off data ready interrupt
	UCSR1B &= ~(1 << UDRIE1);
}
#endif // UBRR1H

/**
 * \brief Initialize the UART with correct baud rate settings
 *
 * This function will initialize the UART baud rate registers with the correct
 * values using the AVR libc setbaud utility. In addition set the UART to
 * 8-bit, 1 stop and no parity.
 */
static void uart_init(void)
{
#if defined UBRR0H
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#else
#error "Device is not supported by the driver"
#endif

#if USE_2X
	UCSR0A |= (1 << U2X0);
#endif

	// enable RX and TX and set interrupts on rx complete
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8-bit, 1 stop bit, no parity, asynchronous UART
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0) |
			(0 << UPM01) | (0 << UPM00) | (0 << UMSEL01) |
			(0 << UMSEL00);	
}

//If debug serial port is available.
#ifdef UBRR1H
/**
 * \brief Initialize the UART for debugging with settings: 9600 8None1
 */
static void uart_debugInit(void)
{
	if (trace == GX_TRACE_LEVEL_VERBOSE)
	{
		UBRR1H = UBRRH_VALUE;
		UBRR1L = UBRRL_VALUE;
	#if USE_2X
		UCSR1A |= (1 << U2X1);
	#endif
		// enable TX only.
		UCSR1B = (1 << TXEN1);
		// 8-bit, 1 stop bit, no parity
		UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
	}
}
#endif //UBRR1H

// Calculate the value needed for the CTC match value in OCR1A.
#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

//Init timer.
static void timer_init(void)
{
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = CTC_MATCH_OVERFLOW;

	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);
}

#endif //__AVR__

int main(void)
{
#ifdef __AVR__
	uart_init();
#ifdef UBRR1H
	uart_debugInit();
#endif //UBRR1H
	timer_init();
	/* Enable interrupts */
	sei();
#endif // __AVR__

#ifdef __ASF__
	atmel_start_init();	
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_set_mode(&USART_0, USART_MODE_SYNCHRONOUS);
	usart_sync_set_baud_rate(&USART_0, 9600);
	usart_sync_set_character_size(&USART_0, USART_CHARACTER_SIZE_8BITS);
	usart_sync_set_parity(&USART_0, USART_PARITY_NONE);
	usart_sync_set_stopbits(&USART_0, USART_STOP_BITS_ONE );
	usart_sync_enable(&USART_0);
#endif //__ASF__

	cl_init(&settings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
	GXTRACE(GET_STR_FROM_EEPROM("Start application"), NULL);
	BYTE_BUFFER_INIT(&frameData);
	//Set frame capacity.
	bb_capacity(&frameData, 128);
	while(1)
	{		
		com_readAllObjects();
#ifdef __AVR__
		_delay_ms(1000);
#endif // __AVR__
#ifdef __ASF__
		delay_ms(1000);
#endif //__ASF__
	}
}