using System;
using System.Runtime.InteropServices;
using TASE2.Library.Client;
using TASE2.Library.Server;

namespace TASE2.Library.Common
{
    /// <summary>
    /// The quality class of the indication point (\ref IndicationPoint).
    /// </summary>
    public enum QualityClass
    {
        /// <summary>
        /// the indication point has no quality indicator
        /// </summary>
        NO_QUALITY = 0,

        /// <summary>
        /// the indication point has a quality indicator
        /// </summary>
        QUALITY
    }

    /// <summary>
    /// Time stamp class.
    /// </summary>
    public enum TimeStampClass
    {
        /// <summary>
        /// the indication point has no time stamp
        /// </summary>
        NO_TIMESTAMP = 0,

        /// <summary>
        /// the indication point has a 32 bit timestamp (only seconds resolution)
        /// </summary>
        TIMESTAMP,

        /// <summary>
        /// the indication point has a 64 bit timestamp
        /// </summary>
        TIMESTAMP_EXTENDED
    }


    /// <summary>
    /// State value. Values can have other meanings as well.
    /// See section 8.2 ("Use of IndicationPoint Model") of IEC 60870-6-802.
    /// </summary>
    public enum StateValue
    {
        BETWEEN = 0,
        OFF = 1,
        ON = 2,
        INVALID = 3
    }

    /// <summary>
    /// Validity of the value of an IndicationPoint
    /// </summary>
    public enum Validity
    {
        VALID = 0,
        HELD = 1,
        SUSPECT = 2,
        NOT_VALID = 3
    }

    /// <summary>
    /// Source of the value of an IndicationPoint
    /// </summary>
    public enum Source
    {
        TELEMETERED = 0,
        CALCULATED = 1,
        ENTERED = 2,
        ESTIMATED = 3
    }

    public enum NormalValue
    {
        NORMAL = 0,
        ABNORMAL = 1
    }

    public static class Extensions
    {
        public static StateValue GetStateValue(this DataState state)
        {
            int value = ((state & DataState.LO) > 0) ? 1 : 0;
            value += ((state & DataState.HI) > 0) ? 2 : 0;

            return (StateValue)value;
        }

        public static Validity GetValidity(this DataState state)
        {
            int value = ((state & DataState.VALIDITY_LO) > 0) ? 1 : 0;
            value += ((state & DataState.VALIDITY_HI) > 0) ? 2 : 0;

            return (Validity)value;
        }

        public static Source GetCurrentSource(this DataState state)
        {
            int value = ((state & DataState.CURRENT_SOURCE_LO) > 0) ? 1 : 0;
            value += ((state & DataState.CURRENT_SOURCE_HI) > 0) ? 2 : 0;

            return (Source)value;
        }

        public static NormalValue GetNormalValue(this DataState state)
        {
            int value = ((state & DataState.NORMAL_VALUE) > 0) ? 1 : 0;

            return (NormalValue)value;
        }

        public static Validity GetValidity(this DataFlags state)
        {
            int value = ((state & DataFlags.VALIDITY_LO) > 0) ? 1 : 0;
            value += ((state & DataFlags.VALIDITY_HI) > 0) ? 2 : 0;

            return (Validity)value;
        }

        public static Source GetCurrentSource(this DataFlags state)
        {
            int value = ((state & DataFlags.CURRENT_SOURCE_LO) > 0) ? 1 : 0;
            value += ((state & DataFlags.CURRENT_SOURCE_HI) > 0) ? 2 : 0;

            return (Source)value;
        }

        public static NormalValue GetNormalValue(this DataFlags state)
        {
            int value = ((state & DataFlags.NORMAL_VALUE) > 0) ? 1 : 0;

            return (NormalValue)value;
        }

        public static StateValue GetStateValue(this DataStateSupplemental state)
        {
            int value = ((state & DataStateSupplemental.LO) > 0) ? 1 : 0;
            value += ((state & DataStateSupplemental.HI) > 0) ? 2 : 0;

            return (StateValue)value;
        }

