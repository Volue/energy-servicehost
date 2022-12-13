using System;
using System.Runtime.InteropServices;

namespace TASE2.Library.Server
{

    /// <summary>
    /// Represents a transfer set in the server data model.
    /// </summary>
    public class TransferSet
    {
        private IntPtr self;

        internal IntPtr Self
        {
            get
            {
                return self;
            }
        }

        private string name;

        /// <summary>
        /// Gets the name of the transfer set (visible to the client)
        /// </summary>
        /// <value>transfer set name</value>
        public string Name
        {
            get
            {
                return name;
            }
        }

        internal TransferSet(IntPtr transferSetPtr, string name)
        {
            self = transferSetPtr;
            this.name = name;
        }
    }

    /// <summary>
    /// Represents a data set transfer set in the server data model.
    /// </summary>
    /// DSTransfer sets are used to report data set events to the client
    public class DSTransferSet : TransferSet
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DSTransferSet_getDataSetName(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32 Tase2_DSTransferSet_getStartTime(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32 Tase2_DSTransferSet_getInterval(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32 Tase2_DSTransferSet_getTLE(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32 Tase2_DSTransferSet_getBufferTime(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32 Tase2_DSTransferSet_getIntegrityCheck(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_DSTransferSet_getBlockData(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_DSTransferSet_getCritical(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_DSTransferSet_getRBE(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_DSTransferSet_getAllChangesReported(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_DSTransferSet_getStatus(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int16 Tase2_DSTransferSet_getEventCodeRequested(IntPtr self);

        internal DSTransferSet(IntPtr transferSetPtr, string name) : base(transferSetPtr, name)
        {
        }

        public String DataSetName
        {
            get
            {
                IntPtr dataSetNamePtr = Tase2_DSTransferSet_getDataSetName(Self);

                if (dataSetNamePtr == IntPtr.Zero)
                {
                    return null;
                }
                else
                {
                    String retStr = Marshal.PtrToStringAnsi(dataSetNamePtr);

                    Marshal.FreeHGlobal(dataSetNamePtr);

                    return retStr;
                }
            }
        }

        public Int32 StartTime
        {
            get
            {
                return Tase2_DSTransferSet_getStartTime(Self);
            }
        }

        public Int32 Interval
        {
            get
            {
                return Tase2_DSTransferSet_getInterval(Self);
            }
        }

        public Int32 TLE
        {
            get
            {
                return Tase2_DSTransferSet_getTLE(Self);
            }
        }


        public Int32 BufferTime
        {
            get
            {
                return Tase2_DSTransferSet_getBufferTime(Self);
            }
        }

        public Int32 IntegrityCheck
        {
            get
            {
                return Tase2_DSTransferSet_getIntegrityCheck(Self);
            }
        }

        public bool BlockData
        {
            get
            {
                return Tase2_DSTransferSet_getBlockData(Self);
            }
        }

        public bool Critical
        {
            get
            {
                return Tase2_DSTransferSet_getCritical(Self);
            }
        }

        public bool RBE
        {
            get
            {
                return Tase2_DSTransferSet_getRBE(Self);
            }
        }

        public bool AllChangesReported
        {
            get
            {
                return Tase2_DSTransferSet_getAllChangesReported(Self);
            }
        }

        public bool Status
        {
            get
            {
                return Tase2_DSTransferSet_getStatus(Self);
            }
        }

        public Int16 EventCodeRequested
        {
            get
            {
                return Tase2_DSTransferSet_getEventCodeRequested(Self);
            }
        }

    }

}

