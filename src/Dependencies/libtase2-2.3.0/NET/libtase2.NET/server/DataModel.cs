using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using TASE2.Library.Common;

namespace TASE2.Library.Server
{
    /// <summary>
    /// TASE.2/ICCP version
    /// </summary>
    public enum Edition
    {
        EDITION_1996_08 = 1,
        EDITION_2000_08 = 2
    }

    /// <summary>
    /// Represents to complete data model of a TASE.2 server. This class is used as the entry point to create a new data model.
    /// </summary>
    public class DataModel : IDisposable
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataModel_create();

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_DataModel_destroy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataModel_getVCC(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataModel_addDomain(IntPtr self, string domainName);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_DataModel_setEdition(IntPtr self, int edition);

        private IntPtr self;

        private List<Domain> domains = new List<Domain>();

        private Dictionary<IntPtr, ControlPoint> controlPoints = new Dictionary<IntPtr, ControlPoint>();

        private Dictionary<IntPtr, DataPoint> dataPoints = new Dictionary<IntPtr, DataPoint>();

        private Dictionary<IntPtr, TransferSet> transferSets = new Dictionary<IntPtr, TransferSet>();

        private Domain vcc = null;

        internal IntPtr Self
        {
            get
            {
                return self;
            }
        }

        internal Domain LookupDomain(IntPtr domainPtr)
        {
            return domains.Find(y => y.Self == domainPtr);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="TASE2.Library.Server.DataModel"/> class.
        /// </summary>
        public DataModel()
        {
            self = Tase2_DataModel_create();
        }

        /// <summary>
        /// Releases all resource used by the <see cref="TASE2.Library.Server.DataModel"/> object.
        /// </summary>
        /// <remarks>Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Server.DataModel"/>. The
        /// <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Server.DataModel"/> in an unusable state. After
        /// calling <see cref="Dispose"/>, you must release all references to the <see cref="TASE2.Library.Server.DataModel"/>
        /// so the garbage collector can reclaim the memory that the <see cref="TASE2.Library.Server.DataModel"/> was occupying.</remarks>
        public void Dispose()
        {
            lock (this)
            {
                if (self != IntPtr.Zero)
                {
                    Tase2_DataModel_destroy(self);
                    self = IntPtr.Zero;
                }
            }
        }

        ~DataModel()
        {
            Dispose();
        }

        /// <summary>
        /// Set the TASE.2/ICCP version to be used by the server.
        /// </summary>
        /// <param name="edition">TASE.2/ICCP version to use by the server.</param>
        public void SetEdition(Edition edition)
        {
            Tase2_DataModel_setEdition(self, (int)edition);
        }

        /// <summary>
        /// Adds a new domain to the data model
        /// </summary>
        /// <returns>Reference of the new domain (is requried to add items to the domain)</returns>
        /// <param name="domainName">the unique name of the domain</param>
        public Domain AddDomain(string domainName)
        {
            Domain domain = new Domain(this, Tase2_DataModel_addDomain(self, domainName), domainName);

            domains.Add(domain);

            return domain;
        }

        /// <summary>
        /// Gets a domain.
        /// </summary>
        /// <returns>The domain object.</returns>
        /// <param name="domainName">domain name</param>
        public Domain GetDomain(string domainName)
        {
            foreach (Domain domain in domains)
            {
                if (domainName == domain.Name)
                    return domain;
            }

            return null;
        }

        /// <summary>
        /// Gets the VCC domain of a data model
        /// </summary>
        /// <remarks>All variables in the VCC domain are externally visible as VCC scope variables.</remarks>
        /// <remarks>The VCC domain is created automatically when the \ref DataModel is created</remarks>
        /// <value>The VCC domain object</value>
        public Domain VCC
        {
            get
            {
                if (vcc == null)
                    vcc = new Domain(this, Tase2_DataModel_getVCC(self), null);

                return vcc;
            }
        }

        internal void AddControlPoint(ControlPoint controlPoint)
        {
            controlPoints.Add(controlPoint.Self, controlPoint);
        }

        internal void AddDataPoint(DataPoint dataPoint)
        {
            dataPoints.Add(dataPoint.Self, dataPoint);
        }

        internal void AddTransferSet(TransferSet transferSet)
        {
            transferSets.Add(transferSet.Self, transferSet);
        }

        internal ControlPoint LookupControlPoint(IntPtr controlPointRef)
        {
            return controlPoints[controlPointRef];
        }

        internal DataPoint LookupDataPoint(IntPtr dataPointRef)
        {
            return dataPoints[dataPointRef];
        }

        internal TransferSet LookupTransferSet(IntPtr transferSetRef)
        {
            return transferSets[transferSetRef];
        }
    }

}

