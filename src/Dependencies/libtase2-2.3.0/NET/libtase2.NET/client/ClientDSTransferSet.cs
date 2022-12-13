using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using TASE2.Library.Common;

namespace TASE2.Library.Client
{
    /// <summary>
    /// Client side representation of a DS transfer set.
    /// </summary>
    /// The properties and getter and setter functions access a locally buffered copy of the
    /// transfer set attributes. To read and write the actual values the
    /// <see cref="ReadValues"/> and <see cref="SetValues"/> methods have to be used.
    /// For direct write access to the attributes the "Write..." methods can be used.
    public class ClientDSTransferSet : IDisposable
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_destroy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_readValues(IntPtr self, IntPtr client);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeValues(IntPtr self, IntPtr client);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDSTransferSet_getDomain(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDSTransferSet_getName(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDSTransferSet_getDataSetDomain(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDSTransferSet_getDataSetName(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_getStartTime(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_getInterval(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_getTLE(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_getBufferTime(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_getIntegrityCheck(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_getDSConditionsRequested(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_ClientDSTransferSet_getBlockData(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_ClientDSTransferSet_getCritical(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern short Tase2_ClientDSTransferSet_getEventCodeRequested(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_ClientDSTransferSet_getRBE(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_ClientDSTransferSet_getAllChangesReported(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_ClientDSTransferSet_getStatus(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setDataSetName(IntPtr self, string domainId, string dataSetId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setDataSet(IntPtr self, IntPtr dataSet);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setStartTime(IntPtr self, int startTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setInterval(IntPtr self, int interval);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setTLE(IntPtr self, int tle);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setBufferTime(IntPtr self, int bufferTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setIntegrityCheck(IntPtr self, int integrityCheck);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setDSConditionsRequested(IntPtr self, int dsConditionsRequested);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setBlockData(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool blockData);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setCritical(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool critical);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setRBE(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool rbe);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setAllChangesReported(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool allChangesReported);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDSTransferSet_setStatus(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool status);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeDataSetName(IntPtr self, IntPtr client, string domainId, string dataSetId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeStartTime(IntPtr self, IntPtr client, int startTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeInterval(IntPtr self, IntPtr client, int interval);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeTLE(IntPtr self, IntPtr client, int tle);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeBufferTime(IntPtr self, IntPtr client, int bufferTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeIntegrityCheck(IntPtr self, IntPtr client, int integrityCheck);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeDSConditionsRequested(IntPtr self, IntPtr client, int dsConditionsRequested);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeBlockData(IntPtr self, IntPtr client, [MarshalAs(UnmanagedType.I1)] bool blockData);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeCritical(IntPtr self, IntPtr client, [MarshalAs(UnmanagedType.I1)] bool critical);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeRBE(IntPtr self, IntPtr client, [MarshalAs(UnmanagedType.I1)] bool rbe);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeAllChangesReported(IntPtr self, IntPtr client, [MarshalAs(UnmanagedType.I1)] bool allChangesReported);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDSTransferSet_writeStatus(IntPtr self, IntPtr client, [MarshalAs(UnmanagedType.I1)] bool status);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern byte
        Tase2_ClientDSTransferSet_getDSConditionsDetected(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int16
        Tase2_ClientDSTransferSet_getEventCodeDetected(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32
        Tase2_ClientDSTransferSet_getTransferSetTimestamp(IntPtr self);

        internal IntPtr self;

        private Client client;

        /* keep reference to prevent garbage collection */
        private ClientDataSet dataSet = null;

        internal ClientDSTransferSet(IntPtr transferSetRef, Client client)
        {
            self = transferSetRef;
            this.client = client;
        }

        /// <summary>
        /// Releases all resource used by the <see cref="TASE2.Library.Client.ClientDSTransferSet"/> object.
        /// </summary>
        /// <remarks>Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Client.ClientDSTransferSet"/>.
        /// The <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Client.ClientDSTransferSet"/> in an unusable
        /// state. After calling <see cref="Dispose"/>, you must release all references to the
        /// <see cref="TASE2.Library.Client.ClientDSTransferSet"/> so the garbage collector can reclaim the memory that the
        /// <see cref="TASE2.Library.Client.ClientDSTransferSet"/> was occupying.</remarks>
        public void Dispose()
        {
            lock (this)
            {
                if (self != IntPtr.Zero)
                {

                    client.RemoveDSTransferSetInstance(self);

                    Tase2_ClientDSTransferSet_destroy(self);
                    self = IntPtr.Zero;
                }
            }
        }

        internal bool IsDisposed()
        {
            lock (this)
            {
                if (self == IntPtr.Zero)
                    return true;
            }

            return false;
        }

        ~ClientDSTransferSet()
        {
            Dispose();
        }

        /// <summary>
        /// Reads the values of the DS transfer set variable from the server
        /// </summary>
        public void ReadValues()
        {
            int errorInt = Tase2_ClientDSTransferSet_readValues(self, client.self);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("ClientDSTransferSet read values failed", clientError);
        }

        /// <summary>
        /// Writes the buffered values of the DS transfer set variable to the server.
        /// Values have to be read before with the ReadValues method.
        /// </summary>
        public void WriteValues()
        {
            int errorInt = Tase2_ClientDSTransferSet_writeValues(self, client.self);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("ClientDSTransferSet write values failed", clientError);
        }

        /// <summary>
        /// Gets the domain name of the transfer set
        /// </summary>
        /// <returns>The domain name</returns>
        public string GetDomain()
        {
            IntPtr result = Tase2_ClientDSTransferSet_getDomain(self);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Gets the variable name of the transfer set.
        /// </summary>
        /// <returns>The name.</returns>
        public string GetName()
        {
            IntPtr result = Tase2_ClientDSTransferSet_getName(self);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Gets the data set domain name
        /// </summary>
        /// <returns>The data set domain name</returns>
        public string GetDataSetDomain()
        {
            IntPtr result = Tase2_ClientDSTransferSet_getDataSetDomain(self);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Gets the name of the data set.
        /// </summary>
        /// <returns>The data set name.</returns>
        public string GetDataSetName()
        {
            IntPtr result = Tase2_ClientDSTransferSet_getDataSetName(self);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Time value for the TASE.2 server to begin the Condition Monitoring. If Start Time is zero, then
        /// the TASE.2 server shall assume it to be its current time and shall begin Condition Monitoring
        /// immediately
        /// </summary>
        /// <value>The Start Time value as seconds from epoch</value>
        public int StartTime
        {
            get
            {
                return GetStartTime();
            }
            set
            {
                SetStartTime(value);
            }
        }

        /// <summary>
        /// This attribute is the time interval between TASE.2 server reports. The Interval starts when the
        /// Start Time occurs.
        /// </summary>
        /// <value>The Interval value in seconds.</value>
        public int Interval
        {
            get
            {
                return GetInterval();
            }
            set
            {
                SetInterval(value);
            }
        }

        /// <summary>
        /// Time value for Time Limit for Execution (TLE). This is a time that is primarily relevant to the TASE.2
        /// server. It is the time over which the TASE.2 server has to send the Data Set Transfer Report
        /// to the TASE.2 client. If the TASE.2 server anticipates that it cannot send the Data Set
        /// Transfer Report to the TASE.2 client within the TLE time, then it shall not generate the report,
        /// and shall discard the data.
        /// </summary>
        /// <value>The TLE value in seconds.</value>
        public int TLE
        {
            get
            {
                return GetTLE();
            }
            set
            {
                SetTLE(value);
            }
        }


        /// <summary>
        /// This attribute is the time interval for buffering the ObjectChange condition before reporting to
        /// the TASE.2 client. The Buffer Time starts when the first ObjectChange condition occurs.
        /// If multiple changes occur to the same object
        /// during the Buffer Time and Report All Changes is false, then only the final value is
        /// reported. If Report All Changes is true, then all changes to the same object during the
        /// Buffer Time are reported.
        /// </summary>
        /// <value>The buffer time value in seconds</value>
        public int BufferTime
        {
            get
            {
                return GetBufferTime();
            }
            set
            {
                SetBufferTime(value);
            }
        }

        /// <summary>
        /// This attribute is the time value for integrity check when IntegrityTimeOut condition is used.
        /// </summary>
        /// <value>The integrity check time in seconds.</value>
        public int IntegrityCheck
        {
            get
            {
                return GetIntegrityCheck();
            }
            set
            {
                SetIntegrityCheck(value);
            }
        }

        /// <summary>
        /// This attribute gives the conditions under which the Transfer Report shall be made
        /// </summary>
        /// <value>The report reason value (<see cref="ReportReason"/>)</value>
        public ReportReason DSConditionsRequested
        {
            get
            {
                return GetDSConditionsRequested();
            }
            set
            {
                SetDSConditionsRequested(value);
            }
        }

        /// <summary>
        /// This attribute indicates the type of Transfer reporting mechanism to be used. Blocking means that
        /// the values are reported in a more efficient way (no MMS structure information is sent). This can
        /// only be used when CB 3 is implemented.
        /// </summary>
        /// <value><c>true</c> mean blocking transfer report; not blocking transfer report, <c>false</c>.</value>
        public bool BlockData
        {
            get
            {
                return GetBlockData();
            }
            set
            {
                SetBlockData(value);
            }
        }

        /// <summary>
        /// This attribute indicates if Report by Exception (RBE) is used. 
        /// RBE means that only changed object values shall be included in the Transfer Report.
        /// Otherwise all object values are included.
        /// </summary>
        /// <value><c>true</c> if RBE is used; otherwise, <c>false</c>.</value>
        public bool RBE
        {
            get
            {
                return GetRBE();
            }
            set
            {
                SetRBE(value);
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether all changes are reported when buffer time > 0
        /// </summary>
        /// <value><c>true</c> if all changes reported; otherwise, <c>false</c>.</value>
        public bool AllChangesReported
        {
            get
            {
                return Tase2_ClientDSTransferSet_getAllChangesReported(self);
            }
            set
            {
                Tase2_ClientDSTransferSet_setAllChangesReported(self, value);
            }
        }

        /// <summary>
        /// Indicates if the Transfer Report is critical (requires acknowledgment by the client).
        /// </summary>
        /// <value><c>true</c> if critical; otherwise, <c>false</c>.</value>
        public bool Critical
        {
            get
            {
                return GetCritical();
            }
            set
            {
                SetCritical(value);
            }
        }

        /// <summary>
        /// Gets the Start Time value
        /// </summary>
        /// <returns>The Start Time value as seconds from epoch</returns>
        public int GetStartTime()
        {
            return Tase2_ClientDSTransferSet_getStartTime(self);
        }

        /// <summary>
        /// Gets the Interval value.
        /// </summary>
        /// <returns>The Interval value in seconds.</returns>
        public int GetInterval()
        {
            return Tase2_ClientDSTransferSet_getInterval(self);
        }

        /// <summary>
        /// Gets the time value for Time Limit for Execution (TLE)
        /// </summary>
        /// <returns>The TLE value in seconds.</returns>
        public int GetTLE()
        {
            return Tase2_ClientDSTransferSet_getTLE(self);
        }

        /// <summary>
        /// Gets the buffer time.
        /// </summary>
        /// <returns>The buffer time in seconds.</returns>
        public int GetBufferTime()
        {
            return Tase2_ClientDSTransferSet_getBufferTime(self);
        }

        /// <summary>
        /// Gets the integrity check time interval
        /// </summary>
        /// <returns>The integrity check value in seconds.</returns>
        public int GetIntegrityCheck()
        {
            return Tase2_ClientDSTransferSet_getIntegrityCheck(self);
        }

        /// <summary>
        /// Gets the DSConditionsRequested value
        /// </summary>
        /// <returns>The DSConditionsRequested value.</returns>
        public ReportReason GetDSConditionsRequested()
        {
            return (ReportReason)Tase2_ClientDSTransferSet_getDSConditionsRequested(self);
        }

        /// <summary>
        /// Gets the block data value
        /// </summary>
        /// <returns><c>true</c>, if block data is set, <c>false</c> otherwise.</returns>
        public bool GetBlockData()
        {
            return Tase2_ClientDSTransferSet_getBlockData(self);
        }

        /// <summary>
        /// Gets the critical value
        /// </summary>
        /// <returns><c>true</c>, if critical is set, <c>false</c> otherwise.</returns>
        public bool GetCritical()
        {
            return Tase2_ClientDSTransferSet_getCritical(self);
        }

        /// <summary>
        /// Get the external event code that caused the transmission
        /// </summary>
        /// <returns>The event code.</returns>
        public short GetEventCodeRequested()
        {
            return Tase2_ClientDSTransferSet_getEventCodeRequested(self);
        }

        /// <summary>
        /// Gets the value of the RBE (Report By Exception) flag.
        /// </summary>
        /// <returns><c>true</c>, if RBE is enabled, <c>false</c> otherwise.</returns>
        public bool GetRBE()
        {
            return Tase2_ClientDSTransferSet_getRBE(self);
        }

        /// <summary>
        /// Gets the status value (indicates if transfer set is enabled or not)
        /// </summary>
        /// <returns><c>true</c>, if status is enabled, <c>false</c> otherwise.</returns>
        public bool GetStatus()
        {
            return Tase2_ClientDSTransferSet_getStatus(self);
        }

        /// <summary>
        /// Sets the name of the transfer set data set.
        /// </summary>
        /// <param name="domainName">Domain name of the data set</param>
        /// <param name="dataSetName">Name of the data set.</param>
        public void SetDataSetName(string domainName, string dataSetName)
        {
            Tase2_ClientDSTransferSet_setDataSetName(self, domainName, dataSetName);
        }

        /// <summary>
        /// Associate a data set instance with the transfer set
        /// </summary>
        /// <param name="dataSet">The data set instance to be used.</param>
        public void SetDataSet(ClientDataSet dataSet)
        {
            Tase2_ClientDSTransferSet_setDataSet(self, dataSet.self);
            this.dataSet = dataSet;
        }

        /// <summary>
        /// Sets the start time of transfer condition monitoring.
        /// </summary>
        /// <param name="value">Start time as seconds since epoch.</param>
        public void SetStartTime(int value)
        {
            Tase2_ClientDSTransferSet_setStartTime(self, value);
        }

        /// <summary>
        /// Sets the interval time for transfer set reports
        /// </summary>
        /// <param name="value">interval time in seconds.</param>
        public void SetInterval(int value)
        {
            Tase2_ClientDSTransferSet_setInterval(self, value);
        }

        /// <summary>
        /// Sets the TLE (Time Limit for Execution) value.
        /// </summary>
        /// <param name="value">TLE value in seconds</param>
        public void SetTLE(int value)
        {
            Tase2_ClientDSTransferSet_setTLE(self, value);
        }

        /// <summary>
        /// Sets the buffer time.
        /// </summary>
        /// <param name="value">buffer time value in seconds.</param>
        public void SetBufferTime(int value)
        {
            Tase2_ClientDSTransferSet_setBufferTime(self, value);
        }

        /// <summary>
        /// Sets the integrity check interval time
        /// </summary>
        /// <param name="value">integrity check value in seconds</param>
        public void SetIntegrityCheck(int value)
        {
            Tase2_ClientDSTransferSet_setIntegrityCheck(self, value);
        }

        /// <summary>
        /// Sets the DSConditionsRequested value
        /// </summary>
        /// <param name="value">DSConditionsRequested  value</param>
        public void SetDSConditionsRequested(ReportReason value)
        {
            Tase2_ClientDSTransferSet_setDSConditionsRequested(self, (int)value);
        }

        /// <summary>
        /// Sets the block data enable flag
        /// </summary>
        /// <param name="value">If set to <c>true</c> block data in enabled.</param>
        public void SetBlockData(bool value)
        {
            Tase2_ClientDSTransferSet_setBlockData(self, value);
        }

        /// <summary>
        /// Sets the critical flag
        /// </summary>
        /// <param name="value">If set to <c>true</c> critical transfer report is enabled.</param>
        public void SetCritical(bool value)
        {
            Tase2_ClientDSTransferSet_setCritical(self, value);
        }

        /// <summary>
        /// Sets the RBE (Report by Exception) flag value
        /// </summary>
        /// <param name="value">If set to <c>true</c> RBE is enabled.</param>
        public void SetRBE(bool value)
        {
            Tase2_ClientDSTransferSet_setRBE(self, value);
        }

        /// <summary>
        /// Sets the status (of condition monitoring) of the transfer set
        /// </summary>
        /// <param name="value">If set to <c>true</c> condition monitoring is enabled.</param>
        public void SetStatus(bool value)
        {
            Tase2_ClientDSTransferSet_setStatus(self, value);
        }

        /// <summary>
        /// Immediately write the name of the transfer set data set to the server.
        /// </summary>
        /// <param name="domainName">Domain name of the data set</param>
        /// <param name="dataSetName">Name of the data set.</param>
        public void WriteDataSetName(string domainName, string dataSetName)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeDataSetName(self, client.self, domainName, dataSetName);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write data set name failed", clientError);
        }

        /// <summary>
        /// Immediately write the start time of transfer condition monitoring to the server.
        /// </summary>
        /// <param name="value">Start time as seconds since epoch.</param>
        public void WriteStartTime(int value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeStartTime(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write start time failed", clientError);
        }

        /// <summary>
        /// Immediately write the interval time for transfer set reports to the server.
        /// </summary>
        /// <param name="value">interval time in seconds.</param>
        public void WriteInterval(int value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeInterval(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write interval failed", clientError);
        }

        /// <summary>
        /// Immediately write the TLE (Time Limit for Execution) value to the server.
        /// </summary>
        /// <param name="value">TLE value in seconds</param>
        public void WriteTLE(int value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeTLE(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write TLE failed", clientError);
        }

        /// <summary>
        /// Immediately write the buffer time to the server.
        /// </summary>
        /// <param name="value">buffer time value in seconds.</param>
        public void WriteBufferTime(int value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeBufferTime(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write buffer time failed", clientError);
        }

        /// <summary>
        /// Immediately write the integrity check interval time to the server.
        /// </summary>
        /// <param name="value">integrity check value in seconds</param>
        public void WriteIntegrityCheck(int value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeIntegrityCheck(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write integrity check failed", clientError);
        }

        /// <summary>
        /// Immediately write the DSConditionsRequested value to the server
        /// </summary>
        /// <param name="value">DSConditionsRequested  value</param>
        public void WriteDSConditionsRequested(ReportReason value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeDSConditionsRequested(self, client.self, (int)value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write DS conditions requested failed", clientError);
        }

        /// <summary>
        /// Immediately write the block data enable flag to the server.
        /// </summary>
        /// <param name="value">If set to <c>true</c> block data in enabled.</param>
        public void WriteBlockData(bool value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeBlockData(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write block data failed", clientError);
        }

        /// <summary>
        /// Immediately write the critical flag to the server.
        /// </summary>
        /// <param name="value">If set to <c>true</c> critical transfer report is enabled.</param>
        public void WriteCritical(bool value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeCritical(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write critical failed", clientError);
        }

        /// <summary>
        /// Immediately write the RBE (Report by Exception) flag value to the server.
        /// </summary>
        /// <param name="value">If set to <c>true</c> RBE is enabled.</param>
        public void WriteRBE(bool value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeRBE(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write RBE failed", clientError);
        }

        /// <summary>
        /// Immediately write the all changes reported flag value to the server.
        /// </summary>
        /// <param name="value">If set to <c>true</c> all changes reported is enabled.</param>
        public void WriteAllChangesReported(bool value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeAllChangesReported(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write AllChangesReported failed", clientError);
        }

        /// <summary>
        /// Immediately write the status (enable/disable condition monitoring) of the transfer set
        /// </summary>
        /// <param name="value">If set to <c>true</c> condition monitoring is enabled.</param>
        public void WriteStatus(bool value)
        {
            int errorInt = Tase2_ClientDSTransferSet_writeStatus(self, client.self, value);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write status failed", clientError);
        }

        /// <summary>
        /// Get the DSConditions Detected value from the last received report
        /// </summary>
        /// <remarks>
        /// This represents an optional special data set variable. When part of the
        /// transfer set data set this will be sent with reports.
        /// 
        /// At the time of reporting, this attribute uniquely identifies the conditions that
        /// caused the generation of the report.
        /// </remarks>
        /// <returns>The DS conditions detected value (reason code)</returns>
        public ReportReason GetDSConditionsDetected()
        {
            byte reasonCode = Tase2_ClientDSTransferSet_getDSConditionsDetected(self);

            return (ReportReason)reasonCode;
        }

        /// <summary>
        /// Get the event code detected value from the last received report.
        /// </summary>
        /// <remarks>
        /// This represents an optional special data set variable. When part of the
        /// transfer set data set this will be sent with reports.
        /// </remarks>
        /// <returns>The event code detected.</returns>
        public Int16 GetEventCodeDetected()
        {
            return Tase2_ClientDSTransferSet_getEventCodeDetected(self);
        }

        /// <summary>
        /// Get the transfer set timestamp value from the last received report
        /// </summary>
        /// <remarks>
        /// This represents an optional special data set variable. When part of the
        /// transfer set data set this will be sent with reports.
        /// 
        /// This attribute specifies the time the transfer report was generated at the
        /// server.
        /// </remarks>
        /// <returns>transfer set timestamp value in seconds since 01.01.1970 UTC.</returns>
        public Int32 GetTransferSetTimestamp()
        {
            return Tase2_ClientDSTransferSet_getTransferSetTimestamp(self);
        }

    }
}

