using System;

namespace TASE2.Library.Common
{
    /// <summary>
    /// Represents the supported features of a server
    /// </summary>
    [Flags]
    public enum SupportedFeatures : int
    {
        FB_1 = 1,
        FB_2 = 2,
        FB_3 = 4,
        FB_4 = 8,
        FB_5 = 16,
        FB_6 = 32,
        FB_7 = 64,
        FB_8 = 128,
        FB_9 = 256,
        FB_10 = 512
    }
}

