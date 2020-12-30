See An [Gurux](http://www.gurux.org/ "Gurux") for an overview.

Join the Gurux Community or follow [@Gurux](https://twitter.com/guruxorg "@Gurux") for project updates.

GuruxDLMS.c library is a high-performance ANSI C component that helps you to read you DLMS/COSEM compatible electricity, gas or water meters. 
We have try to make component so easy to use that you do not need understand protocol at all.

For more info check out [Gurux.DLMS](http://www.gurux.fi/index.php?q=Gurux.DLMS "Gurux.DLMS").

We are updating documentation on Gurux web page. 

Read should read [DLMS/COSEM FAQ](https://www.gurux.org/index.php?q=DLMSCOSEMFAQ) first to get started. Read Instructions for making your own [meter reading application](https://www.gurux.org/index.php?q=DLMSIntro) or build own 
DLMS/COSEM [meter/simulator/proxy](https://www.gurux.fi/Gurux.DLMS.Server).

If you have problems you can ask your questions in Gurux [Forum](https://www.gurux.org/forum).

You can use any connection (TCP, serial, PLC) library you want to.
Gurux.DLMS classes only parse the data.

Before start
=========================== 

If you find an issue, please report it here:
http://www.gurux.fi/fi/project/issues/gurux.dlms.c

We have made makefile for linux environment. You should go development folder and create lib and obj directories.
Then run make. gurux_dlms_c static library is made.

For Client example you should also create bin and obj -directories.
Change Host name, port and DLMS settings for example. Run make and you are ready to test.

Server side functionality is added later in this spring.

Simple example
=========================== 
Before use you must set following device parameters. 
Parameters are manufacturer spesific.


```C

All default parameters are given in constructor.
// Is used Logican Name or Short Name referencing.
dlmsSettings settings;
//Initialize settings using Logican Name referencing and HDLC.
ln_init(&settings);
//Initialize settings using Short Name referencing and HDLC.

```

After you have set parameters you can try to connect to the meter.
First you should send SNRM request and handle UA response.
After that you will send AARQ request and handle AARE response.


```C

message messages;
gxReplyData reply;
mes_init(&messages);
reply_init(&reply);
//Get meter's send and receive buffers size.
if ((ret = cl_snrmRequest(settings, &messages)) != 0 ||
        (ret = com_readDataBlock(settings, &messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(settings, &reply.data)) != 0)
{
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}
mes_clear(&messages);
reply_clear(&reply);
if (settings->interfaceType == GXDLMS_INTERFACETYPE_NET)
{
    con_initializeBuffers(&settings->con, 0xFFFF);
}
else
{
    //Initialize send and receive buffers to same as meter's buffers.
    con_initializeBuffers(&settings->con, settings->maxInfoTX);
}
if ((ret = cl_aarqRequest(settings, &messages)) != 0 ||
        (ret = com_readDataBlock(settings, &messages, &reply)) != 0 ||
        (ret = cl_parseAAREResponse(settings, &reply.data)) != 0)
{
    mes_clear(&messages);
    reply_clear(&reply);
    if (ret == ERROR_CODES_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
    {
        printf("Use Logical Name referencing is wrong. Change it!\r\n");
        return ret;
    }
    return ret;
}
mes_clear(&messages);
reply_clear(&reply);
return ERROR_CODES_OK;

```

If parameters are right connection is made.
Next you can read Association view and show all objects that meter can offer.

```C
/// Read Association View from the meter.
int ret;
message data;
gxReplyData reply;
mes_init(&data);
reply_init(&reply);
if ((ret = cl_getObjectsRequest(settings, &data)) != 0 ||
        (ret = com_readDataBlock(settings, &data, &reply)) != 0 ||
        (ret = cl_parseObjects(settings, &reply.data)) != 0)
{
    printf("GetObjects failed %s\r\n", getErrorMessage(ret));
}
mes_clear(&data);
reply_clear(&reply);
return ret;

```
Now you can read wanted objects. After read you must close the connection by sending
disconnecting request.

```C

reply_init(&reply);
mes_init(&msg);
if ((ret = cl_disconnectRequest(settings, &msg)) != 0 ||
        (ret = com_readDataBlock(settings, &msg, &reply)) != 0)
{
    //Show error but continue close.
}
reply_clear(&reply);
mes_clear(&msg);

```

```C

// Read DLMS Data frame from the device.
int readDLMSPacket(dlmsSettings* settings, byteArray* data, gxReplyData* reply)
{
    char *hex;
#if defined(_WIN32) || defined(_WIN64)//Windows
    unsigned long sendSize = 0;
#endif
    int ret = ERROR_CODES_OK, cnt;
    if (data->size == 0)
    {
        return ERROR_CODES_OK;
    }
    reply->complete = 0;
    settings->con.data.size = 0;
    settings->con.data.position = 0;
    if (settings->con.comPort != INVALID_HANDLE_VALUE)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows
        ret = WriteFile(settings->con.comPort, data->data, data->size, &sendSize, &settings->con.osWrite);
        if (ret == 0)
        {
            DWORD err = GetLastError();
            //If error occurs...
            if (err != ERROR_IO_PENDING)
            {
                return ERROR_CODES_SEND_FAILED;
            }
            //Wait until data is actually sent
            ret = WaitForSingleObject(settings->con.osWrite.hEvent, settings->con.waitTime);
            if (ret != 0)
            {
                DWORD err = GetLastError();
                return ERROR_CODES_SEND_FAILED;
            }
        }
#else
        ret = write(settings->con.comPort, data->data, data->size);
        if (ret != data->size)
        {
            return ERROR_CODES_SEND_FAILED;
        }
#endif
    }
    else
    {
        if (send(settings->con.socket, (const char*) data->data, data->size, 0) == -1)
        {
            //If error has occurred
            return ERROR_CODES_SEND_FAILED;
        }
    }
    //Loop until packet is complete.
    do
    {
        if (settings->con.comPort != INVALID_HANDLE_VALUE)
        {
            if(com_read(settings, 0x7E) != 0)
            {
                return ERROR_CODES_SEND_FAILED;
            }
        }
        else
        {
            cnt = settings->con.data.capacity - settings->con.data.size;
            if (cnt < 1)
            {
                return ERROR_CODES_OUTOFMEMORY;
            }
            if ((ret = recv(settings->con.socket, (char*) settings->con.data.data + settings->con.data.size, cnt, 0)) == -1)
            {
                return ERROR_CODES_RECEIVE_FAILED;
            }
            settings->con.data.size += ret;
        }
        if ((ret = cl_getData(&settings->con, settings, reply)) != 0)
        {
            break;
        }
    }
    while (reply->complete == 0);
    return ret;
}
```