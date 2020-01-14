See An [Gurux](http://www.gurux.org/ "Gurux") for an overview.
Join the Gurux Community or follow [@Gurux](https://twitter.com/guruxorg "@Gurux") for project updates.
Gurux.DLMS library is a high-performance .NET component that helps you to read you DLMS/COSEM compatible electricity, gas or water meters. We have try to make component so easy to use that you do not need understand protocol at all.
For more info check out [Gurux.DLMS](http://www.gurux.fi/index.php?q=Gurux.DLMS "Gurux.DLMS").
We are updating documentation on Gurux web page. 

Read should read [DLMS/COSEM FAQ](http://www.gurux.org/index.php?q=DLMSCOSEMFAQ) first to get started. Read Instructions for making your own [meter reading application](http://www.gurux.org/index.php?q=DLMSIntro) or build own 
DLMS/COSEM [meter/simulator/proxy](http://www.gurux.org/index.php?q=OwnDLMSMeter).

If you have problems you can ask your questions in Gurux [Forum](http://www.gurux.org/forum).

Introduction
=========================== 
Reading DLMS meters Over The Air can be a real pain. DLMS data frames are not the smallest ones and communication channel might lose messages all the time. For this reason we have made Gurux Data Colletor. Idea is that Gurux Data Collector is inside of the NIC card and it collects data from the meter when needed. Collected data is then send as a Push messages to the server. Server makes a connection to the Gurux Data Colletor only when needed. Example when clock is updated.

In this project we have used [Wirepas](https://wirepas.com/ "Wirepas") Mesh device to read data from the meter. Example reads instantaneous profile using Indian Standard 15959, but it's easy to modify for your needs.

Before Start
=========================== 
You must copy include and src folders from the development folder under dlms folder. You get a lot of compiler errors if this is not done.

You need to change default_network_address and app_specific_area_id fro config.mk file.
You can use example this random generator. https://www.browserling.com/tools/random-hex
```C
# TODO: Change this. This must be unique for each NIC.
default_network_address ?= 0x123456

# Define a specific application area_id
# TODO: Change this. This SINK ID.
app_specific_area_id=0x83744C
```
Programming
=========================== 

First, you need to define what you want to read from the meter and add them to the push object. In this example, we are reading meter time and active power from L1. This information is then sent in push message to the HES.

```C
static gxClock clock1;
static gxRegister activePowerL1;
///////////////////////////////////////////////////////////////////////
//This method adds clock object.
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
//This method adds register object.
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
    //Read time.
    PUSH_OBJECTS[1].target = &clock1.base;
    PUSH_OBJECTS[1].attributeIndex = 2;
    PUSH_OBJECTS[1].dataIndex = 0;
    //Register value.
    PUSH_OBJECTS[2].target = &activePowerL1.base;
    PUSH_OBJECTS[2].attributeIndex = 2;
    PUSH_OBJECTS[2].dataIndex = 0;
    //Register scaler and unit.
    PUSH_OBJECTS[3].target = &activePowerL1.base;
    PUSH_OBJECTS[3].attributeIndex = 3;
    PUSH_OBJECTS[3].dataIndex = 0;
    ARR_ATTACH(push.pushObjectList, PUSH_OBJECTS, 4);
    return ret;
}
```

Indian Standard 15959 objects are defined on india.h file. You can remove them or add other objects for your needs.
This example doesn't use malloc, so data buffers must be defined before use. The size of the data buffers might depend on the read objects.

All business logic (functionality) is in app.c file. Don't modify other files than app.c, objects.c and and objects.h.
Read [DLMS Server](https://www.gurux.fi/Gurux.DLMS.Server "DLMS Server") to understand purpose of the methods.

When profile generic data is read from the meter it's send as read. No modifications are made. You can modify this for your needs as everything else. 

You must define meter settings before you can read data from the meter. Settings depends from the meter and model.
Initial settings are:

```C
unsigned char CLIENT_ADDRESS = 0x10;
unsigned short SERVER_ADDRESS = 0x1;
DLMS_AUTHENTICATION AUTHENTICATION_LEVEL = DLMS_AUTHENTICATION_NONE;
static char* PASSWORD = "Gurux";
```

In default there is a one push object, but you can add others for your need. Push object is added with addPushSetup-method. There you define what you want to read from the meter.

DEFAULT_PERIOD_S tells how ofter connection is made for the meter and data is read.

Debugging
=========================== 
In debugging you can use GuruxDLMSServerExample. It's easier to play agains Windows app where you can see incoming messages than real meter where you don't see what is happening between the meter and Data collector. You can run GuruxDLMSServerExample with parameters GuruxDLMSServerExample -S COM_X -t Verbose
COM_X is serial port that GuruxDLMSServerExample uses.

Memory optimization
=========================== 
If memory is causing problems, you can remove non-needed functionality adding compiler flags to the make file.
Adding microcontroller flag is always recommendable.

```C
CFLAGS += -DGX_DLMS_MICROCONTROLLER
```




