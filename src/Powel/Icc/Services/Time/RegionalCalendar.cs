using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using log4net;

namespace Powel.Icc.Services.Time 
{
	/// <summary>	
	/// WeekOneRule determines how the RegionalCalendar
	/// interprets and calculates weeknumbers. There
	/// are currently 3 known rules:
	/// FirstJanWeek: Week 1 is the week where 1.jan is located
	/// FirstFullWeek: Week 1 is the first full week in the year
	/// First4DayWeek: Week 1 is the first week with 4+ days in the new year
	/// </summary>
	public enum WeekOneRule { FirstJanWeek,FirstFullWeek,First4DayWeek };

    /// <summary>
    /// Takes care of julian day calculations, 
    /// i.e. mapping from day linear scale in ticks to y,m,d greg.cal.
    /// </summary>
    public class Julian 
	{
		/// <summary>
		/// The Offset in days from julian day 0 to the time reference zero 1970 Utc
		/// </summary>
		//private const long JulianDay1970= 1721426L;// year 1 1970 was 2440588L;
		//private const long JulianDay1970Ticks=JulianDay1970*Utc.Day;
		internal const long ReferenceDay=1721426L;// reference year 1 gives 1721426, reference year 1970 gives 2440588L.
		internal const long ReferenceTicks=ReferenceDay*Utc.Day; // above number calculated in thicks
		/// <summary>
		/// Converts calendar units year, month and day of month into
		/// a julian date number
		/// </summary>
		/// <param name="year"></param>
		/// <param name="month"></param>
		/// <param name="day"></param>
		/// <returns>julian date number</returns>
		static public long YMD2j(int year,int month,int day) 
		{
			return ((day) - 32075L + 1461L * ((year) + 4800 + ((month) - 14L) / 12L) / 
				4L + 367L * ((month) - 2L - ((month) - 14L) / 12L * 12L) / 12L - 3L * (((year) + 4900L + 
				((month) - 14L) / 12L) / 100L) / 4L);
		}
		/// <summary>
		/// Converts a julian date int its year,month and day of month
		/// calendar units
		/// </summary>
		/// <param name="jul_date"></param>
		/// <param name="year"></param>
		/// <param name="month"></param>
		/// <param name="day"></param>
		static public void j2YMD( long jul_date, out int year ,out int month,out int day) 
		{ 
			long  t1,     t2,      yr,       mo;
			t1 = (jul_date) + 68569L;   t2 = 4L * t1 / 146097L;
			t1 = t1 - (146097L * t2 + 3L) / 4L;   yr = 4000L * (t1 + 1) / 1461001L;
			t1 = t1 - 1461L * yr / 4L + 31;   mo = 80L * t1 / 2447L;
			day = (int)(t1 - 2447L * mo / 80L);  t1 = mo / 11L;
			month = (int)(mo + 2L - 12L * t1);   year = (int)(100L * (t2 - 49L) + yr + t1);
		}
		/// <summary>
		/// Calculate the year, given a julian date
		/// </summary>
		/// <param name="jul_date"></param>
		/// <returns>calculated year</returns>
		static public int j2Y(long jul_date) 
		{
			long  t1,t2,yr,mo;
			t1 = (jul_date) + 68569L;   t2 = 4L * t1 / 146097L;
			t1 = t1 - (146097L * t2 + 3L) / 4L;   yr = 4000L * (t1 + 1) / 1461001L;
			t1 = t1 - 1461L * yr / 4L + 31;   mo = 80L * t1 / 2447L;
			t1 = mo / 11L;
			return (int)(100L * (t2 - 49L) + yr + t1);
		}
		/// <summary>
		/// Convert a year to its julian date number
		/// </summary>
		/// <param name="year"></param>
		/// <returns></returns>
		static public long Y2j(int year) 
		{ 
			return ( -31738L + 1461L * ((year) + 4799) / 4L  - 3L * (((year) + 4899L ) / 100L) / 4L);
		}
		/// <summary>
		/// Convert a julianDay number to UTCTime.ticks value
		/// </summary>
		/// <param name="julianDay"></param>
		/// <returns>UtcTime ticks value</returns>
		static public long j2ticks(long julianDay) 
		{
			return ((julianDay)-Julian.ReferenceDay)*Utc.Day;
		}
		/// <summary>
		/// Convert a UtcTime.ticks value to julianDay number.
		/// </summary>
		/// <param name="t"></param>
		/// <returns>julianDay number</returns>
		static public long ticks2j(long t) 
		{
			return  (Julian.ReferenceTicks + t)/Utc.Day;
		}
		static public long ticks2j(UtcTime t) { return ticks2j(t.Ticks);}

	};

    /// <summary>
    /// RegionalCalendar takes care of 
    /// all calendar-conversion, 
    /// adds/diffs and trim functions
    /// </summary>
    public class RegionalCalendar
    {
        private const int DaylightSavingFirstYear = 1900;
        private const int DaylightSavingYears = 200;
        public const string localName = "local";
        public const string standardName = "standard";
        public const string utcName = "utc";    // Should correspond to the PDZone names.
        public const string dbName = "db";
        static readonly ILog Log = LogManager.GetLogger(typeof(RegionalCalendar));

        private static readonly Dictionary<string, long> TimeSpanDict = new Dictionary<string, long>
        {
            {"YEAR", Utc.Year},
            {"ÅR", Utc.Year},
            {"MONTH", Utc.Month},
            {"MÅNED", Utc.Month},
            {"MÅNAD", Utc.Month},
            {"MND", Utc.Month},
            {"DAY", Utc.Day},
            {"TODAY", Utc.Day},
            {"DAG", Utc.Day},
            {"IDAG", Utc.Day},
            {"DØGN", Utc.Day},
            {"DYGN", Utc.Day},
            {"WEEK", Utc.Week},
            {"UKE", Utc.Week},
            {"VECKA", Utc.Week},
            {"VEKE", Utc.Week},
            {"UGE", Utc.Week},
            {"HOUR", Utc.Hour},
            {"TIME", Utc.Hour},
            {"TIMME", Utc.Hour},
            {"MINUTE", Utc.Minute},
            {"MINUTT", Utc.Minute},
            {"MINUT", Utc.Minute},
            {"MIN15", Utc.Minute*15},
            {"M15", Utc.Minute*15},
            {"QUARTER", Utc.Minute*15},
            {"KVARTER", Utc.Minute*15},
            {"SECOND", Utc.Second},
            {"SEKUND", Utc.Second},
            {"NOW", Utc.Second},
            {"NÅ", Utc.Second},
            {"NUH", Utc.Second},
            {"NU", Utc.Second}
        };

        /// <summary>
        /// Creates RegionalCalendar with
        /// Utc offset 0, no daylight saving time, WeekRule First4DayWeek, and FirstDayOfWeek DayOfWeek.Monday
        /// </summary>
        public RegionalCalendar()
        {
            init();
        }

        /// <summary>
        /// Creates RegionalCalendar with
        /// Utc offset 0, no daylight saving time, WeekRule First4DayWeek, and FirstDayOfWeek DayOfWeek.Monday
        /// </summary>
        public RegionalCalendar(TimeSpan utcOffset, bool daylightObserved)
        {
            init();
            UtcOffset = utcOffset;
            DaylightObserved = daylightObserved;
        }

        /// <summary>
        /// Creates a new regional calendar equal to given c
        /// </summary>
        /// <param name="c"></param>
        public RegionalCalendar(RegionalCalendar c)
        {
            dst = c.dst;
            Name = c.Name;
            AlternateName = c.AlternateName;
            WeekRule = c.WeekRule;
            UtcOffset = c.UtcOffset;
            daylightStart = c.daylightStart.Clone() as UtcTime[];
            daylightEnd = c.daylightEnd.Clone() as UtcTime[];
            dstOffset = c.dstOffset.Clone() as TimeSpan[];
        }

        public override string ToString()
        {
            return $"{Name}, UtcOffset = {Math.Round(UtcOffset.TotalSeconds)}, dst = {dst}";
        }

        /// <summary>
        /// Initializes a RegionalCalendar to a 'standard' Utc
        /// </summary>
        protected void init()
        {
            dst = false;
            Name = utcName;
            AlternateName = utcName;
            WeekRule = WeekOneRule.First4DayWeek; // default for 99.9% powel apps.
            FirstDayOfWeek = System.DayOfWeek.Monday;
            daylightStart = new UtcTime[DaylightSavingYears];
            daylightEnd = new UtcTime[DaylightSavingYears];
            dstOffset = new TimeSpan[DaylightSavingYears];
        }

        /// <summary>
        /// Note that this isn't a complete Equals, as we won't bother
        /// to check the entire arrays.
        /// </summary>
        public override bool Equals(object obj)
        {
            RegionalCalendar other = obj as RegionalCalendar;

            if (other == null)
                return false;

            return AlternateName.Equals(other.AlternateName)
                   && daylightEnd[0].Equals(other.daylightEnd[0])
                   && daylightStart[0].Equals(other.daylightStart[0])
                   && dst.Equals(other.dst)
                   && dstOffset[0].Equals(other.dstOffset[0])
                   && FirstDayOfWeek.Equals(other.FirstDayOfWeek)
                   && UtcOffset.Equals(other.UtcOffset)
                   && WeekRule.Equals(other.WeekRule);
        }

        public override int GetHashCode()
        {
            return AlternateName.GetHashCode()
                   ^ daylightEnd[0].GetHashCode()
                   ^ daylightStart[0].GetHashCode()
                   ^ dst.GetHashCode()
                   ^ dstOffset[0].GetHashCode()
                   ^ FirstDayOfWeek.GetHashCode()
                   ^ UtcOffset.GetHashCode()
                   ^ WeekRule.GetHashCode();
        }

        /// <summary>
        /// Calculates the RegionalCalendar Utc difference for the specified t
        /// taking any daylight savings at time t into account.
        /// Null argument gives standard UtcOffset
        /// </summary>
        /// <param name="t">time that we calculate Utc diff for</param>
        /// <returns>TimeSpan containing the difference</returns>
        public TimeSpan diffUtc(UtcTime t)
        {
            if (!dst || t.Ticks == Utc.Null)
            {
                return UtcOffset;
            }

            int y = Julian.j2Y(Julian.ticks2j(t.Ticks));
            y -= DaylightSavingFirstYear;
            if (y < 0 || y >= DaylightSavingYears || daylightStart[y].Ticks == Utc.Null ||
                daylightEnd[y].Ticks == Utc.Null)
            {
                return UtcOffset;
            }

            var daylightOffset = TimeSpan.Zero;
            if (!IsSummerTimeReversed(y)) //northern hemisphere 
                daylightOffset = (t >= daylightStart[y] && t < daylightEnd[y])
                           ? dstOffset[y]
                           : TimeSpan.Zero;
            else // southern hemisphere
                daylightOffset = t < daylightEnd[y] || t >= daylightStart[y]
                           ? dstOffset[y]
                           : TimeSpan.Zero;

            return UtcOffset + daylightOffset;
        }

        /// <summary>
        /// res is in timezone z, convert it to utc
        /// take care of special effects when daylight 
        /// changes
        /// </summary>
        /// <param name="res"></param>
        /// <returns></returns>
        private UtcTime adjustToTz(UtcTime res)
        {
            if (res != UtcTime.Null)
            {
                TimeSpan diffUtc1 = diffUtc(res);
                UtcTime res2 = res - diffUtc1;
                if (!dst) return res2;
                TimeSpan diffUtc2 = diffUtc(res2);
                if (diffUtc1 != diffUtc2) // detect if we are in critical hour
                    res = res2 + (diffUtc1 - diffUtc2); // compensate
                else
                    res = res2;
            }
            return res;
        }

        /// <summary>
        /// Conversion from calendar units to UtcTime
        /// </summary>
        public UtcTime ToUtcTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
        {
            var res = Julian.j2ticks(Julian.YMD2j(year, month, day)) + hour*Utc.Hour + minute*Utc.Minute +
                       second*Utc.Second + millisecond*Utc.Second/1000;
            var r = new UtcTime(res);
            return adjustToTz(r);
        }

        /// <summary>
        /// Conversion from calendar units to UtcTime
        /// </summary>
        public UtcTime ToUtcTime(int year, int month, int day, int hour, int minute, int second)
        {
            return ToUtcTime(year, month, day, hour, minute, second, 0);
        }

        /// <summary>
        /// Conversion from calendar units to UtcTime
        /// </summary>
        public UtcTime ToUtcTime(int year, int month, int day)
        {
            return ToUtcTime(year, month, day, 0, 0, 0, 0);
        }

        public UtcTime ToUtcTimeW(int Y,int W,int Wd,int h,int m,int s) 
        { /* assumes correct Y,W,Wd, h, m,s */
            UtcTime t = ToUtcTime(Y,2,1,0,0,0);// just pick a date in correct year;
            t= trimWeek(t);// trim to nearest week start
            int xY,xW;
            asYW(t,out xY,out xW);// figure out what week we have here
            t = Add(t,TimeSpan.FromTicks(Utc.Week),W-xW);// then add number of weeks difference
            t = AddDays(t, Wd == 0 ? 6 : Wd - 1);
            t = AddHours(t, h);
            t = AddMinutes(t,m);
            t = AddSeconds(t,s); // add day and h,m,s
            return t;
        }

        /// <summary>
        /// The name of the Regional Calendar
        /// </summary>
        public string Name { get; private set; }

        /// <summary>
        /// The name of the Regional Calendar during 
        /// daylight saving time (if any)
        /// </summary>
        public string AlternateName { get; private set; }

        /// <summary>
        /// Standard Utc offset for this timezone
        /// defined as positive when east of Utc
        /// i.e. GMT+1, the TimeSpan is +1 hour.
        /// Utc+1 is 3600, Utc-1 is -3600 etc
        /// </summary>
        public TimeSpan UtcOffset { get; private set; } = TimeSpan.Zero;

        /// <summary>
        /// Specifies when week one in the year should start
        /// according to the WeekOneRule enumeration type.
        /// The WeekRule defines when week one
        /// starts.
        /// </summary>
        /// <seealso>WeekOneRule</seealso>
        public WeekOneRule WeekRule { get; set; }

        /// <summary>
        /// The week day regarded as start of week
        /// </summary>
        public DayOfWeek FirstDayOfWeek { get; set; }

        /// <summary>
        /// If daylight is observed in this timezone
        /// this property is true. Adding daylight saving
        /// rules automatically turns this property ON.
        /// It could be turned off by setting to false
        /// to get a regional calendar that reflects the
        /// timezone standard time instead of local time.
        /// </summary>
        public bool DaylightObserved
        {
            get => dst;
            set { if (!value || daylightStart != null) dst = value; }
        }

        /// <summary>
        /// Returns current weekday for t
        /// </summary>
        /// <param name="t">UtcTime</param>
        /// <returns>weekday 0..6 su-mo..fri-sa</returns>
        public int weekDay(UtcTime t)
        {
            t += diffUtc(t);
            return (int) ((7 + Utc.WeekDayStartNumber + (t.Ticks%Utc.Week)/Utc.Day)%7);
        }

        //-- Daylight saving time:

        /// <summary>
        /// Returns -1 or a valid index into the daylightsavingtime
        /// vectors according to specified t
        /// </summary>
        /// <param name="t"></param>
        /// <returns>-1 or index into the daylightsavingtime vectors</returns>
        private int dstYearIndex(UtcTime t)
        {
            if (!dst) return -1;
            int y = Julian.j2Y(Julian.ticks2j(t.Ticks)) - DaylightSavingFirstYear;
            if (y < 0 || y >= DaylightSavingYears) return -1;
            return y;
        }

