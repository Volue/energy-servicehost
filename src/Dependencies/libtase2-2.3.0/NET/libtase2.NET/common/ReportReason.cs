using System;

namespace TASE2.Library.Common
{
    /// <summary>
    /// Reason for data set transfer set reports
    /// </summary>
    [Flags]
    public enum ReportReason : int
    {
        /// <summary>
        /// Interval timeout elapsed
        /// </summary>
        INTERVAL_TIMEOUT = 1,

        /// <summary>
        /// Integrity period
        /// </summary>
        INTEGRITY_TIMEOUT = 2,

        /// <summary>
        /// object changed value of quality
        /// </summary>
        OBJECT_CHANGE = 4,

        /// <summary>
        /// report was requested by an operator
        /// </summary>
        OPERATOR_REQUESTED = 8,

        /// <summary>
        /// other external event caused the report
        /// </summary>
        OTHER_EXTERNAL_EVENT = 16
    }
}

