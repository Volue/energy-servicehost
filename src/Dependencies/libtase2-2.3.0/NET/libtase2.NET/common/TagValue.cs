using System;

namespace TASE2.Library.Common
{
    public enum TagValue
    {
        /** No tag present */
        NO_TAG = 0,

        /** The tag prohibits to open or close the device */
        OPEN_AND_CLOSE_INHIBIT = 1,

        /** Tag tag allow to open but inhibits to close the device */
        CLOSE_ONLY_INHIBIT = 2,

        /** The device has an invalid tag */
        INVALID = 3
    }
}

