using System;
using System.Runtime.InteropServices;
using TASE2.Library.Common;

namespace TASE2.Library.Server
{

    /// <summary>
    /// Type of and indication point (\ref IndicationPoint)
    /// </summary>
    public enum IndicationPointType
    {
        /// <summary>
        /// REAL type: 32 bit floating point
        /// </summary>
        REAL = 0,

        /// <summary>
        /// STATE type: Single or double point (state) with some flags
        /// </summary>
        STATE,

        /// <summary>
        /// DISCRETE type: 32 bit integer
        /// </summary>
        DISCRETE,

        /// <summary>
        /// STATE SUPPLEMENTAL: state type with additional flags
        /// </summary>
        STATE_SUPPLEMENTAL
    }

    /// <summary>
    /// The device class of a control point (\ref ControlPoint)
    /// </summary>
    public enum DeviceClass
    {

        /// <summary>
        /// The device support select-before-operate (SBO) commands. The operate has to be preceeded by a select command.
        /// </summary>
        SBO = 0,

        /// <summary>
        /// The device supports direct operate commands (without a prior select command)
        /// </summary>
        DIRECT
    }

    /// <summary>
    /// Represents an indication point in the server data model
    /// </summary>
    public class IndicationPoint : DataPoint
    {

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setReal(IntPtr self, float value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setRealQ(IntPtr self, float value, byte flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setRealQTimeStamp(IntPtr self, float value, byte flags, ulong timestamp);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setDiscrete(IntPtr self, Int32 value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setDiscreteQ(IntPtr self, Int32 value, byte flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setDiscreteQTimeStamp(IntPtr self, Int32 value, byte flags, ulong timestamp);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setState(IntPtr self, byte value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setStateTimeStamp(IntPtr self, byte value, ulong timestamp);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setStateSupplemental(IntPtr self, byte value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setStateSupplementalQ(IntPtr self, byte value, byte flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setStateSupplementalQTimeStamp(IntPtr self, byte value, byte flags, ulong timestamp);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setQuality(IntPtr self, byte flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setTimeStamp(IntPtr self, ulong timestamp);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_IndicationPoint_setCOV(IntPtr self, UInt16 cov);

        internal IndicationPoint(IntPtr self, string name, Domain domain) : base(self, name, domain)
        {

        }

        /// <summary>
        /// Sets the value of an indication point with type \ref IndicationPointType.REAL
        /// </summary>
        /// <remarks>Can only be used after the IndicationPoint is connected to the server!</remarks>
        /// <remarks>When calling on other types the value will not be changed!</remarks>
        /// <param name="value">the new value</param>
        public void SetReal(float value)
        {
            Tase2_IndicationPoint_setReal(self, value);
        }

        public void SetRealQ(float value, DataFlags flags)
        {
            Tase2_IndicationPoint_setRealQ(self, value, (byte)flags);
        }

        public void SetRealQTimeStamp(float value, DataFlags flags, UInt64 timestamp)
        {
            Tase2_IndicationPoint_setRealQTimeStamp(self, value, (byte)flags, timestamp);
        }

        public void SetRealQTimeStamp(float value, DataFlags flags, DateTime timestamp)
        {
            Tase2_IndicationPoint_setRealQTimeStamp(self, value, (byte)flags, DataPoint.msTimeFromDateTime(timestamp));
        }

        public void SetDiscrete(int value)
        {
            Tase2_IndicationPoint_setDiscrete(self, value);
        }

        public void SetDiscreteQ(int value, DataFlags flags)
        {
            Tase2_IndicationPoint_setDiscreteQ(self, value, (byte)flags);
        }

        public void SetDiscreteQTimeStamp(int value, DataFlags flags, UInt64 timestamp)
        {
            Tase2_IndicationPoint_setDiscreteQTimeStamp(self, value, (byte)flags, timestamp);
        }

        public void SetDiscreteQTimeStamp(int value, DataFlags flags, DateTime timestamp)
        {
            Tase2_IndicationPoint_setDiscreteQTimeStamp(self, value, (byte)flags, DataPoint.msTimeFromDateTime(timestamp));
        }

        public void SetState(DataState value)
        {
            Tase2_IndicationPoint_setState(self, (byte)value);
        }

        public void SetStateTimeStamp(DataState value, UInt64 timestamp)
        {
            Tase2_IndicationPoint_setStateTimeStamp(self, (byte)value, timestamp);
        }

        public void SetStateTimeStamp(DataState value, DateTime timestamp)
        {
            Tase2_IndicationPoint_setStateTimeStamp(self, (byte)value, DataPoint.msTimeFromDateTime(timestamp));
        }

        public void SetStateSupplemental(DataStateSupplemental value)
        {
            Tase2_IndicationPoint_setStateSupplemental(self, (byte)value);
        }

        public void SetStateSupplementalQ(DataStateSupplemental value, DataFlags flags)
        {
            Tase2_IndicationPoint_setStateSupplementalQ(self, (byte)value, (byte)flags);
        }

        public void SetStateSupplementalQTimeStamp(DataStateSupplemental value, DataFlags flags, UInt64 timestamp)
        {
            Tase2_IndicationPoint_setStateSupplementalQTimeStamp(self, (byte)value, (byte)flags, timestamp);
        }

        public void SetStateSupplementalQTimeStamp(DataStateSupplemental value, DataFlags flags, DateTime timestamp)
        {
            Tase2_IndicationPoint_setStateSupplementalQTimeStamp(self, (byte)value, (byte)flags, DataPoint.msTimeFromDateTime(timestamp));
        }

        public void SetQuality(DataFlags flags)
        {
            Tase2_IndicationPoint_setQuality(self, (byte)flags);
        }

        public void SetTimeStamp(UInt64 timestamp)
        {
            Tase2_IndicationPoint_setTimeStamp(self, timestamp);
        }

        public void SetTimeStamp(DateTime timestamp)
        {
            Tase2_IndicationPoint_setTimeStamp(self, DataPoint.msTimeFromDateTime(timestamp));
        }

        public void SetCOV(UInt16 cov)
        {
            Tase2_IndicationPoint_setCOV(self, cov);
        }
    }
}

