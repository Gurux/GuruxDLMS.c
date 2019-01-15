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

#include "connection.h"
#include <stdlib.h> // malloc and free needs this or error is generated.
#include <stdio.h>
#include <string.h> // string function definitions
#include <time.h>   // time calls

//Initialize connection buffers.
void con_initializeBuffers(connection* connection, int size)
{
    if (size == 0)
    {
        bb_clear(&connection->data);
    }
    else
    {
        bb_capacity(&connection->data, size);
    }
}

//Initialize connection settings.
void con_init(connection* con, GX_TRACE_LEVEL trace)
{
    con->trace = trace;
    //Reply wait time is 5 seconds.
    con->waitTime = 5000;
    bb_init(&con->data);
    bb_capacity(&con->data, 500);
}

//Close connection..
void con_close(connection* con)
{
    bb_clear(&con->data);
    con_initializeBuffers(con, 0);
}
