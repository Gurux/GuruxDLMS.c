See An [Gurux](http://www.gurux.org/ "Gurux") for an overview.
Join the Gurux Community or follow [@Gurux](https://twitter.com/guruxorg "@Gurux") for project updates.
Gurux.DLMS library is a high-performance ANSI C library that helps you to read you DLMS/COSEM compatible electricity, gas or water meters. We have try to make component so easy to use that you do not need understand protocol at all.
For more info check out [Gurux.DLMS](http://www.gurux.fi/index.php?q=Gurux.DLMS "Gurux.DLMS").
We are updating documentation on Gurux web page. 

Read should read [DLMS/COSEM FAQ](http://www.gurux.org/index.php?q=DLMSCOSEMFAQ) first to get started. Read Instructions for making your own [meter reading application](http://www.gurux.org/index.php?q=DLMSIntro) or build own 
DLMS/COSEM [meter/simulator/proxy](http://www.gurux.org/index.php?q=OwnDLMSMeter).

If you have problems you can ask your questions in Gurux [Forum](http://www.gurux.org/forum).


Build
=========================== 

In this example we are using SEGGER Embedded Studio v4.14.

TCP/IP support.
=========================== 

TCP/IP is not used as a default because retreaving TCP/IP address might take a long time.

If TCP/IP supported is needed open main.c and set server port for BIND_PORT.
Then open prj.conf and change config networking to y 
CONFIG_NETWORKING=y

Server example
=========================== 
In this example we'll create simple DLMS server that have register, data and clock object.
First you need to create new project. After that you need to copy server functions.
Framework will call those functions. Purpose of functions is described here:
[Gurux DLMS server](http://gurux.fi/Gurux.DLMS.Server "Gurux DLMS server").

Make all the changes to this one file (main.c). In that way you can get framework changes automatically. You will make business logic.
Framework will handle everything else.

If you need to save memory you can skip un-needed functionality. Ignored functinality is listed in gxignore.h file. 

First add includes:

```c
#include "../../development/include/dlmssettings.h"
#include "../../development/include/variant.h"
#include "../../development/include/cosem.h"
#include "../../development/include/server.h"
```

Define server settings.
```c
dlmsServerSettings settings;
/HDLC frame size.
#define HDLC_BUFFER_SIZE 128
//PDU size.
#define PDU_BUFFER_SIZE 256
//HDLC frame buffer.
unsigned char frame[HDLC_BUFFER_SIZE];
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
const gxObject* ALL_OBJECTS[] = { &association.base, &ldn.base, &clock1.base, &activePowerL1.base };
```

Next you need to initialize COSEM objects. In this example we have four methods. Each method handles object initialization.

```c
///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociation()
{
  const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
  cosem_init2((gxObject*)&association, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
  //Only Logical Device Name is add to this Association View.
  //Use this if you  need to save heap.
  //  oa_attach(&associationNone.objectList, NONE_OBJECTS, sizeof(NONE_OBJECTS) / sizeof(NONE_OBJECTS[0]));
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
void addLogicalDeviceName(){
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
  return 0;
}
```

Next implement serial port data handling.

```c
static void isr(struct device *x)
{
  uart_irq_update(x);
  if (uart_irq_rx_ready(x)) {
    char ch;
    int ret = uart_fifo_read(x, &ch, 1);
    if (ret != 0)
    {
      if (svr_handleRequest3(&settings, ch, &reply) != 0)
      {
       bb_clear(&reply);
      }
      if (reply.size != 0)
      {
        //Send reply.
        for (size_t i = 0; i < reply.size; i++) {
          uart_poll_out(uart_dev, reply.data[i]);
        }
        bb_clear(&reply);
      }
    }
  }
}

static int gx_uart_init(char *uart_dev_name)
{
  int err;
  uart_dev = device_get_binding(uart_dev_name);
  if (uart_dev == NULL) {
    printf("Cannot bind %s\n", uart_dev_name);
    return EINVAL;
  }
  err = uart_err_check(uart_dev);
  if (err) {
    printf("UART check failed\n");
    return EINVAL;
  }
  uart_irq_rx_enable(uart_dev);
  uart_irq_callback_set(uart_dev, isr);
  return err;
}

void main(void)
{
  int ret;
  started = _impl_k_uptime_get();
  characters = 0;
  bb_init(&reply);
  //Start server using logical name referencing and HDLC framing.
  svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frame, HDLC_BUFFER_SIZE, pdu, PDU_BUFFER_SIZE);
  //Add COSEM objects.
  svr_InitObjects(&settings);
  //Start server
  if ((ret = svr_initialize(&settings)) != 0)
  {
    printf("svr_initialize failed.\r\n");
    return;
  }
  printf("Gurux DLMS sample started.\n");    
  ret = gx_uart_init("UART_0");
  if (ret != 0) {
    printf("Gurux DLMS sample failed.\n");    
    return;
  }
  while (true) {
  }
}

```