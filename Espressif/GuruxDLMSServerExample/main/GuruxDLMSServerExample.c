#include <stdio.h>

#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
// Add support to push messages.
#include <sys/socket.h>

#include "gxmem.h"
#include "dlmssettings.h"
#include "variant.h"
#include "cosem.h"
#include "server.h"
// Add this if you want to send notify messages.
#include "notify.h"
// Add support for serialization.
#include "gxserializer.h"

#define PORT 4059
#define TAG "DLMS_SERVER"
#define WIFI_SSID CONFIG_EXAMPLE_WIFI_SSID
#define WIFI_PASS CONFIG_EXAMPLE_WIFI_PASSWORD

// DLMS settings.
dlmsServerSettings settings;
// DLMS push settings.
dlmsServerSettings pushSettings;

GX_TRACE_LEVEL trace = GX_TRACE_LEVEL_OFF;

const static char *FLAG_ID = "GRX";

// Space for client challenge.
static unsigned char C2S_CHALLENGE[64];
// Space for server challenge.
static unsigned char S2C_CHALLENGE[64];

unsigned char testMode = 1;
uint32_t SERIAL_NUMBER = 123456;

// TODO: Allocate space where profile generic row values are serialized.
#define PDU_MAX_PROFILE_GENERIC_COLUMN_SIZE 100

#define PDU_BUFFER_SIZE 512
#define WRAPPER_BUFFER_SIZE 8 + PDU_BUFFER_SIZE
// Buffer where frames are saved.
static unsigned char frameBuff[WRAPPER_BUFFER_SIZE];
// Buffer where PDUs are saved.
static unsigned char pduBuff[PDU_BUFFER_SIZE];
static unsigned char replyFrame[WRAPPER_BUFFER_SIZE];
// Define server system title.
static unsigned char SERVER_SYSTEM_TITLE[8] = {0};
static gxByteBuffer reply;

uint32_t time_current(void)
{
    // Get current time somewhere.
    return (uint32_t)time(NULL);
}

uint32_t time_elapsed(void)
{
    return (uint32_t)clock() / (CLOCKS_PER_SEC / 1000);
}

// Don't use clock as a name. Some compilers are using clock as reserved word.
static gxClock clock1;
static gxIecHdlcSetup hdlc;
static gxData ldn;
static gxData eventCode;
static gxData unixTime;
static gxData invocationCounter;
static gxAssociationLogicalName associationNone;
static gxAssociationLogicalName associationLow;
static gxAssociationLogicalName associationHigh;
static gxAssociationLogicalName associationHighGMac;
static gxRegister activePowerL1;
static gxScriptTable scriptTableGlobalMeterReset;
static gxScriptTable scriptTableDisconnectControl;
static gxScriptTable scriptTableActivateTestMode;
static gxScriptTable scriptTableActivateNormalMode;
static gxProfileGeneric eventLog;
static gxActionSchedule actionScheduleDisconnectOpen;
static gxActionSchedule actionScheduleDisconnectClose;
static gxPushSetup pushSetup;
static gxDisconnectControl disconnectControl;
static gxProfileGeneric loadProfile;
static gxSapAssignment sapAssignment;
// Security Setup High is for High authentication.
static gxSecuritySetup securitySetupHigh;
// Security Setup HighGMac is for GMac authentication.
static gxSecuritySetup securitySetupHighGMac;

// static gxObject* NONE_OBJECTS[] = { BASE(associationNone), BASE(ldn), BASE(sapAssignment), BASE(clock1) };

static gxObject *ALL_OBJECTS[] = {BASE(associationNone), BASE(associationLow), BASE(associationHigh), BASE(associationHighGMac), BASE(securitySetupHigh), BASE(securitySetupHighGMac),
                                  BASE(ldn), BASE(sapAssignment), BASE(eventCode),
                                  BASE(clock1), BASE(activePowerL1), BASE(pushSetup), BASE(scriptTableGlobalMeterReset), BASE(scriptTableDisconnectControl),
                                  BASE(scriptTableActivateTestMode), BASE(scriptTableActivateNormalMode), BASE(loadProfile), BASE(eventLog), BASE(hdlc),
                                  BASE(disconnectControl), BASE(actionScheduleDisconnectOpen), BASE(actionScheduleDisconnectClose), BASE(unixTime), BASE(invocationCounter)};

////////////////////////////////////////////////////
// Define what is serialized to decrease EEPROM usage.
gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = {
    // Nothing is saved when authentication is not used.
    IGNORE_ATTRIBUTE(BASE(associationNone), GET_ATTRIBUTE_ALL()),
    // Only password is saved for low and high authentication.
    IGNORE_ATTRIBUTE(BASE(associationLow), GET_ATTRIBUTE_EXCEPT(7)),
    IGNORE_ATTRIBUTE(BASE(associationHigh), GET_ATTRIBUTE_EXCEPT(7)),
    IGNORE_ATTRIBUTE(BASE(associationHighGMac), GET_ATTRIBUTE_EXCEPT(7)),
    // Capture objects and period are not saved for profile generic objects.
    IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_PROFILE_GENERIC, GET_ATTRIBUTE(2, 3)),
    // Only scaler and unit are saved for all register objects.
    IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_REGISTER, GET_ATTRIBUTE(2))};

static gxSerializerSettings serializerSettings;
static uint32_t executeTime = 0;

static uint16_t activePowerL1Value = 0;

typedef enum
{
    // Meter is powered.
    GURUX_EVENT_CODES_POWER_UP = 0x1,
    // User has change the time.
    GURUX_EVENT_CODES_TIME_CHANGE = 0x2,
    // DST status is changed.
    GURUX_EVENT_CODES_DST = 0x4,
    // Push message is sent.
    GURUX_EVENT_CODES_PUSH = 0x8,
    // Meter makes auto connect.
    GURUX_EVENT_CODES_AUTO_CONNECT = 0x10,
    // User has change the password.
    GURUX_EVENT_CODES_PASSWORD_CHANGED = 0x20,
    // Wrong password tried 3 times.
    GURUX_EVENT_CODES_WRONG_PASSWORD = 0x40,
    // Disconnect control state is changed.
    GURUX_EVENT_CODES_OUTPUT_RELAY_STATE = 0x80,
    // User has reset the meter.
    GURUX_EVENT_CODES_GLOBAL_METER_RESET = 0x100
} GURUX_EVENT_CODES;

// Returns profile generic buffer column sizes.
int getProfileGenericBufferColumnSizes(
    gxProfileGeneric *pg,
    DLMS_DATA_TYPE *dataTypes,
    uint8_t *columnSizes,
    uint16_t *rowSize)
{
    int ret = 0;
    uint8_t pos;
    gxKey *it;
    gxValueEventArg e;
    ve_init(&e);
    *rowSize = 0;
    uint16_t size;
    unsigned char type;
    // Loop capture columns and get values.
    for (pos = 0; pos != pg->captureObjects.size; ++pos)
    {
        if ((ret = arr_getByIndex(&pg->captureObjects, (uint16_t)pos, (void **)&it)) != 0)
        {
            break;
        }
        // Date time is saved in EPOCH to save space.
        if (((gxObject *)it->key)->objectType == DLMS_OBJECT_TYPE_CLOCK && ((gxTarget *)it->value)->attributeIndex == 2)
        {
            type = DLMS_DATA_TYPE_UINT32;
            size = 4;
        }
        else
        {
            e.target = (gxObject *)it->key;
            e.index = ((gxTarget *)it->value)->attributeIndex;
            if ((ret = cosem_getValue(&settings.base, &e)) != 0)
            {
                break;
            }
            if (bb_size(e.value.byteArr) != 0)
            {
                if ((ret = bb_getUInt8(e.value.byteArr, &type)) != 0)
                {
                    break;
                }
                size = bb_available(e.value.byteArr);
            }
            else
            {
                type = DLMS_DATA_TYPE_NONE;
                size = 0;
            }
        }
        if (dataTypes != NULL)
        {
            dataTypes[pos] = type;
        }
        if (columnSizes != NULL)
        {
            columnSizes[pos] = (uint8_t)size;
        }
        *rowSize += (uint16_t)size;
        ve_clear(&e);
    }
    ve_clear(&e);
    return ret;
}