        /// <summary>
        /// returns daylight saving time offset for 
        /// a specified time. If different from 0
        /// this will normally be +1 hour
        /// </summary>
        /// <param name="t"></param>
        /// <returns>daylight saving time offset</returns>
        public TimeSpan DstAt(UtcTime t)
        {
            int y = dstYearIndex(t);
            if (y < 0)
                return TimeSpan.Zero;
            if (!IsSummerTimeReversed(y)) //northern hemisphere 
                return t.Ticks >= daylightStart[y].Ticks && t.Ticks < daylightEnd[y].Ticks
                           ? dstOffset[y]
                           : TimeSpan.Zero;
            return t.Ticks < daylightEnd[y].Ticks || t.Ticks >= daylightStart[y].Ticks
                           ? dstOffset[y]
                           : TimeSpan.Zero;

        }

        //in southern hemisphere (countries like Chile or Brazil) the summertime is adjusted in winter, so it starts say in october and ends in february
        private bool IsSummerTimeReversed(int yearOffset)
        {
            if ((yearOffset < 0 || yearOffset >= DaylightSavingYears) || !dst || daylightStart[yearOffset].Ticks == Utc.Null ||
                daylightEnd[yearOffset].Ticks == Utc.Null)
            {
                return false;
            }

            if (daylightEnd[yearOffset].Ticks > daylightStart[yearOffset].Ticks) //northern hemisphere
            {
                return false;
            }
            else // southern hemisphere
            {
                return true;
            }
        }

        /// <summary>
        /// Returns UtcTime when daylight saving time
        /// start for a specified year. If there
        /// are no daylight saving time that year
        /// UtcTime.Null is returned.
        /// In most timezones daylight saving starts and
        /// ends at 01:00 Utc
        /// </summary>
        /// <param name="year"></param>
        /// <returns></returns>
        public UtcTime DstStart(int year)
        {
            if (!dst) return UtcTime.Null;
            int y = year - DaylightSavingFirstYear;
            if (y < 0 || y >= DaylightSavingYears) return UtcTime.Null;
            return daylightStart[y];
        }

        /// <summary>
        /// Returns UtcTime when daylight saving time
        /// ends for a specified year. If there
        /// are no daylight saving time that year
        /// UtcTime.Null is returned. 
        /// In most timezones daylight saving starts and
        /// ends at 01:00 Utc
        /// </summary>
        /// <param name="year"></param>
        /// <returns></returns>
        public UtcTime DstEnd(int year)
        {
            if (!dst) return UtcTime.Null;
            int y = year - DaylightSavingFirstYear;
            if (y < 0 || y >= DaylightSavingYears) return UtcTime.Null;
            return daylightEnd[y];
        }

        /// <summary>
        /// returns daylight saving time offset for 
        /// a specified year. If different from 0
        /// this will normally be +1 hour
        /// </summary>
        public TimeSpan DstOffset(int year)
        {
            if (!dst)
                return TimeSpan.Zero;
            var y = year - DaylightSavingFirstYear;
            if (y < 0 || y >= DaylightSavingYears)
                return TimeSpan.Zero;
            return dstOffset[y];
        }

        /// <summary>
        /// Returns the daylight saving time period for a particular year
        /// </summary>
        /// <remarks>In Years outside 
        /// RegionalCalendar.DaylightSavingFirstYear..+DaylightSavingYears
        /// values returned indicates no daylight time.</remarks>
        /// <param name="year"></param>
        /// <returns></returns>
        public DaylightTime GetDaylightChanges(int year)
        {
            if (!dst)
                throw new InvalidOperationException("No daylight saving in this calendar.");

            int y = year - DaylightSavingFirstYear;

            if (y < 0 || y >= DaylightSavingYears)
                throw new ArgumentOutOfRangeException("year", year, "No daylight saving information for this year.");

            if (dstOffset[y].Ticks == 0)
                throw new ArgumentException("The daylight saving offset for this year is zero.", "year");

            // according to ms doc, Start and End should be in 'local' timezone values
            // need to adjust values here to fix Utc and 'local' time differences.
            return new DaylightTime(
                this.ToDateTime(daylightStart[y]).Add(-dstOffset[y]),
                this.ToDateTime(daylightEnd[y]).Add(dstOffset[y]),
                dstOffset[y]);
        }

        /// <summary>
        /// Adds a daylight saving time rule to this regional calendar
        /// timezone.
        /// </summary>
        /// <param name="dstStart">Specifies when daylight saving rule starts</param>
        /// <param name="dstEnd">Specifies when daylight saving rule ends</param>
        /// <param name="dstOffsetA">Specifies utc-offset, GMT+1 is +1h</param>
        /// <returns></returns>
        public bool addDSTRule(UtcTime dstStart, UtcTime dstEnd, TimeSpan dstOffsetA)
        {
            dst = true;
            int y = dstYearIndex(dstStart);
            if (y < 0) return false;
            if (y < 0 || y >= DaylightSavingYears) return false;
            daylightStart[y] = dstStart;
            daylightEnd[y] = dstEnd;
            dstOffset[y] = dstOffsetA;
            return true;
        }

        /// <summary>
        /// Add daylight saving rule
        /// Rule applies to Year Yfrom to Yto (not incl)
        /// if dstOn=true, use begin/end dstOffset arguments:
        /// DST begins at Month_b(1..12) Day_b(1..31 or -1 -2 -3 -4 for nth sunday in month
        /// sec_b, seconds into day when it starts,
        /// DST ends at Month_e, Day_e,sec_e, Utc, defined as the first sec when there is NO DST (01:00
        /// dstOffset number of seconds DST, normally 1 hour
        /// </summary>
        /// <param name="Yfrom"></param>
        /// <param name="Yto"></param>
        /// <param name="dstOn"></param>
        /// <param name="Month_b">Month 1..12</param>
        /// <param name="Day_b"> Values 1..31 or -1 -2 -3 -4 for nth sunday in month</param>
        /// <param name="sec_b">seconds into day when it starts(Utc)</param>
        /// <param name="Month_e">Month 1..12</param>
        /// <param name="Day_e">Values 1..31 or -1 -2 -3 -4 for nth sunday in month</param>
        /// <param name="sec_e">seconds into day when it ends(UTC)</param>
        /// <param name="dstOffsetA">number of seconds DST, normally 1 hour. 0 if not DST</param>
        /// <returns></returns>
        public bool addDSTRule(int Yfrom, int Yto, bool dstOn,
                               int Month_b, int Day_b, TimeSpan sec_b,
                               int Month_e, int Day_e, TimeSpan sec_e,
                               TimeSpan dstOffsetA)
        {
            if (!dstOn)
            {
                // easy,-
                while (Yfrom < Yto)
                {
// wipe out dst in range
                    int y = Yfrom - DaylightSavingFirstYear;
                    if (y >= 0 && y < DaylightSavingYears)
                    {
                        daylightStart[y] = UtcTime.Null;
                        daylightEnd[y] = UtcTime.Null;
                        dstOffset[y] = TimeSpan.Zero;
                    }
                    Yfrom++;
                }
            }
            else
            {
                // some more work:-
                // primitive testing for args
                if (Month_b < 1 || Month_b > 12) return false;
                if (Month_e < 1 || Month_e > 12) return false;
                if (Day_b < -5 || Day_b > 31) return false;
                if (Day_e < -5 || Day_e > 31) return false;

                RegionalCalendar utc = new RegionalCalendar(); // needed for work
                while (Yfrom < Yto)
                {
                    // figure out when it starts, set limits
                    int y = Yfrom - DaylightSavingFirstYear;
                    if (y >= 0 && y < DaylightSavingYears)
                    {
                        UtcTime ts = UtcTime.Null; /* find start */
                        {
                            int mday = Day_b;
                            if (mday < 1)
                            {
                                ts = utc.ToUtcTime(Yfrom, Month_b, mday = 1, 00, 00, 00, 0);
                                var ts_nextm = utc.Add(ts, TimeSpan.FromTicks(Utc.Month), 1);
                                var nth_daycount = 0;
                                var last_sunday = 0;
                                bool found = false;
                                for (; ts < ts_nextm; ts += TimeSpan.FromTicks(Utc.Day), mday++)
                                {
                                    if (utc.weekDay(ts) == 0)
                                    {
                                        // a sunday ?
                                        ++nth_daycount;
                                        last_sunday = mday;
                                        if (nth_daycount == -Day_b)
                                        {
// count it and test
                                            found = true;
                                            break;
                                        }
                                    }
                                }
                                if (Day_b == -5)
                                {
                                    mday = last_sunday;
                                    found = true;
                                }
                                else if (!found) return false; // no such day that month
                            }
                            ts = utc.ToUtcTime(Yfrom, Month_b, mday, 0, 0, 0) + sec_b;
                        }
                        UtcTime te = UtcTime.Null; /* find end */
                        {
                            int mday = Day_e;
                            if (mday < 1)
                            {
                                // then its the nth sunday in that month
                                te = utc.ToUtcTime(Yfrom, Month_e, mday = 1, 0, 0, 0);
                                var te_nextm = utc.Add(te, TimeSpan.FromTicks(Utc.Month), 1);
                                var nth_daycount = 0;
                                var last_sunday = 0;
                                bool found = false;
                                for (; te < te_nextm; te += TimeSpan.FromTicks(Utc.Day), mday++)
                                {
                                    if (utc.weekDay(te) == 0)
                                    {
                                        // a sunday ?
                                        last_sunday = mday;
                                        if (++nth_daycount == -Day_e)
                                        {
// count it and test
                                            found = true;
                                            break;
                                        }
                                    }
                                }
                                if (Day_e == -5)
                                {
                                    mday = last_sunday;
                                    found = true;
                                }
                                else if (!found) return false; // no such day that month
                            }
                            te = utc.ToUtcTime(Yfrom, Month_e, mday, 0, 0, 0) + sec_e;
                        }
                        if (te != UtcTime.Null && ts != UtcTime.Null)
                        {
                            addDSTRule(ts, te, dstOffsetA);
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                    Yfrom++;
                }
            }
            return true;

        }

        /// <summary>
        ///  Performs add with Calendar semantics
        ///	o special care to be taken when moving/into/out of DST.
        ///    This implementation tries to account for the hour lost/gained.
        /// o The following rules applies
        ///   (1) PDZone/dst +-1h logic only applies to step DAY,WEEK,MONTH,YEAR
        ///   (2a)  Transition hour is the hour after DstStart and DstEnd 
        ///   (2b)  If add operation results in crossing the transition-hour,
        ///			then compensate by +- 1hour addtion to the result.  
        /// DST starts normally the last sunday in march
        /// at 01:00 Utc (transition hour 01:00..02:00 Utc) 
        /// and last until (not including) the last sunday in october 
        /// at 01:00 Utc (transition hour 01:00..02:00 Utc)
        /// </summary>
        /// <param name="t">UtcTime</param>
        /// <param name="step">Calendar steps</param>
        /// <param name="count">number of steps</param>
        /// <param name="CheckPrev">Do check previous hour to the resulting/starting hour of the addition. Default/True is backward compatible.</param>
        /// <returns>resulting new UtcTime</returns>
        public UtcTime Add(UtcTime time, TimeSpan step, long count, bool CheckPrev = true)
        {
            UtcTime res;
            TimeSpan tz1, tz2;
            int y, m, d;
            TimeSpan n_x_step = TimeSpan.FromTicks((long) count*step.Ticks);
            TimeSpan dT_HOUR = TimeSpan.FromTicks(Utc.Hour);
            TimeSpan dT_0 = TimeSpan.Zero;
            switch (step.Ticks)
            {
                case Utc.Year:
                    {
                        tz1 = diffUtc(time - (n_x_step.Ticks > 0 ? dT_0 : dT_HOUR));
                        // TODO: solve problem when moving from a leapyear to not a leapyear
                        UtcTime tl = time + tz1; // move into calendar zone axis
                        long j1 = Julian.ticks2j(tl);
                        Julian.j2YMD(j1, out y, out m, out d);
                        y += (int) count;
                        long j2 = Julian.YMD2j(y, m, d);
                        res = time + TimeSpan.FromTicks((j2 - j1)*Utc.Day); // calc diff in julian days
                        tz2 = diffUtc(res - (n_x_step.Ticks > 0 ? dT_HOUR : dT_0));
                    }
                    break;
                case Utc.Month:
                    {
                        tz1 = diffUtc(time - (n_x_step.Ticks > 0 ? dT_0 : dT_HOUR));
                        UtcTime tl = time + tz1;
                            // TODO: a problem here if 1999.01.31 + 1 month, since there is no 1999.02.31..
                        long j1 = Julian.ticks2j(tl); // workaround here to move into 
                        //the local/normal timezone to make sure it works on 1999.02.01 NT + 1 month
                        Julian.j2YMD(j1, out y, out m, out d);
                        int mr = m + (int) count;
                        if (mr < 1)
                        {
                            int borrow_years = 1 - mr/12;
                            y -= borrow_years;
                            m = mr + 12*borrow_years;
                        }
                        else if (mr > 12)
                        {
                            m = 1 + (mr - 1)%12;
                            y += (mr - 1)/12;
                        }
                        else
                            m = mr;
                        switch (m)
                        {
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                                if (d > 30) d = 30;
                                break;
                            case 2:
                                if (y%4 == 0)
                                {
                                    int n = 29;
                                    if (y%100 == 0 && !(y%400 == 0))
                                        n = 28;
                                    if (d > n) d = n;
                                }
                                else
                                {
                                    if (d > 28) d = 28;
                                }
                                break;
                        }
                        long j2 = Julian.YMD2j(y, m, d);
                        res = time + TimeSpan.FromTicks((j2 - j1)*Utc.Day); // calc diff in julian days
                        tz2 = diffUtc(res - (n_x_step.Ticks > 0 ? dT_HOUR : dT_0));
                    }
                    break;
                case Utc.Week:
                case Utc.Day:
                    if (CheckPrev)
                    {
                        tz1 = diffUtc(time - (n_x_step.Ticks > 0 ? dT_0 : dT_HOUR));
                        res = time + n_x_step;
                        tz2 = diffUtc(res - (n_x_step.Ticks > 0 ? dT_HOUR : dT_0));
                    }
                    else
                    {
                        if (!CheckPrev)
                            count *= 7;
                        // Gjør dato-addisjon.
                        int h, M, s, ms;
                        asYMDhms_ms(time, out y, out m, out d, out h, out M, out s, out ms);
                        var dt = new DateTime(y, m, d);
                        dt = dt.AddDays(count);
                        res = ToUtcTime(dt.Year, dt.Month, dt.Day, h, M, s, ms);
                        tz1 = tz2 = dT_0;
                    }
                    break;
                default:
                    return time + n_x_step; // nothing with special zone semantics
            }
            return res + (tz1 - tz2);
        }

        public UtcTime Add(UtcTime time, TimeSpan step)
        {
            return Add(time, step, 1);
        }

        public UtcTime AddSeconds(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromSeconds(1), count);
        }

        public UtcTime AddMilliseconds(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromMilliseconds(1), count);
        }

        public UtcTime AddMinutes(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromMinutes(1), count);
        }

