cmake_minimum_required(VERSION 3.12)
project(dlms_c VERSION 1.0)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -fPIC")
add_compile_definitions(DLMS_USE_EPOCH_TIME)

add_library(dlms STATIC
        src/apdu.c
        src/dlms.c
        src/gxserializer.c
        src/notify.c
        src/bitarray.c
        src/dlmsSettings.c
        src/gxset.c
        src/objectarray.c
        src/bytebuffer.c
        src/gxaes.c
        src/gxsetignoremalloc.c
        src/parameters.c
        src/ciphering.c
        src/gxarray.c
        src/gxsetmalloc.c
        src/replydata.c
        src/client.c
        src/gxget.c
        src/gxsha1.c
        src/server.c
        src/converters.c
        src/gxinvoke.c
        src/gxsha256.c
        src/serverevents.c
        src/cosem.c
        src/gxkey.c
        src/gxvalueeventargs.c
        src/variant.c
        src/datainfo.c
        src/gxmd5.c
        src/helpers.c
        src/date.c
        src/gxobjects.c
        src/message.c
        )

target_include_directories(dlms PUBLIC include)
