using System;
using TASE2.Library.Common;
using System.Runtime.InteropServices;

namespace TASE2.Library.Server
{
    /// <summary>
    /// Represents an abstract data point in the server data model. Base class for \ref ControlPoint, \ref IndicationPoint, and
    /// \ref ProtectionEquipment
    /// </summary>
    public class DataPoint
    {

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataPoint_getPointValue(IntPtr self);

        protected DataPoint(IntPtr self, string name, Domain domain)
        {
            this.self = self;
            this.name = name;
            this.domain = domain;
        }

        internal static UInt64 msTimeFromDateTime(DateTime timestamp)
        {
            DateTime epoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
            DateTime timestampUTC = timestamp.ToUniversalTime();

            TimeSpan timeDiff = timestampUTC - epoch;
            ulong timeVal = Convert.ToUInt64(timeDiff.TotalMilliseconds);

            return timeVal;
        }

        internal protected IntPtr self;

        internal IntPtr Self
        {
            get
            {
                return self;
            }
        }

        private string name;

        /// <summary>
        /// Get the name of the data point (variable name visible by the TASE.2 client)
        /// </summary>
        /// <value>the data point name</value>
        public string Name
        {
            get
            {
                return name;
            }
        }

        internal protected Domain domain;

        /// <summary>
        /// Get the \ref Domain of the data point
        /// </summary>
        /// <value>the domain of the data point</value>
        public Domain Domain
        {
            get
            {
                return domain;
            }
        }

        public PointValue GetPointValue()
        {
            IntPtr ptr = Tase2_DataPoint_getPointValue(self);

            if (ptr != IntPtr.Zero)
            {
                PointValue pointValue = new PointValue(ptr);

                pointValue.responsableToRelease = false;

                return pointValue;
            }
            else
                return null;
        }

    }
}

