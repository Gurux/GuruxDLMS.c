/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */


#include "dlms/include/dlmssettings.h"
#include "dlms/include/variant.h"
#include "dlms/include/cosem.h"
#include "dlms/include/server.h"

long time_current(void)
{
	//Get current time somewhere.
	return (long)time(NULL);
}

long time_elapsed(void)
{
	return (long)0;//millis();
}

void time_now(
gxtime* value)
{
	//Get local time somewhere.
	time_t tm1 = time(NULL);
	struct tm dt = *localtime(&tm1);
	time_init2(value, &dt);
}

//DLMS settings.
dlmsServerSettings settings;
//Meter serial number.
unsigned long SERIAL_NUMBER = 123456;

#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 256
unsigned char pdu[PDU_BUFFER_SIZE];
unsigned char frame[HDLC_BUFFER_SIZE];
gxByteBuffer reply;
gxData ldn;
//Don't use clock as a name. Some compilers are using clock as reserved word.
gxClock clock1;
gxAssociationLogicalName association;
gxRegister activePowerL1;

const gxObject* ALL_OBJECTS[] = { &association.base, &ldn.base, &clock1.base, &activePowerL1.base };

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociation()
{
	const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
	cosem_init2((gxObject*)&association, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
	//Only Logical Device Name is add to this Association View.
	//Use this if you  need to save heap.
	oa_attach(&association.objectList, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
	bb_addString(&association.secret, "Gurux");
	association.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
	return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Logical Device Name. 123456 is meter serial number.
///////////////////////////////////////////////////////////////////////
// COSEM Logical Device Name is defined as an octet-string of 16 octets.
// The first three octets uniquely identify the manufacturer of the device and it corresponds
// to the manufacturer's identification in IEC 62056-21.
// The following 13 octets are assigned by the manufacturer.
//The manufacturer is responsible for guaranteeing the uniqueness of these octets.
void addLogicalDeviceName() {
	char buff[17];
	sprintf(buff, "GRX%.13lu", SERIAL_NUMBER);
	const unsigned char ln[6] = { 0,0,42,0,0,255 };
	cosem_init2((gxObject*)&ldn.base, DLMS_OBJECT_TYPE_DATA, ln);
	var_addBytes(&ldn.value, (unsigned char*)buff, 16);
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
	int ret;
	//Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
	const unsigned char ln[6] = { 0,0,1,0,0,255 };
	if ((ret = cosem_init2(&clock1.base, DLMS_OBJECT_TYPE_CLOCK, ln)) != 0)
	{
		return ret;
	}
	time_init3(&clock1.begin, -1, 9, 1, -1, -1, -1, -1);
	time_init3(&clock1.end, -1, 3, 1, -1, -1, -1, -1);
	return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
	const unsigned char ln[6] = { 1,1,21,25,0,255 };
	cosem_init2((gxObject*)&activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln);
	//10 ^ 3 =  1000
	activePowerL1.scaler = 3;
	activePowerL1.unit = 30;
	return 0;
}

int svr_InitObjects(
dlmsServerSettings *settings)
{
	addLogicalDeviceName();
	addClockObject();
	addRegisterObject();
	addAssociation();
	oa_attach(&settings->base.objects, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
	return 0;
}

int svr_findObject(
dlmsSettings* settings,
DLMS_OBJECT_TYPE objectType,
int sn,
unsigned char* ln,
gxValueEventArg *e)
{
	if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
	{
		e->target = &association.base;
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
	gxValueEventArg *e;
	int ret, pos;
	DLMS_OBJECT_TYPE type;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
		//Let framework handle Logical Name read.
		if (e->index == 1)
		{
			continue;
		}

		//Get target type.
		type = (DLMS_OBJECT_TYPE)e->target->objectType;
		//Let Framework will handle Association objects and profile generic automatically.
		if (type == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
		type == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
		{
			continue;
		}
		//Update value by one every time when user reads register.
		if (e->target == &activePowerL1.base && e->index == 2)
		{
			var_setUInt16(&activePowerL1.value, activePowerL1.value.uiVal + 1);
		}
		//Get time if user want to read date and time.
		if (e->target == &clock1.base && e->index == 2)
		{
			time_now(&((gxClock*)e->target)->time);
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
	#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
	char str[25];
	gxValueEventArg *e;
	int ret, pos;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
		hlp_getLogicalNameToString(e->target->logicalName, str);
		printf("Writing %s\r\n", str);

	}
	#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
dlmsSettings* settings,
gxValueEventCollection* args)
{
	#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
	char str[25];
	#endif
	gxValueEventArg *e;
	int ret, pos;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
		#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
		hlp_getLogicalNameToString(e->target->logicalName, str);
		printf("Action %s:%d\n", str, e->index);
		#endif
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
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postAction(
dlmsSettings* settings,
gxValueEventCollection* args)
{
	gxValueEventArg *e;
	int ret, pos;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
	}
}

unsigned char svr_isTarget(
	dlmsSettings *settings,
	unsigned long serverAddress,
	unsigned long clientAddress)
{
	//Check server address using serial number.
	if ((serverAddress & 0x3FFF) == SERIAL_NUMBER % 10000 + 1000)
	{
		return 1;
	}
	//Check server address with two bytes.
	if ((serverAddress & 0xFFFF0000) == 0 && (serverAddress & 0x7FFF) == 1)
	{
		return 1;
	}
	//Check server address with one byte.
	if ((serverAddress & 0xFFFFFF00) == 0 && (serverAddress & 0x7F) == 1)
	{
		return 1;
	}
	return 0;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
dlmsServerSettings* settings,
DLMS_AUTHENTICATION authentication,
gxByteBuffer* password)
{
	if (authentication == DLMS_AUTHENTICATION_NONE)
	{
		//Uncomment this if authentication is always required.
		//return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
		return DLMS_SOURCE_DIAGNOSTIC_NONE;
	}
	//Uncomment this if only authentication None is supported.
	//return DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
	return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
dlmsSettings *settings,
gxObject *obj,
unsigned char index)
{
	if (index == 1)
	{
		return DLMS_ACCESS_MODE_READ;
	}
	// Only read is allowed
	if (settings->authentication == DLMS_AUTHENTICATION_NONE)
	{
		return DLMS_ACCESS_MODE_READ;
	}
	// Only clock write is allowed.
	if (settings->authentication == DLMS_AUTHENTICATION_LOW)
	{
		if (obj->objectType == DLMS_OBJECT_TYPE_CLOCK)
		{
			return DLMS_ACCESS_MODE_READ_WRITE;
		}
		return DLMS_ACCESS_MODE_READ;
	}
	// All writes are allowed.
	return DLMS_ACCESS_MODE_READ_WRITE;
}

/**
* Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
dlmsSettings *settings,
gxObject *obj,
unsigned char index)
{
	// Methods are not allowed.
	if (settings->authentication == DLMS_AUTHENTICATION_NONE)
	{
		return DLMS_METHOD_ACCESS_MODE_NONE;
	}
	// Only clock methods are allowed.
	if (settings->authentication == DLMS_AUTHENTICATION_LOW)
	{
		if (obj->objectType == DLMS_OBJECT_TYPE_CLOCK)
		{
			return DLMS_METHOD_ACCESS_MODE_ACCESS;
		}
		return DLMS_METHOD_ACCESS_MODE_NONE;
	}
	return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

/////////////////////////////////////////////////////////////////////////////
//Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
dlmsServerSettings *settings)
{
	return 0;
}

/**
* Client has try to made invalid connection. Password is incorrect.
*
* @param connectionInfo
*            Connection information.
*/
int svr_invalidConnection(dlmsServerSettings *settings)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Client has close the connection.
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
dlmsServerSettings *settings)
{
	return 0;
}

void svr_preGet(
dlmsSettings* settings,
gxValueEventCollection* args)
{
	gxValueEventArg *e;
	int ret, pos;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
	}
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

void setup() {
	int ret;
	bb_init(&reply);
	//Start server using logical name referencing and HDLC framing.
	svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frame, HDLC_BUFFER_SIZE, pdu, PDU_BUFFER_SIZE);
	//Add COSEM objects.
	svr_InitObjects(&settings);
	//Start server
	if ((ret = svr_initialize(&settings)) != 0)
	{
		//TODO: Show error.
		return;
	}
	// start serial port at 9600 bps:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
}

void loop() {
	if (Serial.available() > 0) {
		unsigned short pos;
		unsigned char value = Serial.read();
		if (svr_handleRequest3(&settings, value, &reply) != 0)
		{
			bb_clear(&reply);
		}
		if (reply.size != 0)
		{
			//Send reply.
			for(pos = 0; pos != reply.size; ++pos)
			{
				Serial.write(reply.data[pos]);
			}
			bb_clear(&reply);
		}
	}
}