        public UtcTime AddHours(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromHours(1), count);
        }

        public UtcTime AddDays(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromDays(1), count);
        }

        public UtcTime AddMonths(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromTicks(Utc.Month), count);
        }

        public UtcTime AddYears(UtcTime time, int count)
        {
            return Add(time, TimeSpan.FromTicks(Utc.Year), count);
        }

        public DayOfWeek DayOfWeek(UtcTime time)
        {
            return ToDateTime(time).DayOfWeek;
        }

        /// <summary>
        /// diffUnits: return number of calender units 'step' from Tfrom to Tto
        ///  rest = remaining seconds in nUnits intervals Tfrom Tto
        /// </summary>
        /// <param name="Tfrom"></param>
        /// <param name="Tto"></param>
        /// <param name="step">TimeSpan, notice that 
        /// values of TimeSpan(Utc.Year,MONTH,WEEK,DAY) 
        /// has calendar semantics associated</param>
        /// <param name="rest"></param>
        /// <returns></returns>
        public long diffUnits(UtcTime Tfrom, UtcTime Tto, TimeSpan step, out TimeSpan rest)
        {
            rest = TimeSpan.Zero;
            if (step.Ticks == 0)
                return 0;
            long n_units = (long) ((Tto.Ticks - Tfrom.Ticks)/step.Ticks); // figure out number of 'normal steps'
            switch (step.Ticks)
            {
                case Utc.Month:
                    n_units -= n_units/73;
                    goto case Utc.Day; // due to month 30.41/30.5 days, not 30
                case Utc.Week:
                    goto case Utc.Day;
                case Utc.Year:
                    goto case Utc.Day;
                case Utc.Day:
                    {
                        UtcTime calcTto = Add(Tfrom, step, n_units);
                        if (calcTto > Tto)
                        {
                            // we might get one unit to far
                            n_units--;
                            rest = Tto - Add(Tfrom, step, n_units);
                        }
                        else if (calcTto < Tto)
                        {
                            // we ended before, try add one and test.
                            n_units++;
                            UtcTime calcTtoX = Add(Tfrom, step, n_units);
                            if (calcTtoX > Tto)
                            {
                                n_units--;
                                rest = Tto - calcTto;
                            }
                            else
                            {
                                rest = Tto - calcTtoX;
                            }
                        } // else exact match. *rest=0;
                    }
                    break;
                default:
                    {
                        rest = Tto - (Tfrom + TimeSpan.FromTicks(n_units*step.Ticks));
                    }
                    break;
            }
            return n_units;
        }

        /// <summary>
        /// Trim t to the beginning of the year
        /// </summary>
        /// <param name="t"></param>
        /// <returns>t trimmed to nearest year</returns>
        public UtcTime trimYear(UtcTime t)
        {
            t += diffUtc(t); // add tz to get it right
            int y = Julian.j2Y(Julian.ticks2j(t));
            t = new UtcTime(Julian.j2ticks(Julian.Y2j(y)));
            return adjustToTz(t);
        }

        /// <summary>
        /// Trim t to beginning of the month
        /// </summary>
        /// <param name="t"></param>
        /// <returns>t trimmed to nearest month</returns>
        public UtcTime trimMonth(UtcTime t)
        {
            t += diffUtc(t);
            int y, m, d;
            Julian.j2YMD(Julian.ticks2j(t), out y, out m, out d);
            t = new UtcTime(Julian.j2ticks(Julian.YMD2j(y, m, 1)));
            return adjustToTz(t);
        }

        /// <summary>
        /// Trim t to beginning of the week
        /// </summary>
        /// <param name="t"></param>
        /// <returns>t trimmed to nearest week</returns>
        public UtcTime trimWeek(UtcTime t)
        {
            return trim(t, TimeSpan.FromTicks(Utc.Week));
        }

        /// <summary>
        /// Trim t to nearest specified TimeSpan x
        /// If x is zero or negative, t is returned unmodified
        /// </summary>
        /// <param name="t"></param>
        /// <param name="x">TimeSpan to trim</param>
        /// <returns>t trimmed to nearest specified TimeSpan</returns>
        public UtcTime trim(UtcTime t, TimeSpan x)
        {
            if (x.Ticks <= 0 || x.Ticks == Utc.Null) return t;
            switch (x.Ticks)
            {
                case Utc.Year:
                    return trimYear(t);
                case Utc.Month:
                    return trimMonth(t);
            }
            long tj = Julian.ReferenceTicks + diffUtc(t).Ticks + t.Ticks; //move to julian ticks local time
            tj = (tj/x.Ticks)*x.Ticks; // perform trim on the positive value julian ticks local time axis
            t = new UtcTime((tj - Julian.ReferenceTicks)); // move back to "local" time
            return adjustToTz(t); // move back to utc
        }

        /// <summary>
        /// Return an UtcTime with only date (year,month, day) set
        /// </summary>
        /// <param name="t"></param>
        /// <returns>t with hour, minute, seconds zeroed out</returns>
        public UtcTime date(UtcTime t)
        {
            int y, month, d;

            t += diffUtc(t);
            Julian.j2YMD(Julian.ticks2j(t), out y, out month, out d);
            t = new UtcTime(Julian.j2ticks(Julian.YMD2j(y, month, d)));
            return adjustToTz(t);
        }

        /// <summary>
        /// Convert UtcTime to calendar units
        /// </summary>
        /// <param name="t">Utc Time</param>
        /// <param name="Y">Year</param>
        /// <param name="M">Month</param>
        /// <param name="D">Day</param>
        /// <returns></returns>
        public bool asYMD(UtcTime t, out int Y, out int M, out int D)
        {
            int tick, h, mi, s;
            var res = asYMDhms_tick(t, out Y, out M, out D, out h, out mi, out s, out tick);
            return res;
        }

        /// <summary>
        /// returns Y (4 digit), M 1..12, D 1..31
        ///  h 0..23 m,s 0..59
        ///  weekd 0..6 Su..Sat
        /// </summary>
        /// <param name="t"></param>
        /// <param name="Y"></param>
        /// <param name="M"></param>
        /// <param name="D"></param>
        /// <param name="h"></param>
        /// <param name="mi"></param>
        /// <param name="s"></param>
        /// <param name="weekday"></param>
        /// <returns>true </returns>
        public bool asYMDhms_wd(UtcTime t, out int Y, out int M, out int D, out int h, out int mi, out int s,
                                out int weekday)
        {
            int tick;
            bool res = asYMDhms_tick(t, out Y, out M, out D, out h, out mi, out s, out tick);
            weekday = (int) (7 + Utc.WeekDayStartNumber + ((t + diffUtc(t)).Ticks%Utc.Week)/Utc.Day)%7;
            return true;
        }

        /// <summary>
        /// Convert UtcTime to calendar units including
        /// millisecond
        /// </summary>
        /// <param name="t"></param>
        /// <param name="Y"></param>
        /// <param name="M"></param>
        /// <param name="D"></param>
        /// <param name="h"></param>
        /// <param name="mi"></param>
        /// <param name="s"></param>
        /// <param name="ms"></param>
        /// <returns></returns>
        public bool asYMDhms_ms(UtcTime t, out int Y, out int M, out int D, out int h, out int mi, out int s, out int ms)
        {
            int tick;
            bool res = asYMDhms_tick(t, out Y, out M, out D, out h, out mi, out s, out tick);
            ms = tick/(int) Utc.MilliSecond;
            return res;
        }

        /// <summary>
        /// Convert internal representation ticks 
        /// to calendar units Year Month Day hour minute seconds and tick
        /// One tick is 0.1 microSecond, there are 10 000 000 ticks in one second
        /// </summary>
        /// <param name="t">UtcTime to convert</param>
        /// <param name="Y">year</param>
        /// <param name="M">month</param>
        /// <param name="D">day</param>
        /// <param name="h">hour</param>
        /// <param name="mi">minute</param>
        /// <param name="s">second</param>
        /// <param name="tick">ticks, number of 0.1 microSeconds</param>
        /// <returns>true</returns>
        private bool asYMDhms_tick(UtcTime t, out int Y, out int M, out int D, out int h, out int mi, out int s,
                                   out int tick)
        {
            long tj = (t + diffUtc(t)).Ticks + Julian.ReferenceTicks;
            Julian.j2YMD(tj/Utc.Day, out Y, out M, out D);
            long rem = tj%Utc.Day;
            h = (int) System.Math.DivRem(rem, Utc.Hour, out rem);
            mi = (int) System.Math.DivRem(rem, Utc.Minute, out rem);
            s = (int) System.Math.DivRem(rem, Utc.Second, out rem);
            tick = (int) rem;
            return true;
        }

        /// <summary>
        /// Convert internal representation ticks into 
        /// calendar units year,week, day of week, hour, minute,seconds,ticks
        /// 
        /// </summary>
        /// <param name="t"></param>
        /// <param name="Yw"></param>
        /// <param name="Wn"></param>
        /// <param name="Dw"></param>
        /// <param name="h"></param>
        /// <param name="m"></param>
        /// <param name="s"></param>
        /// <param name="tick"></param>
        /// <returns></returns>
        private bool asYWDhms_ticks(UtcTime t, out int Yw, out int Wn, out int Dw, out int h, out int m, out int s,
                                    out int tick)
        {
            /// overall, this is quite tricky due to rules for week one
            //-- 1. calc easy stuff, h,m,s wd
            UtcTime tl = t + diffUtc(t);
            UtcTime tj = new UtcTime(tl.Ticks + Julian.ReferenceTicks); //julian1970_s;
            long rem = tj.Ticks%Utc.Day;
            h = (int) System.Math.DivRem(rem, Utc.Hour, out rem);
            ;
            m = (int) System.Math.DivRem(rem, Utc.Minute, out rem);
            s = (int) System.Math.DivRem(rem, Utc.Second, out rem);
            tick = (int) rem;
            Dw = (int) (7 + Utc.WeekDayStartNumber + (tl.Ticks%Utc.Week)/Utc.Day)%7;

            //-- 2. then the hard part, figure out Yw, Wn
            UtcTime Tweek = trimWeek(t); // week starts here
            int Y, M, D;
            Julian.j2YMD(Julian.ticks2j(Tweek.Ticks + diffUtc(t).Ticks), out Y, out M, out D);
            switch (WeekRule)
            {
                default:
                case WeekOneRule.FirstJanWeek:
                    {
                        if (M == 12 && D >= 26)
                        {
                            // we hit week one that starts prev.year
                            Wn = 1;
                            Y++;
                        }
                        else if ((M == 1 && D == 1))
                        {
//we hit week one that starts this year
                            Wn = 1;
                        }
                        else
                        {
                            // some other week, in Year Y, we need week one for Y
                            UtcTime Tyearw = trimWeek(trimYear(Tweek));
                            Wn = (int) (1 + (Utc.Day + Tweek.Ticks - Tyearw.Ticks)/Utc.Week);
                        }
                    }
                    break;
                case WeekOneRule.FirstFullWeek:
                    {
                        if (M == 1 && D <= 7)
                        {
                            Wn = 1;
                        }
                        else
                        {
                            UtcTime Tyearw = trimWeek(trimYear(Tweek));
                            int Yyw, Myw, Dyw;
                            Julian.j2YMD(Julian.ticks2j(Tyearw), out Yyw, out Myw, out Dyw);
                            if (Myw == 12) // week one start on first full week.
                                Tyearw += TimeSpan.FromTicks(Utc.Week);
                            Wn = (int) (1 + (Utc.Day + Tweek.Ticks - Tyearw.Ticks)/Utc.Week);
                        }
                    }
                    break;
                case WeekOneRule.First4DayWeek:
                    {
                        if (M == 12 && D >= 29)
                        {
                            // we hit week one, and it start prev year
                            Wn = 1;
                            Y++;
                        }
                        else if (M == 1 && D == 1)
                        {
// we hit week one, it starts this year
                            Wn = 1;
                        }
                        else
                        {
// hit some other week. figure out when week one starts:
                            UtcTime Tyearw = trimWeek(trimYear(Tweek));
                            int Yyw, Myw, Dyw;
                            Julian.j2YMD(Julian.ticks2j(Tyearw + diffUtc(Tweek)), out Yyw, out Myw, out Dyw);
                            if ((Myw == 12 && Dyw >= 29) || (Myw == 1 && Dyw == 1))
                            {
                                ;
                            }
                            else
                                Tyearw += TimeSpan.FromTicks(Utc.Week); // not 4 days? => week 1 is next week.
                            Wn = (int) (1 + (Utc.Day + Tweek.Ticks - Tyearw.Ticks)/Utc.Week);
                        }
                    }
                    break;
            }
            Yw = Y;
            return true;
        }

        /// <summary>
        /// Converts UtcTime to Year and week,
        /// Year is related to the year when the week starts.
        /// The function is an utility alias for the fullblown
        /// asYWDhms_ticks
        /// </summary>
        /// <param name="t">The UtcTime</param>
        /// <param name="Yw">Returned Year releated to start of week</param>
        /// <param name="Wn">Week number</param>
        /// <returns>true if valid time input specified</returns>
        public bool asYW(UtcTime t, out int Yw, out int Wn)
        {
            int Dw, h, m_, s, ticks;
            return asYWDhms_ticks(t, out Yw, out Wn, out Dw, out h, out m_, out s, out ticks);
        }

        /// <summary>
        /// Convert t to a datetime that represents
        /// the calendar values according to the regionalcalendar
        /// timezone and dls
        /// </summary>
        /// <param name="t"></param>
        /// <returns></returns>
        public DateTime ToDateTime(UtcTime t)
        {
            //TODO: Consider range check t
            if (t == UtcTime.MinValue)
                return DateTime.MinValue;
            if (t == UtcTime.MaxValue)
                return DateTime.MaxValue;

            /* Sets DateTimeKind according to Regional calender definition. This is used in string transformation to Json etc (..Z..).
             * Utc: if UtcOffset is 0 for this calendar
             * Local: if the Current calendar is used. This is defined from Regional settings and reflects the localtime calendar for the OS.
             * Unspecified otherwise because although this may be a calendar with DLS settings we don't know if it is a Local time calendar defined equal to the regional settings or some other time zone.
             */
            return new DateTime(t.Ticks + Utc.TimeBase + diffUtc(t).Ticks,UtcOffset == TimeSpan.Zero ? DateTimeKind.Utc : this == Current ? DateTimeKind.Local : DateTimeKind.Unspecified);
        }

        /// <summary>
        /// Convert DateTime t, interpreted as having values
        /// according to this RegionalCalendar to the corresponding
        /// UtcTime
        /// </summary>
        /// <param name="t"></param>
        /// <returns>UtcTime</returns>
        public UtcTime ToUtcTime(DateTime t)
        {
            //TODO: Consider range check t
            if (t == DateTime.MinValue)
                return UtcTime.MinValue;
            if (t == DateTime.MaxValue)
                return UtcTime.MaxValue;

            if (t.Kind == DateTimeKind.Utc)
                return new UtcTime(t.Ticks);

            UtcTime r = new UtcTime(t.Ticks - Utc.TimeBase); //ToUtcTime(t.Year,t.Month,t.Day,t.Hour,t.Minute,t.Second);
            return adjustToTz(r);
        }

        /// <summary>
        /// Convert a standard powel 17 char datetime
        /// YYYYMMDDhhmmssxxx to UtcTime as fast
        /// as possible.
        /// </summary>
        /// <remarks>9999xxx maps to GNABGIB
        /// and 0000xxxx maps to BIGBANG constants.
        /// Minimum YYYY needs to be specified
        /// all remaining fields are considered to
        /// be 0 or 1, whatever is most appropriate.
        /// </remarks>
        /// 
        /// <param name="databaseString">Powel 17char datetime, YYYYMMDDhhmmssxxx</param>
        /// <returns>UtcTime</returns>
        public UtcTime ToUtcTime(string databaseString)
        {
            if (databaseString == null)
                return UtcTime.Null;

            const int cnull_x_1111 = '0'*1111;
            const int cnull_x_111 = '0'*111;
            const int cnull_x_11 = '0'*11;
            int Y = 0, M = 0, D = 0;
            UtcTime res = UtcTime.Null;
            string c = databaseString;
            int l = c.Length;
            if (l > 3)
            {
                Y = 1000*c[0] + 100*c[1] + 10*c[2] + c[3] - cnull_x_1111;
                if (Y == 9999) return UtcTime.MaxValue;
                if (Y == 0) return UtcTime.MinValue;
                if (l > 5)
                {
                    M = 10*c[4] + c[5] - cnull_x_11;
                    if (l > 7)
                    {
                        D = 10*c[6] + c[7] - cnull_x_11;
                        res = new UtcTime(Julian.j2ticks(Julian.YMD2j(Y, M, D)));
                        if (l > 9)
                        {
                            res += TimeSpan.FromTicks(Utc.Hour*(10*c[8] + c[9] - cnull_x_11));
                            if (l > 11)
                            {
                                res += TimeSpan.FromTicks(Utc.Minute*(10*c[10] + c[11] - cnull_x_11));
                                if (l > 13)
                                {
                                    res += TimeSpan.FromTicks(Utc.Second*(10*c[12] + c[13] - cnull_x_11));
                                    if (l > 16)
                                    {
                                        res += TimeSpan.FromTicks(Utc.MilliSecond*(100*c[14] + 10*c[15] + c[16] - cnull_x_111));
                                    }
                                }
                            }
                        }
                    }
                    else
                        res = new UtcTime(Julian.j2ticks(Julian.YMD2j(Y, M, 1)));
                }
                else
                    res = new UtcTime(Julian.j2ticks(Julian.YMD2j(Y, 1, 1)));


            }
            return res.Ticks != Utc.Null ? adjustToTz(res) : res;
        }

        public static UtcTime Txt2UtcTime(string txt, UtcTime Tnowref, out RegionalCalendar calUsed)
        {
            /*
            o converts a free form user input into pdtime
              fmt ::= {LOCAL|STANDARD|UTC} (YEAR|MONTH..){ (+|-) (hh:mm:ss | <num>(y|m|w|d) ) }*
            o if not z is specified, use some default z or
              try to figure out z from the user input. could be of interest - example: create profile series
            o Backward compat (could be removed ..) Method to scan the txt for something that
              identifies a timezone: $ (l,L)|(s,S,n,N) ' ' timespec
            */
            calUsed = null;
            if (string.IsNullOrEmpty(txt))
                return UtcTime.Null;

            if (txt.Length > 2 && txt[1] == ' ')
            {
                // backward compat. tz interpretation
                switch (txt[0])
                {
                    case 'l':
                    case 'L':
                        calUsed = RegionalCalendar.Current;
                        txt = txt.Remove(0, 2);
                        break;
                    case 's':
                    case 'S':
                    case 'n':
                    case 'N':
                        calUsed = RegionalCalendar.CurrentNT;
                        txt = txt.Remove(0, 2);
                        break;
                }
            }
            if (calUsed == null)
                calUsed = RegionalCalendar.CurrentNT;
            //--first try to figure out if user has specified explicit timezone, different from z
            txt = txt.Trim();
            if (txt.StartsWith("Z:", StringComparison.OrdinalIgnoreCase))
            {
                txt = txt.Remove(0, 2);
                var wlen = txt.IndexOf(' ');
                if (wlen != -1)
                {
                    string sTxt = txt.Substring(0,wlen);
                    calUsed = RegionalCalendar.GetCalendar(sTxt);
                    if (calUsed == null)
                        throw new KeyNotFoundException(sTxt);
                    txt = txt.Remove(0, wlen).Trim();
                }
            }
            else if (txt.StartsWith("local", StringComparison.OrdinalIgnoreCase) ||
                     txt.StartsWith("lokal", StringComparison.OrdinalIgnoreCase))
            {
                calUsed = RegionalCalendar.Current;
                txt = txt.Remove(0, 5).Trim();
            }
            else if (txt.StartsWith("standard", StringComparison.OrdinalIgnoreCase) ||
                     txt.StartsWith("normal", StringComparison.OrdinalIgnoreCase))
            {
                calUsed = RegionalCalendar.CurrentNT;
                txt = txt.Remove(0, txt.StartsWith("normal", StringComparison.OrdinalIgnoreCase) ? 5 : 8).Trim();
            }
            else
            {
                int wlen = txt.IndexOf(' ');
                if (wlen != -1)
                {
                    string sTxt = txt.Substring(0, wlen);
                    try
                    {
                        calUsed = RegionalCalendar.GetCalendar(sTxt);
                    }
                    catch (ArgumentException)
                    {
                    }
                    if (calUsed == null)
                        calUsed = RegionalCalendar.CurrentNT;
                }
                else
                {
                    const int pos = 27;
                    if (txt.Length > pos && (txt[pos] == '+' || txt[pos] == '-'))
                    {
                        // XML-Format: YYYY-MM-DDTHH:mm:ss.ttttt+hh:SS
                        // Extract calendar.
                        var utcOff = txt.Substring(pos, 6);
                        var hmOff = utcOff.Split(new char[] {':'});
                        if (hmOff.Length == 2)
                        {
                            int h = 0, m = 0;
                            if (!int.TryParse(hmOff[0], out h) || h < -12 || h > 12)
                                return UtcTime.Null;
                            if (!int.TryParse(hmOff[1], out m) || m < 0 || m > 59)
                                return UtcTime.Null;
                            calUsed = new RegionalCalendar();
                            calUsed.UtcOffset = TimeSpan.FromMinutes(h * 60 + m);
                            txt = txt.Remove(pos);
                        }
                    }
                }
            }
            return calUsed.Txt2UtcTime(txt, Tnowref);
        }

        public UtcTime Txt2UtcTime(string txt, UtcTime TnowRef)
        {
            UtcTime t = (TnowRef == UtcTime.Null) ? UtcTime.Now : TnowRef;
            var dTrim = TimeSpan.Zero;
            char[] delims = new char[] { '+', '-' };
            if (txt.StartsWith("HYDYEAR", StringComparison.OrdinalIgnoreCase))
            {
                // Special calendar base - start 1.sept
                UtcTime tref = AddMonths(t, 4);
                tref = trimYear(tref);
                t = AddMonths(tref, -4);
            }
            else
            {
                // Standard way
                dTrim = InterpretTimeSpanText(txt);
                if (dTrim == TimeSpan.Zero)
                {
                    // No time macro used. Standard text decoding from numbers and month to date/time
                    string[] err;
                    if (TnowRef != UtcTime.Null && txt.IndexOf('#') == -1)
                        txt += "#" + ToDatabaseString(TnowRef);
                    if (txt.IndexOfAny(delims) != -1 && txt.IndexOfAny(delims) >= 8 && Datetoken(txt.Split(delims)[0], out err) != UtcTime.Null)
                    {
                        t = Datetoken(txt.Split(delims)[0], out err); //allow fixed date as time reference (cases like "30.08.2015+2d")
                    }
                    else
                    {
                        t = Datetoken(txt, out err);
                        if (err.Length > 0)
                            Log.Error($"Txt2UtcTime: {string.Join("\n", err)}");
                        return t;
                    }
                }
                t = trim(t, dTrim); // then trim t according dT and to specified timezone
            }
            // Time macro found check relative additions to time macro.
            txt = txt.Replace(" ", null); // remove spaces
            int pos = -1;
            string s = txt;
            while ((pos = s.IndexOfAny(delims)) != -1)
            {
                // while more options +-, process until exhausted
                // allow MONTH+1w+12:00:00
                string x = s.Remove(0, pos);
                int sign = x[0] == '+' ? 1 : -1;
                x = x.Substring(1); // skip sign
                s = x;  // Ta vare på resten av uttrykket.
                pos = x.IndexOfAny(delims);
                if (pos != -1)
                    x = x.Remove(pos);  // Fjerner øvrige uttrykk for å jobbe med et uttrykk om gangen.
                int[] hms = x.Split(':').Select(n => { int i=0; Int32.TryParse(n, out i); return i; }).ToArray();
                if (hms.Length == 3)
                {
                    t += new TimeSpan(hms[0], hms[1], hms[2]);
                }
                else if (hms.Length == 2)
                {
                    t += new TimeSpan(hms[0], hms[1], 0);
                }
                else
                {
                    Regex rex = new Regex(@"(?<num>\d+)(?<ymdwh>\w?)");  // [YyÅåMmDdWwVvUuXxHhTt]
                    Match match = rex.Match(x);
                    if (match.Success)
                    {
                        if (!string.IsNullOrEmpty(match.Groups["ymdwh"].Value))
                        {
                            TimeSpan dT;
                            switch (match.Groups["ymdwh"].Value[0])
                            {
                                case 'Y':
                                case 'y':
                                case 'Å':
                                case 'å':
                                    dT = TimeSpan.FromTicks(Utc.Year);
                                    break;
                                case 'M':
                                case 'm':
                                    dT = TimeSpan.FromTicks(Utc.Month);
                                    break;
                                case 'D':
                                case 'd':
                                    dT = TimeSpan.FromTicks(Utc.Day);
                                    break;
                                case 'W':
                                case 'w':
                                case 'V':
                                case 'v':
                                case 'U':
                                case 'u':
                                    dT = TimeSpan.FromTicks(Utc.Week);
                                    break;
                                case 'X':
                                case 'x':
                                    dT = TimeSpan.FromTicks(Utc.Minute);
                                    break; // m is already used
                                default: /*case 'H':case 'h':case 'T':case 't':*/
                                    dT = TimeSpan.FromTicks(Utc.Hour);
                                    break;
                            }
                            t = Add(t, dT, sign*Int32.Parse(match.Groups["num"].Value));
                        }
                        else {
                            t = Add(t, dTrim, sign*Int32.Parse(match.Groups["num"].Value));
                        }
                    }
                }
            }
            return t;
        }

        public static TimeSpan InterpretTimeSpanText(string txt)
        {
            /*
            o search to see if txt begins with any occurrences
              of keywords indicating year, month etc.
            o returns TimeSpan corresponding to found time span else TimeSpan().
            */
            if (string.IsNullOrEmpty(txt))
                return TimeSpan.Zero;

            char[] chars = new char[] {' ', '+', '-', ':'}; // delimiter chars
            int pos = txt.IndexOfAny(chars);
            string uprTxt = txt.Substring(0, pos == -1 ? txt.Length : pos).ToUpper();
            if (TimeSpanDict.TryGetValue(uprTxt, out var ticks))
                return TimeSpan.FromTicks(ticks);
            return TimeSpan.Zero;
        }

        public static string RemoveLocalFromString(string dateString)
        {
            if (dateString.StartsWith("local", StringComparison.OrdinalIgnoreCase) ||
                 dateString.StartsWith("lokal", StringComparison.OrdinalIgnoreCase))
            {
                dateString = dateString.Remove(0, 5).Trim();
            }

            return dateString;
        }

        private static bool dateOrderSet = false; // computer setting fetched for current user.
        public static bool DateOrderSet { get { return dateOrderSet; } set { dateOrderSet = value; } }

        private enum LOCALE_IDATE // Using the C++ enum/constant name used for GetLocaleInfo()
        {
            // Must adhere to GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_IDATE...)
            MDY,
            DMY,
            YMD
        };

        private static LOCALE_IDATE DateOrder { get; set; }

        private static bool MonthBeforeDayConvention()
        {
            /*
            return nonzero (true) if month should be
            before day when interpreting two numbers as a date. */
            if (!dateOrderSet)
            {
                // done only once, at first conversion
                DateOrder = LOCALE_IDATE.DMY;
                dateOrderSet = true;
                // possible to override user setting with config.parameter
                if (!string.IsNullOrEmpty(Environment.GetEnvironmentVariable("ICC_DATEORDER_MD")))
                    DateOrder = LOCALE_IDATE.MDY;
                else if (!string.IsNullOrEmpty(Environment.GetEnvironmentVariable("ICC_DATEORDER_DM")))
                    DateOrder = LOCALE_IDATE.DMY;
                else
                {
                    // GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_IDATE|LOCALE_RETURN_NUMBER,(LPTSTR)&DateOrder,sizeof(DateOrder))==sizeof(DateOrder)) {
                    string pat = CultureInfo.CurrentCulture.DateTimeFormat.ShortDatePattern;
                        // C# has no method for obtaining this LOCALE_IDATE constants. We have to decode from other patterns.
                    int y = pat.IndexOf('y'), m = pat.IndexOf('M'), d = pat.IndexOf('d');
                    if (y != -1 && m != -1 && d != -1)
                        DateOrder = d == 0 ? LOCALE_IDATE.DMY : m == 0 ? LOCALE_IDATE.MDY : LOCALE_IDATE.YMD;
                            // dateOrder filled in by the system call
                    else
                    {
                        // something wrong, use language to decide
                        string lang = Environment.GetEnvironmentVariable("ICC_LANGUAGE");
                        if (lang == "english")
                            DateOrder = LOCALE_IDATE.MDY;
                    }
                }
            }
            return DateOrder == LOCALE_IDATE.MDY || DateOrder == LOCALE_IDATE.YMD;
        }

        private enum AmPmIndicator
        {
            NO_AMPM,
            AM,
            PM
        };

        public UtcTime Datetoken(string instr, out string[] err)
        {
            /*	Returns :
         *	Convert an unformatted date string to a formatted date string.
         *	Typical usage is to convert date/time input from an interactiv session.
         *		  - error codes:
         *		  - -------------
         *		  - -1 -> error in Year.
         *		  - -2 -> error in Month.
         *		  - -3 -> error in Day.
         *		  - -4 -> error in Hour.
         *		  - -5 -> error in Minute.
         *		  - -6 -> error in Second.
         *		  - -7 -> error in Millisecond.
         *		  - -10  -> unreadable string.
         *
         *		 - status codes:
         *		 - --------------
         *		 - 0 ->	 string is blank or NULL
         *		 - 1 ->  string contains Year, Month and Day.
         *		 - 2 ->  string contains Year, Month, Day and Hour.
         *		 - 3 ->  string contains Year, Month, Day, Hour and Minute.
         *		 - 4 ->  string contains Year, Month, Day, Hour, Minute and Second.
         *		 - 5 ->  string contains Year, Month, Day, Hour, Minute, Second and Millisecond. 
         *	
         *	Args    : 
         *	Function: Try to read date and time from the unformatted string "in",
         *		  missing values is appended with values of current date and time.
         *
         *		 -  key word : "idag","now","today" -> current date and time
         *	
         *			     - " sep 5 1992"        -> "19920905<current time>"
         *			     - "17-5-92 10:30"      -> "199205171030<current sec.mills>"
         *			     - "1992/05/17 10:30    -> "199205171030<current sec.mills>"
         *	                     - "17"                 -> "<cur.year><cur.Month>17<cur.time>
         *	
         *	Limitations:
         *
         *	Author  :	Jan-Otto Bakkland		EFI
         *	Revision:	n.n yymmdd	who     what is done
         *			1.0 920908	job	first
         * 
         *	Pseudo code :
         *	Local variables :	*/
            string datobuffer/*, datoref*/;
            int Year = 0, Month = 0, Day = 0, Hour = 0, Minute = 0, Second = 0, Millisec = 0;
            /*==========================================================================*/
            var oerr = new List<string>();
            if (string.IsNullOrEmpty(instr))
            {
                oerr.Add("-10: empty string");
                err = oerr.ToArray();
                return UtcTime.Null;
            }
            datobuffer = instr;
            //datoref = "*"; // * means not there
            MonthBeforeDayConvention();
            /*
	        *	find all seperate elements of the date/time string
	        */
            UtcTime tRef = UtcTime.Now;
            if (instr.Contains("#"))
            {
                // this datestring contains a reference to another date.
                var parts = instr.Split('#');
                datobuffer = parts[0];
                //datoref = parts[1];
                tRef = ToUtcTime(parts[1]);
            }

            UtcTime t;
            // first  if this is a timemacrostring
            if (datobuffer[0] == '-')
            {
                var argv = datobuffer.Split(' ');
                t = Timemacro(argv, out err);
                return t;
            }

            datobuffer = datobuffer.ToLower();

            bool clockString = datobuffer.Contains(":"); // if the string contains : 

            // Split into separate elements
            char[] delim = new char[] {'\\', '-', '.', ' ', ':', ',', '/', '\t', '\n', 't'};
            var tokarr = datobuffer.Split(delim);
            tokarr = (from tok in tokarr where !string.IsNullOrEmpty(tok) select tok).ToArray();    // Remove empty strings e.g. ":15"
            if (tokarr.Length > 7)
            {
                // QC 7951
                oerr.Add("-10: Too many elements in string.");
                err = oerr.ToArray();
                return UtcTime.Null;
            }
            bool illegal = false;
            foreach (var tok in tokarr)
            {
                if (!ValidDateTimeParam(tok))
                {
                    illegal = true;
                    break;
                }
            }
            if (illegal)
            {
                oerr.Add("-10: One or more unrecognized time parameters:" + string.Join(",", tokarr));
                err = oerr.ToArray();
                return UtcTime.Null;
            }

            /* am pm check */
            AmPmIndicator ampm = AmPmIndicator.NO_AMPM;
            foreach (var tok in tokarr)
            {
                if (tok.Equals("pm", StringComparison.OrdinalIgnoreCase))
                {
                    ampm = AmPmIndicator.PM;
                    break;
                }
                else if (tok.Equals("am", StringComparison.OrdinalIgnoreCase))
                {
                    ampm = AmPmIndicator.AM;
                    break;
                }
            }
            if (ampm != AmPmIndicator.NO_AMPM) // am/pm found. Remove it from tokarr
                tokarr = (from tok in tokarr where !(tok.Equals("am", StringComparison.OrdinalIgnoreCase) || tok.Equals("pm", StringComparison.OrdinalIgnoreCase)) select tok).ToArray();

            if (tokarr.Length == 1)
            { /* input string contains only one element */
                Int64 idum;
                if (!Int64.TryParse(tokarr[0], out idum))
                {
                    var rexp = new Regex(@"(idag|today|now)");
                    var match = rexp.Match(tokarr[0]);
                    if (match.Success)
                    {
                        //t = UtcTime.Now;
                        err = oerr.ToArray();
                        return trim(tRef, TimeSpan.FromTicks(Utc.Day));
                    }
                    else
                    {
                        oerr.Add("-10: Non-numeric and not matched by: " + rexp);
                        int w = IndexOfWeek(tokarr[0]);
                        if (w > 0) {
                            int y, wRef;
                            asYW(tRef, out y, out wRef);
                            oerr.Clear();
                            err = oerr.ToArray();
                            return ToUtcTimeW(y, w, 1,0,0,0);
                        }
                        else
                        {
                            oerr[0] += ", or: " + rexp;
                            err = oerr.ToArray();
                            return UtcTime.Null;
                        }
                    }
                }
                else if (idum < 32 && !clockString)
                {
                    /*
			        *	input contains only day as input,
			        *	return current year and month, and day from input string.
			        */
                    Day = (int)idum;
                    int dum;
                    asYMD(tRef, out Year, out Month, out dum);
                    Hour = Minute = Second = Millisec = 0;
                }
                else if (idum < 60 && clockString)
                {
                    // input string is :min
                    asYMDhms_ms(tRef, out Year, out Month, out Day, out Hour, out Minute, out Second, out Millisec);
                    Minute = (int)idum;
                    Second=Millisec=0;
                }
                else if (tokarr[0].Length >= 4 && (tokarr[0][0] == '1' || tokarr[0][0] == '2'))
                { /*
			        *	input string contains date/time without separator
			        *	character, and the year is described with 4 character
                   *	ToDo: Her tror jeg det er forbedringsmuligheter.
			        */
                    int h, m, s, ms;
                    asYMDhms_ms(tRef, out Year, out Month, out Day, out h, out m, out s, out ms);
                    if (tokarr[0].Length < 17)
                    {
                        int[] i = new int[7];
                        int ii = 0;
                        for (s = 0; s + 2 <= tokarr[0].Length; s += 2, ii++)
                        {
                            i[ii] = Int16.Parse(tokarr[0].Substring(s, s == 0 ? 4 : 2));
                            if (s == 0)
                                s += 2;
                        }
                        if (ii > 0)
                            Year = i[0];
                        if (ii > 1)
                            Month = i[1];
                        if (ii > 2)
                            Day = i[2];
                        if (ii > 3)
                            Hour = i[3];
                        if (ii > 4)
                            Minute = i[4];
                        if (ii > 5)
                            Second = i[5];
                    }
                    else
                    {
                        var tmp = ToUtcTime(tokarr[0]);
                        asYMDhms_ms(tmp, out Year, out Month, out Day,out Hour,out Minute,out Second,out Millisec);
                    }
                }
                else
                { // ToDo:
                    oerr.Add("-1: unreadable string.");
                    err = oerr.ToArray();
                    return UtcTime.Null;
                }
            }
            else if (tokarr.Length == 2 && clockString)
            {
                // input string is hour:minute
                asYMD(tRef, out Year, out Month, out Day);
                Hour = Int32.Parse(tokarr[0]);
                Minute = Int32.Parse(tokarr[1]);
                Second = Millisec = 0;
            }
            else
            {
                /* more elements:
	            input string is given with separator character,
	            try to find Year,Month and day or weekn first
	            */
                int yearFound = 0; /* 0 not found 1,2,3: position found */
                int monthFound = 0; /* 0 not found 1,2,3: position found */
                int weekFound = 0; /* 0 not found 1,2,3: position found */
                int weekno = 0;

                // Default date elements
                asYMD(tRef, out Year, out Month, out Day);

                for (int j = 0; j < tokarr.Length; j++)
                {
                    /* translate symbolic names into numbers */
                    int num;
                    if (Int32.TryParse(tokarr[j], out num))
                    {
                        if (yearFound == 0 && num > 1800)
                        {
                            Year = num;
                            yearFound = j + 1;
                            tokarr[j] = "";
                        }
                    }
                    else if (monthFound == 0 && (num = IndexOfMonth(tokarr[j])) > 0)
                    { /* month ?*/
                        Month = num;
                        monthFound = j + 1;
                        tokarr[j] = ""; /* used field */
                    }
                    else if (weekFound == 0 && (num = IndexOfWeek(tokarr[j])) > 0)
                    { /* week ?*/
                        weekno = num;
                        weekFound = j + 1;
                        tokarr[j] = "";
                    }
                    else
                    {
                        oerr.Add("-10: Not numeric and unknown month or week");
                        err = oerr.ToArray();
                        return UtcTime.Null;
                    }
                }
                /* 	figure out remaining numbers: month/day    */
                int hourIdx = -1;
                var nums = new List<int>();
                for (int j = 0; j < tokarr.Length; j++)
                {
                    int num;
                    if (!string.IsNullOrEmpty(tokarr[j]) && Int32.TryParse(tokarr[j],out num)) 
                        nums.Add(num);
                }
                if (weekFound == 0)
                { /* search for month/day */
                    if (yearFound > 0)
                    {   /* then we have  YYYY MM DD, DD MM YYYY, ... */
                        if (nums.Count == 1)
                        {
                            if (monthFound == 0)
                            {
                                /* then we miss something  */
                                oerr.Add("-10: Missing month.");
                                err = oerr.ToArray();
                                return UtcTime.Null;
                            }
                            Day = nums[0];
                            hourIdx = 1;
                        }
                        else if (nums.Count >= 2)
                        {   /* two numbers like: 31 10, 12 10 , 1 2 , we have to figure out what is what */
                            if (monthFound > 0)
                            {
                                /* not valid  spec */
                                oerr.Add("-10: Too many date parameters.");
                                err = oerr.ToArray();
                                return UtcTime.Null;
                            }
                            switch (yearFound)
                            {
                                    /* where did we find YYYY */
                                case 1: /* OK its a YYYY.MM.DD */
                                    Month = nums[0];
                                    Day = nums[1];
                                    hourIdx = 2;
                                    break;
                                case 2: /* like DD YYYY MM or MM YYYY DD */
                                    oerr.Add("-10: Unknown date element sequence."); /* give up */
                                    err = oerr.ToArray();
                                    return UtcTime.Null;
                                case 3: /*  assume like DD MM YYYY  or MM DD YYYY according to DateOrder */
                                    Day = nums[DateOrder == LOCALE_IDATE.DMY ? 0 : DateOrder == LOCALE_IDATE.MDY ? 1 : 2];
                                    Month = nums[DateOrder == LOCALE_IDATE.DMY ? 1 : DateOrder == LOCALE_IDATE.MDY ? 0 : 2];
                                    hourIdx = 2;    // Year is not part of nums.
                                    break;
                            }
                        }
                        else {
                            oerr.Add("-10: Unhandled number of date parameters.");
                            err = oerr.ToArray();
                            return UtcTime.Null;
                        }
                    }
                    else { /* then we have  DD MM or MM DD */
                        switch (nums.Count) {
                            case 1: /* it has to be month day */
                                if (monthFound == 0) { /* has to be month here */
                                    oerr.Add("-10: Missing month.");
                                    err = oerr.ToArray();
                                    return UtcTime.Null;
                                }
                                Day = nums[0];
                                break;
                            case 3:
                            case 2: /* now it has to be DD MM (?) */
                                if (monthFound > 0) { /* now we cant have month */
                                    oerr.Add("-10: Too many date parameters.");
                                    err = oerr.ToArray();
                                    return UtcTime.Null;
                                }
                                Day = nums[DateOrder == LOCALE_IDATE.DMY ? 0 : DateOrder == LOCALE_IDATE.MDY ? 1 : 2];
                                Month = nums[DateOrder == LOCALE_IDATE.DMY ? 1 : DateOrder == LOCALE_IDATE.MDY ? 0 : 2];
                                break;
                            default:
                                oerr.Add("-10: Unhandled number of date parameters.");
                                err = oerr.ToArray();
                                return UtcTime.Null;
                        }
                    }
                }
                else { /* Translate week into month/day, and now its only TWO numbers year/week */
                    UtcTime tmp = ToUtcTimeW(Year, weekno, 1, 0, 0, 0);
                    asYMD(tmp, out Year, out Month, out Day);
                    hourIdx = 2;
                }
	            /* find Hour Minute and Second */
                if (hourIdx != -1 && hourIdx < nums.Count)
                {
                    Hour = nums[hourIdx++];
                    if (hourIdx < nums.Count)
                    {
                        Minute = nums[hourIdx++];
                        if (hourIdx < nums.Count)
                        {
                            Second = nums[hourIdx++];
                            if (hourIdx < nums.Count)
                                Millisec = nums[hourIdx++];
                        }
                    }
                }
            }
            switch (ampm)
            {
                    // fix the am/pm mapping here.
                case AmPmIndicator.NO_AMPM:
                    break;
                case AmPmIndicator.AM:
                    Hour = Hour == 12 ? 0 : Hour;
                    break; // 12 AM means 00, 22 AM means just 22
                case AmPmIndicator.PM:
                    Hour = Hour >= 12 ? Hour : Hour + 12;
                    break; // 12 PM means 12, 1 PM means 13 . etc. 
            }
	        if (Year < 1000 || Year > 9999 )
	            oerr.Add("-1: Illegal year value.");
            if (Month < 1 || Month > 12 ) 
	            oerr.Add("-2: Illegal month value.");
	        if (Day < 1 || Day > DaysInMonthYear(Month,Year)) 
	            oerr.Add("-3: Illegal day value.");
	        if (Hour  < 0 || Hour> 23) {
	            if ( Hour != 24 || Minute != 0 || Second != 0 || Millisec != 0 )
	                oerr.Add("-4: Illegal hour value.");
	        }
	        if (Minute< 0 || Minute > 59) 
	            oerr.Add("-5: Illegal minute value.");
	        if (Second< 0 || Second > 59) 
	            oerr.Add("-6: Illegal seconds value");
	        if (Millisec < 0 || Millisec > 999) 
	            oerr.Add("-7: Illegal milliseconds value.");
            if (oerr.Count > 0)
            {
	            err = oerr.ToArray();
	            return UtcTime.Null;
            }
            err = oerr.ToArray();
            return ToUtcTime(Year, Month, Day, Hour, Minute, Second, Millisec);
        }

        private static int[] daysInMonth = new int[] {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        public static int DaysInMonthYear(int M,int Y)
        {
            if (M < 0 || M > daysInMonth.Length) 
                return 0;
            int d = daysInMonth[M - 1];
            if (M == 2 && LeapYear(Y)) 
                d++;
            return d;
        }

        public static bool LeapYear(int year)
        {
          return (year&3) == 0 && (year % 100 != 0 || year % 400 == 0); // (year&3) is equivalent to (year % 4)
        }

        private bool ValidDateTimeParam(string param)
        {
            if (InterpretTimeSpanText(param) != TimeSpan.Zero)
                return true;
            if (param.TakeWhile(c => { return char.IsDigit(c); }).ToArray().Length == param.Length) // Sjekk om bare tall.
                return true;
            Regex months = new Regex(@"\w+{ja|fe|mar|ap|mai|maj|may|jun|jul|au|se|ok|oc|no|de}*");
            var match = months.Match(param);
            if (match.Success)
                return true;
            Regex weeks = new Regex(@"\w+{u|uke|veke|v|vecko|vecka|w|week}\d{2}");
            match = weeks.Match(param);
            if (match.Success)
                return true;
            if (param.Equals("am", StringComparison.OrdinalIgnoreCase) ||
                param.Equals("pm", StringComparison.OrdinalIgnoreCase))
                return true;
            return false;
        }

        // ToDo: This is not good. Should use some locale-definition to handle languages
        // Dictionary containing abbreviated month name and index/monthNum
        private static readonly Dictionary<string,int> _monthNamesAbbr = 
            new Dictionary<string, int>()
                { {"ja",1},{"fe",2},{"mar",3},{"ap",4},{"mai",5},{"may",5},{"maj",5},{"jun",6},{"jul",7}
                    ,{"au",8},{"se",9},{"ok",10},{"oc",10},{"no",11},{"de",12}};

        private static int IndexOfMonth(string Mname)
        {
            return (from month in _monthNamesAbbr where Mname.StartsWith(month.Key) select month.Value).FirstOrDefault();
        }

        private static int IndexOfWeek(string Wstr)
        {
            Regex weeks = new Regex(@"(?<str>\w+{u|uke|veke|v|vecko|vecka|w|week}?)(?<ind>\d{2}?)");
            var match = weeks.Match(Wstr);
            int w = 0;
            return match.Success && Int32.TryParse(match.Groups["ind"].Value, out w) && w > 0 && w < 54 ? w : 0;
        }

        public static UtcTime Timemacro(string[] argv,out string[] emsg,out RegionalCalendar calUsed)
        {
            calUsed = null;
            bool zoneFound = false;
            for (int i = 0; i < argv.Length; i++)
            {
                if (argv[i] == "-z")
                {
                    if (argv[i + 1].Length == 1)
                    {
                        switch (argv[i + 1][0])
                        {
                            case 's':
                            case 'S':
                            case 'n':
                            case 'N':
                                calUsed = RegionalCalendar.CurrentNT;
                                break;
                            case 'l':
                            case 'L':
                                calUsed = RegionalCalendar.Current;
                                break;
                        }
                    }
                    else
                        calUsed = GetCalendar(argv[i + 1]);
                    zoneFound = true;
                    argv[i] = argv[i + 1] = ""; // Handled
                }
                else if (argv[i] == "-T")
                {
                    if (!zoneFound)
                    {
                        emsg = new List<string>().ToArray();
                        return Txt2UtcTime(argv[i + 1], UtcTime.Null,out calUsed); // Static-versjon. Mulig zone-angivelse i argv[i+1]
                    }
                }
            }

            if (calUsed == null) 
                calUsed = RegionalCalendar.Current;
            return calUsed.Timemacro(argv, out emsg);
        }

        public UtcTime Timemacro(string[] argv,out string[] emsg)
        {
            UtcTime tid = UtcTime.Null;	// Arbeidsvariabelen som påvirkes av alle opsjonene.
            bool start_of_period = true;
            int hour_offset = 0;
            int day_offset = 0;
            List<string> errs = new List<string>();

            /*
             *  Scan command line options.
             */
            for (int i = 0;i < argv.Length;i++) {
                switch (argv[i]) {
		        case "-n":	/* Now, including hour */
			        tid = UtcTime.Now;
			        tid = trim(tid,TimeSpan.FromTicks(Utc.Hour));
			        break;
		        case "-N":	/* Now including minute and seconds */
			        tid = UtcTime.Now;
			        tid = trim(tid,TimeSpan.FromTicks(Utc.Second));
			        break;
		        case "-i":	/* Start of today , hour 00 */
			        tid = UtcTime.Now;
                    tid = trim(tid,TimeSpan.FromTicks(Utc.Day)); //   (DAY,Zlocal);
			        if (!start_of_period) 
				        tid = AddDays(tid,1);
			        break;
		        case "-M":	/* This month */
			        tid = UtcTime.Now;
                    tid = trimMonth(tid);   // (MONTH,Zlocal);
			        break;
		        case "-e":   // rekkefølge er viktig i forhold til i-opsjon
			        start_of_period = false;
			        break;
		        case "-b":
			        start_of_period = true;
			        break;
		        case "-w":	/* This week */
			        tid = UtcTime.Now;
			        tid = trim(tid,TimeSpan.FromTicks(Utc.Week));   //,Zlocal);
			        if (!start_of_period) 
				        tid = AddDays(tid,7);
			        break;
		        case "-T":
			        tid = Txt2UtcTime(argv[i+1],UtcTime.Null);
			        break;
		        case "-t":	/* time is decoded by TIME_datetoken */
			        // Dette burde vært ryddet opp i. Det er tull å ha mange funksjoner
			        // som alle tester på 'idag' ++
                    string[] err;
                    tid = Datetoken(argv[i + 1],out err);
			        break;
		        case "-m":	/* handles macros like idag-1 */
                    TimeSpan dT = InterpretTimeSpanText(argv[i + 1]);
                    if (dT == TimeSpan.Zero) 
                        errs.Add("Unknown macro" + argv[i+1]);
                    else
                    {
                        tid = UtcTime.Now;
                        tid = trim(tid, dT);
                        if (!start_of_period)
                            tid = Add(tid, dT);
                    }
                    break;
		        case "-a":	/* Add hour to start time */
			        hour_offset = Int32.Parse(argv[i+1]);
			        break;
		        case "-d":	/* Add days to start time */
			        day_offset = Int32.Parse(argv[i+1]);
			        break;
		        case "-h":   // When used from timeconv.exe
			        errs.Add("Make time string.\nAvailable options :\n");
			        errs.Add("-e  End of period\n-b  Start of period\n");
			        errs.Add("-n  Now, including hour\n");
			        errs.Add("-N  Now, including hour, minute and seconds\n");
			        errs.Add("-i  Start of today\n");
			        errs.Add("-T  Decode time string (using txt2pdtime)\n");
			        errs.Add("-t  Decode time string (using TIME_datetoken)\n");
			        errs.Add("-m  Decode time macro (f.eks. idag, igaar)\n");
			        errs.Add("-a  Add hour to specified time\n");
			        errs.Add("-d  Add day to specified time\n");
			        errs.Add("-w  This week\n");
			        errs.Add("-M  This month\n");
			        errs.Add("-z s|S|n|N|l|L|<time zone name> : Report time zone. Default: Local\n");
			        errs.Add("-h  Help message\n");
                    emsg = errs.ToArray();
			        return tid;
		        }
            }
            if (tid == UtcTime.Null)
            {
                emsg = errs.ToArray();
                return tid;
            }

            if ( hour_offset != 0) 
		        tid = AddHours(tid,hour_offset);
	        if ( day_offset != 0) 
		        tid = AddDays(tid,day_offset);
            emsg = errs.ToArray();
            return tid;
        }

        /// <summary>
		/// Converts specifed UtcTime into
		/// a Powel  standard 17character
		/// YYYYMMDDhhmmssxxx
		/// using this timezone
		/// </summary>
		/// <remarks>Null maps to empty string
		/// MaxValue maps to GNABGIB
		/// MinValue maps to BIGBANG
		/// other values are mapped through
		/// standard conversion routine toYMDhms_ms
		/// <seealso>ToUtcTime(string 17c)</seealso></remarks>
		/// <param name="x">UtcTime</param>
		/// <returns></returns>
		public string ToDatabaseString(UtcTime x) 
		{
			const string BIGBANG="00010101000000000";
			const string GNABGIB="99991231235959000";
			const string Null="";
			if(x == UtcTime.Null)
				return Null;// no value maps to empty string
			if(x == UtcTime.MaxValue) 
				return GNABGIB;
			if(x == UtcTime.MinValue) 
				return BIGBANG;
			int Y,M,D,h,mi,s,ms;
			asYMDhms_ms(x,out Y,out M,out D,out h,out mi,out s,out ms);
			char[] c=  new char[17];

			c[ 3]= (char)('0' + (char)(Y%10));Y/=10;
			c[ 2]= (char)('0' + (char)(Y%10));Y/=10;
			c[ 1]= (char)('0' + (char)(Y%10));c[ 0]= (char)('0' + (char)(Y/10));
			c[ 5]= (char)('0' + (char)(M%10));c[ 4] =(char)('0' + (char)(M/10));
			c[ 7]= (char)('0' + (char)(D%10));c[ 6]= (char)('0' + (char)(D/10));
			c[ 9]= (char)('0' + (char)(h%10));c[ 8]= (char)('0' + (char)(h/10));
			c[11]= (char)('0' + (char)(mi%10));c[10]= (char)('0' + (char)(mi/10));
			c[13]= (char)('0' + (char)(s%10));c[12]= (char)('0' + (char)(s/10));
			c[16]= (char)('0'+ (char)(ms%10));ms/=10;
			c[15]= (char)('0'+ (char)(ms%10));ms/=10;	
			c[14]= (char)('0'+ ms);

			return new string(c);	
		}

        /// <summary>
        /// true if DayLightSaving is observed in this RegionalCalendar 
        /// </summary>
        bool dst; 
		/// <summary>
		/// Marks the start of DaylightSaving time for a specified year
		/// </summary>
		private UtcTime[] daylightStart;//  
		/// <summary>
		/// Marks the end of the DaylightSaving time for a specified year
		/// </summary>
		private UtcTime[] daylightEnd;// < DST  :index by yearOf(t) - DaylightSavingFirstYear
		/// <summary>
		/// During DaylightSaving time, the additional # seconds west of GMT 
		/// </summary>
		private TimeSpan[] dstOffset;
		/// <summary>
		/// Converts a UtcTime to a standard representation suitable for
		/// XML formats etc.
		/// </summary>
		/// <param name="t">The UtcTime to convert</param>
		/// <returns></returns>
		public String ToString(UtcTime t) 
		{
			if(t ==UtcTime.Null) 
			{
				return "Null";
			} 
			else 
			{
				int Y,M,D,h,m,s,tick;
				asYMDhms_tick(t,out Y,out M,out D,out h,out m,out s,out tick);
				return System.String.Format("{0,4:0000}-{1,2:00}-{2,2:00}T{3,2:00}:{4,2:00}:{5,2:00}.{6,7:0000000}+00:00",Y,M,D,h,m,s,tick);
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="t"></param>
		/// <param name="format"></param>
		/// <returns></returns>
		public String ToString(UtcTime t, string format)
		{
		    return t == UtcTime.Null ? "Null" : ToDateTime(t).ToString(format);
		}

		/// <summary>
		/// Formats time to string
		/// </summary>
		/// <param name="t">UTC time</param>
		/// <param name="format">time format</param>
		/// <param name="usePowelTimeFormat">Use Powel time formatting. If not regular DateTime format is used.</param>
		/// <returns>Time formatted string</returns>
		public string ToString(UtcTime t, string format, bool usePowelTimeFormat)
		{
			if (!usePowelTimeFormat) return ToString(t, format);

			//Ported from the C++ method: PDZone::asString
			/*        
			          - TYPE	Description
			          - --------------------------------------------
			          - %Y 	Year (eks. 1992)
			          - %2Y	Year with 2 digits (eks . 92)
			          - %y	Year related to weeknumber.
			          - %2y	Year related to weeknumber with 2 digits.
			          - %M	Month.		(1-12)
			          - %W	Weeknumber.	(1-53)
			          - %D	Day in month.	(1-31)
			          - %w	Weekday.	(1-7)
			          - %h	Hour		(0-23)
			          - %H	Hour-number	(1-24)
			          - %m	Minute		(0-59)
			          - %s	Second		(0-59)
			          - %x	Date		(22.04.2005)	lokalisert
			          - %X	Time		(13:51:30)		lokalisert
			          - %z	Timezone	(CET)
			          - %Z	Timezone offset (+01)
			          
			          - $M or #M	Month presented by name 	(eks. January)
			          - $w or #w	Weekday presented by name	(eks. Monday)
			          
			          Example of use:
			          
			          Example #1
			          
			          - date_string 		="19920517123000"
			          - format     		="$w %D $M %Y kl.%02h:%02m"
			          - return_date_string	="Sunday 17 May 1992 kl.12:30"
			          
			          Example #2
			          
			          - date_string 		="19920517123000"
			          - format     		="Week %W %y"
			          - returnDateString	="Week 20 1992"
			          
			          Example #3
			          
			          - date_string 		="19920517123000"
			          - format      		="%2Y-%02M-%02D"
			          - returnDateString	="92-05-17"
			          
			          Example #4
			          
			          - date_string 		="19920517123000"
			          - format      		="$.3w %02D/%02M"
			          - returnDateString	="Sun 17/05"
			          
			*/
			if (string.IsNullOrEmpty(format)) return string.Empty;
			if (t == UtcTime.Null) return string.Empty;
			if (t == UtcTime.MaxValue) return string.Format("+{0}", (char) 8734); // 8734 is the unicode for infinity (oo)
			if (t == UtcTime.MinValue) return string.Format("-{0}", (char) 8734);

			var returnDateString = new StringBuilder(80);
			var formatBuffer = new StringBuilder();
			var tempFormattedString = string.Empty;
			var len = format.Length;

			int year, month, day, hour, minute, second, yearW, weekno, weekday;
			asYMDhms_wd(t, out year, out month, out day, out hour, out minute, out second, out weekday);
			asYW(t, out yearW, out weekno);

			if (weekday == 0 && FirstDayOfWeek == System.DayOfWeek.Monday)
				{
					// according to old TIME_formdate def.: 1..7 Mo..Su
					weekday = 7;
			}
			var year2 = (year >= 1000) ? year - ((year/100)*100) : year;
			var yearW2 = (yearW >= 1000) ? yearW - ((yearW/100)*100) : yearW;
			var i = 0;

			while (i < len) {
				bool formatFound;
				if (format[i] == '%') {
					formatFound = false;
					while (i < len - 1 && !formatFound) {
						i++;
						switch (format[i]) {
						case 'Y': // year
							tempFormattedString = formatBuffer.Length > 0 &&
								formatBuffer[formatBuffer.Length - 1] == '2'
                                ? CreatePaddedString(formatBuffer, year2)
								: CreatePaddedString(formatBuffer, year);
							formatFound = true;
							break;
						case 'y': // year related to weeknumber
							tempFormattedString = formatBuffer.Length > 0 &&
								formatBuffer[formatBuffer.Length - 1] == '2'
                                ? CreatePaddedString(formatBuffer, yearW2)
								: CreatePaddedString(formatBuffer, yearW);
							formatFound = true;
							break;
						case 'M': // month
							tempFormattedString = CreatePaddedString(formatBuffer, month);
							formatFound = true;
							break;
						case 'W': // weeknumber
							tempFormattedString = CreatePaddedString(formatBuffer, weekno);
							formatFound = true;
							break;
						case 'D': // daynumber in month
							tempFormattedString = CreatePaddedString(formatBuffer, day);
							formatFound = true;
							break;
						case 'w': // daynumber in week
							tempFormattedString = CreatePaddedString(formatBuffer, weekday);
							formatFound = true;
							break;
						case 'h': // hour
							tempFormattedString = CreatePaddedString(formatBuffer, hour);
							formatFound = true;
							break;
						case 'H': // hour-number
							tempFormattedString = CreatePaddedString(formatBuffer, hour + 1);
							formatFound = true;
							break;
						case 'm': // minute
							tempFormattedString = CreatePaddedString(formatBuffer, minute);
							formatFound = true;
							break;
						case 's': // seconds
							tempFormattedString = CreatePaddedString(formatBuffer, second);
							formatFound = true;
							break;
						case 'x': // date, eg 22.04.2005 (localized)
							tempFormattedString = ToString(t, "d");
							formatFound = true;
							break;
						case 'X': // time, eg 13:51:30 (localized)
							tempFormattedString = ToString(t, "T");
							formatFound = true;
							break;
						case 'z': // Timezone name with DST info, eg Utc2 instead of W. Europe Daylight Time
							tempFormattedString = DstAt(t) == DstOffset(year) ? AlternateName : Name;
							// TimeZone uses the setting for the OS, not the calendar used...
							//var tz = TimeZone.CurrentTimeZone;    
							//tempFormattedString = ToDateTime(t).IsDaylightSavingTime()
							//                          ? tz.DaylightName
							//                          : tz.StandardName;
							formatFound = true;
							break;
						case 'Z': // Timezone offset, eg +2
							if (UtcOffset.Ticks == 0) {
								tempFormattedString = string.Empty;
							} else {
								tempFormattedString = string.Format("{0}{1}",
								                                    UtcOffset.TotalHours == 0
								                                    ? string.Empty
								                                    : (UtcOffset.TotalHours > 0 ? "+" : "-"),
								                                    UtcOffset.TotalHours);
							}
							formatFound = true;
							break;
						default:
							formatBuffer.Append(format[i]);
							break;
						}
					}
					if (!formatFound) {
						tempFormattedString = formatBuffer.ToString();
					}
				} else if (format[i] == '$' || format[i] == '#') { 
					//dates presented with characters
					formatFound = false;
					while (i < len - 1 && !formatFound) {
						i++;
						switch (format[i]) {
						case 'M': // month
							tempFormattedString = CreateSubstring(formatBuffer,
							                                      DateTimeFormatInfo.CurrentInfo.MonthNames[month - 1]);
							formatFound = true;
							break;
						case 'w': // weekday
							tempFormattedString = CreateSubstring(formatBuffer,
							                                      DateTimeFormatInfo.CurrentInfo.DayNames[weekday%7]);
							formatFound = true;
							break;
						default:
							formatBuffer.Append(format[i]);
							break;
						}
					}
					if (!formatFound) {
						tempFormattedString = formatBuffer.ToString();
					}
				} else {
					tempFormattedString = format[i].ToString();
				}
				returnDateString.Append(tempFormattedString);
				formatBuffer.Clear();
				i++;
			}
			return returnDateString.ToString();
		}

		private static string CreateSubstring(StringBuilder formatBuffer, string s)
		{
			int stringLength;
			if (formatBuffer.Length > 1 && formatBuffer[0] == '.' && int.TryParse(formatBuffer.ToString().Substring(1), out stringLength) && s.Length > stringLength) {
				return s.Substring(0, stringLength);
			}
			return s;
		}

		private static string CreatePaddedString(StringBuilder formatBuffer, int number)
		{
			const char paddingChar = '0';
			string tempFormattedString;
			int padding;
			if (int.TryParse(formatBuffer.ToString(), out padding)) {
				tempFormattedString = number.ToString().PadLeft(padding, paddingChar);
			} else {
				tempFormattedString = number.ToString();
			}
			return tempFormattedString;
		}

        //---- Some calendars are known ..
        // Lager static Dictionary med kalendere.
        private static Lazy<ConcurrentDictionary<string, RegionalCalendar>> calendars = new Lazy<ConcurrentDictionary<string, RegionalCalendar>>();
        private static ConcurrentDictionary<string, RegionalCalendar> Calendars
        {
            get
            {
                var dict = calendars.Value;
                if (dict.Count != 0)
                {
                    // The dictionary is already initialized:
                    return dict;
                }

                // First time request. Add standard, local, utc and db to the dictionary
                var normalTimeCalendar = CreateFromOs(standardName, false);
                dict.TryAdd(standardName, normalTimeCalendar);

                var localCalendar = CreateFromOs(localName, true);
                dict.TryAdd(localName, localCalendar);

                var utcCalendar = new RegionalCalendar();// - and then install this as StandardUtcStringProvider
                dict.TryAdd(utcName, utcCalendar);
                Log.Warn($"\n\tutcCalendar:\n\t{utcCalendar}");

                UtcTime.standardUtcStringProvider = new UtcTime.StandardUtcStringProviderDelegate(utcCalendar.ToString);

                // db can be overruled, but defaults to standard time zone on computer (normal time).
                var dbCalendar = Create(dbName);
                if (dbCalendar == null)
                {
                    dbCalendar = normalTimeCalendar;
                    Log.Warn($"\n\tCannot create dbCalendar, using normalTimeCalendar instead:\n\t{normalTimeCalendar}");
                }

                dict.TryAdd(dbName, dbCalendar);

                return dict;
            }
        }

        public static RegionalCalendar CreateFromOs(string Name,bool WithDst)
        {
            // Checks first if it is overruled by config settings
            var cal = Create(Name);
            if (cal == null)
            {   // If not overruled, create from OS.
                var wDay = CultureInfo.CurrentCulture.DateTimeFormat.FirstDayOfWeek;
                var week1 = (WeekOneRule) (int) CultureInfo.CurrentCulture.DateTimeFormat.CalendarWeekRule;
                cal = CreateFromTimeZone(TimeZone.CurrentTimeZone, week1 /*WeekOneRule.First4DayWeek*/, wDay/*System.DayOfWeek.Monday*/, WithDst);
            }
            return cal;
        }

        private static RegionalCalendar Create(string Name)
        {
            var tzName = "TZ_" + Name;
            var cfgVal = Environment.GetEnvironmentVariable(tzName + "_cfg");
            return string.IsNullOrEmpty(cfgVal) ? CreateFromFile(Name, tzName) : CreateFromConfig(cfgVal);
        }

        /// <summary>
        /// if Calendar[name] exists, return that
        /// else check if envvar TZ_&lt;name&gt;_cfg contains a complete simplified timezone definition
        /// else check if envvar TZ_Name adresses a file with full definition
        /// else check if envvar TZ_Name is a file which exists in ICC_LANGPATH + \basis
        /// Reads the definition and adds it to Calendars.
        /// </summary>
        public static RegionalCalendar GetCalendar(string name)
        {
            var calendarFound = Calendars.TryGetValue(name, out var cal);
            if (calendarFound)
            {
                return cal;
            }

            cal = Create(name);
            if (cal == null)
            {
                throw new ArgumentException($"Could not get calendar with the name '{name}'");
            }

            Calendars.TryAdd(name, cal);

            return cal;
        }

        private static RegionalCalendar CreateFromFile(string name, string tzName)
        {
            RegionalCalendar calendar = null;
            var envVal = Environment.GetEnvironmentVariable(tzName);
            string source = "";
            try
            {
                if (!string.IsNullOrEmpty(envVal))
                {
                    // Sjekk om den inneholder full fil-path Eks: ..\templates\TZ_example.dat
                    var fileInfo = AttemptToLocateCalendarFileInfo(envVal);
                    if (fileInfo.Exists)
                    {
                        using (var fileStream = new StreamReader(fileInfo.FullName))
                        {
                            var definition = fileStream.ReadToEnd();
                            calendar = CreateFromDefinition(definition);
                            if (calendar != null)
                            {
                                source = $"tzName = {tzName} -> envVal = {envVal} -> fileInfo = '{fileInfo.FullName}'";
                            }
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Log.Error($"\n\ttzName = {tzName}, envVal = {envVal}. Exception:\n{e.Message}");
                throw new ApplicationException("Failed to create calendar from file", e);
            }

            if (calendar == null)
            {
                // Check hardcoded definitions
                var definition = GetStaticDefinition(name);
                if (!string.IsNullOrEmpty(definition))
                {
                    calendar = CreateFromDefinition(definition);
                    if (calendar != null)
                    {
                        source = $"static definition '{name}'";
                    }
                }

                if (calendar == null)
                {
                    source = $"name = '{name}', tzName = '{tzName}'";
                }
            }

            Log.Warn($"\n\tsource = {source}\n\tcalendar = {(calendar == null ? "null" : calendar.ToString())}");

            return calendar;
        }

        private static FileInfo AttemptToLocateCalendarFileInfo(string envVal)
        {
            var fileInfo = new FileInfo(envVal);
            var langpath = Environment.GetEnvironmentVariable("ICC_LANGPATH");
            if (!fileInfo.Exists && !string.IsNullOrEmpty(langpath))
            {
                // Sjekk om fil ligger i ICC_LANGPATH
                char[] delim = { ';' };
                var paths = langpath.Split(delim);
                foreach (var path in paths)
                {
                    fileInfo = new FileInfo(path + @"\" + envVal + ".txt");
                    if (fileInfo.Exists)
                        break;

                    fileInfo = new FileInfo(path + @"\basis\" + envVal);
                    if (fileInfo.Exists)
                        break;
                }
                if (!fileInfo.Exists) // Ikke funnet
                    fileInfo = new FileInfo(envVal + ".txt"); // Sjekk med .txt-extension
            }
            return fileInfo;
        }

        /// <summary>
        /// Returns the standard UTC calendar, reuses a global calendar object.
        /// The first time called, it also installs the Utc.ToString(UtcTime t) as
        /// the StandardUtcStringProvider for the UtcTime class.
        /// </summary>
        public static RegionalCalendar UtcCalendar
        {
            get
            {
                var calendarFound = Calendars.TryGetValue(utcName, out var cal);
                if (calendarFound)
                {
                    return cal;
                }
                //SIV. Calendars.get() should have initialized and added utc calendar to dict.
                //If it's not there throw an exception:
                throw new ArgumentException($"Internal error. Could not get calendar with the name '{utcName}'");

                //cal = new RegionalCalendar();// - and then install this as StandardUtcStringProvider
                //Calendars.TryAdd(utcName, cal);
                //UtcTime.standardUtcStringProvider = new UtcTime.StandardUtcStringProviderDelegate(cal.ToString);

                //return cal;
            }
        }

        /// <summary>
        /// Returns the current regional calendar according to setting control panel etc.
        /// </summary>
        public static RegionalCalendar Current => GetCalendar(localName);

        /// <summary>
	    /// Returns the current regional calendar according to setting control panel etc without DTS.
	    /// </summary>
	    public static RegionalCalendar CurrentNT => GetCalendar(standardName);

        /// <summary>
        /// Creates a Norwegian regional calendar with historically correct
        /// daylight saving time.
        /// </summary>
        /// <param name="withDaylightSaving"> If set true, daylight saving time is recognized, otherwise
        /// the returned calendar corresponds to Norwegian standard time.</param>
        /// <returns></returns>
        public static RegionalCalendar CreateNorwegian(bool withDaylightSaving)
        {
	          //this is really hard coded stuff, for the historical period this
	          //should be no problem as long as its correct. For the future
	          //it assumes that current rule will apply until end of the dls period !
            var tzName = withDaylightSaving ? "UTC+2" : "UTC+1";
            var calendarFound = Calendars.TryGetValue(tzName, out var tz);

            if (calendarFound)
            {
                return tz;
            }

            tz = new RegionalCalendar()
            {
                UtcOffset = TimeSpan.FromHours(1),
                WeekRule = WeekOneRule.First4DayWeek,
                FirstDayOfWeek = System.DayOfWeek.Monday,
                Name = "UTC+1",
            };

            if (withDaylightSaving)
            {
                var c0100 = TimeSpan.FromHours(1);
                var dstOffset = TimeSpan.FromHours(1);
                //--	 	Yfrom,Yto,dst,Month_b Day_b sec_b,int Month_e,int Day_e,ticks sec_e, dst_off
                tz.addDSTRule(1916, 1917, true, 5, 22, c0100, 9, 30, c0100, dstOffset);
                tz.addDSTRule(1945, 1946, true, 4, 2, c0100, 10, 1, c0100, dstOffset);
                tz.addDSTRule(1959, 1965, true, 3, -3, c0100, 10, -3, c0100, dstOffset);
                tz.addDSTRule(1965, 1966, true, 4, 24, c0100, 9, 19, c0100, dstOffset);
                tz.addDSTRule(1980, 1981, true, 4, 6, c0100, 9, 28, c0100, dstOffset);
                tz.addDSTRule(1981, 1996, true, 3, -5, c0100, 9, -5, c0100, dstOffset);
                tz.addDSTRule(1996, 2099, true, 3, -5, c0100, 10, -5, c0100, dstOffset);
                tz.AlternateName = "UTC+2";
            }
            else
            {
                tz.AlternateName = "UTC+1";
            }

            Calendars.TryAdd(tzName, tz);

            return tz;
        }

        /// <summary>
        /// Create a Regional calendar according to given .NET TimeZone
        /// </summary>
        /// <remarks></remarks>
        /// <param name="tz">.NET timezone</param>
        /// <param name="weekRule">days of week still in current year</param>
        /// <param name="firstDayOfWeek">first day of week </param>
        /// <param name="withDst">If false, the dst settings for the tz is ignored</param>
        /// <param name="utcOffsetSec">assigned to new time zone UtcOffset if specified</param>
        /// <returns>RegionalCalendar</returns>
        public static RegionalCalendar CreateFromTimeZone(
            TimeZone tz, WeekOneRule weekRule, DayOfWeek firstDayOfWeek, bool withDst, double utcOffsetSec = -1000000) 
		{
            var calendar = new RegionalCalendar
            {
                WeekRule = weekRule,
                FirstDayOfWeek = firstDayOfWeek,
                Name = tz.StandardName
            };
            if (withDst) 
			{
				calendar.AlternateName = tz.DaylightName;
			    for (var y = RegionalCalendar.DaylightSavingFirstYear; y < DaylightSavingFirstYear + DaylightSavingYears; y++)
				{
                    DaylightTime dls=tz.GetDaylightChanges(y);
					if(dls.Delta.Ticks != 0) 
					{
						calendar.addDSTRule(
							new UtcTime(tz.ToUniversalTime(dls.Start).Ticks - Utc.TimeBase),
							new UtcTime((tz.ToUniversalTime(dls.End)-dls.Delta).Ticks - Utc.TimeBase),
							dls.Delta);
					}
				}
			}
		    else
		    {
		        calendar.AlternateName = calendar.Name;
		    }

            calendar.UtcOffset = utcOffsetSec < -100000 ? GetUtcOffset(calendar, tz) : TimeSpan.FromSeconds(utcOffsetSec);

            Log.Warn($"\n\ttz = {tz}, WeekRule = {weekRule}, FirstDayOfWeek = {firstDayOfWeek}, withDst = {withDst}\n\tcalendar = {calendar}");

		    return calendar;
		}

        private static TimeSpan GetUtcOffset(RegionalCalendar calendar,TimeZone tz)
        {
            //In southern hemisphere dst is reversed comparing to northern hemisphere, the normal/base time is in july, daylight saving time is in january
            if (calendar.IsSummerTimeReversed(2000 - DaylightSavingFirstYear))
            {
                return tz.GetUtcOffset(new DateTime(2000, 7, 1, 0, 0, 0, 0)); //for base utc time in southern hemisphere check 1st of July
            }
            return tz.GetUtcOffset(new DateTime(2000, 1, 1, 0, 0, 0, 0)); //for base utc time in northern hemisphere check 1st of January
        }

        public static RegionalCalendar CreateFromTimeZone(TimeSpan UtcOffset)
		{
			RegionalCalendar calendar = new RegionalCalendar();
			calendar.UtcOffset = UtcOffset;

			return calendar;
		}

        /// <summary>
        /// This method contains an incomplete reimplementation of the C++
        /// method PDTime::ReadZoneFromFile(). Refer to that method when
        /// implementing the missing pieces.
        /// </summary>
        public static RegionalCalendar CreateFromDefinition(string definition)
        {
            /* Eksempel:
#Definition of the Norwegian local time.
Name=GMT+1 AltName=GMT+1_DST
UTC=3600 # UTC+1 -> 1*3600, UTC-5 = -5 *3600
# years dst START month (month_day | - <nth-sunday>) sec_utc END month (month_day | - <nth-sunday>) dst_off
YEARS 1916 1917 1 START 5 22 3600 END 9 30 0 DST 3600
YEARS 1940 1940 1 START 8 10 82800 END 12 31 86400 DST 3600
YEARS 1941 1941 1 START 1 1 0 END 12 31 86400 DST 3600
YEARS 1942 1942 1 START 1 1 0 END 11 3 0 DST 3600
YEARS 1945 1946 1 START 4 2 7200 END 10 1 7200 DST 3600
YEARS 1959 1965 1 START 3 -3 7200 END 10 -3 3600 DST 3600
YEARS 1965 1966 1 START 4 25 7200 END 9 19 3600 DST 3600
YEARS 1980 1981 1 START 4 6 3600 END 9 28 3600 DST 3600
YEARS 1981 1996 1 START 3 -5 3600 END 9 -5 3600 DST 3600
YEARS 1996 2036 1 START 3 -5 3600 END 10 -5 3600 DST 3600
W1RULE=First4DayWeek # FirstJanWeek, First4DayWeek, FirstFullWeek
WEEKSTART=1 # 0:sunday, 1:monday, 2:tuesday etc.
EOF
             */
            //Trace.WriteLine("RegionalCalendar.CreateFromDefinition");
            //Trace.WriteLine("..definition = \n" + definition);
            var stPattern = @"Name=(?<name>\S+)\s+AltName=(?<altname>\S+)\s+UTC=(?<utc>[+-]?\d+)";
            //Trace.WriteLine("..stPattern = \n" + stPattern);
            var regex = new Regex(stPattern);
            var match = regex.Match(definition);

            //Trace.WriteLine("match.Groups.Count = " + match.Groups.Count);
            //if (match.Groups.Count > 0)
            //{
            //    for (int I = 0; I < match.Groups.Count; I++)
            //    {
            //        Trace.WriteLine("match.Groups[" + I.ToString() + "].Value = " + match.Groups[I].Value);
            //    }
            //}

            if (match.Groups.Count < 3)
                throw new Exception("Calendar definition '" + definition.Substring(0, 80) +
                    "..' has " + match.Groups.Count.ToString() + " items; expected 3");

            var name = match.Groups["name"].Value;
            var altName = match.Groups["altname"].Value;
            //Trace.WriteLine(false, "name = " + name + "; altName = " + altName +
            //        "; match.Groups[utc].Value = " + match.Groups["utc"].Value);

            if (!int.TryParse(match.Groups["utc"].Value, out var utcOffset))
                throw new Exception("Calendar definition '" + definition.Substring(0, 80) +
                    "..' has wrong representation for UTC offset");

            var calendar = new RegionalCalendar
            {
                UtcOffset = TimeSpan.FromSeconds(utcOffset),
                Name = name,
                AlternateName = altName,
            };

            // Check for DST rules
            stPattern = @"YEARS\s(?<startYear>\d+)\s(?<endYear>\d+)\s(?<dstObs>[01])\sSTART\s(?<startMonth>\d+)\s(?<startDay>-?\d+)\s(?<startSec>-?\d+)\sEND\s(?<endMonth>\d+)\s(?<endDay>-?\d+)\s(?<endSec>-?\d+)\sDST\s(?<dstOff>-?\d+)";
            regex = new Regex(stPattern);
            match = regex.Match(definition);
            while (match.Success)
            {
                calendar.DaylightObserved = true;
                //Trace.WriteLine("DST rules: " + match.Groups.Count);
                //for (int i = 0;i < match.Groups.Count;i++)
                //    Trace.WriteLine("Groups[" + i.ToString() + "].Value: " + match.Groups[i].Value);
                if (!calendar.addDSTRule(Convert.ToInt32(match.Groups["startYear"].Value),
                    Convert.ToInt32(match.Groups["endYear"].Value),
                    Convert.ToInt32(match.Groups["dstObs"].Value) == 1 ? true : false,
                    Convert.ToInt32(match.Groups["startMonth"].Value),
                    Convert.ToInt32(match.Groups["startDay"].Value),
                    TimeSpan.FromSeconds(Convert.ToInt32(match.Groups["startSec"].Value)),
                    Convert.ToInt32(match.Groups["endMonth"].Value),
                    Convert.ToInt32(match.Groups["endDay"].Value),
                    TimeSpan.FromSeconds(Convert.ToInt32(match.Groups["endSec"].Value)),
                    TimeSpan.FromSeconds(Convert.ToInt32(match.Groups["dstOff"].Value))))
                {
                    throw new Exception("Calendar definition " + calendar.Name +
                        ". Wrong DST definition for year: " + match.Groups["startYear"].Value);
                }
                match = match.NextMatch();
            }

            // Check for week1 rule
            stPattern = @"W1RULE=(?<weekOne>\w+)";
            regex = new Regex(stPattern);
            match = regex.Match(definition);
            if (match.Success)
            {
                //Trace.WriteLine("Week1 rule: " + match.Groups.Count);
                //for (int i = 0; i < match.Groups.Count; i++)
                //    Trace.WriteLine("Groups[" + i.ToString() + "].Value: " + match.Groups[i].Value);
                string rule = match.Groups["weekOne"].Value;
                calendar.WeekRule = rule == "First4DayWeek"
                                        ? WeekOneRule.First4DayWeek
                                        : rule == "FirstJanWeek" ? WeekOneRule.FirstJanWeek : WeekOneRule.FirstFullWeek;
            }

            // Check for weekStart rule
            stPattern = @"WEEKSTART=(?<weekStart>\d)";
            regex = new Regex(stPattern);
            match = regex.Match(definition);
            if (match.Success)
            {
                //Trace.WriteLine("WeekStart rule: " + match.Groups.Count);
                //for (int i = 0; i < match.Groups.Count; i++)
                //    Trace.WriteLine("Groups[" + i.ToString() + "].Value: " + match.Groups[i].Value);
                calendar.FirstDayOfWeek = (DayOfWeek)Convert.ToInt32(match.Groups["weekStart"].Value);
            }
            return calendar;
        }

        /// <summary>
        /// Converts a one line string (eg from the configuration system) to a Calendar
        /// Reimplementation of PDZone::CreatePDZoneFromCfg().
        /// 
        /// 2 field ::= 'NAME'@+-hour
        /// 3 field ::= 'NAME'@+-hour'DSTNAME'
        /// 7 field ::= 'NAME'@+-hour'DSTNAME'[m d_m d]
        ///  m ::= 1..12
        ///  d ::= 1..31 | -1..-5 {  -nth sunday in month, -5 means last sunday in month }
        /// </summary>
        /// <param name="definition"></param>
        /// <returns>Returns a Calendar, null if invalid input</returns>
        public static RegionalCalendar CreateFromConfig(string definition)
        {
            if (string.IsNullOrEmpty(definition))
            {
                Log.Warn("\n\tdefinition is empty; calendar = null");
                return null;
            }


            definition = definition.Trim();
            // NOTE: to improve performance e.g. make regex static member but will increase init time
            var addDaylightSavingRule = true;
            const int dstStartYear = 1900;
            const int dstEndYear = 2100;
            var dstStartMonth = 3;
            var dstStartDay = -5;
            var dstEndMonth = 10;
            var dstEndDay = -5;

            // try to match one of the three different formats
            var match = new Regex(
              @"^'(?<name>.+)'@(?<utcoffset>[+-]{0,1}\d*[,.]{0,1}\d*)'(?<altname>.+)'\[(?<dstStartMonth>\d+)\s(?<dstStartDay>[+-]{0,1}\d+)_(?<dstEndMonth>\d+)\s(?<dstEndDay>[+-]{0,1}\d+)\]$")
              .Match(definition);
            if (match.Success)
            {
                dstStartMonth = int.Parse(match.Groups["dstStartMonth"].Value);
                dstStartDay = int.Parse(match.Groups["dstStartDay"].Value);
                dstEndMonth = int.Parse(match.Groups["dstEndMonth"].Value);
                dstEndDay = int.Parse(match.Groups["dstEndDay"].Value);
            }
            else
            {
                match =
                  new Regex(@"^'(?<name>.+)'@(?<utcoffset>[+-]{0,1}\d*[,.]{0,1}\d*)'(?<altname>.+)'$").Match(definition);
                if (!match.Success)
                {
                    match = (new Regex(@"^'(?<name>.+)'@(?<utcoffset>[+-]{0,1}\d*[,.]{0,1}\d*)$")).Match(definition);
                    addDaylightSavingRule = false;

                    if (!match.Success)
                    {
                        Log.Warn($"\n\tCannot parse definition = {definition}; calendar = null");
                        return null;
                    }
                }
            }

            var name = match.Groups["name"].Value;

            var decSep = NumberFormatInfo.InvariantInfo.CurrencyDecimalSeparator;
            var utcOffsetString = match.Groups["utcoffset"].Value.Replace(",", decSep);

            if (!double.TryParse(utcOffsetString, out var utcOffset))
            {
                //Console.WriteLine($"TryParse({utcOffsetString}) failed");
                Log.Warn($"TryParse({utcOffsetString}) failed");
                if (utcOffsetString.Contains(","))
                    utcOffsetString = utcOffsetString.Replace(",", ".");
                else if (utcOffsetString.Contains("."))
                    utcOffsetString = utcOffsetString.Replace(".", ",");
                try
                {
                    utcOffset = Convert.ToDouble(utcOffsetString);
                }
                catch (FormatException)
                {
                    //Console.WriteLine($"ToDouble({utcOffsetString}) failed");
                    Log.Error($"ToDouble({utcOffsetString}) failed; assume 0");
                    utcOffset = 0.0;
                }
            }

            var calendar = new RegionalCalendar
            {
                Name = name,
                UtcOffset = TimeSpan.FromHours(utcOffset)
            };

            if (addDaylightSavingRule)
            {
                var alternateName = match.Groups["altname"].Value;
                calendar.AlternateName = alternateName;
                var dstOffset = TimeSpan.FromHours(1);
                for (var year = dstStartYear; year < dstEndYear; year++)
                {
                    var dstStart = calendar.ToUtcTime(year, dstStartMonth, dstStartDay);
                    var dstEnd = calendar.ToUtcTime(year, dstEndMonth, dstEndDay);
                    calendar.addDSTRule(dstStart, dstEnd, dstOffset);
                }
            }
            Log.Warn($"\n\tdefinition = {definition}\n\tcalendar = {calendar}");
            return calendar;
        }

        /// <summary>
        /// Returns the week number of a given utc time
        /// </summary>
        /// <param name="t">time in utc</param>
        /// <param name="year">out - year of week. Not necessarily same as year of utctime</param>
        /// <returns>week number</returns>
        public int GetWeekNumber(UtcTime t, out int year)
        {
            CalendarWeekRule WeekRule = CalendarWeekRule.FirstDay;
            switch (this.WeekRule)
            {
                case WeekOneRule.First4DayWeek:
                    WeekRule = CalendarWeekRule.FirstFourDayWeek;
                    if (FirstDayOfWeek == System.DayOfWeek.Monday)
                    {
                        /* Special treatment because .NET is not in conjunction with ISO standard 8601 regarding transition week numbers
                         * ISO rules defines first 4 day week and First day of week as Monday
                         */
                        int isoWeek = GetIsoWeek(this.ToDateTime(t));
                        year = isoWeek / 100;
                        return isoWeek % 100;
                    }
                    break;
                case WeekOneRule.FirstJanWeek:
                    WeekRule = CalendarWeekRule.FirstDay;
                    break;
                case WeekOneRule.FirstFullWeek:
                    WeekRule = CalendarWeekRule.FirstFullWeek;
                    break;
            }

            DateTime localTime = ToDateTime(t);

            //assume FirstJanWeek and FirstFullWeek is handled correct in .Net Framework Gregorian Calendar
            Calendar cal = new GregorianCalendar();
            int week = cal.GetWeekOfYear(localTime, WeekRule, this.FirstDayOfWeek);

            //add or subtract to year of week if necessary
            if (week == 1 && localTime.Month == 12)
                year = localTime.Year + 1; //date belongs to week in next year
            else if ((week == 52 || week == 53) && localTime.Month == 1)
                year = localTime.Year - 1;  //date belongs to week in previous year
            else
                year = localTime.Year;
            return week;
        }

        /*******Code fetched from http://www.boyet.com/Articles/PublishedArticles/CalculatingtheISOweeknumb.html*****/
        /// <summary>
        /// Returns date of Monday in week one of given year
        /// </summary>
        /// <param name="Year"></param>
        /// <returns></returns>
        private static DateTime GetIsoWeekOne(int Year) 
		{
			// get the date for the 4-Jan for this year
			DateTime dt = new DateTime(Year, 1, 4);

			// get the ISO day number for this date 1==Monday, 7==Sunday
			int dayNumber = (int) dt.DayOfWeek; // 0==Sunday, 6==Saturday
			if (dayNumber == 0) 
			{
				dayNumber = 7;
			}

			// return the date of the Monday that is less than or equal
			// to this date
			return dt.AddDays(1 - dayNumber);
		}
		/// <summary>
		/// Returns the week of a given datetime,a ccording to the ISO 8601 standard
		/// </summary>
		/// <param name="dt"></param>
		/// <returns></returns>
		private static int GetIsoWeek(DateTime dt) 
		{
			DateTime week1;
			int IsoYear = dt.Year;
			if (dt >= new DateTime(IsoYear, 12, 29)) 
			{
				week1 = GetIsoWeekOne(IsoYear + 1);
				if (dt < week1) 
				{
					week1 = GetIsoWeekOne(IsoYear);
				}
				else 
				{
					IsoYear++;
				}
			}
			else 
			{
				week1 = GetIsoWeekOne(IsoYear);
				if (dt < week1) 
				{
					week1 = GetIsoWeekOne(--IsoYear);
				}
			}
			
			return (IsoYear * 100) + ((dt - week1).Days / 7 + 1);
		}
		/********************************/

        private static readonly Dictionary<string, string> StaticDefs = new Dictionary<string, string>(5) {
            {"nordpool",
                "Name=NordPool(CET) AltName=NordPool(CEST)\n" +
                "UTC=3600\n" +               // # UTC+1 -> 1*3600
                // # years dst START month (month_day | - <nth-sunday>) sec_utc END month (month_day | - <nth-sunday>) dst_off
                "YEARS 1916 1917 1 START 5 22 3600 END 9 30 0 DST 3600\n" +
                "YEARS 1940 1940 1 START 8 10 82800 END 12 31 86400 DST 3600\n" +
                "YEARS 1941 1941 1 START 1 1 0 END 12 31 86400 DST 3600\n" +
                "YEARS 1942 1942 1 START 1 1 0 END 11 3 0 DST 3600\n" +
                "YEARS 1945 1946 1 START 4 2 7200 END 10 1 7200 DST 3600\n" +
                "YEARS 1959 1965 1 START 3 -3 7200 END 10 -3 3600 DST 3600\n" +
                "YEARS 1965 1966 1 START 4 25 7200 END 9 19 3600 DST 3600\n" +
                "YEARS 1980 1981 1 START 4 6 3600 END 9 28 3600 DST 3600\n" +
                "YEARS 1981 1996 1 START 3 -5 3600 END 9 -5 3600 DST 3600\n" +
                "YEARS 1996 2036 1 START 3 -5 3600 END 10 -5 3600 DST 3600\n" +
                "W1RULE=First4DayWeek\n" +      // # FirstJanWeek, First4DayWeek, FirstFullWeek
                "WEEKSTART=1\n"
            },
            {"statnett",
                "Name=Statnett(CET) AltName=Statnett(CEST)\n" +
                "UTC=3600\n" +               // # UTC+1 -> 1*3600
                // # years dst START month (month_day | - <nth-sunday>) sec_utc END month (month_day | - <nth-sunday>) dst_off
                "YEARS 1916 1917 1 START 5 22 3600 END 9 30 0 DST 3600\n" +
                "YEARS 1940 1940 1 START 8 10 82800 END 12 31 86400 DST 3600\n" +
                "YEARS 1941 1941 1 START 1 1 0 END 12 31 86400 DST 3600\n" +
                "YEARS 1942 1942 1 START 1 1 0 END 11 3 0 DST 3600\n" +
                "YEARS 1945 1946 1 START 4 2 7200 END 10 1 7200 DST 3600\n" +
                "YEARS 1959 1965 1 START 3 -3 7200 END 10 -3 3600 DST 3600\n" +
                "YEARS 1965 1966 1 START 4 25 7200 END 9 19 3600 DST 3600\n" +
                "YEARS 1980 1981 1 START 4 6 3600 END 9 28 3600 DST 3600\n" +
                "YEARS 1981 1996 1 START 3 -5 3600 END 9 -5 3600 DST 3600\n" +
                "YEARS 1996 2036 1 START 3 -5 3600 END 10 -5 3600 DST 3600\n" +
                "W1RULE=First4DayWeek\n" +      // # FirstJanWeek, First4DayWeek, FirstFullWeek
                "WEEKSTART=1\n"
            },
            {"svkl",
                "Name=SVK(CET) AltName=SVK(CEST)\n" +
                "UTC=3600\n" +               // # UTC+1 -> 1*3600, UTC-5 = -5 *3600
                // # years dst START month (month_day | - <nth-sunday>) sec_utc END month (month_day | - <nth-sunday>) dst_off
                "YEARS 1916 1917 1 START 5 14 3600 END 10 1 0 DST 3600\n" +     // Forkortet litt i forhold til PDZone jfr. ftp-def. over.
                "YEARS 1980 1981 1 START 4 6 3600 END 9 28 3600 DST 3600\n" +
                "YEARS 1981 1996 1 START 3 -5 3600 END 9 -5 3600 DST 3600\n" +
                "YEARS 1996 2036 1 START 3 -5 3600 END 10 -5 3600 DST 3600\n" +
                "W1RULE=First4DayWeek\n" +      // # FirstJanWeek, First4DayWeek, FirstFullWeek
                "WEEKSTART=1\n"
            },
            {"svks",
                "Name=SVK(CET) AltName=SVK(CET)\n" +
                "UTC=3600\n" +
                "W1RULE=First4DayWeek\n" +      // # FirstJanWeek, First4DayWeek, FirstFullWeek
                "WEEKSTART=1\n"
            },
            {"svk",
                "Name=SVK(CET) AltName=SVK(CET)\n" +
                "UTC=3600\n" +
                "W1RULE=First4DayWeek\n" +      // # FirstJanWeek, First4DayWeek, FirstFullWeek
                "WEEKSTART=1\n"
            },
            {"fingrid",
                "Name=Fingrid(EET) AltName=Fingrid(EEST)\n" +
                "UTC=7200\n" +
                "YEARS 1942 1942 1 START 4 3 0 END 10 3 0 DST 3600\n" +
                "YEARS 1981 1996 1 START 3 -5 3600 END 9 -5 3600 DST 3600\n" +
                "YEARS 1996 2036 1 START 3 -5 3600 END 10 -5 3600 DST 3600\n" +
                "W1RULE=First4DayWeek\n" +      // # FirstJanWeek, First4DayWeek, FirstFullWeek
                "WEEKSTART=1\n"
            }
        };
        private static string GetStaticDefinition(string defKey)
        {
            var def = "";
            if (StaticDefs.ContainsKey(defKey))
                def = StaticDefs[defKey];
            return def;
        }
	}
}