        public static TagValue GetTagValue(this DataStateSupplemental state)
        {
            int value = ((state & DataStateSupplemental.TAG_LO) > 0) ? 1 : 0;
            value += ((state & DataStateSupplemental.TAG_HI) > 0) ? 2 : 0;

            return (TagValue)value;
        }

        public static StateValue GetExpectedStateValue(this DataStateSupplemental state)
        {
            int value = ((state & DataStateSupplemental.LO) > 0) ? 1 : 0;
            value += ((state & DataStateSupplemental.HI) > 0) ? 2 : 0;

            return (StateValue)value;
        }

        public static StateValue GetEventState(this SingleEventFlags flags)
        {
            int value = ((flags & SingleEventFlags.EVENT_STATE_LO) > 0) ? 1 : 0;
            value += ((flags & SingleEventFlags.EVENT_STATE_HI) > 0) ? 2 : 0;

            return (StateValue)value;
        }
    }

    [Flags]
    public enum DataFlags : byte
    {
        UNUSED_0 = 1,
        UNUSED_1 = 2,
        VALIDITY_HI = 4,
        VALIDITY_LO = 8,
        CURRENT_SOURCE_HI = 16,
        CURRENT_SOURCE_LO = 32,
        NORMAL_VALUE = 64,
        TIME_STAMP_QUALITY = 128
    }

    [Flags]
    public enum DataState : byte
    {
        HI = 1,
        LO = 2,
        VALIDITY_HI = 4,
        VALIDITY_LO = 8,
        CURRENT_SOURCE_HI = 16,
        CURRENT_SOURCE_LO = 32,
        NORMAL_VALUE = 64,
        TIME_STAMP_QUALITY = 128
    }

    /// <summary>
    /// Represents the value of DataStateSupplemental including the value, tag, and expected value
    /// </summary>
    [Flags]
    public enum DataStateSupplemental : byte
    {
        HI = 1,
        LO = 2,
        TAG_HI = 4,
        TAG_LO = 8,
        EXPECTED_HI = 16,
        EXPECTED_LO = 32,
    }

    /// <summary>
    /// Represents the value and quality flags of a single event
    /// </summary>
    [Flags]
    public enum SingleEventFlags : byte
    {
        ELAPSED_TIME_VALIDITY = 1,
        BLOCKED = 2,
        SUBSTITUTED = 4,
        TOPICAL = 8,
        EVENT_VALIDITY = 16,
        UNUSED = 32,
        EVENT_STATE_HI = 64,
        EVENT_STATE_LO = 128
    }

    /// <summary>
    /// Represents that value of a packed event
    /// </summary>
    [Flags]
    public enum EventFlags : byte
    {
        GENERAL = 1,
        PHASE_1 = 2,
        PHASE_2 = 4,
        PHASE_3 = 8,
        EARTH = 16,
        REVERSE = 32
    }

    /// <summary>
    /// Represents quality flags of a packed event
    /// </summary>
    [Flags]
    public enum PackedEventFlags : byte
    {
        ELAPSED_TIME_VALIDITY = 1,
        BLOCKED = 2,
        SUBSTITUTED = 4,
        TOPICAL = 8,
        EVENT_VALIDITY = 16
    }

    /// <summary>
    /// Point value type that was guessed by examining the MMS data structure
    /// </summary>
    public enum PointValueType
    {
        /// <summary>
        /// Type is a state value
        /// </summary>
        STATE = 0,

        /// <summary>
        /// Type is a STATE_SUPPLEMENTAL value
        /// </summary>
        STATE_SUPPLEMENTAL,

        /// <summary>
        /// Type is a discrete (integer) value
        /// </summary>
        DISCRETE,

        /// <summary>
        /// Type is a REAL (float) value
        /// </summary>
        REAL,

        /// <summary>
        /// Type is a single protection event
        /// </summary>
        SINGLE_EVENT,

        /// <summary>
        /// Type is a packed protection event
        /// </summary>
        PACKED_EVENT,

        /// <summary>
        /// Point value instance represents an error (no point value received)
        /// </summary>
        ERROR = 99,

