#include <stdio.h>
#include <wtypes.h>
#include <wtsapi32.h>

#ifndef WTS_CHANNEL_OPTION_DYNAMIC
#define WTS_CHANNEL_OPTION_DYNAMIC 0x00000001
#endif


typedef HANDLE (*WTSVirtualChannelOpenExFn)(DWORD SessionId, LPSTR pVirtualName, DWORD flags);
typedef BOOL (*WTSVirtualChannelWriteFn)(HANDLE hChannelHandle, PCHAR  Buffer, ULONG  Length, PULONG pBytesWritten);
typedef BOOL (*WTSVirtualChannelCloseFn)(HANDLE hChannelHandle);

/** @brief functions and module for wtsapi32 */
typedef struct {
    HANDLE dll;
    WTSVirtualChannelOpenExFn WTSVirtualChannelOpenEx;
    WTSVirtualChannelWriteFn WTSVirtualChannelWrite;
    WTSVirtualChannelCloseFn WTSVirtualChannelClose;
} WTSApi;

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
    LOAD_FUNCTION(WTSVirtualChannelClose);

#undef LOAD_FUNCTION

    return TRUE;
error:
    FreeLibrary(wtsapi->dll);
    return FALSE;
}

/** @brief channel configuration */
typedef struct {
    BOOL dynamic;
    ULONG packetsLimit;
    DWORD sleepDelay;
} EchoConfig;

/**
 * @brief adjust the configuration from parameters
 *
 * @param argc number of arguments
 * @param argv arguments array
 * @param config target configuration
 * @return if the function succeeded
 */
static BOOL parseParameters(int argc, char **argv, EchoConfig *config) {
    int i;

    config->dynamic = TRUE;
    config->packetsLimit = ULONG_MAX;
    config->sleepDelay = 100;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-static") == 0) {
            config->dynamic = FALSE;
        }
        else if(strstr(argv[i], "--packets=") == argv[i]) {
            char *str = argv[i] + strlen("--packets=");
            config->packetsLimit = atol(str);
        }
        else if(strstr(argv[i], "--sleepDelay=") == argv[i]) {
            char *str = argv[i] + strlen("--sleepDelay=");
            config->sleepDelay = atol(str);
        }
        else
            return FALSE;
    }

    return TRUE;
}

static void printHelp(const char *argv0) {
    printf("usage: %s [-static] [-doReads] [--packets=<nb>] [--sleepDelay=<milli>]\n", argv0);
    printf("\t-static: opens the echo channel as static channel(default: no)\n");
    printf("\t--packets=<nb>: sends <nb> packets and then close the channel (default: infinite)\n");
    printf("\t--sleepDelay=<milli>: wait <milli> milliseconds between each packet sending (default: 100ms)\n");
}


int main(int argc, char **argv) {
    ULONG i;
    BYTE payload = 0;
    WTSApi wtsapi;
    EchoConfig config;
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

    HANDLE hChannel = wtsapi.WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, "ECHO", flags);

    for (i = 0; i < config.packetsLimit; i++) {
        ULONG ulBytesWritten;

        BOOL bSuccess = wtsapi.WTSVirtualChannelWrite(hChannel, (PCHAR)&payload, 1, &ulBytesWritten);
        if (!bSuccess) {
            fprintf(stderr, "WTSVirtualChannelWrite failed (GetLastError() = %ld)\n",
                    GetLastError());
            break;
        }
        payload++;

        if (config.sleepDelay)
            Sleep(config.sleepDelay);
    }

    wtsapi.WTSVirtualChannelClose(hChannel);
	return 0;
}
