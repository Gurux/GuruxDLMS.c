cmake_minimum_required(VERSION 3.20)
project(dlms_c VERSION 1.0)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -fPIC")

add_compile_definitions(DLMS_USE_EPOCH_TIME)

add_executable(dlms_server1 src/connection.c
        src/exampleserver.c
        src/getopt.c
        src/main.c)

link_directories(../development)
target_link_libraries(dlms_server1 PUBLIC dlms m)
