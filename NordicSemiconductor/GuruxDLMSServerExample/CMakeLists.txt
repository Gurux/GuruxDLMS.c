#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

cmake_minimum_required(VERSION 3.8.2)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

target_sources(app PRIVATE dlms/include/gxignore.h src/main.c dlms/src/apdu.c dlms/src/bitarray.c dlms/src/bytebuffer.c dlms/src/ciphering.c dlms/src/converters.c dlms/src/cosem.c dlms/src/datainfo.c dlms/src/date.c dlms/src/dlms.c dlms/src/dlmsSettings.c dlms/src/gxaes.c dlms/src/gxarray.c dlms/src/gxget.c dlms/src/gxinvoke.c dlms/src/gxkey.c dlms/src/gxmd5.c dlms/src/gxobjects.c dlms/src/gxset.c dlms/src/gxsha1.c dlms/src/gxsha256.c dlms/src/gxvalueeventargs.c dlms/src/helpers.c dlms/src/message.c dlms/src/notify.c dlms/src/objectarray.c dlms/src/parameters.c dlms/src/replydata.c dlms/src/server.c dlms/src/variant.c)
