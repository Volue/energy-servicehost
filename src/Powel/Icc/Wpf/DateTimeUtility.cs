using System;
using System.Configuration;
using System.Globalization;

namespace Powel.Wpf.Common
{
    public class DateTimeUtility : IDateTimeUtility
    {
        private static DateTimeUtility _current;

        private readonly string _dateTimeStringFormat;
        private readonly string _dateStringFormat;
        private readonly string _timeStringFormat;
        private readonly CultureInfo _cultureInfo;

        public static DateTimeUtility Current
        {
            get
            {
                if(_current == null)
                {
                    _current = new DateTimeUtility(CultureInfo.CurrentCulture,
                        ConfigurationManager.AppSettings["DateTimeStringFormat"] ?? string.Empty,
                        ConfigurationManager.AppSettings["DateStringFormat"] ?? string.Empty,
                        ConfigurationManager.AppSettings["TimeStringFormat"] ?? string.Empty);
                }

                return _current;
            }
        }

        public DateTimeUtility(CultureInfo cultureInfo) : this(cultureInfo, string.Empty, string.Empty, string.Empty)
        {
        }

        public DateTimeUtility(string dateTimeStringFormat, string dateStringFormat, string timeStringFormat) : this(CultureInfo.InvariantCulture, dateTimeStringFormat, dateStringFormat, timeStringFormat)
        {
        }

        public DateTimeUtility(CultureInfo cultureInfo, string dateTimeStringFormat, string dateStringFormat, string timeStringFormat)
        {
            _cultureInfo = cultureInfo;
            _dateTimeStringFormat = dateTimeStringFormat;
            _dateStringFormat = dateStringFormat;
            _timeStringFormat = timeStringFormat;
        }

        public string ConvertDateTimeToDateString(DateTime value)
        {
            string result;

            if(_dateStringFormat.Length > 0)
            {
                result = value.ToString(_dateStringFormat);
            }
            else
            {
                result = value.ToString("d", _cultureInfo);
            }

            return result;
        }

        public string ConvertTimeSpanToTimeString(TimeSpan value)
        {
            string result;
            DateTime temp = new DateTime().Add(value);

            if(_timeStringFormat.Length > 0)
            {
                result = temp.ToString(_timeStringFormat);
            }
            else
            {
                result = temp.ToString("t", _cultureInfo);
            }

            return result;
        }

        public string ConvertDateTimeToDateTimeString(DateTime value)
        {
            string result;

            if (_dateTimeStringFormat.Length > 0)
            {
                result = value.ToString(_dateTimeStringFormat);
            }
            else
            {
                result = value.ToString("g", _cultureInfo);
            }

            return result;
        }

        public DateTime ConvertDateStringToDateTime(string value)
        {
            DateTime result;

            if (_dateStringFormat.Length > 0)
            {
                result = DateTime.ParseExact(value, _dateStringFormat, CultureInfo.InvariantCulture, DateTimeStyles.None);
            }
            else
            {
                result = DateTime.ParseExact(value, "d", _cultureInfo, DateTimeStyles.None);
            }

            return result;
        }

        public TimeSpan ConvertTimeStringToTimeSpan(string value)
        {
            DateTime temp;

            if(_timeStringFormat.Length > 0)
            {
                temp = DateTime.ParseExact(value, _timeStringFormat, CultureInfo.InvariantCulture, DateTimeStyles.None);
            }
            else
            {
                temp = DateTime.ParseExact(value, "t", _cultureInfo, DateTimeStyles.None);
            }

            return new TimeSpan(0, temp.Hour, temp.Minute, 0);
        }

        public DateTime ConvertDateTimeStringToDateTime(string value)
        {
            DateTime result;

            if (_dateTimeStringFormat.Length > 0)
            {
                result = DateTime.ParseExact(value, _dateTimeStringFormat, CultureInfo.InvariantCulture, DateTimeStyles.None);
            }
            else
            {
                result = DateTime.ParseExact(value, "g", _cultureInfo, DateTimeStyles.None);
            }

            return result;
        }
    }
}
