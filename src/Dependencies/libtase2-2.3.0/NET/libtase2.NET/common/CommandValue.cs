using System;

namespace TASE2.Library.Common
{
    /// <summary>
    /// Values for the device command (e.g. to be used by the SendCommand method of Client)
    /// </summary>
    public static class CommandValue
    {
        public const Int16 TRIP = 0;
        public const Int16 CLOSE = 1;
        public const Int16 OPEN = 0;
        public const Int16 OFF = 0;
        public const Int16 ON = 1;
        public const Int16 LOWER = 0;
        public const Int16 RAISE = 1;
    }

}