// Get max row count for allocated buffer.
uint16_t getProfileGenericBufferMaxRowCount(
    gxProfileGeneric *pg)
{
    // TODO:
    return 0;
}

// Get current row count for allocated buffer.
uint16_t getProfileGenericBufferEntriesInUse(gxProfileGeneric *pg)
{
    // TODO:
    return 0;
}

int captureProfileGeneric(gxProfileGeneric *pg)
{
    // TODO:
    return 0;
}

void updateState(uint16_t value)
{
    GX_UINT16(eventCode.value) = value;
    captureProfileGeneric(&eventLog);
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char *str, const char *data)
{
    // Send trace to the serial port in test mode.
    if (testMode)
    {
        if (data == NULL)
        {
            ESP_LOGI(TAG, "%s", str);
        }
        else
        {
            ESP_LOGI(TAG, "%s %s", str, data);
        }
    }
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_INT(const char *str, int32_t value)
{
    ESP_LOGI(TAG, "%s: %ld", str, value);
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_LN(const char *str, uint16_t type, unsigned char *ln)
{
    ESP_LOGI(TAG, "%d %d.%d.%d.%d.%d.%d", type, ln[0], ln[1], ln[2], ln[3], ln[4], ln[5]);
}

uint32_t SERIALIZER_SIZE()
{
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        0x40,
        "storage");
    if (!part)
    {
        ESP_LOGE(TAG, "Partition not found!");
        return 0;
    }
    return part->size;
}

int SERIALIZER_LOAD(uint32_t index, uint32_t count, void *value)
{
    if (index + count > SERIALIZER_SIZE())
    {
        return DLMS_ERROR_CODE_SERIALIZATION_LOAD_FAILURE;
    }
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        0x40,
        "storage");

    if (!part)
    {
        ESP_LOGE(TAG, "Partition not found!");
        return DLMS_ERROR_CODE_SERIALIZATION_LOAD_FAILURE;
    }
    unsigned char *buffer = (unsigned char *)value;
    esp_err_t err = esp_partition_read(part, index, buffer, count);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Settings read failed: %s", esp_err_to_name(err));
        return DLMS_ERROR_CODE_SERIALIZATION_LOAD_FAILURE;
    }
    return 0;
}

int SERIALIZER_SAVE(uint32_t index, uint32_t count, const void *value)
{
    if (index + count > SERIALIZER_SIZE())
    {
        return DLMS_ERROR_CODE_SERIALIZATION_LOAD_FAILURE;
    }
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        0x40,
        "storage");

    if (!part)
    {
        ESP_LOGE(TAG, "Partition not found!");
        return DLMS_ERROR_CODE_SERIALIZATION_LOAD_FAILURE;
    }
    unsigned char *buffer = (unsigned char *)value;
    esp_err_t err = esp_partition_write(part, index, buffer, count);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Settings write failed: %s", esp_err_to_name(err));
        return DLMS_ERROR_CODE_SERIALIZATION_LOAD_FAILURE;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Save data to the flash.
