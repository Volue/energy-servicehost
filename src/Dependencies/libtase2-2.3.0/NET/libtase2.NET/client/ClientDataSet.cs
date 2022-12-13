using System;
using System.Runtime.InteropServices;
using TASE2.Library.Common;

namespace TASE2.Library.Client
{

    /// <summary>
    /// Client side representation of a data set
    /// </summary>
    public class ClientDataSet : IDisposable
    {

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDataSet_getDomainName(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDataSet_getDataSetName(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_ClientDataSet_isDeletable(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDataSet_getSize(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDataSet_getPointDomainName(IntPtr self, int index);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDataSet_getPointVariableName(IntPtr self, int index);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_ClientDataSet_getPointValue(IntPtr self, int index);


        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ClientDataSet_read(IntPtr self, IntPtr client);


        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ClientDataSet_destroy(IntPtr self);

        internal IntPtr self;

        private Client client;

        internal ClientDataSet(IntPtr ptr, Client client)
        {
            self = ptr;
            this.client = client;
        }

        /// <summary>
        /// Releases all resource used by the <see cref="TASE2.Library.Client.ClientDataSet"/> object.
        /// </summary>
        /// <remarks>Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Client.ClientDataSet"/>. The
        /// <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Client.ClientDataSet"/> in an unusable state.
        /// After calling <see cref="Dispose"/>, you must release all references to the
        /// <see cref="TASE2.Library.Client.ClientDataSet"/> so the garbage collector can reclaim the memory that the
        /// <see cref="TASE2.Library.Client.ClientDataSet"/> was occupying.</remarks>
        public void Dispose()
        {
            lock (this)
            {
                if (self != IntPtr.Zero)
                {
                    Tase2_ClientDataSet_destroy(self);
                    self = IntPtr.Zero;
                }
            }
        }

        ~ClientDataSet()
        {
            Dispose();
        }

        /// <summary>
        /// Gets the domain name of the data set
        /// </summary>
        /// <returns>The domain name.</returns>
        public string GetDomainName()
        {
            IntPtr result = Tase2_ClientDataSet_getDomainName(self);

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
            IntPtr result = Tase2_ClientDataSet_getDataSetName(self);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Determines whether the data set can be deleted at the server
        /// </summary>
        /// <returns><c>true</c> if the data set can be deleted; otherwise, <c>false</c>.</returns>
        public bool IsDeletable()
        {
            return Tase2_ClientDataSet_isDeletable(self);
        }

        /// <summary>
        /// Gets the size of the data set (number of data set entries)
        /// </summary>
        /// <returns>The size of the data set</returns>
        public int GetSize()
        {
            return Tase2_ClientDataSet_getSize(self);
        }

        /// <summary>
        /// Gets the domain name of the data point with the given index position in the data sets
        /// </summary>
        /// <returns>the domain name of the data point</returns>
        /// <param name="index">the position index starting with 0</param>
        public string GetPointDomainName(int index)
        {
            IntPtr result = Tase2_ClientDataSet_getPointDomainName(self, index);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Gets the variable name of the data point with the given index position in the data sets
        /// </summary>
        /// <returns>The variable name of the data point</returns>
        /// <param name="index">the position index starting with 0</param>
        public string GetPointVariableName(int index)
        {
            IntPtr result = Tase2_ClientDataSet_getPointVariableName(self, index);

            if (result == IntPtr.Zero)
                return null;
            else
                return Marshal.PtrToStringAnsi(result);
        }

        /// <summary>
        /// Gets the point value of the data point with the given index position in the data sets.
        /// </summary>
        /// <returns>The point value.</returns>
        /// <param name="index">the position index starting with 0</param>
        public PointValue GetPointValue(int index)
        {
            IntPtr valuePtr = Tase2_ClientDataSet_getPointValue(self, index);

            if (valuePtr == IntPtr.Zero)
                return null;

            return new PointValue(valuePtr);
        }

        /// <summary>
        /// Read the current data set values from the server
        /// </summary>
        public void Read()
        {
            int errorInt = Tase2_ClientDataSet_read(self, client.self);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("ClientDataSet read failed", clientError);
        }
    }
}

