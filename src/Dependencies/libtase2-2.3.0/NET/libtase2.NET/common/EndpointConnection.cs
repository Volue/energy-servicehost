using System;
using System.Runtime.InteropServices;

namespace TASE2.Library.Common
{

    /// <summary>
    /// Represents a connection to a peer
    /// </summary>
    public class EndpointConnection
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Endpoint_Connection_getPeerIpAddress(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Endpoint_Connection_getPeerApTitle(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_Connection_getPeerAeQualifier(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_Connection_getMaxPduSize(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_FreeMemory(IntPtr ptr);

        internal IntPtr self;

        private string peerAddress = null;
        private string peerApTitle = null;
        private int peerAeQualifier = 0;
        private int maxPduSize = 0;

        internal EndpointConnection(IntPtr conPtr)
        {
            self = conPtr;

            IntPtr strPtr = Tase2_Endpoint_Connection_getPeerIpAddress(self);

            if (strPtr != IntPtr.Zero)
            {
                peerAddress = Marshal.PtrToStringAnsi(strPtr);
            }

            strPtr = Tase2_Endpoint_Connection_getPeerApTitle(self);

            if (strPtr != IntPtr.Zero)
            {
                peerApTitle = Marshal.PtrToStringAnsi(strPtr);
            }

            peerAeQualifier = Tase2_Endpoint_Connection_getPeerAeQualifier(self);

            maxPduSize = Tase2_Endpoint_Connection_getMaxPduSize(self);
        }

        /// <summary>
        /// Gets the peer address (IP address and TCP port)
        /// </summary>
        /// <value>The peer address including IP address and TCP port (separated by ":")</value>
        public String PeerAddress
        {
            get
            {
                return peerAddress;
            }
        }

        /// <summary>
        /// Gets the AP-title of the peer
        /// </summary>
        /// <value>The peer AP-title.</value>
        public String PeerApTitle
        {
            get
            {
                return peerApTitle;
            }
        }

        /// <summary>
        /// Gets the peer AE-qualifier.
        /// </summary>
        /// <value>The peer  AE-qualifier.</value>
        public int PeerAeQualifier
        {
            get
            {
                return peerAeQualifier;
            }
        }

        public int MaxPduSize
        {
            get
            {
                return maxPduSize;
            }
        }


    }
}