        /// <summary>
        /// The type could not be inferred
        /// </summary>
        UNKNOWN = 100,

        /// <summary>
        /// Type is one of STATE or STATE_SUPPLEMENTAL
        /// </summary>
        STATE_OR_STATE_SUPPLEMENTAL = 101,

        /// <summary>
        /// Type is one of STATE or DISCRETE
        /// </summary>
        STATE_OR_DISCRETE = 102,
    }

    /// <summary>
    /// Represents a value of a data point
    /// </summary>
    public class PointValue : IDisposable
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_getCopy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_PointValue_getType(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setType(IntPtr self, int type);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createReal(float value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createDiscrete(int value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createState(DataState value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateSupplemental(DataStateSupplemental value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createRealQ(float value, DataFlags flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateQ(DataState value, DataFlags flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createDiscreteQ(int value, DataFlags flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateSupplementalQ(DataStateSupplemental value, DataFlags flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createRealQTimeTag(float value, DataFlags flags, int timeTag);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateQTimeTag(DataFlags valueAndFlags, int timeTag);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createDiscreteQTimeTag(int value, DataFlags flags, int timeTag);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateSupplementalQTimeTag(DataStateSupplemental value, DataFlags flags, int timeTag);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createRealExtended(float value, DataFlags flags, int timeTag, UInt16 cov);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateExtended(DataFlags valueAndFlags, int timeTag, UInt16 cov);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createDiscreteExtended(int value, DataFlags flags, int timeTag, UInt16 cov);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateSupplementalExtended(DataStateSupplemental value, DataFlags flags, int timeTag, UInt16 cov);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createRealQTimeTagExtended(float value, DataFlags flags, UInt64 timeTagExtended);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateQTimeTagExtended(DataFlags valueAndFlags, UInt64 timeTagExtended);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createDiscreteQTimeTagExtended(int value, DataFlags flags, UInt64 timeTagExtended);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_PointValue_createStateSupplementalQTimeTagExtended(DataStateSupplemental value, DataFlags flags, UInt64 timeTagExtended);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_PointValue_getError(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern float Tase2_PointValue_getValueReal(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setValueReal(IntPtr self, float value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_PointValue_getValueDiscrete(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setValueDiscrete(IntPtr self, int value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern DataState Tase2_PointValue_getValueState(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setValueState(IntPtr self, DataState value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern DataStateSupplemental Tase2_PointValue_getValueStateSupplemental(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setValueStateSupplemental(IntPtr self, DataStateSupplemental value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool Tase2_PointValue_hasTimeStamp(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_PointValue_getTimeStampClass(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt64 Tase2_PointValue_getTimeStamp(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setTimeStamp(IntPtr self, UInt64 timestamp);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern DataFlags Tase2_PointValue_getFlags(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setFlags(IntPtr self, DataFlags flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt16 Tase2_PointValue_getCOV(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_setCOV(IntPtr self, UInt16 cov);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_PointValue_destroy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr
        Tase2_PointValue_createSingleProtectionEvent(SingleEventFlags eventFlags, Int32 operatingTime, UInt64 eventTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr
        Tase2_PointValue_createPackedProtectionEvent(EventFlags packedEvent, PackedEventFlags packedEventFlags, Int32 operatingTime, UInt64 eventTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern SingleEventFlags
        Tase2_PointValue_getSingleEventFlags(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void
        Tase2_PointValue_setSingleEventFlags(IntPtr self, SingleEventFlags eventFlags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern EventFlags
        Tase2_PointValue_getPackedEvent(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void
        Tase2_PointValue_setPackedEvent(IntPtr self, EventFlags packedEvent);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern PackedEventFlags
        Tase2_PointValue_getPackedEventFlags(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void
        Tase2_PointValue_setPackedEventFlags(IntPtr self, PackedEventFlags flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32
        Tase2_PointValue_getOperatingTime(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void
        Tase2_PointValue_setOperatingTime(IntPtr self, Int32 operatingTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt64
        Tase2_PointValue_getEventTime(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void
        Tase2_PointValue_setEventTime(IntPtr self, UInt64 eventTime);

        private IntPtr self;
        internal bool responsableToRelease = true;

        internal IntPtr Self
        {
            get
            {
                return self;
            }
        }

        internal PointValue(IntPtr pointValuePtr, bool clone)
        {
            if (clone)
                self = Tase2_PointValue_getCopy(pointValuePtr);
            else
                self = pointValuePtr;
        }

        internal PointValue(IntPtr pointValuePtr)
        {
            self = pointValuePtr;
        }

        public void Dispose()
        {
            lock (this)
            {
                if (self != IntPtr.Zero)
                {
                    if (responsableToRelease)
                    {
                        Tase2_PointValue_destroy(self);
                        self = IntPtr.Zero;
                    }
                }
            }
        }

        ~PointValue()
        {
            Dispose();
        }

        /// <summary>
        /// Gets or sets the TASE.2 type of the value
        /// </summary>
        /// <remarks>
        /// The elements of a point value can only be interpreted correctly when the TASE.2 type is known.
        /// In most cases the TASE.2 type can be derived from the MMS type of the MMS value that has been received.
        /// However, in some cases this is not possible because the same MMS type is used for different TASE.2 types.
        /// In this case it is required to tell the library what TASE.2 type has to be assumed to interpret the value elements.
        /// When you get a type UNKNOWN, STATE_OR_STATE_SUPPLEMENTAL, or
        /// STATE_OR_DISCRETE you have to use this function to advise the correct type.
        /// </remarks>
        /// <value>The estimated or set type</value>
        public PointValueType Type
        {
            get
            {
                return (PointValueType)Tase2_PointValue_getType(self);
            }
            set
            {
                Tase2_PointValue_setType(self, (int)value);
            }
        }

        /// <summary>
        /// Gets the error code (when point value is of type PointValueType.ERROR)
        /// </summary>
        /// <value>The error code</value>
        public ClientError Error
        {
            get
            {
                int errorVal = Tase2_PointValue_getError(self);

                return (ClientError)errorVal;
            }
        }

        public PointValue Clone()
        {
            return new PointValue(Tase2_PointValue_getCopy(self));
        }

        public static PointValue CreateReal(float value)
        {
            return new PointValue(Tase2_PointValue_createReal(value));
        }

        public static PointValue CreateDiscrete(int value)
        {
            return new PointValue(Tase2_PointValue_createDiscrete(value));
        }

        public static PointValue CreateState(DataState value)
        {
            return new PointValue(Tase2_PointValue_createState(value));
        }

        public static PointValue CreateStateSupplemental(DataStateSupplemental value)
        {
            return new PointValue(Tase2_PointValue_createStateSupplemental(value));
        }

        public static PointValue CreateRealQ(float value, DataFlags flags)
        {
            return new PointValue(Tase2_PointValue_createRealQ(value, flags));
        }

        public static PointValue CreateStateQ(DataState value, DataFlags flags)
        {
            return new PointValue(Tase2_PointValue_createStateQ(value, flags));
        }

        public static PointValue CreateDiscreteQ(int value, DataFlags flags)
        {
            return new PointValue(Tase2_PointValue_createDiscreteQ(value, flags));
        }

        public static PointValue CreateStateSupplementalQ(DataStateSupplemental value, DataFlags flags)
        {
            return new PointValue(Tase2_PointValue_createStateSupplementalQ(value, flags));
        }

        public static PointValue CreateRealQTimeTag(float value, DataFlags flags, Int32 timeTag)
        {
            return new PointValue(Tase2_PointValue_createRealQTimeTag(value, flags, timeTag));
        }

        public static PointValue CreateStateQTimeTag(DataFlags valueAndFlags, Int32 timeTag)
        {
            return new PointValue(Tase2_PointValue_createStateQTimeTag(valueAndFlags, timeTag));
        }

        public static PointValue CreateDiscreteQTimeTag(int value, DataFlags flags, Int32 timeTag)
        {
            return new PointValue(Tase2_PointValue_createDiscreteQTimeTag(value, flags, timeTag));
        }

        public static PointValue CreateStateSupplementalQTimeTag(DataStateSupplemental value, DataFlags flags, Int32 timeTag)
        {
            return new PointValue(Tase2_PointValue_createStateSupplementalQTimeTag(value, flags, timeTag));
        }

        public static PointValue CreateRealExtended(float value, DataFlags flags, Int32 timeTag, UInt16 cov)
        {
            return new PointValue(Tase2_PointValue_createRealExtended(value, flags, timeTag, cov));
        }

        public static PointValue CreateStateExtended(DataFlags valueAndFlags, Int32 timeTag, UInt16 cov)
        {
            return new PointValue(Tase2_PointValue_createStateExtended(valueAndFlags, timeTag, cov));
        }

        public static PointValue CreateDiscreteExtended(int value, DataFlags flags, Int32 timeTag, UInt16 cov)
        {
            return new PointValue(Tase2_PointValue_createDiscreteExtended(value, flags, timeTag, cov));
        }

        public static PointValue CreateStateSupplementalExtended(DataStateSupplemental value, DataFlags flags, Int32 timeTag, UInt16 cov)
        {
            return new PointValue(Tase2_PointValue_createStateSupplementalExtended(value, flags, timeTag, cov));
        }

        public static PointValue CreateRealQTimeTagExtended(float value, DataFlags flags, UInt64 timeTag)
        {
            return new PointValue(Tase2_PointValue_createRealQTimeTagExtended(value, flags, timeTag));
        }

        public static PointValue CreateStateQTimeTagExtended(DataFlags valueAndFlags, UInt64 timeTag)
        {
            return new PointValue(Tase2_PointValue_createStateQTimeTagExtended(valueAndFlags, timeTag));
        }

        public static PointValue CreateDiscreteQTimeTagExtended(int value, DataFlags flags, UInt64 timeTag)
        {
            return new PointValue(Tase2_PointValue_createDiscreteQTimeTagExtended(value, flags, timeTag));
        }

        public static PointValue CreateStateSupplementalQTimeTagExtended(DataStateSupplemental value, DataFlags flags, UInt64 timeTag)
        {
            return new PointValue(Tase2_PointValue_createStateSupplementalQTimeTagExtended(value, flags, timeTag));
        }

        /// <summary>
        /// Creates a new PointValue instance of type single event.
        /// </summary>
        /// <returns>The new PointValue instance</returns>
        /// <param name="flags">event value and quality flags</param>
        /// <param name="operatingTime">Operating time.</param>
        /// <param name="eventTime">Event time.</param>
        public static PointValue CreateSingleEvent(SingleEventFlags flags, Int32 operatingTime, UInt64 eventTime)
        {
            return new PointValue(Tase2_PointValue_createSingleProtectionEvent(flags, operatingTime, eventTime));
        }

        /// <summary>
        /// Creates a new PointValue instance of type packed event.
        /// </summary>
        /// <returns>The new PointValue instance</returns>
        /// <param name="value">Value.</param>
        /// <param name="flags">Flags.</param>
        /// <param name="operatingTime">Operating time.</param>
        /// <param name="eventTime">Event time.</param>
        public static PointValue CreatePackedEvent(EventFlags value, PackedEventFlags flags, Int32 operatingTime, UInt64 eventTime)
        {
            return new PointValue(Tase2_PointValue_createPackedProtectionEvent(value, flags, operatingTime, eventTime));
        }

        /// <summary>
        /// Gets or sets the real (floating point) value.
        /// </summary>
        /// <value>The value of type real (floating point).</value>
        public float ValueReal
        {
            get
            {
                return Tase2_PointValue_getValueReal(self);
            }
            set
            {
                Tase2_PointValue_setValueReal(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the discrete value.
        /// </summary>
        /// <value>The discrete value</value>
        public int ValueDiscrete
        {
            get
            {
                return Tase2_PointValue_getValueDiscrete(self);
            }
            set
            {
                Tase2_PointValue_setValueDiscrete(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the state value.
        /// </summary>
        /// <value>The state value.</value>
        public DataState ValueState
        {
            get
            {
                return Tase2_PointValue_getValueState(self);
            }
            set
            {
                Tase2_PointValue_setValueState(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the state supplemental value.
        /// </summary>
        /// <value>The state supplemental value.</value>
        public DataStateSupplemental ValueStateSupplemental
        {
            get
            {
                return Tase2_PointValue_getValueStateSupplemental(self);
            }
            set
            {
                Tase2_PointValue_setValueStateSupplemental(self, value);
            }
        }

        /// <summary>
        /// Gets a value indicating whether this instance has a time stamp.
        /// </summary>
        /// <value><c>true</c> if this instance has time stamp; otherwise, <c>false</c>.</value>
        public bool HasTimeStamp
        {
            get
            {
                return Tase2_PointValue_hasTimeStamp(self);
            }
        }

        public TimeStampClass TimeStampClass
        {
            get
            {
                return (TimeStampClass)Tase2_PointValue_getTimeStampClass(self);
            }
        }

        /// <summary>
        /// Gets or sets the time stamp in ms since 01.01.1970 UTC
        /// </summary>
        /// <value>The time stamp value</value>
        public UInt64 TimeStamp
        {
            get
            {
                return Tase2_PointValue_getTimeStamp(self);
            }
            set
            {
                Tase2_PointValue_setTimeStamp(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the flags.
        /// </summary>
        /// <value>The flags.</value>
        public DataFlags Flags
        {
            get
            {
                return Tase2_PointValue_getFlags(self);
            }
            set
            {
                Tase2_PointValue_setFlags(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the COV (Counter of value changes)
        /// </summary>
        /// <value>The COV value</value>
        public UInt16 COV
        {
            get
            {
                return Tase2_PointValue_getCOV(self);
            }
            set
            {
                Tase2_PointValue_setCOV(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the state value and quality flags of a single event
        /// </summary>
        /// <value>The single event flags value</value>
        public SingleEventFlags SingleEventFlags
        {
            get
            {
                return Tase2_PointValue_getSingleEventFlags(self);
            }
            set
            {
                Tase2_PointValue_setSingleEventFlags(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the value of a packed event.
        /// </summary>
        /// <value>The packed event value</value>
        public EventFlags PackedEvent
        {
            get
            {
                return Tase2_PointValue_getPackedEvent(self);
            }
            set
            {
                Tase2_PointValue_setPackedEvent(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the quality flags of a packed event.
        /// </summary>
        /// <value>The packed event quality flags.</value>
        public PackedEventFlags PackedEventFlags
        {
            get
            {
                return Tase2_PointValue_getPackedEventFlags(self);
            }
            set
            {
                Tase2_PointValue_setPackedEventFlags(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the operating time (event duration, or duration time) of a single or packed event.
        /// 
        /// The "operating time" is the time in milliseconds from the start of operation until the first
        /// command to an output circuit was issued.
        /// 
        /// The "event duration" or "duration time" is the total time the fault was detected or the time 
        /// between the start of operation and the execution of the trip command.
        /// </summary>
        /// <value>The operating time.</value>
        public int OperatingTime
        {
            get
            {
                return Tase2_PointValue_getOperatingTime(self);
            }
            set
            {
                Tase2_PointValue_setOperatingTime(self, value);
            }
        }

        /// <summary>
        /// Gets or sets the event time (start time, or trip time) of a single or packed event.
        /// The event time is the time of the start of the operation.
        /// 
        /// For a packed event of class "START" it is the start time. For a packed event of
        /// class "TRIP" it is the trip time.
        /// </summary>
        /// <value>The event time in ms since 01.01.1970 00:00 UTC.</value>
        public UInt64 EventTime
        {
            get
            {
                return Tase2_PointValue_getEventTime(self);
            }
            set
            {
                Tase2_PointValue_setEventTime(self, value);
            }
        }
    }
}

