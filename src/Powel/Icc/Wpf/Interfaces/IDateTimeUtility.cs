using System;

namespace Powel.Wpf.Common
{
    public interface IDateTimeUtility
    {
        string ConvertDateTimeToDateString(DateTime value);
        string ConvertTimeSpanToTimeString(TimeSpan value);
        string ConvertDateTimeToDateTimeString(DateTime value);
        DateTime ConvertDateStringToDateTime(string value);
        TimeSpan ConvertTimeStringToTimeSpan(string value);
        DateTime ConvertDateTimeStringToDateTime(string value);
    }
}
