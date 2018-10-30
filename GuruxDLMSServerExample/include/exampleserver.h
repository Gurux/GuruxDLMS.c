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

#pragma once

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Winsock.h> //Add support for sockets	
#endif

#include "../../development/include/server.h"

#if defined(_WIN32) | defined(_WIN64) || defined(__linux__)
#include "connection.h"
char DATAFILE[FILENAME_MAX];
char IMAGEFILE[FILENAME_MAX];

int svr_start(
    connection *con,
    unsigned short port);

#endif //defined(_WIN32) | defined(_WIN64) || defined(__linux__)

int svr_InitObjects(
    dlmsServerSettings *settings);


/**
* Check is data sent to this server.
*
* @param serverAddress
*            Server address.
* @param clientAddress
*            Client address.
* @return True, if data is sent to this server.
*/
unsigned char svr_isTarget(
    dlmsSettings *settings, 
    unsigned long int serverAddress,
    unsigned long clientAddress);

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index);

/**
* Get method access level.
*/
extern DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index);

/**
* called when client makes connection to the server.
*/
int svr_connected(
    dlmsServerSettings *settings);

/**
    * Client has try to made invalid connection. Password is incorrect.
    *
    * @param connectionInfo
    *            Connection information.
    */
int svr_invalidConnection(dlmsServerSettings *settings);

/**
* called when client clses connection to the server.
*/
int svr_disconnected(
    dlmsServerSettings *settings);

/**
    * Read selected item(s).
    *
    * @param args
    *            Handled read requests.
    */
void svr_preRead(
    dlmsSettings* settings,
    gxValueEventCollection* args);

/**
    * Write selected item(s).
    *
    * @param args
    *            Handled write requests.
    */
void svr_preWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args);

/**
     * Action is occurred.
     *
     * @param args
     *            Handled action requests.
     */
void svr_preAction(
    dlmsSettings* settings,
    gxValueEventCollection* args);

/**
* Read selected item(s).
*
* @param args
*            Handled read requests.
*/
void svr_postRead(
    dlmsSettings* settings,
    gxValueEventCollection* args);

/**
* Write selected item(s).
*
* @param args
*            Handled write requests.
*/
void svr_postWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args);

/**
* Action is occurred.
*
* @param args
*            Handled action requests.
*/
void svr_postAction(
    dlmsSettings* settings,
    gxValueEventCollection* args);

/**
    * Check whether the authentication and password are correct.
    *
    * @param authentication
    *            Authentication level.
    * @param password
    *            Password.
    * @return Source diagnostic.
    */
DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings* settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer* password);

/**
     * Find object.
     *
     * @param objectType
     *            Object type.
     * @param sn
     *            Short Name. In Logical name referencing this is not used.
     * @param ln
     *            Logical Name. In Short Name referencing this is not used.
     * @return Found object or NULL if object is not found.
     */
int svr_findObject(
    dlmsSettings* settings,
    DLMS_OBJECT_TYPE objectType,
    int sn,
    unsigned char* ln,
    gxValueEventArg *e);

void svr_preGet(dlmsSettings* settings,
    gxValueEventCollection* args);

void svr_postGet(dlmsSettings* settings,
    gxValueEventCollection* args);


/**
* This is reserved for future use.
*
* @param args
*            Handled data type requests.
*/
void svr_getDataType(
    dlmsSettings* settings,
    gxValueEventCollection* args);