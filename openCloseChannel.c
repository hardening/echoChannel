/**
 *  OpenClose channel
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

/** @brief channel configuration */
typedef struct {
    BOOL dynamic;
    DWORD sleepDelay;
    char *channelName;
} OpenCloseConfig;


/**
 * @brief adjust the configuration from parameters
 *
 * @param argc number of arguments
 * @param argv arguments array
 * @param config target configuration
 * @return if the function succeeded
 */
static BOOL parseParameters(int argc, char **argv, OpenCloseConfig *config) {
    int i;

    config->dynamic = TRUE;
    config->sleepDelay = 10;
    config->channelName = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-static") == 0) {
            config->dynamic = FALSE;
        }
        else if(strstr(argv[i], "--name=") == argv[i]) {
            char *str = argv[i] + strlen("--name=");
            config->channelName = str;
        }
        else if(strstr(argv[i], "--sleepDelay=") == argv[i]) {
            char *str = argv[i] + strlen("--sleepDelay=");
            config->sleepDelay = atol(str);
        }
        else
            return FALSE;
    }

    if (!config->channelName || !strlen(config->channelName)) {
        printf("ERROR: missing or empty channel name\n");
        return FALSE;
    }
    return TRUE;
}

static void printHelp(const char *argv0) {
    printf("usage: %s --name=<name> [-static] [--sleepDelay=<milli>]\n", argv0);
    printf("\t-static: opens the channel as a static channel(default: no)\n");
    printf("\t--name=<name>: channel name\n");
    printf("\t--sleepDelay=<delay>: wait <delay> seconds before closing the channel (default: 10s)\n");
}

typedef struct {
    HANDLE hChannel;
} OpenCloseState;

int main(int argc, char **argv) {
    WTSApi wtsapi;
    OpenCloseConfig config;
    OpenCloseState state = { 0 };
    DWORD flags;

    if (!parseParameters(argc, argv, &config)) {
        printHelp(argv[0]);
        return -2;
    }

    if (!loadWtsApi(&wtsapi)) {
        fprintf(stderr, "unable to load wtsapi32.dll\n");
        return -2;
    }

    if (config.dynamic)
        flags = WTS_CHANNEL_OPTION_DYNAMIC;

    state.hChannel = wtsapi.WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, config.channelName, flags);
    if (!state.hChannel) {
        fprintf(stderr, "unable to open the channel (GetLastError() = %ld)\n", GetLastError());
        return -3;
    }

    Sleep(config.sleepDelay * 1000);

    wtsapi.WTSVirtualChannelClose(state.hChannel);
    return 0;
}
