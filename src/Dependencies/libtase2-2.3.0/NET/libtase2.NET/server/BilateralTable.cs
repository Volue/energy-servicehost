using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace TASE2.Library.Server
{
    /// <summary>
    /// Represents a bilateral table (BLT) that is used to define access rights of a client
    /// </summary>
    /// A bilateral table is associated with a specific application identifier (ap-title and ae-qualifier).
    /// After creation of the new BilateralTable obejct rules for data points can be created with the
    /// \ref AddDataPoint and \ref AddControlPoint methods. The generic rule is: Everything that is not
    /// explicitely allowed in a rule will be denied by the server.
    /// The following rules apply:
    /// <list>
    /// <item>if no BLT is available all data can be accessed</item>
    /// <item>you should define a BLT for each expected client</item>
    /// <item>the BLT has an associated domain. This domain is especially dedicated to the assigned client application.</item>
    /// <item>in general only data/control points that are part of the BLT can be accessed by the client</item>
    /// <item>but all data/control point that are part of the client's  ICC can be accessed</item>
    /// </list>
    public class BilateralTable
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_BilateralTable_create(string bltId, IntPtr domain, string apTitle, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_BilateralTable_addDataPoint(IntPtr self, IntPtr dataPoint, [MarshalAs(UnmanagedType.I1)] bool allowRead, [MarshalAs(UnmanagedType.I1)] bool allowWrite);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_BilateralTable_addControlPoint(IntPtr self, IntPtr controlPoint, Int16 checkBackId, [MarshalAs(UnmanagedType.I1)] bool allowSetTag, [MarshalAs(UnmanagedType.I1)] bool allowGetTag, [MarshalAs(UnmanagedType.I1)] bool allowOperate, bool allowRead);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_BilateralTable_addInformationMessage(IntPtr self, Int32 infoRef, Int32 localRef, [MarshalAs(UnmanagedType.I1)] bool vccScope);

        [StructLayout(LayoutKind.Sequential)]
        private struct NativeTSelector
        {
            public byte size;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)] public byte[] value;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct NativeSSelector
        {
            public byte size;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] public byte[] value;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct NativePSelector
        {
            public byte size;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] public byte[] value;
        }

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_BilateralTable_setIsoParameters(IntPtr self, NativePSelector pSelector, NativeSSelector sSelector, NativeTSelector tSelector);

        private IntPtr self;

        internal IntPtr Self
        {
            get
            {
                return self;
            }
        }

        private static Dictionary<IntPtr, WeakReference<BilateralTable>> instances = new Dictionary<IntPtr, WeakReference<BilateralTable>>();

        internal static BilateralTable GetInstance(IntPtr ptr)
        {
            WeakReference<BilateralTable> bltRef = instances[ptr];

            BilateralTable blt = null;

            if (bltRef != null)
            {
                if (bltRef.TryGetTarget(out blt) == false)
                    return null;
            }

            return blt;
        }


        internal BilateralTable(IntPtr bltPtr)
        {
            this.self = bltPtr;
            instances.Add(bltPtr, new WeakReference<BilateralTable>(this));
        }

        private string bltId;
        private string apTitle;
        private int aeQualifier;
        private Domain domain;

        /// <summary>
        /// Set the ISO protocol layer addresses to associate the endpoint to this BLT
        /// </summary>
        /// <remarks>>
        /// By default only AP-title and AE-qualifier are used to associate an endpoint
        /// with the BLT. You can change this behavior with <see cref="TASE2.Library.Common.Endpoint.SetAddressMatchingRules"/>
        /// method. When using the ISO layer addresses you have to set them with this function.
        /// </remarks>
        /// <param name="pSelector">P selector.</param>
        /// <param name="sSelector">S selector.</param>
        /// <param name="tSelector">T selector.</param>
        public void SetIsoParameters(byte[] pSelector, byte[] sSelector, byte[] tSelector)
        {
            if (pSelector.Length > 16)
                throw new ArgumentOutOfRangeException("pSelector", "maximum size (16) exceeded");

            NativePSelector nativePSelector;
            nativePSelector.size = (byte)pSelector.Length;
            nativePSelector.value = new byte[16];

            for (int i = 0; i < pSelector.Length; i++)
                nativePSelector.value[i] = pSelector[i];

            if (tSelector.Length > 4)
                throw new ArgumentOutOfRangeException("tSelector", "maximum size (4) exceeded");

            NativeTSelector nativeTSelector;
            nativeTSelector.size = (byte)tSelector.Length;
            nativeTSelector.value = new byte[4];

            for (int i = 0; i < tSelector.Length; i++)
                nativeTSelector.value[i] = tSelector[i];

            if (sSelector.Length > 16)
                throw new ArgumentOutOfRangeException("sSelector", "maximum size (16) exceeded");

            NativeSSelector nativeSSelector;
            nativeSSelector.size = (byte)sSelector.Length;
            nativeSSelector.value = new byte[16];

            for (int i = 0; i < sSelector.Length; i++)
                nativeSSelector.value[i] = sSelector[i];

            Tase2_BilateralTable_setIsoParameters(self, nativePSelector, nativeSSelector, nativeTSelector);
        }

        /// <summary>
        /// Get the bilateral table ID
        /// </summary>
        /// <value>The BLT ID</value>
        public string ID
        {
            get
            {
                return this.bltId;
            }
        }

        /// <summary>
        /// Gets the ap-title of the client application
        /// </summary>
        /// <value>The ap-title</value>
        public string ApTitle
        {
            get
            {
                return this.apTitle;
            }
        }

        /// <summary>
        /// Gets the ae-qualifier of the client application
        /// </summary>
        /// <value>The ae-qualifier.</value>
        public int AeQualifier
        {
            get
            {
                return this.aeQualifier;
            }
        }

        /// <summary>
        /// Gets the domain associated with this BLT
        /// </summary>
        /// <value>The TASE.2 domain</value>
        public Domain Domain
        {
            get
            {
                return this.domain;
            }
        }


        /// <summary>
        /// Initializes a new instance of the <see cref="TASE2.Library.Server.BilateralTable"/> class.
        /// </summary>
        /// <remarks>There can only be one BLT in each \ref Domain</remarks>
        /// <param name="bltId">the bilateral table ID (visible to client)</param>
        /// <param name="domain">the domain of the BLT</param>
        /// <param name="apTitle">the AP-title to identify the client.</param>
        /// <param name="aeQualifier">the AE-qualifier to identify the client.</param>
        public BilateralTable(string bltId, Domain domain, string apTitle, int aeQualifier)
        {
            this.bltId = bltId;
            this.domain = domain;
            this.apTitle = apTitle;
            this.aeQualifier = aeQualifier;

            self = Tase2_BilateralTable_create(bltId, domain.Self, apTitle, aeQualifier);

            if (instances.ContainsKey(self))
            {
                instances.Remove(self);
            }

            instances.Add(self, new WeakReference<BilateralTable>(this));
        }

        /// <summary>
        /// Adds a data point rule (for IndicationPoint and ProtectionEquipment)
        /// </summary>
        /// <param name="dataPoint">rule target object</param>
        /// <param name="allowRead">If set to <c>true</c> allow read.</param>
        /// <param name="allowWrite">If set to <c>true</c> allow write.</param>
        public void AddDataPoint(DataPoint dataPoint, bool allowRead, bool allowWrite)
        {
            Tase2_BilateralTable_addDataPoint(self, dataPoint.Self, allowRead, allowWrite);
        }

        /// <summary>
        /// Adds a control point rule (for ControlPoint)
        /// </summary>
        /// <param name="controlPoint">rule target object</param>
        /// <param name="checkbackId">Checkback identifier.</param>
        /// <param name="allowSetTag">If set to <c>true</c> allow set tag.</param>
        /// <param name="allowGetTag">If set to <c>true</c> allow get tag.</param>
        /// <param name="allowOperate">If set to <c>true</c> allow operate.</param>
        /// <param name="allowRead">If set to <c>true</c> allow read.</param>
        public void AddControlPoint(ControlPoint controlPoint, Int16 checkbackId, bool allowSetTag, bool allowGetTag, bool allowOperate, bool allowRead)
        {
            Tase2_BilateralTable_addControlPoint(self, controlPoint.Self, checkbackId, allowSetTag, allowGetTag, allowOperate, allowRead);
        }

        /// <summary>
        /// Add an information message object to the BLT
        /// </summary>
        /// <param name="infoRef">information reference parameter (specifies application)</param>
        /// <param name="localRef">local reference parameter</param>
        /// <param name="vccScope">indicates if object has VCC scope (true) or ICC scope (false)</param>
        public void AddInformationMessage(int infoRef, int localRef, bool vccScope)
        {
            Tase2_BilateralTable_addInformationMessage(self, infoRef, localRef, vccScope);
        }

    }
}

