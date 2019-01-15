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

#ifndef MEDIA_H
#define MEDIA_H

#include "include/bytebuffer.h"
#include "include/dlmssettings.h"

static const unsigned int RECEIVE_BUFFER_SIZE = 200;

#ifdef  __cplusplus
extern "C" {
#endif

    typedef struct
    {
  		struct io_descriptor *io;
		unsigned short waitTime;
		//Is trace used.
        GX_TRACE_LEVEL trace;
        //Received data.
        gxByteBuffer data;
        dlmsSettings settings;
    } connection;

    void con_initializeBuffers(
        connection* connection,
        int size);

    //Initialize connection settings.
    void con_init(
        connection* con,
        GX_TRACE_LEVEL trace);

    //Close connection..
    void con_close(connection* con);

#ifdef  __cplusplus
}
#endif

#endif //MEDIA_H