//
// Only updated value is saved. This is done because write to EEPROM is slow
// and there is a limit how many times value can be written to the EEPROM.
/////////////////////////////////////////////////////////////////////////////
int saveSettings()
{
    int ret = 0;
    uint16_t count = sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]);
    GXTRACE_INT(GET_STR_FROM_EEPROM("Saving settings: "), count);
    // Erase old data.
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        0x40,
        "storage");
    esp_err_t err = esp_partition_erase_range(part, 0, part->size);
    if (err == ESP_OK)
    {
        if ((ret = ser_saveObjects(&serializerSettings, ALL_OBJECTS, count)) != 0)
        {
            GXTRACE_INT(GET_STR_FROM_EEPROM("saveObjects failed: "), ret);
        }
        else
        {
            GXTRACE_INT(GET_STR_FROM_EEPROM("saveObjects succeeded. count: "), serializerSettings.position);
        }
    }
    else
    {
        GXTRACE_INT(GET_STR_FROM_EEPROM("Failed to erase partition: "), err);
        return DLMS_ERROR_CODE_SERIALIZATION_SAVE_FAILURE;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Load data from the flash.
// Returns serialization version or zero if data is not saved.
/////////////////////////////////////////////////////////////////////////////
int loadSettings()
{
    int ret = 0;
    serializerSettings.position = 0;
    uint16_t count = sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]);
    GXTRACE_INT(GET_STR_FROM_EEPROM("Loading settings: "), count);

    ret = ser_loadObjects(&settings.base, &serializerSettings, ALL_OBJECTS, count);
    if (ret != 0)
    {
        GXTRACE_INT(GET_STR_FROM_EEPROM("Failed to load settings from EEPROM."), serializerSettings.position);
    }
    else
    {
        GXTRACE_INT(GET_STR_FROM_EEPROM("loadSettings succeeded. Position: "), serializerSettings.position);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Print detailed information to the serial port.
void svr_trace(const char *str, const char *data)
{
    GXTRACE(str, data);
}

void time_now(
    gxtime *value,
    unsigned char meterTime)
{
    uint32_t offset = time_current();
    time_initUnix(value, offset);
    // If date time is wanted in meter time, not in utc time.
    if (meterTime)
    {
        clock_utcToMeterTime(&clock1, value);
    }
}

static void dlms_server(void *pvParameters)
{
    // TCP/IP max size is 1500 bytes.
    unsigned char rx_buffer[WRAPPER_BUFFER_SIZE];
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    listen(listen_sock, 1);

    ESP_LOGI(TAG, "Socket listening on port %d", PORT);

    while (1)
    {
        struct sockaddr_in6 source_addr;
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);

        ESP_LOGI(TAG, "Client connected: %s", addr_str);

        while (1)
        {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0)
            {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            else if (len == 0)
            {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
            else
            {
                if (trace == GX_TRACE_LEVEL_VERBOSE)
                {
                    ESP_LOGI(TAG, "RX:");
                    ESP_LOG_BUFFER_HEX_LEVEL(TAG, rx_buffer, len, ESP_LOG_INFO);
                }
                if (svr_handleRequest2(&settings, rx_buffer, len, &reply) != 0)
                {
                    GXTRACE_INT(GET_STR_FROM_EEPROM("handleRequest failed:"), len);
                    bb_clear(&reply);
                }
                if (reply.size != 0)
                {
                    // Send reply.
                    send(sock, reply.data, reply.size, 0);
                    if (trace == GX_TRACE_LEVEL_VERBOSE)
                    {
                        ESP_LOGI(TAG, "TX:");
                        ESP_LOG_BUFFER_HEX_LEVEL(TAG, rx_buffer, len, ESP_LOG_INFO);
                    }
                    bb_clear(&reply);
                }
            }
        }

        shutdown(sock, 0);
        close(sock);
    }

    close(listen_sock);
    vTaskDelete(NULL);
}

// Event handler for IP address acquisition
// This function is called when the device gets an IP address.
static void on_got_ip(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    esp_netif_ip_info_t ip_info = event->ip_info;

    ESP_LOGI(TAG, "Meter IP Address: " IPSTR, IP2STR(&ip_info.ip));
}

///////////////////////////////////////////////////////////////////////
// This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociationNone()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 40, 0, 1, 255};
    if ((ret = INIT_OBJECT(associationNone, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        // All objects are shown also without authentication.
        OA_ATTACH(associationNone.objectList, ALL_OBJECTS);
        // Uncomment this if you want to show only part of the objects without authentication.
        // OA_ATTACH(associationNone.objectList, NONE_OBJECTS);
        associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        associationNone.clientSAP = 0x10;
        // Max PDU is half of PDU size. This is for demonstration purposes only.
        associationNone.xDLMSContextInfo.maxSendPduSize = associationNone.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE / 2;
        associationNone.xDLMSContextInfo.conformance = DLMS_CONFORMANCE_GET;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociationLow()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 40, 0, 2, 255};
    if ((ret = INIT_OBJECT(associationLow, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        // Only Logical Device Name is add to this Association View.
        OA_ATTACH(associationLow.objectList, ALL_OBJECTS);
        associationLow.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_LOW;
        associationLow.clientSAP = 0x11;
        associationLow.xDLMSContextInfo.maxSendPduSize = associationLow.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationLow.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
                                                                         DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
                                                                         DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
                                                                         DLMS_CONFORMANCE_SET |
                                                                         DLMS_CONFORMANCE_SELECTIVE_ACCESS |
                                                                         DLMS_CONFORMANCE_ACTION |
                                                                         DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
                                                                         DLMS_CONFORMANCE_GET);
        bb_addString(&associationLow.secret, "Gurux");
        associationLow.securitySetup = NULL;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds example Logical Name Association object for High authentication.
//  UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addAssociationHigh()
{
    int ret;
    // Dedicated key.
    static unsigned char CYPHERING_INFO[20] = {0};
    const unsigned char ln[6] = {0, 0, 40, 0, 3, 255};
    if ((ret = INIT_OBJECT(associationHigh, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHigh.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
        OA_ATTACH(associationHigh.objectList, ALL_OBJECTS);
        BB_ATTACH(associationHigh.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        associationHigh.clientSAP = 0x12;
        associationHigh.xDLMSContextInfo.maxSendPduSize = associationHigh.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationHigh.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
                                                                          DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
                                                                          DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
                                                                          DLMS_CONFORMANCE_SET |
                                                                          DLMS_CONFORMANCE_SELECTIVE_ACCESS |
                                                                          DLMS_CONFORMANCE_ACTION |
                                                                          DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
                                                                          DLMS_CONFORMANCE_GET);
        bb_addString(&associationHigh.secret, "Gurux");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        associationHigh.securitySetup = &securitySetupHigh;
#else
        memcpy(associationHigh.securitySetupReference, securitySetupHigh.base.logicalName, 6);
#endif // DLMS_IGNORE_OBJECT_POINTERS
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds example Logical Name Association object for GMAC High authentication.
//  UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addAssociationHighGMac()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 40, 0, 4, 255};
    if ((ret = INIT_OBJECT(associationHighGMac, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHighGMac.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_GMAC;
        OA_ATTACH(associationHighGMac.objectList, ALL_OBJECTS);
        associationHighGMac.clientSAP = 0x1;
        associationHighGMac.xDLMSContextInfo.maxSendPduSize = associationHighGMac.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationHighGMac.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
                                                                              DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
                                                                              DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
                                                                              DLMS_CONFORMANCE_SET |
                                                                              DLMS_CONFORMANCE_SELECTIVE_ACCESS |
                                                                              DLMS_CONFORMANCE_ACTION |
                                                                              DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
                                                                              DLMS_CONFORMANCE_GET);
        // GMAC authentication don't need password.
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        associationHighGMac.securitySetup = &securitySetupHighGMac;
#else
        memcpy(associationHighGMac.securitySetupReference, securitySetupHigh.base.logicalName, 6);
#endif // DLMS_IGNORE_OBJECT_POINTERS
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds security setup object for High authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupHigh()
{
    int ret;
    // Define client system title.
    static unsigned char CLIENT_SYSTEM_TITLE[8] = {0};
    const unsigned char ln[6] = {0, 0, 43, 0, 1, 255};
    if ((ret = INIT_OBJECT(securitySetupHigh, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        BB_ATTACH(securitySetupHigh.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        BB_ATTACH(securitySetupHigh.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
        securitySetupHigh.securityPolicy = DLMS_SECURITY_POLICY_NOTHING;
        securitySetupHigh.securitySuite = DLMS_SECURITY_SUITE_V0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds security setup object for High GMAC authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupHighGMac()
{
    int ret;
    // Define client system title.
    static unsigned char CLIENT_SYSTEM_TITLE[8] = {0};
    const unsigned char ln[6] = {0, 0, 43, 0, 2, 255};
    if ((ret = INIT_OBJECT(securitySetupHighGMac, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        BB_ATTACH(securitySetupHighGMac.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        BB_ATTACH(securitySetupHighGMac.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
        // Only Authenticated encrypted connections are allowed.
        securitySetupHighGMac.securityPolicy = DLMS_SECURITY_POLICY_AUTHENTICATED_ENCRYPTED;
        securitySetupHighGMac.securitySuite = DLMS_SECURITY_SUITE_V0;
        securitySetupHighGMac.minimumInvocationCounter = 1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add SAP Assignment object.
///////////////////////////////////////////////////////////////////////
int addSapAssignment()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 41, 0, 0, 255};
    if ((ret = INIT_OBJECT(sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln)) == 0)
    {
        char tmp[17];
        gxSapItem *it = (gxSapItem *)gxmalloc(sizeof(gxSapItem));
        BYTE_BUFFER_INIT(&it->name);
        ret = sprintf(tmp, "%s%.13lu", FLAG_ID, SERIAL_NUMBER);
        bb_addString(&it->name, tmp);
        it->id = 1;
        ret = arr_push(&sapAssignment.sapAssignmentList, it);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add Logical Device Name. 123456 is meter serial number.
///////////////////////////////////////////////////////////////////////
// COSEM Logical Device Name is defined as an octet-string of 16 octets.
// The first three octets uniquely identify the manufacturer of the device and it corresponds
// to the manufacturer's identification in IEC 62056-21.
// The following 13 octets are assigned by the manufacturer.
// The manufacturer is responsible for guaranteeing the uniqueness of these octets.
int addLogicalDeviceName()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 42, 0, 0, 255};
    if ((ret = INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        char tmp[17];
        sprintf(tmp, "%s%.13lu", FLAG_ID, SERIAL_NUMBER);
        var_addBytes(&ldn.value, (unsigned char *)tmp, 16);
    }
    return ret;
}

// Add event code object.
int addEventCode()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 96, 11, 0, 255};
    if ((ret = INIT_OBJECT(eventCode, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventCode.value) = 0;
    }
    return ret;
}

// Add unix time object.
int addUnixTime()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 1, 1, 0, 255};
    if ((ret = INIT_OBJECT(unixTime, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        // Set initial value.
        GX_UINT32(unixTime.value) = 0;
    }
    return ret;
}

// Add invocation counter object.
int addInvocationCounter()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 43, 1, 0, 255};
    if ((ret = INIT_OBJECT(invocationCounter, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        // Initial invocation counter value.
        GX_UINT32_BYREF(invocationCounter.value, securitySetupHighGMac.minimumInvocationCounter);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addPushSetup()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 25, 9, 0, 255};
    if ((ret = INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln)) == 0)
    {
        pushSetup.service = DLMS_SERVICE_TYPE_HDLC;
        gxtime *start, *end;
        start = (gxtime *)gxmalloc(sizeof(gxtime));
        end = (gxtime *)gxmalloc(sizeof(gxtime));
        // This push is sent every minute, but max 10 seconds over.
        time_init(start, -1, -1, -1, -1, -1, 0, 0, 0);
        time_init(end, -1, -1, -1, -1, -1, 10, 0, 0);
        arr_push(&pushSetup.communicationWindow, key_init(start, end));
        // This push is sent every half minute, but max 40 seconds over.
        start = (gxtime *)gxmalloc(sizeof(gxtime));
        end = (gxtime *)gxmalloc(sizeof(gxtime));
        time_init(start, -1, -1, -1, -1, -1, 30, 0, 0);
        time_init(end, -1, -1, -1, -1, -1, 40, 0, 0);
        arr_push(&pushSetup.communicationWindow, key_init(start, end));
        // Add logical device name.
        gxTarget *capture = (gxTarget *)gxmalloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&pushSetup.pushObjectList, key_init(BASE(ldn), capture));

        // Add push object logical name. This is needed to tell structure of data to the Push listener.
        // Also capture object list can be used here.
        capture = (gxTarget *)gxmalloc(sizeof(gxTarget));
        capture->attributeIndex = 1;
        capture->dataIndex = 0;
        arr_push(&pushSetup.pushObjectList, key_init(BASE(pushSetup), capture));
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
    int ret = 0;
    // Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
    const unsigned char ln[6] = {0, 0, 1, 0, 0, 255};
    if ((ret = INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
    {
        // Set default values.
        time_init(&clock1.begin, -1, 3, -1, 2, 0, 0, 0, 0);
        clock1.begin.extraInfo = DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
        time_init(&clock1.end, -1, 10, -1, 3, 0, 0, 0, 0);
        clock1.end.extraInfo = DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
        // Meter is using UTC time zone.
        clock1.timeZone = 0;
        // Deviation is 60 minutes.
        clock1.deviation = 60;
        clock1.clockBase = DLMS_CLOCK_BASE_FREQUENCY_50;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
    int ret;
    const unsigned char ln[6] = {1, 1, 21, 25, 0, 255};
    if ((ret = INIT_OBJECT(activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        activePowerL1Value = 10;
        GX_UINT16_BYREF(activePowerL1.value, activePowerL1Value);
        // 10 ^ 3 =  1000
        activePowerL1.scaler = 3;
        activePowerL1.unit = 30;
    }
    return ret;
}

uint16_t readActivePowerValue()
{
    return ++activePowerL1Value;
}

uint16_t readEventCode()
{
    return eventCode.value.uiVal;
}
///////////////////////////////////////////////////////////////////////
// Add script table object for meter reset. This will erase the EEPROM.
///////////////////////////////////////////////////////////////////////
int addscriptTableGlobalMeterReset()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 10, 0, 0, 255};
    if ((ret = INIT_OBJECT(scriptTableGlobalMeterReset, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript *s = (gxScript *)gxmalloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        // Add executed script to script list.
        arr_push(&scriptTableGlobalMeterReset.scripts, s);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////
// Add script table object for disconnect control.
// Action 1 calls remote_disconnect #1 (close).
// Action 2 calls remote_connect #2(open).
///////////////////////////////////////////////////////////////////////
int addscriptTableDisconnectControl()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 10, 0, 106, 255};
    if ((ret = INIT_OBJECT(scriptTableDisconnectControl, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript *s = (gxScript *)gxmalloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        gxScriptAction *a = (gxScriptAction *)gxmalloc(sizeof(gxScriptAction));

        a->type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        a->target = BASE(disconnectControl);
        a->index = 1;
        var_init(&a->parameter);
        // Action data is Int8 zero.
        GX_INT8(a->parameter) = 0;
        arr_push(&s->actions, a);
        // Add executed script to script list.
        arr_push(&scriptTableDisconnectControl.scripts, s);

        s = (gxScript *)gxmalloc(sizeof(gxScript));
        s->id = 2;
        arr_init(&s->actions);
        a = (gxScriptAction *)gxmalloc(sizeof(gxScriptAction));
        a->type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        a->target = BASE(disconnectControl);
        a->index = 2;
        var_init(&a->parameter);
        // Action data is Int8 zero.
        GX_INT8(a->parameter) = 0;
        arr_push(&s->actions, a);
        // Add executed script to script list.
        arr_push(&scriptTableDisconnectControl.scripts, s);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add script table object for test mode. In test mode meter is sending trace to the serial port.
///////////////////////////////////////////////////////////////////////
int addscriptTableActivateTestMode()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 10, 0, 101, 255};
    if ((ret = INIT_OBJECT(scriptTableActivateTestMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript *s = (gxScript *)gxmalloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        // Add executed script to script list.
        arr_push(&scriptTableActivateTestMode.scripts, s);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add script table object for Normal mode. In normal mode meter is NOT sending trace to the serial port.
///////////////////////////////////////////////////////////////////////
int addscriptTableActivateNormalMode()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 10, 0, 102, 255};
    if ((ret = INIT_OBJECT(scriptTableActivateNormalMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript *s = (gxScript *)gxmalloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        // Add executed script to script list.
        arr_push(&scriptTableActivateNormalMode.scripts, s);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addLoadProfileProfileGeneric()
{
    int ret;
    const unsigned char ln[6] = {1, 0, 99, 1, 0, 255};
    if ((ret = INIT_OBJECT(loadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        gxTarget *capture;
        // Set default values if load the first time.
        loadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        // Add 2 columns.
        // Add clock obect.
        capture = (gxTarget *)gxmalloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&loadProfile.captureObjects, key_init(&clock1, capture));
        // Add active power.
        capture = (gxTarget *)gxmalloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&loadProfile.captureObjects, key_init(&activePowerL1, capture));
        ///////////////////////////////////////////////////////////////////
        // Update amount of capture objects.
        // Set clock to sort object.
        loadProfile.sortObject = BASE(clock1);
        loadProfile.sortObjectAttributeIndex = 2;
        loadProfile.profileEntries = getProfileGenericBufferMaxRowCount(&loadProfile);
        loadProfile.entriesInUse = getProfileGenericBufferEntriesInUse(&loadProfile);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
// Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addEventLogProfileGeneric()
{
    int ret;
    const unsigned char ln[6] = {1, 0, 99, 98, 0, 255};
    if ((ret = INIT_OBJECT(eventLog, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        eventLog.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        // Add 2 columns as default.
        gxTarget *capture;
        // Add clock obect.
        capture = (gxTarget *)gxmalloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&eventLog.captureObjects, key_init(&clock1, capture));

        // Add event code.
        capture = (gxTarget *)gxmalloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&eventLog.captureObjects, key_init(&eventCode, capture));
        // Set clock to sort object.
        eventLog.sortObject = BASE(clock1);
        eventLog.sortObjectAttributeIndex = 2;
        eventLog.profileEntries = getProfileGenericBufferMaxRowCount(&eventLog);
        eventLog.entriesInUse = getProfileGenericBufferEntriesInUse(&eventLog);
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////
// Add action schedule object for disconnect control to close the led.
///////////////////////////////////////////////////////////////////////
int addActionScheduleDisconnectClose()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 15, 0, 1, 255};
    if ((ret = INIT_OBJECT(actionScheduleDisconnectClose, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionScheduleDisconnectClose.executedScript = &scriptTableDisconnectControl;
        actionScheduleDisconnectClose.executedScriptSelector = 1;
        actionScheduleDisconnectClose.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add action schedule object for disconnect control to open the led.
///////////////////////////////////////////////////////////////////////
int addActionScheduleDisconnectOpen()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 15, 0, 3, 255};
    // Action schedule execution times.
    if ((ret = INIT_OBJECT(actionScheduleDisconnectOpen, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionScheduleDisconnectOpen.executedScript = &scriptTableDisconnectControl;
        actionScheduleDisconnectOpen.executedScriptSelector = 2;
        actionScheduleDisconnectOpen.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add Disconnect control object.
///////////////////////////////////////////////////////////////////////
int addDisconnectControl()
{
    int ret;
    const unsigned char ln[6] = {0, 0, 96, 3, 10, 255};
    if ((ret = INIT_OBJECT(disconnectControl, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln)) == 0)
    {
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup()
{
    int ret = 0;
    unsigned char ln[6] = {0, 0, 22, 0, 0, 255};
    if ((ret = INIT_OBJECT(hdlc, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
    {
        hdlc.communicationSpeed = DLMS_BAUD_RATE_9600;
        hdlc.windowSizeReceive = hdlc.windowSizeTransmit = 1;
        hdlc.maximumInfoLengthTransmit = hdlc.maximumInfoLengthReceive = 128;
        hdlc.inactivityTimeout = 120;
        hdlc.deviceAddress = 0x10;
    }
    settings.hdlc = &hdlc;
    return ret;
}

// Create objects and load values from EEPROM.
int createObjects()
{
    int ret;
    OA_ATTACH(settings.base.objects, ALL_OBJECTS);
    if ((ret = addLogicalDeviceName()) != 0 ||
        (ret = addSapAssignment()) != 0 ||
        (ret = addEventCode()) != 0 ||
        (ret = addUnixTime()) != 0 ||
        (ret = addInvocationCounter()) != 0 ||
        (ret = addClockObject()) != 0 ||
        (ret = addRegisterObject()) != 0 ||
        (ret = addAssociationNone()) != 0 ||
        (ret = addAssociationLow()) != 0 ||
        (ret = addAssociationHigh()) != 0 ||
        (ret = addAssociationHighGMac()) != 0 ||
        (ret = addSecuritySetupHigh()) != 0 ||
        (ret = addSecuritySetupHighGMac()) != 0 ||
        (ret = addPushSetup()) != 0 ||
        (ret = addscriptTableGlobalMeterReset()) != 0 ||
        (ret = addscriptTableDisconnectControl()) != 0 ||
        (ret = addscriptTableActivateTestMode()) != 0 ||
        (ret = addscriptTableActivateNormalMode()) != 0 ||
        (ret = addLoadProfileProfileGeneric()) != 0 ||
        (ret = addEventLogProfileGeneric()) != 0 ||
        (ret = addActionScheduleDisconnectOpen()) != 0 ||
        (ret = addActionScheduleDisconnectClose()) != 0 ||
        (ret = addDisconnectControl()) != 0 ||
        (ret = addIecHdlcSetup()) != 0 ||
        (ret = oa_verify(&settings.base.objects)) != 0 ||
        (ret = svr_initialize(&settings)) != 0)
    {
        GXTRACE_INT(("Failed to start the meter!"), ret);
        executeTime = 0;
        return ret;
    }
    if ((ret = loadSettings()) != 0)
    {
        GXTRACE_INT(("Failed to load settings!"), ret);
        if ((ret = saveSettings()) != 0)
        {
            GXTRACE_INT(("Failed to save settings!"), ret);
            return ret;
        }
        executeTime = 0;
        return ret;
    }
    updateState(GURUX_EVENT_CODES_POWER_UP);
    GXTRACE(("Meter started."), NULL);
    return 0;
}

int svr_findObject(
    dlmsSettings *settings,
    DLMS_OBJECT_TYPE objectType,
    int sn,
    unsigned char *ln,
    gxValueEventArg *e)
{
    GXTRACE_LN(("findObject"), objectType, ln);
    if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        uint16_t pos;
        objectArray objects;
        gxObject *tmp[6];
        oa_attach(&objects, tmp, sizeof(tmp) / sizeof(tmp[0]));
        objects.size = 0;
        if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, &objects) == 0)
        {
            gxAssociationLogicalName *a;
            for (pos = 0; pos != objects.size; ++pos)
            {
                if (oa_getByIndex(&objects, pos, (gxObject **)&a) == 0)
                {
                    if (a->clientSAP == settings->clientAddress &&
                        a->authenticationMechanismName.mechanismId == settings->authentication)
                    {
                        e->target = (gxObject *)a;
                        break;
                    }
                }
            }
        }
    }
    if (e->target == NULL)
    {
        GXTRACE_LN(("Unknown object"), objectType, ln);
    }
    return 0;
}

/**
  Find restricting object.
*/
int getRestrictingObject(dlmsSettings *settings,
                         gxValueEventArg *e,
                         gxObject **obj,
                         short *index)
{
    int ret;
    dlmsVARIANT *it, *it2;
    if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) != 0)
    {
        return ret;
    }
    DLMS_OBJECT_TYPE ot;
    unsigned char *ln;
    if ((ret = va_getByIndex(it->Arr, 0, &it2)) != 0)
    {
        return ret;
    }
    ot = it2->iVal;
    if ((ret = va_getByIndex(it->Arr, 1, &it2)) != 0)
    {
        return ret;
    }
    ln = it2->byteArr->data;
    if ((ret = va_getByIndex(it->Arr, 3, &it2)) != 0)
    {
        return ret;
    }
    *index = it2->iVal;
    if ((ret = oa_findByLN(&settings->objects, ot, ln, obj)) != 0)
    {
        return ret;
    }
    return ret;
}

int init_dlms()
{
    int ret;
    // Initialize serialization settings.
    ser_init(&serializerSettings);
    serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
    serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);

    // Add FLAG ID.
    memcpy(SERVER_SYSTEM_TITLE, FLAG_ID, 3);
    // ADD serial number.
    memcpy(SERVER_SYSTEM_TITLE + 4, &SERIAL_NUMBER, 4);

    bb_attach(&reply, replyFrame, 0, sizeof(replyFrame));
    // Start server using logical name referencing and WRAPPER framing.
    svr_init(&settings, 1, DLMS_INTERFACE_TYPE_WRAPPER, WRAPPER_BUFFER_SIZE, PDU_BUFFER_SIZE, frameBuff, WRAPPER_BUFFER_SIZE, pduBuff, PDU_BUFFER_SIZE);
    // Allocate space for client challenge.
    BB_ATTACH(settings.base.ctoSChallenge, C2S_CHALLENGE, 0);
    // Allocate space for server challenge.
    BB_ATTACH(settings.base.stoCChallenge, S2C_CHALLENGE, 0);
    // Set master key (KEK) to 1111111111111111.
    unsigned char KEK[16] = {0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31};
    BB_ATTACH(settings.base.kek, KEK, sizeof(KEK));
    // Add COSEM objects.
    if ((ret = createObjects()) != 0)
    {
        return ret;
    }
    // Set default clock.
    settings.defaultClock = &clock1;

    // Push service uses the same settings as main service.
    // This makes possible to used different settings for push service.
    svr_init(&pushSettings, 1, DLMS_INTERFACE_TYPE_WRAPPER, WRAPPER_BUFFER_SIZE, PDU_BUFFER_SIZE, frameBuff, WRAPPER_BUFFER_SIZE, pduBuff, PDU_BUFFER_SIZE);
    // Allocate space for client challenge.
    BB_ATTACH(pushSettings.base.ctoSChallenge, C2S_CHALLENGE, 0);
    // Allocate space for server challenge.
    BB_ATTACH(pushSettings.base.stoCChallenge, S2C_CHALLENGE, 0);
    BB_ATTACH(pushSettings.base.kek, KEK, sizeof(KEK));
    OA_ATTACH(pushSettings.base.objects, ALL_OBJECTS);
    return 0;
}

/*Show partition info.*/
void showPartitionInfo()
{
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (it != NULL)
    {
        const esp_partition_t *p = esp_partition_get(it);
        //        ESP_LOGI(TAG, "Found: label=%s, type=0x%x, subtype=0x%x, addr=0x%x",
        //                 p->label, p->type, p->subtype, p->address);
        ESP_LOGI(TAG, "Found partition: %s (type %d, subtype 0x%02x)",
                 p->label, p->type, p->subtype);
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // showPartitionInfo();
    ESP_ERROR_CHECK(init_dlms());
    esp_netif_init();

    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_LOGI(TAG, "Connecting to SSID: %s", WIFI_SSID);
    ESP_LOGI(TAG, "Using password: %s", WIFI_PASS);

    ESP_LOGI(TAG, "----------------------------------------------------------");
    ESP_LOGI(TAG, "Authentication levels:");
    ESP_LOGI(TAG, "None: Client address 16 (0x10)");
    ESP_LOGI(TAG, "Low: Client address 17 (0x11)");
    ESP_LOGI(TAG, "High: Client address 18 (0x12)");
    ESP_LOGI(TAG, "HighGMac: Client address 1 (1)");
    ESP_LOGI(TAG, "----------------------------------------------------------");
    ESP_LOGI(TAG, "System Title");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, settings.base.cipher.systemTitle.data, settings.base.cipher.systemTitle.size, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Authentication key");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, settings.base.cipher.authenticationKey.data, settings.base.cipher.authenticationKey.size, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Block cipher key");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, settings.base.cipher.blockCipherKey.data, settings.base.cipher.blockCipherKey.size, ESP_LOG_INFO);

    if (settings.base.preEstablishedSystemTitle != NULL)
    {
        ESP_LOGI(TAG, "Pre-established System title");
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, settings.base.preEstablishedSystemTitle->data, settings.base.preEstablishedSystemTitle->size, ESP_LOG_INFO);
    }
    else
    {
        ESP_LOGI(TAG, "No pre-established System title");
    }
    ESP_LOGI(TAG, "Master key (KEK)");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, settings.base.kek.data, settings.base.kek.size, ESP_LOG_INFO);
    ESP_LOGI(TAG, "----------------------------------------------------------");

    // Connect to Wi-Fi (SSID + PASSWORD are set in sdkconfig)
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        }};
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL, NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_connect();
    // Start DLMS server task
    xTaskCreate(dlms_server, "dlms_server", 4096, NULL, 5, NULL);
}

void handleProfileGenericActions(
    gxValueEventArg *it)
{
    if (it->index == 1)
    {
        // Profile generic clear is called. Clear data.
        ((gxProfileGeneric *)it->target)->entriesInUse = 0;
    }
    else if (it->index == 2)
    {
        // Increase power value before each load profile read to increase the value.
        // This is needed for demo purpose only.
        if (it->target == BASE(loadProfile))
        {
            readActivePowerValue();
        }
        captureProfileGeneric(((gxProfileGeneric *)it->target));
    }
    saveSettings();
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preRead(
    dlmsSettings *settings,
    gxValueEventCollection *args)
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
        GXTRACE_LN(("svr_preRead: "), e->target->objectType, e->target->logicalName);
        // Let framework handle Logical Name read.
        if (e->index == 1)
        {
            continue;
        }

        // Get target type.
        type = (DLMS_OBJECT_TYPE)e->target->objectType;
        // Let Framework will handle Association objects and profile generic automatically.
        if (type == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
            type == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
        {
            continue;
        }
        // Update value by one every time when user reads register.
        if (e->target == BASE(activePowerL1) && e->index == 2)
        {
            readActivePowerValue();
        }
        // Get time if user want to read date and time.
        if (e->target == BASE(clock1) && e->index == 2)
        {
            gxtime dt;
            time_now(&dt, 1);
            if (e->value.byteArr == NULL)
            {
                e->value.byteArr = (gxByteBuffer *)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(e->value.byteArr);
            }
            e->error = cosem_setDateTimeAsOctetString(e->value.byteArr, &dt);
            e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
            e->handled = 1;
        }
        else if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC && e->index == 2)
        {
            // TODO: e->error = (DLMS_ERROR_CODE)readProfileGeneric(settings, (gxProfileGeneric*)e->target, e);
        }
        // Update Unix time.
        if (e->target == BASE(unixTime) && e->index == 2)
        {
            gxtime dt;
            time_now(&dt, 0);
            e->value.ulVal = time_toUnixTime2(&dt);
            e->value.vt = DLMS_DATA_TYPE_UINT32;
            e->handled = 1;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preWrite(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
    char str[25];
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        // If client try to update low level password when high level authentication is established.
        // This is possible in Indian standard.
        if (e->target == BASE(associationHigh) && e->index == 7)
        {
            ret = cosem_getOctetString(e->value.byteArr, &associationLow.secret);
            saveSettings();
            e->handled = 1;
        }
        hlp_getLogicalNameToString(e->target->logicalName, str);
        GXTRACE("Writing", str);
    }
}

void sendPush_task(void *pvParameter)
{
    char buff[32];
    gxPushSetup *push = (gxPushSetup *)pvParameter;
    if (!(push->destination.size < sizeof(buff)))
    {
        vTaskDelete(NULL); // End task.
        return;
    }
    strncpy(buff, (char *)push->destination.data, push->destination.size);
    buff[push->destination.size + 1] = '\0';
    char *colon = strchr(buff, ':');
    if (!colon)
    {
        vTaskDelete(NULL); // End task.
        return;
    }
    *colon = '\0';
    gxByteBuffer *bb;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(buff);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(atoi(colon + 1));
    ESP_LOGI(TAG, "sendPush %s: %d", buff, atoi(colon + 1));
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Socket creation failed: errno %d", errno);
        vTaskDelete(NULL); // End task.
        return;
    }
    int ret = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret != 0)
    {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
    }
    else
    {

        int pos;
        if (push->pushObjectList.size == 0)
        {
            GXTRACE(("sendPush Failed. No objects selected."), NULL);
        }
        message messages;
        mes_init(&messages);
        if ((ret = notify_generatePushSetupMessages(&settings.base, 0, push, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                bb = messages.data[pos];
                if ((ret = send(sock, (char *)bb->data, bb->size, 0)) == -1)
                {
                    break;
                }
            }
        }
        else
        {
            GXTRACE(("generatePushSetupMessages Failed."), NULL);
        }
        mes_clear(&messages);
    }
    close(sock);
    vTaskDelete(NULL); // End task.
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_preAction: "), e->target->objectType, e->target->logicalName);
        if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            handleProfileGenericActions(e);
            e->handled = 1;
        }
        else if (e->target == BASE(activePowerL1))
        {
            // Set default value for active power.
            activePowerL1Value = 0;
            e->handled = 1;
        }
        else if (e->target == BASE(pushSetup) && e->index == 1)
        {
            updateState(GURUX_EVENT_CODES_PUSH);
            xTaskCreate(sendPush_task, "sendPush", 4096, (void *)e->target, 5, NULL);
            e->handled = 1;
        }
        // If client wants to clear EEPROM data using Global meter reset script.
        else if (e->target == BASE(scriptTableGlobalMeterReset) && e->index == 1)
        {
            // Erase old data.
            const esp_partition_t *part = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA,
                0x40,
                "storage");
            esp_partition_erase_range(part, 0, part->size);
            updateState(GURUX_EVENT_CODES_GLOBAL_METER_RESET);
            e->handled = 1;
        }
        else if (e->target == BASE(disconnectControl))
        {
            updateState(GURUX_EVENT_CODES_OUTPUT_RELAY_STATE);
            // Disconnect. Turn led OFF.
            if (e->index == 1)
            {
                ESP_LOGI(TAG, "Led is OFF.");
            }
            else // Reconnnect. Turn LED ON.
            {
                ESP_LOGI(TAG, "Led is OFF.");
            }
        }
        else if (e->target == BASE(scriptTableActivateTestMode))
        {
            // Activate test mode.
            testMode = 1;
        }
        else if (e->target == BASE(scriptTableActivateNormalMode))
        {
            // Activate normal mode.
            testMode = 0;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postRead(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_postRead: "), e->target->objectType, e->target->logicalName);
    }
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postWrite(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_postWrite: "), e->target->objectType, e->target->logicalName);
        if (e->target == BASE(clock1) && e->index == 2)
        {
            struct timeval now = {.tv_sec = time_toUnixTime2(&clock1.time), .tv_usec = 0};
            settimeofday(&now, NULL);
            updateState(GURUX_EVENT_CODES_TIME_CHANGE);
            e->handled = 1;
        }
        if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            // Use want to change capture objects.
            if (e->index == 3)
            {
                saveSettings();
                // Clear buffer if user changes captured objects.
                gxValueEventArg it;
                ve_init(&it);
                it.index = 1;
                handleProfileGenericActions(&it);
                // Count how many rows fit to the buffer.
                ((gxProfileGeneric *)e->target)->profileEntries = getProfileGenericBufferMaxRowCount(((gxProfileGeneric *)e->target));
                if (((gxProfileGeneric *)e->target)->captureObjects.size != 0)
                {
                    gxKey *k = NULL;
                    arr_getByIndex(&((gxProfileGeneric *)e->target)->captureObjects, 0, (void **)&k);
                    // Set 1st object to sort object.
                    ((gxProfileGeneric *)e->target)->sortObject = (gxObject *)k->key;
                }
                else
                {
                    ((gxProfileGeneric *)e->target)->sortObject = NULL;
                }
            }
            // Use want to change max amount of profile entries.
            else if (e->index == 8)
            {
                // Count how many rows fit to the buffer.
                uint16_t maxCount = getProfileGenericBufferMaxRowCount(((gxProfileGeneric *)e->target));
                // If use try to set max profileEntries bigger than can fit to allocated memory.
                if (maxCount < ((gxProfileGeneric *)e->target)->profileEntries)
                {
                    ((gxProfileGeneric *)e->target)->profileEntries = maxCount;
                }
            }
        }
        if (e->error == 0)
        {
            // Save settings to EEPROM.
            if ((ret = saveSettings(e->target, GET_ATTRIBUTE(e->index))) != 0)
            {
                GXTRACE_INT(("Failed to save settings!"), ret);
                e->error = ret;
            }
            if ((ret = loadSettings()) != 0)
            {
                GXTRACE_INT(("Failed to load settings!"), ret);
                e->error = ret;
            }
        }
        else
        {
            // Reject changes loading previous settings if there is an error.
            loadSettings();
        }
    }
    // Reset execute time to update execute time if user add new execute times or changes the time.
    executeTime = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postAction(
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_postAction: "), e->target->objectType, e->target->logicalName);
        // Check is client changing the settings with action.
        if (svr_isChangedWithAction(e->target->objectType, e->index))
        {
            // Save settings to EEPROM.
            if (e->error == 0)
            {
                saveSettings();
            }
            else
            {
                // Load default settings if there is an error.
                loadSettings();
            }
        }
    }
}

unsigned char svr_isTarget(
    dlmsSettings *settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    GXTRACE(("svr_isTarget."), NULL);
    objectArray objects;
    oa_init(&objects);
    unsigned char ret = 0;
    uint16_t pos;
    gxObject *tmp[6];
    oa_attach(&objects, tmp, sizeof(tmp) / sizeof(tmp[0]));
    objects.size = 0;
    if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, &objects) == 0)
    {
        gxAssociationLogicalName *a;
        for (pos = 0; pos != objects.size; ++pos)
        {
            if (oa_getByIndex(&objects, pos, (gxObject **)&a) == 0)
            {
                if (a->clientSAP == clientAddress)
                {
                    ret = 1;
                    switch (a->authenticationMechanismName.mechanismId)
                    {
                    case DLMS_AUTHENTICATION_NONE:
                        // Client connects without authentication.
                        GXTRACE(("Connecting without authentication."), NULL);
                        break;
                    case DLMS_AUTHENTICATION_LOW:
                        // Client connects using low authentication.
                        GXTRACE(("Connecting using Low authentication."), NULL);
                        break;
                    default:
                        // Client connects using High authentication.
                        GXTRACE(("Connecting using High authentication."), NULL);
                        break;
                    }
                    settings->proposedConformance = a->xDLMSContextInfo.conformance;
                    settings->expectedClientSystemTitle = NULL;
                    // Set Invocation counter value.
                    settings->expectedInvocationCounter = NULL;
                    // Client can establish a ciphered connection only with Security Suite 1.
                    settings->expectedSecuritySuite = 0;
                    // Security policy is not defined by default. Client can connect using any security policy.
                    settings->expectedSecurityPolicy = 0xFF;
                    if (a->securitySetup != NULL)
                    {
                        // Set expected client system title. If this is set only client that is using expected client system title can connect to the meter.
                        if (a->securitySetup->clientSystemTitle.size == 8)
                        {
                            settings->expectedClientSystemTitle = a->securitySetup->clientSystemTitle.data;
                        }
                        // GMac authentication uses innocation counter.
                        if (a->securitySetup == &securitySetupHighGMac)
                        {
                            // Set invocation counter value. If this is set client's invocation counter must match with server IC.
                            settings->expectedInvocationCounter = &securitySetupHighGMac.minimumInvocationCounter;
                        }

                        // Set security suite that client must use.
                        settings->expectedSecuritySuite = a->securitySetup->securitySuite;
                        // Set security policy that client must use if it is set.
                        if (a->securitySetup->securityPolicy != 0)
                        {
                            settings->expectedSecurityPolicy = a->securitySetup->securityPolicy;
                        }
                    }
                    break;
                }
            }
        }
    }
    if (ret == 0)
    {
        GXTRACE_INT(("Invalid authentication level."), clientAddress);
        // Authentication is now allowed. Meter is quiet and doesn't return an error.
    }
    else
    {
        // If address is not broadcast or serial number.
        // Remove logical address from the server address.
        unsigned char broadcast = (serverAddress & 0xFFFF) == 0x3FFF || (serverAddress & 0xFF) == 0x7F;
        if (!(broadcast ||
              (serverAddress & 0x3FFF) == SERIAL_NUMBER % 10000 + 1000))
        {
            ret = 0;
            // Find address from the SAP table.
            gxSapAssignment *sap;
            objects.size = 0;
            if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, &objects) == 0)
            {
                gxSapItem *it;
                uint16_t sapIndex, pos;
                for (sapIndex = 0; sapIndex != objects.size; ++sapIndex)
                {
                    if (oa_getByIndex(&objects, sapIndex, (gxObject **)&sap) == 0)
                    {
                        for (pos = 0; pos != sap->sapAssignmentList.size; ++pos)
                        {
                            if (arr_getByIndex(&sap->sapAssignmentList, pos, (void **)&it) == 0)
                            {
                                // Check server address with one byte.
                                if (((serverAddress & 0xFFFFFF00) == 0 && (serverAddress & 0x7F) == it->id) ||
                                    // Check server address with two bytes.
                                    ((serverAddress & 0xFFFF0000) == 0 && (serverAddress & 0x7FFF) == it->id))
                                {
                                    ret = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            oa_empty(&objects);
        }
        // Set serial number as meter address if broadcast is used.
        if (broadcast)
        {
            settings->serverAddress = SERIAL_NUMBER % 10000 + 1000;
        }
        if (ret == 0)
        {
            GXTRACE_INT(("Invalid server address"), serverAddress);
        }
    }
    return ret;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings *settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer *password)
{
    GXTRACE(("svr_validateAuthentication"), NULL);
    if (authentication == DLMS_AUTHENTICATION_NONE)
    {
        // Uncomment this if authentication is always required.
        // return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
        return DLMS_SOURCE_DIAGNOSTIC_NONE;
    }
    // Check Low Level security..
    if (authentication == DLMS_AUTHENTICATION_LOW)
    {
        if (bb_compare(password, associationLow.secret.data, associationLow.secret.size) == 0)
        {
            GXTRACE(("Invalid low level password."), (const char *)associationLow.secret.data);
            return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
        }
    }
    // Hith authentication levels are check on phase two.
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

// Get attribute access level for profile generic.
DLMS_ACCESS_MODE getProfileGenericAttributeAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index)
{
    // Only read is allowed for event log.
    if (obj == BASE(eventLog))
    {
        return DLMS_ACCESS_MODE_READ;
    }
    // Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 3: // captureObjects.
            return DLMS_ACCESS_MODE_READ_WRITE;
        case 4: // capturePeriod
            return DLMS_ACCESS_MODE_READ_WRITE;
        case 8: // Profile entries.
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for Push Setup.
DLMS_ACCESS_MODE getPushSetupAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    // Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 2: // Object List
        case 3: // Destination And Method
        case 4: // Communication Window
        case 5: // Randomisation Start Interval
        case 6: // Number Of Retries
        case 7: // Repetition Delay
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for Disconnect Control.
DLMS_ACCESS_MODE getDisconnectControlAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for register schedule.
DLMS_ACCESS_MODE getActionSchduleAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    // Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 4: // Execution time.
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for register.
DLMS_ACCESS_MODE getRegisterAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for data objects.
DLMS_ACCESS_MODE getDataAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for script table.
DLMS_ACCESS_MODE getScriptTableAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for IEC HDLS setup.
DLMS_ACCESS_MODE getHdlcSetupAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    // Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 2: // Communication speed.
        case 7:
        case 8:
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for association LN.
DLMS_ACCESS_MODE getAssociationAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    // If secret
    if (settings->authentication == DLMS_AUTHENTICATION_LOW && index == 7)
    {
        return DLMS_ACCESS_MODE_READ_WRITE;
    }
    return DLMS_ACCESS_MODE_READ;
}

// Get attribute access level for security setup.
DLMS_ACCESS_MODE getSecuritySetupAttributeAccess(
    dlmsSettings *settings,
    unsigned char index)
{
    // Only client system title is writable.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW && index == 4)
    {
        return DLMS_ACCESS_MODE_READ_WRITE;
    }
    return DLMS_ACCESS_MODE_READ;
}

/**
 * Get attribute access level.
 */
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index)
{
    GXTRACE("svr_getAttributeAccess", NULL);
    // Only read is allowed if authentication is not used.
    if (index == 1 || settings->authentication == DLMS_AUTHENTICATION_NONE)
    {
        return DLMS_ACCESS_MODE_READ;
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        return getAssociationAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
    {
        return getProfileGenericAttributeAccess(settings, obj, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP)
    {
        return getPushSetupAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
    {
        return getDisconnectControlAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
    {
        return getDisconnectControlAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_ACTION_SCHEDULE)
    {
        return getActionSchduleAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_SCRIPT_TABLE)
    {
        return getScriptTableAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER)
    {
        return getRegisterAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_DATA)
    {
        return getDataAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_IEC_HDLC_SETUP)
    {
        return getHdlcSetupAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_SECURITY_SETUP)
    {
        return getSecuritySetupAttributeAccess(settings, index);
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
// Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
    dlmsServerSettings *settings)
{
    if (settings->base.connected == DLMS_CONNECTION_STATE_HDLC)
    {
        GXTRACE(("svr_connected to HDLC level."), NULL);
    }
    else if ((settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
    {
        GXTRACE(("svr_connected DLMS level."), NULL);
    }
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
    GXTRACE(("svr_invalidConnection"), NULL);
    updateState(GURUX_EVENT_CODES_WRONG_PASSWORD);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Client has close the connection.
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
    dlmsServerSettings *settings)
{
    GXTRACE(("svr_disconnected"), NULL);
    if (settings->base.cipher.security != 0 && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
    {
        // Save Invocation counter value when connection is closed.
        // saveSettings();
    }
    return 0;
}

void svr_preGet(
    dlmsSettings *settings,
    gxValueEventCollection *args)
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
    dlmsSettings *settings,
    gxValueEventCollection *args)
{
}