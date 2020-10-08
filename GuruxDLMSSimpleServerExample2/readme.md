See An [Gurux](http://www.gurux.org/ "Gurux") for an overview.
Join the Gurux Community or follow [@Gurux](https://twitter.com/guruxorg "@Gurux") for project updates.
Gurux.DLMS library is a high-performance .NET component that helps you to read you DLMS/COSEM compatible electricity, gas or water meters. We have try to make component so easy to use that you do not need understand protocol at all.
For more info check out [Gurux.DLMS](http://www.gurux.fi/index.php?q=Gurux.DLMS "Gurux.DLMS").
We are updating documentation on Gurux web page. 

Read should read [DLMS/COSEM FAQ](http://www.gurux.org/index.php?q=DLMSCOSEMFAQ) first to get started. Read Instructions for making your own [meter reading application](http://www.gurux.org/index.php?q=DLMSIntro) or build own 
DLMS/COSEM [meter/simulator/proxy](http://www.gurux.org/index.php?q=OwnDLMSMeter).

If you have problems you can ask your questions in Gurux [Forum](http://www.gurux.org/forum).


Before start
=========================== 
Before start you need to copy files from development src and include folder to dlms folder.

If you need to save memory you can skip un-needed functionality. Ignored functinality is listed in gxignore.h file. 

This example server is implemented without malloc. You can skip malloc usage by define DLMS_IGNORE_MALLOC.
You can do this in compiler settings or uncomment #define DLMS_IGNORE_MALLOC from gxignore.h

```c
#define DLMS_IGNORE_MALLOC
```

This is already added to Windows and gcc compiler settings.

Because malloc is not used, you must allocate memory for all arrays and octect strings.
You can see how this is implemented in addAssociation-method.

Other new feature is that you can attach values to cosem objects. In this way you can use ANSI C stadard value types and you don't need to care from COSEM objects after you have initialize them.

```c
//Active power value. We are updating this.
unsigned long activePowerL1Value = 0;
//Attach value to COSEM attribute.
GX_UINT32_BYREF(activePowerL1.value) = &activePowerL1Value;
```

Build
=========================== 

In this example we are using Visual Studio. Steps are same for other compilers. Only thing what is different is how bytes are send and received from the serial port.
This example is using TCP/IP connection. If you want to use serial port connection idea is the same. Just handle data sending and receiving from the serial port.
We will add examples for other compilers if needed. 

Server example
=========================== 
In this example we'll create simple DLMS server that have register, data and clock object.
First you need to create new project. After that you need to copy server functions.
Framework will call those functions. Purpose of functions is described here:
[Gurux DLMS server](http://gurux.fi/Gurux.DLMS.Server "Gurux DLMS server").

Make all the changes to this one file (main.c). In that way you can get framework changes automatically. You will make business logic.
Framework will handle everything else.


First add includes:

```c
#include "dlms/include/dlmssettings.h"
#include "dlms/include/variant.h"
#include "dlms/include/cosem.h"
#include "dlms/include/server.h"
```

Define server settings.

//Active power value. We are updating this.
unsigned long activePowerL1Value = 0;


```c
dlmsServerSettings settings;
/HDLC frame size.
#define HDLC_BUFFER_SIZE 128
//PDU size.
#define PDU_BUFFER_SIZE 1024
//HDLC frame buffer.
unsigned char frame[11 + HDLC_BUFFER_SIZE];
//PDU buffer.
unsigned char pdu[PDU_BUFFER_SIZE];
//Reply data that is sent back to the client.
gxByteBuffer reply;
```

Then define used COSEM objects in top of the file.

```c
gxData ldn;
//Don't use clock as a name. Some compilers are using clock as reserved word.
gxClock clock1;
gxAssociationLogicalName association;
gxRegister activePowerL1;
```

You need to add all COSEM objects to the list so framework will know what objects it have. 

```c
gxObject* ALL_OBJECTS[] = { BASE(association), BASE(ldn), BASE(clock1), BASE(activePowerL1) };
```

Next you need to initialize COSEM objects. In this example we have four methods. Each method handles object initialization.

```c
///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociation()
{
  int ret;
    //User list.
    static gxUser USER_LIST[10] = { 0 };
    //Dedicated key.
    static char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
    if ((ret = INIT_OBJECT(associationNone, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        //Only Logical Device Name is add to this Association View.
        //Use this if you  need to save heap.
        OA_ATTACH(associationNone.objectList, ALL_OBJECTS);
        associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        ARR_ATTACH(associationNone.userList, USER_LIST, 0);
        BB_ATTACH(associationNone.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        associationNone.clientSAP = 0x10;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Logical Device Name. 123456 is meter serial number.
///////////////////////////////////////////////////////////////////////
// COSEM Logical Device Name is defined as an octet-string of 16 octets.
// The first three octets uniquely identify the manufacturer of the device and it corresponds
// to the manufacturer's identification in IEC 62056-21.
// The following 13 octets are assigned by the manufacturer.
//The manufacturer is responsible for guaranteeing the uniqueness of these octets.
void addLogicalDeviceName()
{
  sprintf(LDN, "GRX%.13lu", SERIAL_NUMBER);
  const unsigned char ln[6] = { 0, 0, 42, 0, 0, 255 };
  INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln);
  GX_OCTECT_STRING(ldn.value, LDN, sizeof(LDN));
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
        GX_UINT32_BYREF(activePowerL1.value) = &activePowerL1Value;
        //10 ^ 3 =  1000
        activePowerL1.scaler = -2;
        activePowerL1.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Initialize COSEM objects.
///////////////////////////////////////////////////////////////////////
int svr_InitObjects(
  dlmsServerSettings *settings)
{
  addLogicalDeviceName();
  addClockObject();
  addRegisterObject();
  addAssociation();
  oa_attach(&settings->base.objects, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
  return oa_verify(&settings->base.objects);
}
```

Next implement socket data handling.

```c
int main()
  unsigned char data;
  struct sockaddr_in client;
  int ret, ls, s;
  struct sockaddr_in add = { 0 };
#if defined(_WIN32) || defined(_WIN64)//If Windows
  int len;
  int AddrLen = sizeof(add);
#else //If Linux
  socklen_t len;
  socklen_t AddrLen = sizeof(add);
#endif
#if defined(_WIN32) || defined(_WIN64)//If Windows
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
      // Tell the user that we could not find a usable WinSock DLL.
      return 1;
  }
#endif

  //Initialize reply data.
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

  ls = socket(AF_INET, SOCK_STREAM, 0);
  add.sin_port = htons(4061);
  add.sin_addr.s_addr = htonl(INADDR_ANY);
  add.sin_family = AF_INET;
  if ((ret = bind(ls, (struct sockaddr*) &add, sizeof(add))) == -1)
  {
      return -1;
  }

  while (1)
  {
    if ((ret = listen(ls, 1)) == -1)
    {
      //socket listen failed.
      return -1;
    }
    len = sizeof(client);
    s = accept(ls, (struct sockaddr*)&client, &len);
    while (1)
    {
      //Read one char at the time.
      if ((ret = recv(s, (char*)&data, 1, 0)) == -1)
      {
#if defined(_WIN32) || defined(_WIN64)//If Windows
        closesocket(s);
        s = INVALID_SOCKET;
#else //If Linux
        close(s);
        s = -1;
#endif
        break;
      }      
      //If client closes the connection.
      if (ret == 0)
      {
#if defined(_WIN32) || defined(_WIN64)//If Windows
        closesocket(s);
        s = INVALID_SOCKET;
#else //If Linux
        close(s);
        s = -1;
#endif
        break;
      }
      if (svr_handleRequest3(&settings, data, &reply) != 0)
      {
#if defined(_WIN32) || defined(_WIN64)//If Windows
        closesocket(s);
        s = INVALID_SOCKET;
#else //If Linux
        close(s);
        s = -1;
#endif
        break;
      }
      if (reply.size != 0)
      {
        if (send(s, (const char*)reply.data, reply.size - reply.position, 0) == -1)
        {
#if defined(_WIN32) || defined(_WIN64)//If Windows
          closesocket(s);
          s = INVALID_SOCKET;
#else //If Linux
          close(s);
          s = -1;
#endif
          break;
        }
        bb_clear(&reply);
      }
    }
  }

#if defined(_WIN32) || defined(_WIN64)//Windows
    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif
  return 0;
}
```