/*
 *  tase2_log.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2018 MZ Automation GmbH
 *
 *  All rights reserved.
 */


#include "tase2_common_internal.h"

#include <stdarg.h>
#include <stdio.h>
#include "lib_memory.h"

static Tase2_Log_Function logFunc = NULL;

static Tase2_Log_FunctionEx logFuncEx = NULL;

static Tase2_LogLevel logLevel = TASE2_LOG_INFO;

typedef void
(*libmms_Log_Function) (void* parameter, int logSource, int logLevel, const char* message);

LIBMMS_API void
libmms_setLogFunction(libmms_Log_Function logFunction, void* parameter);

void
Tase2_Library_setLogLevel(Tase2_LogLevel level)
{
    logLevel = level;
}

static void
memoryExceptionHandler(void* parameter)
{
    tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Out of memory!");
}

static void
libmmsLogFunction(void* parameter, int logSource, int logLevel, const char* message)
{
    tase2_log((Tase2_LogLevel)logLevel, (Tase2_LogSource)logSource, NULL, NULL, message);
}

static void
setLowLayerLogHandlers()
{
    Memory_installExceptionHandler(memoryExceptionHandler, NULL);
    libmms_setLogFunction(libmmsLogFunction, NULL);
}

void
Tase2_Library_setLogFunction(Tase2_Log_Function logFunction)
{
    setLowLayerLogHandlers();
    logFunc = logFunction;
}

void
Tase2_Library_setLogFunctionEx(Tase2_Log_FunctionEx logFunction)
{
    setLowLayerLogHandlers();
	logFuncEx = logFunction;
}

#ifdef TASE2_DEBUG_OUTPUT

void
tase2_log(Tase2_LogLevel level, Tase2_LogSource source, Tase2_Endpoint endpoint, Tase2_Endpoint_Connection peer,  const char* fmt, ...)
{
    char msg[TASE2_MAX_LOG_MESSAGE_SIZE];

    if (level >= logLevel) {
        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(msg, TASE2_MAX_LOG_MESSAGE_SIZE, fmt, argptr);
        va_end(argptr);

		if (logFuncEx) {
			logFuncEx(level, source, endpoint, peer, msg);
		}
        else if (logFunc) {
            logFunc(level, msg);
        }
        else {
            const char* levelStr = "UNKNOWN";

            switch (level) {
            case TASE2_LOG_DEBUG:
                levelStr = "DEBUG";
                break;
            case TASE2_LOG_INFO:
                levelStr = "INFO";
                break;
            case TASE2_LOG_WARNING:
                levelStr = "WARNING";
                break;
            case TASE2_LOG_ERROR:
                levelStr = "ERROR";
                break;
            default:
                break;
            }

            const char* sourceStr = "UNKNOWN";

            switch (source) {
            case TASE2_LOG_SOURCE_ICCP:
                sourceStr = "ICCP";
                break;
            case TASE2_LOG_SOURCE_MMS:
                sourceStr = "MMS";
                break;
            case TASE2_LOG_SOURCE_TLS:
                sourceStr = "TLS";
                break;
            case TASE2_LOG_SOURCE_TCP:
                sourceStr = "TCP";
                break;
            case TASE2_LOG_SOURCE_ISO_LAYERS:
                sourceStr = "ISO";
                break;
            }

            printf("[%s]: %s\n", levelStr, msg);
        }

    }
}

#endif /* TASE2_DEBUG_OUTPUT */
