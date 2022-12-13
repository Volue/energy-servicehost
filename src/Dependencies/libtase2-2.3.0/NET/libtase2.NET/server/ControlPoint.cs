using System;
using System.Runtime.InteropServices;
using TASE2.Library.Common;

namespace TASE2.Library.Server
{

    /// <summary>
    /// The type of the \ref ControlPoint (device)
    /// </summary>
    public enum ControlPointType
    {
        /// <summary>
        /// The device can handle commands (e.g. single-point commands)
        /// </summary>
        COMMAND = 0,

        /// <summary>
        /// The device can handle REAL (floating point) set points
        /// </summary>
        REAL = 1,

        /// <summary>
        /// The device can handle DISCRETE (32 bit integer) set points
        /// </summary>
        DISCRETE = 2
    }

    /// <summary>
    /// Represents a control point (device) in the server data model.
    /// </summary>
    public class ControlPoint : DataPoint
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_ControlPoint_getTag(IntPtr self, out string reason, out IntPtr tagOwner);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_ControlPoint_setTag(IntPtr self, int value, string reason, IntPtr tagOwner);

        internal ControlPoint(IntPtr self, string name, Domain domain) : base(self, name, domain)
        {
        }

        /// <summary>
        /// Gets the tag value and additional tag information.
        /// </summary>
        /// <remarks>>Only when the device supports tags. Otherwise TagValue.INVALID will be returned!</remarks>
        /// <returns>the tag value</returns>
        /// <param name="reason">the reason of the tag</param>
        /// <param name="tagOwner">the BLT of the owner of the tag</param>
        public TagValue GetTag(out string reason, out BilateralTable tagOwner)
        {
            tagOwner = null;
            reason = "unknown";

            IntPtr tagOwnerRef;

            TagValue tagValue = (TagValue)Tase2_ControlPoint_getTag(self, out reason, out tagOwnerRef);

            if (tagOwnerRef != IntPtr.Zero)
            {
                tagOwner = BilateralTable.GetInstance(tagOwnerRef);
            }

            return tagValue;
        }

        /// <summary>
        /// Set the tag value and additional tag information locally.
        /// </summary>
        /// <param name="value">the tag value</param>
        /// <param name="reason">the reason of the tag as a human readable message</param>
        /// <param name="tagOwner">should be null to indicate that the tag was set locally (cannot overwritten by remote client)</param>
        public void SetTag(TagValue value, string reason, BilateralTable tagOwner)
        {
            Tase2_ControlPoint_setTag(self, (int)value, reason, (tagOwner == null) ? IntPtr.Zero : tagOwner.Self);
        }
    }
}

