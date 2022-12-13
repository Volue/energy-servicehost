using System;
using System.Runtime.InteropServices;

namespace TASE2.Library.Server
{
    /********************************************************
    * Types and functions for protection event types
    *********************************************************/

    /// <summary>
    /// Single event flags for \ref ProtectionEquipment.
    /// </summary>
    [Flags]
    public enum SingleEventFlags : byte
    {
        /// <summary>
        /// the elapsed time (operation time/duration time) is not valid.
        /// </summary>
        ELAPSED_TIME_VALIDITY = 1,

        /// <summary>
        /// The value of the protection event is blocked for transmission
        /// </summary>
        BLOCKED = 2,

        /// <summary>
        /// Process value is substitued by an operator or automated source
        /// </summary>
        SUBSTITUTED = 4,

        /// <summary>
        /// most recent update was not successful
        /// </summary>
        TOPICAL = 8,

        /// <summary>
        /// the event value is not valid
        /// </summary>
        EVENT_VALIDITY = 16,

        /// <summary>
        /// unused
        /// </summary>
        UNUSED = 32,

        /// <summary>
        /// The high bit of the event state
        /// </summary>
        EVENT_STATE_HI = 64,

        /// <summary>
        /// The low bit of the event state
        /// </summary>
        EVENT_STATE_LO = 128
    }

    /// <summary>
    /// Event indication flags for \ref ProtectionEquipment of protection class packed (can be either start or trip events)
    /// </summary>
    [Flags]
    public enum EventFlags : byte
    {
        /// <summary>
        /// general event
        /// </summary>
        GENERAL = 1,

        /// <summary>
        /// phase 1 event
        /// </summary>
        PHASE_1 = 2,

        /// <summary>
        /// phase 2 event
        /// </summary>
        PHASE_2 = 4,

        /// <summary>
        /// phase 3 event
        /// </summary>
        PHASE_3 = 8,

        /// <summary>
        /// earth current event
        /// </summary>
        EARTH = 16,

        /// <summary>
        /// reverse direction event
        /// </summary>
        REVERSE = 32
    }

    /// <summary>
    /// Event quality flags for \ref ProtectionEquipment of protection class packed 
    /// </summary>
    [Flags]
    public enum PackedEventFlags : byte
    {
        /// <summary>
        /// the elapsed time (operation time/duration time) is not valid.
        /// </summary>
        ELAPSED_TIME_VALIDITY = 1,

        /// <summary>
        /// The value of the protection event is blocked for transmission
        /// </summary>
        BLOCKED = 2,

        /// <summary>
        /// Process value is substitued by an operator or automated source
        /// </summary>
        SUBSTITUTED = 4,

        /// <summary>
        /// most recent update was not successful
        /// </summary>
        TOPICAL = 8,

        /// <summary>
        /// the event value is not valid
        /// </summary>
        EVENT_VALIDITY = 16
    }

    /// <summary>
    /// Represents a protection equipment in the server data model
    /// </summary>
    /// There are two types of protection equipment (protection classes):
    /// <list>
    /// <item>single: represents a single event</item>
    /// <item>packed: represents events in multi-phase systems</item>
    /// </list>
    public class ProtectionEquipment : DataPoint
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ProtectionEquipment_setSingleEventFlags(IntPtr self, byte eventFlags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ProtectionEquipment_setPackedEvent(IntPtr self, byte packedEvent);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ProtectionEquipment_setPackedEventFlags(IntPtr self, byte flags);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ProtectionEquipment_setOperatingTime(IntPtr self, Int32 operatingTime);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ProtectionEquipment_setEventTime(IntPtr self, UInt64 eventTime);

        internal ProtectionEquipment(IntPtr self, string name, Domain domain) : base(self, name, domain)
        {
        }

        /// <summary>
        /// Sets the event/flags for the single event protection class
        /// </summary>
        /// <param name="flags">event flags</param>
        public void SetSingleEventFlags(SingleEventFlags flags)
        {
            Tase2_ProtectionEquipment_setSingleEventFlags(self, (byte)flags);
        }

        /// <summary>
        /// Sets the event indication flags for the packed event protection class
        /// </summary>
        /// <param name="packedEvent">event indication flags</param>
        public void SetPackedEvent(EventFlags packedEvent)
        {
            Tase2_ProtectionEquipment_setPackedEvent(self, (byte)packedEvent);
        }

        /// <summary>
        /// Sets the event flags for the packed event protection class
        /// </summary>
        /// <param name="flags">event flags</param>
        public void SetPackedEventFlags(PackedEventFlags flags)
        {
            Tase2_ProtectionEquipment_setPackedEventFlags(self, (byte)flags);
        }

        /// <summary>
        /// Sets the operating/duration time of an event
        /// </summary>
        /// <param name="operatingTime">operating time</param>
        public void SetOperatingTime(int operatingTime)
        {
            Tase2_ProtectionEquipment_setOperatingTime(self, operatingTime);
        }

        /// <summary>
        /// Sets the event time (time when the event happend)
        /// </summary>
        /// <param name="eventTime">event time as 64 bit milliseconds since epoch</param>
        public void SetEventTime(UInt64 eventTime)
        {
            Tase2_ProtectionEquipment_setEventTime(self, eventTime);
        }

        /// <summary>
        /// Sets the event time (time when the event happend)
        /// </summary>
        /// <param name="eventTime">event time as \ref System.DateTime</param>
        public void SetEventTime(DateTime eventTime)
        {
            Tase2_ProtectionEquipment_setEventTime(self, DataPoint.msTimeFromDateTime(eventTime));
        }
    }
}

