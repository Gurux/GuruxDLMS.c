/* *****************************************************************
*
* Copyright 2016 Microsoft
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef GETOPT_H__
#define GETOPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)//Windows
#include <tchar.h>

    extern char *optarg;
    extern int optind;
    int getopt(int argc, _TCHAR *const argv[], const char *optstring);
#endif


#ifdef __cplusplus
}
#endif

#endif
