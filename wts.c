/**
 *  WTSapi routines
 *
 * Copyright 2022 David Fort <contact@hardening-consulting.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "wts.h"

/**
 * Loads wtsapi32 and grab function
 *
 * @param wtsapi target structure that will store the function pointers
 * @return if the operation was successful
 */
BOOL loadWtsApi(WTSApi* wtsapi) {
#define LOAD_FUNCTION(V) \
    wtsapi->V = (V##Fn)GetProcAddress(wtsapi->dll, #V); \
    if (!wtsapi->V) { \
        fprintf(stderr, "unable to retrieve function %s in wtsapi32.dll\n", #V); \
        goto error; \
    }

    wtsapi->dll = LoadLibrary("wtsapi32.dll");
    if (!wtsapi->dll)
        return FALSE;

    LOAD_FUNCTION(WTSVirtualChannelOpenEx);
    LOAD_FUNCTION(WTSVirtualChannelWrite);
    LOAD_FUNCTION(WTSVirtualChannelRead);
    LOAD_FUNCTION(WTSVirtualChannelQuery);
    LOAD_FUNCTION(WTSVirtualChannelClose);

#undef LOAD_FUNCTION

    return TRUE;
error:
    FreeLibrary(wtsapi->dll);
    return FALSE;
}
