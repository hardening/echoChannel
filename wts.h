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

#ifndef WTS_H_
#define WTS_H_

#include <wtypes.h>
#include <wtsapi32.h>

#ifndef WTS_CHANNEL_OPTION_DYNAMIC
#define WTS_CHANNEL_OPTION_DYNAMIC 0x00000001
#endif


typedef HANDLE (*WTSVirtualChannelOpenExFn)(DWORD SessionId, LPSTR pVirtualName, DWORD flags);
typedef BOOL (*WTSVirtualChannelWriteFn)(HANDLE hChannelHandle, PCHAR  Buffer, ULONG  Length, PULONG pBytesWritten);
typedef BOOL (*WTSVirtualChannelReadFn)(HANDLE hChannelHandle, ULONG TimeOut, PCHAR  Buffer, ULONG  BufferSize, PULONG pBytesRead);
typedef BOOL (*WTSVirtualChannelQueryFn)(HANDLE hChannelHandle, WTS_VIRTUAL_CLASS unnamedParam2, PVOID *ppBuffer, DWORD *pBytesReturned);
typedef BOOL (*WTSVirtualChannelCloseFn)(HANDLE hChannelHandle);

/** @brief functions and module for wtsapi32 */
typedef struct {
    HANDLE dll;
    WTSVirtualChannelOpenExFn WTSVirtualChannelOpenEx;
    WTSVirtualChannelWriteFn WTSVirtualChannelWrite;
    WTSVirtualChannelReadFn WTSVirtualChannelRead;
    WTSVirtualChannelQueryFn WTSVirtualChannelQuery;
    WTSVirtualChannelCloseFn WTSVirtualChannelClose;
} WTSApi;

#ifdef __cplusplus
extern "C" {
#endif

BOOL loadWtsApi(WTSApi* wtsapi);

#ifdef __cplusplus
}
#endif

#endif /* WTS_H_ */
