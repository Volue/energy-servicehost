package com.libtase2.common;

public interface LogHandlerEx {
    void logEvent(LogLevel logLevel, LogSource source, Endpoint endpoint, EndpointConnection peer, String message);
}
