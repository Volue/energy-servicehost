package com.libtase2.common;

import java.util.HashMap;
import java.util.Map;

import com.sun.jna.Callback;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * Generic library functions
 */
public class LibTase2 {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        void Tase2_Library_setLogLevel(int logLevel);

        interface Tase2_Log_FunctionEx extends Callback {
            void invoke(int logLevel, int logSource, Pointer endpointPtr, Pointer peerPtr, String message);
        }

        void Tase2_Library_setLogFunctionEx(Tase2_Log_FunctionEx logFunction);

        String Tase2_Library_getVersionString();
    }

    private static Map<String, ?> getOptions() {
        Map<String, Object> result = new HashMap<String, Object>();

        DefaultTypeMapper typeMapper = new DefaultTypeMapper();

        typeMapper.addTypeConverter(Boolean.class, new TypeConverter() {
            @Override
            public Object fromNative(Object nativeValue, FromNativeContext context) {
                return ((Byte) nativeValue) != 0;
            }

            @Override
            public Class<?> nativeType() {
                return byte.class;
            }

            @Override
            public Object toNative(Object value, ToNativeContext context) {
                if (value.getClass() == Boolean.class) {
                    value = (Boolean) value;
                }
                if ((Boolean) value) {
                    return (byte) 1;
                } else {
                    return 0;
                }
            }
        });

        result.put(Library.OPTION_TYPE_MAPPER, typeMapper);
        return result;
    }

    private static LogHandler logHandler = null;
    private static LogHandlerEx logHandlerEx = null;
    private static com.libtase2.common.LibTase2.NativeLibTase2.Tase2_Log_FunctionEx logFunction = null;

    /**
     * Set the library log level.
     *
     * NOTE: Default log level is LogLevel.INFO
     *
     * @param level the log level to apply
     */
    public static void setLogLevel(LogLevel level) {
        NativeLibTase2.INSTANCE.Tase2_Library_setLogLevel(level.getValue());
    }

    private static class InternalEndpoint extends Endpoint {
        protected InternalEndpoint(Pointer nativeInstance) {
            super(nativeInstance);
        }

        protected static Endpoint getManagedInstance(Pointer ptr) {
            return Endpoint.getManagedInstance(ptr);
        }
    }

    private static void installNativeLogFunction() {
        if (logFunction == null) {
            logFunction = new com.libtase2.common.LibTase2.NativeLibTase2.Tase2_Log_FunctionEx() {

                @Override
                public void invoke(int logLevel, int logSource, Pointer endpointPtr, Pointer peerPtr, String message) {
                    if (logHandler != null) {

                        LogLevel level = LogLevel.fromValue(logLevel);

                        logHandler.logEvent(level, message);
                    }

                    if (logHandlerEx != null) {
                        LogLevel level = LogLevel.fromValue(logLevel);
                        LogSource source = LogSource.fromValue(logSource);

                        Endpoint endpoint = null;
                        EndpointConnection peer = null;

                        if (endpointPtr != Pointer.NULL)
                            endpoint = InternalEndpoint.getManagedInstance(endpointPtr);

                        if (peerPtr != Pointer.NULL)
                            peer = new EndpointConnection(peerPtr);

                        logHandlerEx.logEvent(level, source, endpoint, null, message);
                    }

                }
            };

            NativeLibTase2.INSTANCE.Tase2_Library_setLogFunctionEx(logFunction);
        }
    }

    /**
     * Set a user provided log message handler.
     *
     * @param handler user provided log handler or null.
     */
    public static void setLogHandler(LogHandler handler) {
        logHandler = handler;

        installNativeLogFunction();
    }

    /**
     * Set a user provided log messages handler (use extended log handler)
     * 
     * @param handler user provided log handler or null.
     */
    public static void setLogHandler(LogHandlerEx handler) {
        logHandlerEx = handler;

        installNativeLogFunction();
    }

    /**
     * Log message with provided log level (intended for library internal use)
     * 
     * @param logLevel
     * @param message
     */
    public static void log(LogLevel logLevel, String message) {
        if (logHandler != null) {
            logHandler.logEvent(logLevel, message);
        }
    }

    /**
     * Get the version string of the library (e.g. "1.2.3")
     *
     * @return version string in the format MAJOR.MINOR.PATCH
     */
    public static String getVersion() {
        return NativeLibTase2.INSTANCE.Tase2_Library_getVersionString();
    }

}
