using System.Runtime.InteropServices;
using System;

namespace TASE2.Library.Common
{
    /// <summary>
    /// Log level for library log messages
    /// </summary>
    public enum LogLevel
    {
        /// <summary>
        /// shows the most information (useful for debugging applications)
        /// </summary>
        DEBUG = 1,

        /// <summary>
        ///  show informational messages (useful to trace communication problems)
        /// </summary>
        INFO = 2,

        /// <summary>
        /// shows only errors and warning message that indicate wrong configuration
        /// </summary>
        WARNING = 3,

        /// <summary>
        /// show critical problems and communication errors
        /// </summary>
        ERROR = 4,

        /// <summary>
        /// don't show any log messages
        /// </summary>
        NONE = 5
    }

    /// <summary>
    /// Log source for library log messages
    /// </summary>
    public enum LogSource
    {
        /// <summary>
        /// log message from TASE.2/ICCP layer
        /// </summary>
        ICCP = 1,

        /// <summary>
        /// log message from MMS layer
        /// </summary>
        MMS = 2,

        /// <summary>
        /// log message from TLS layer
        /// </summary>
        TLS = 3,

        /// <summary>
        /// log message from TCP layer
        /// </summary>
        TCP = 4,

        /// <summary>
        /// log messages from lower ISO layers (presentation, session, ACSE)
        /// </summary>
        ISO = 5
    }

    /// <summary>
    /// Handler for library log messages
    /// </summary>
    /// <param name="parameter">User provided context parameter</param>
    /// <param name="controlPoint">the target device of the event</param>
    public delegate void LogHandler(LogLevel level, string message);

    public delegate void LogHandlerEx(LogLevel level, LogSource source, Endpoint endpoint, EndpointConnection peer, string message);

    /// <summary>
    /// libtase2.NET generic library functions
    /// </summary>
    public static class Tase2Library
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Library_setLogLevel(int level);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void InternalLogHandlerEx(int level, int source, IntPtr endpoint, IntPtr peer, string message);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Library_setLogFunctionEx(InternalLogHandlerEx level);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Library_getVersionString();

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Library_getMajorVersion();

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Library_getMinorVersion();

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Library_getPatchVersion();

        /// <summary>
        /// Sets the log level.
        /// </summary>
        /// <param name="level">log level</param>
        public static void SetLogLevel(LogLevel level)
        {
            Tase2_Library_setLogLevel((int)level);
        }

        private static InternalLogHandlerEx logHandlerRef = null;
        private static LogHandler logHandler = null;
        private static LogHandlerEx logHandlerEx = null;

        private static void internalLogHandler(int level, int source, IntPtr endpoint, IntPtr peer, string message)
        {
            if (logHandler != null)
                logHandler((LogLevel)level, message);

            if (logHandlerEx != null)
            {
                EndpointConnection epCon = null;
                Endpoint ep = null;

                if (peer != IntPtr.Zero)
                {
                    epCon = new EndpointConnection(peer);
                }

                if (endpoint != IntPtr.Zero)
                {
                    ep = Endpoint.LookupKnownEndpointOrCreate(endpoint);
                }

                logHandlerEx((LogLevel)level, (LogSource)source, ep, epCon, message);
            }
        }

        /// <summary>
        /// Sets the log handler function
        /// </summary>
        /// <param name="handler">log handler callback function</param>
        public static void SetLogHandler(LogHandler handler)
        {
            if (logHandlerRef == null)
                logHandlerRef = new InternalLogHandlerEx(internalLogHandler);

            Tase2_Library_setLogFunctionEx(logHandlerRef);

            logHandler = handler;
        }

        public static void SetLogHandler(LogHandlerEx handler)
        {
            if (logHandlerRef == null)
                logHandlerRef = new InternalLogHandlerEx(internalLogHandler);

            Tase2_Library_setLogFunctionEx(logHandlerRef);

            logHandlerEx = handler;
        }

        /// <summary>
        /// Get the version string of the library (e.g. "1.2.3")
        /// </summary>
        /// <returns>The version string (format is MAJOR.MINOR.PATCH)</returns>
        public static string GetVersionString()
        {
            return Marshal.PtrToStringAnsi(Tase2_Library_getVersionString());
        }

        /// <summary>
        /// Gets the major version.
        /// </summary>
        /// <returns>The major version.</returns>
        public static int GetMajorVersion()
        {
            return Tase2_Library_getMajorVersion();
        }

        /// <summary>
        /// Gets the minor version.
        /// </summary>
        /// <returns>The minor version.</returns>
        public static int GetMinorVersion()
        {
            return Tase2_Library_getMinorVersion();
        }

        /// <summary>
        /// Gets the patch version.
        /// </summary>
        /// <returns>The patch version.</returns>
        public static int GetPatchVersion()
        {
            return Tase2_Library_getPatchVersion();
        }
    }

}
