using System;
using System.Runtime.InteropServices;

using TASE2.Library.Common;

namespace TASE2.Library.Server
{
    /// <summary>
    /// Represents a domain of the data model.
    /// </summary>
    /// <remarks>A special domain ("VCC") is used as a container for VCC scope data model elements.</remarks>
    public class Domain
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Domain_addIndicationPoint(IntPtr self, string name, int type,
            int qualityClass, int timeStampClass, [MarshalAs(UnmanagedType.I1)] bool hasCOV, [MarshalAs(UnmanagedType.I1)] bool readOnly);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Domain_addControlPoint(IntPtr self, string deviceName, int pointType, int deviceTag, [MarshalAs(UnmanagedType.I1)] bool hasTag, Int16 checkBackId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Domain_addProtectionEquipment(IntPtr self, string equipmentName, [MarshalAs(UnmanagedType.I1)] bool isPacked);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Domain_addDataSet(IntPtr self, string name);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Domain_addDSTransferSet(IntPtr self, string name);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Domain_addInformationMessageObject(IntPtr self, int maxSize);

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
        /// Gets the domain name
        /// </summary>
        /// <remarks>The VCC domain has no name!</remarks>
        /// <value>The name of the domain</value>
        public string Name
        {
            get
            {
                return name;
            }
        }

        private DataModel dataModel;

        internal Domain(DataModel model, IntPtr domainPtr, string domainName)
        {
            this.dataModel = model;
            this.self = domainPtr;
            this.name = domainName;
        }

        /// <summary>
        /// Adds an indication point (\ref IndicationPoint) to the domain
        /// </summary>
        /// <returns>Reference of the new \ref IndicationPoint object.</returns>
        /// <param name="name">name of the new indication point</param>
        /// <param name="type">type of the indication point</param>
        /// <param name="quality">the quality class of the indication point.</param>
        /// <param name="timeStampClass">the time stamp class of the indication point.</param>
        /// <param name="hasCOV">If set to <c>true</c> has a COV (counter of value changes).</param>
        /// <param name="readOnly">If set to <c>true</c> the indication point is read only.</param>
        public IndicationPoint AddIndicationPoint(String name, IndicationPointType type, QualityClass quality,
            TimeStampClass timeStampClass, bool hasCOV, bool readOnly)
        {
            IntPtr ptr = Tase2_Domain_addIndicationPoint(self, name, (int)type, (int)quality, (int)timeStampClass, hasCOV, readOnly);

            IndicationPoint point = new IndicationPoint(ptr, name, this);

            dataModel.AddDataPoint(point);

            return point;
        }

        /// <summary>
        /// Adds a control point (device) to the domain
        /// </summary>
        /// <returns>Reference of the new \ref ControlPoint object.</returns>
        /// <param name="deviceName">the name of the new device (control point)</param>
        /// <param name="type">the device type</param>
        /// <param name="deviceClass">the device class (direct or SBO)</param>
        /// <param name="hasTag">If set to <c>true</c> the device has a tag.</param>
        /// <param name="checkBackId">check back ID used for select</param>
        public ControlPoint AddControlPoint(String deviceName, ControlPointType type, DeviceClass deviceClass, bool hasTag, Int16 checkBackId)
        {
            IntPtr ptr = Tase2_Domain_addControlPoint(self, deviceName, (int)type, (int)deviceClass, hasTag, checkBackId);

            ControlPoint point = new ControlPoint(ptr, deviceName, this);

            dataModel.AddControlPoint(point);
            dataModel.AddDataPoint(point);

            return point;
        }

        /// <summary>
        /// Adds a new protection equipment.
        /// </summary>
        /// <returns>Reference of the new \ref ProtectionEquipment object.</returns>
        /// <param name="equipmentName">the name of the new protection equipment.</param>
        /// <param name="isPacked">If set to <c>true</c> is of type "packed".</param>
        public ProtectionEquipment AddProtectionEquipment(string equipmentName, bool isPacked)
        {
            IntPtr ptr = Tase2_Domain_addProtectionEquipment(self, equipmentName, isPacked);

            ProtectionEquipment equipment = new ProtectionEquipment(ptr, equipmentName, this);

            dataModel.AddDataPoint(equipment);

            return equipment;
        }

        /// <summary>
        /// Adds a new data set
        /// </summary>
        /// <returns>Reference of the new data set object</returns>
        /// <param name="name">data set name</param>
        public DataSet AddDataSet(string name)
        {
            IntPtr dataSetPtr = Tase2_Domain_addDataSet(self, name);

            return new DataSet(dataSetPtr, name);
        }

        /// <summary>
        /// Adds a new data set transfer set
        /// </summary>
        /// <remarks>Transfer sets cannot be added to VCC scope</remarks>
        /// <returns>Reference of the new data set transfer set (\ref DSTransferSet) object</returns>
        /// <param name="name">transfer set name</param>
        public DSTransferSet AddDSTransferSet(string name)
        {
            IntPtr transferSetPtr = Tase2_Domain_addDSTransferSet(self, name);

            DSTransferSet transferSet = new DSTransferSet(transferSetPtr, name);

            dataModel.AddTransferSet(transferSet);

            return transferSet;
        }


        public void AddInformationMessageObject(int maxSize)
        {
            Tase2_Domain_addInformationMessageObject(self, maxSize);
        }
    }
}

