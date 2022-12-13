using System;

namespace Powel.Icc.Services.Time
{
	/// <summary>
	/// UtcTimeConstants alias UTC
	/// </summary>
	public struct Utc
    { // UtcTimeConstants
		public const long MaxValue           = 0x7FFFFFFFFFFFFFFFL;
		public const long MinValue           = -MaxValue;
		public const long Null               = 0L; // MinValue -1L;
		public const long Second             = 1 * TimeSpan.TicksPerSecond; // actually it is TimeSpan.TicksPerSecond
		public const long MilliSecond        = Second / 1000L;
		public const long MicroSecond        = MilliSecond / 1000L;
		public const long Minute             = Second * 60;
		public const long Hour               = Minute * 60;
		public const long Day                = Hour * 24;
		public const long Week               = Day * 7;
		public const long Month              = Day * 30; // only for abstract use
		public const long Year               = Day * 365; // only for abstract use
		public const long TimeBase           = 0; //621355968000000000L; // DateTime(1970,1,1,0,0,0).Ticks
		public const long WeekDayStartNumber = 1; // day of week where TimeBase starts. 1970 is 4, year 1 is 1
		public const long PdTime1970         = 621355968000000000L;
		public const long PdtimeBigBang      = long.MinValue;
		public const long PdtimeGnabGib      = long.MaxValue;
		public const long OADateBigBang      = -31241376000000000L;
		public const long OADateGnabGib      = 31241376000000000L;		
	};

	/// <summary>
	/// Represents linear time, a time-axis 
	/// It is related to the calendar by its zero value
	/// which is the same as the date 1970.01.01 UTC
	/// </summary>
	[Serializable]
	public struct UtcTime : IComparable
	{
	    public const string UtcDateFormat = "u"; // "yyyy-MM-dd HH:mm:sszzz";

        /// <summary>
        /// The linear UtcTime does not know any calendar,
        /// but it could be useful to get a standard readable
        /// string representation somehow.
        /// The delegate StandardUtcStringProvider type takes
        /// UtcTime as argument and is supposed to return a
        /// readable standard string as result.
        /// For simplicity this delegate is set to the RegionalCalendar.utc
        /// first time the RegionalCalendar.utc is called.
        /// </summary>
        public delegate string StandardUtcStringProviderDelegate(UtcTime t);

		/// <summary>
		/// public delegate for the standard UtcStringProvider
		/// <see cref="StandardUtcStringProvider"/>
		/// </summary>
		[NonSerialized]
		public static StandardUtcStringProviderDelegate standardUtcStringProvider;

        public UtcTime(long ticks) { Ticks = ticks; }
        /// <summary>
        /// Linear time ticks since Utc.TimeBase UTC
        /// </summary>

        public static UtcTime CreateFromPdtime(long pdtime)
        {
            //pdtime is number of seconds from the date 1970.01.01 UTC
            //Copied from PDTime.h:
            const long NOPDTIME = -0x7FFFFFFFFFFFFFFFL - 1L;
            const long MINPDTIME = -62135769600L;
            const long MAXPDTIME = 253402214400L; // 3AFFF2F000

            if (pdtime == NOPDTIME)
                return Null;
            if (pdtime == Utc.PdtimeBigBang || pdtime < MINPDTIME)
                return MinValue;
            if (pdtime == Utc.PdtimeGnabGib || pdtime > MAXPDTIME)
                return MaxValue;

            return new UtcTime(pdtime * Utc.Second + Utc.PdTime1970);
        }

        public long ToPdtime()
		{
			//pdtime is number of seconds from the date 1970.01.01 UTC			
			if (Ticks == Utc.Null)
				return 0;

			if (Ticks == Utc.MinValue)
				return Utc.PdtimeBigBang;

            if (Ticks == Utc.MaxValue)
				return Utc.PdtimeGnabGib;

            return (Ticks - Utc.PdTime1970) / Utc.Second;
		}

        public long Ticks { get; set; }

        public bool IsNull => Ticks == Utc.Null;

        public DateTime ToDateTime()
        {
            if (this == Null)
                return default(DateTime);
            if (this == MinValue)
                return DateTime.MinValue;
            if (this == MaxValue)
                return DateTime.MaxValue;
            return new DateTime(Ticks + Utc.TimeBase, DateTimeKind.Utc); // Removed the 'z' format character which does not go well with a UTC DateTime. The 'z' is actually wrong because it shows the zone offset for the OS.
        }

        /// <summary>
        /// Gets the local date component in UTC. E.g. 2020-02-01 03:00 UTC will give 2020-01-31 23:00 UTC if UTC offset is +01:00
        /// </summary>
        /// <returns>Returns new UtcTime object truncated to day resolution</returns>
        public UtcTime LocalDate()
        {
            if (this == Null)
                return Null;
            if (this == MinValue)
                return MinValue;
            if (this == MaxValue)
                return MaxValue;
            //SIV. This logic is flawed and results in weird side-effects around DLST switch days.
            //I, however, let it be as it is since it is only used by TimeAxisBuilder tests
            //DLST should be abandoned in EU ca 2022, so the code would be obsolete by that time
            var diffUtc = RegionalCalendar.Current.diffUtc(this - TimeSpan.FromHours(24)); //To account for DLST switch days
            var date = (this + diffUtc).ToDateTime().Date;
            return RegionalCalendar.Current.ToUtcTime(date - diffUtc);
            //var daySpan = TimeSpan.FromDays(1);
            //return new UtcTime(_t - _t % daySpan.Ticks);
        }

        /// <summary>
        /// Converts UtcTime to its canonical form
        /// </summary>
        /// <remarks>Note that the canonical form might
        /// only be valid for year 1 trough 9999</remarks>
        /// <returns></returns>
        public override string ToString()
        {
            if (standardUtcStringProvider != null)
                return standardUtcStringProvider(this);
            if (this == Null)
				return "Null";
            return ToDateTime().ToString(UtcDateFormat);
        }

        public string ToString(string format)
        {
            //if (standardUtcStringProvider != null)
            //	return standardUtcStringProvider(this);
            if (this == Null)
                return "Null";
            return ToDateTime().ToString(format);
        }


        public string ToXmlDateTime()
		{	
			if (this == UtcTime.Null)
				 return null;

			var dt = ToDateTime();
			var fractions = dt.Ticks % 10000000;
		    return 
                $"{dt.Year:0000}-{dt.Month:00}-{dt.Day:00}" +
                $"T{dt.Hour:00}:{dt.Minute:00}:{dt.Second:00}.{fractions:0000000}+00:00";
		}

		public UtcTime TrimToMillisecondAccuracy()
		{
			if( Ticks == Utc.MaxValue || Ticks == Utc.MinValue || Ticks == Utc.Null)
				return this; //trimming of MinValue,MaxValue, Null makes no sense, only errors (when transforming to database string)
			
			return new UtcTime(this.Ticks - this.Ticks % 10000);
		}

		/// <summary>
		/// Constants
		/// </summary>
		public static UtcTime MaxValue => new UtcTime(Utc.MaxValue);
		public static UtcTime MinValue => new UtcTime(Utc.MinValue);
		public static UtcTime Null => new UtcTime(Utc.Null);

        private static readonly string oneOfOperandsIsNull = "One of UtcTime operands is Null";
		/// <summary>
		/// The difference between two points in time is a TimeSpan
		/// Its a question how we should handle Nil ..
		/// We could throw exeption or return
		/// a TimeSpan.Nil value ?
		/// </summary>
		/// <param name="lhs"></param>
		/// <param name="rhs"></param>
		/// <returns>TimeSpan difference</returns>
		public static TimeSpan operator-(UtcTime lhs,UtcTime rhs)
		{
			if (lhs.Ticks == Utc.Null || rhs.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

		    return new TimeSpan(lhs.Ticks - rhs.Ticks);
		}

        /// <summary>
        /// Add TimeSpan to UtcTime, this is a pure arithmetics operation
        /// based on ticks. No calendar arithmetics/sematics whatsoever
        /// </summary>
        /// <param name="lhs"></param>
        /// <param name="rhs"></param>
        /// <returns></returns>
        public static UtcTime operator+(UtcTime lhs,TimeSpan rhs)
		{
			if (lhs.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

		    if (lhs == UtcTime.MinValue)
                return UtcTime.MinValue;
		    if (lhs == UtcTime.MaxValue)
		        return UtcTime.MaxValue;

            return new UtcTime(lhs.Ticks+rhs.Ticks);
		}

		/// Subtract TimeSpan from UtcTime, this is a pure arithmetics operation
		/// based on ticks. No calendar arithmetics/sematics whatsoever
		public static UtcTime operator-(UtcTime lhs,TimeSpan rhs)
		{
			if (lhs.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

		    if (lhs == UtcTime.MinValue)
		        return UtcTime.MinValue;
		    if (lhs == UtcTime.MaxValue)
		        return UtcTime.MaxValue;

		    return new UtcTime(lhs.Ticks - rhs.Ticks);
		}
        public static bool operator >= (UtcTime a, UtcTime b)
		{
			if (a.Ticks == Utc.Null || b.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

			return a.Ticks>=b.Ticks;
		}
		public static bool operator <= (UtcTime a,UtcTime b)
		{
			if (a.Ticks == Utc.Null || b.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

			return a.Ticks<=b.Ticks;
		}
		public static bool operator == (UtcTime a,UtcTime b) 
		{
			return a.Ticks==b.Ticks;
		}
		public static bool operator != (UtcTime a,UtcTime b) 
		{
			return a.Ticks!=b.Ticks;
		}
		public static bool operator <  (UtcTime a,UtcTime b) 
		{
			if (a.Ticks == Utc.Null || b.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

			return a.Ticks <b.Ticks;
		}
		public static bool operator >  (UtcTime a,UtcTime b) 
		{
			if (a.Ticks == Utc.Null || b.Ticks == Utc.Null)
				throw new ArgumentNullException(oneOfOperandsIsNull);

			return a.Ticks >b.Ticks;
		}
		public override bool Equals(object o) {
			if( o==null )
				return false;
			if (o is UtcTime)
				return Ticks==((UtcTime)o).Ticks;
			return false;
		}

		public int CompareTo(object obj)
		{
			return Ticks.CompareTo(((UtcTime)obj).Ticks);
		}

		public static int Compare(UtcTime u1, UtcTime u2)
		{
			return u1.Ticks.CompareTo(u2.Ticks);
		}

		public override int GetHashCode() { return Ticks.GetHashCode(); }

		public static UtcTime Now => new UtcTime(DateTime.UtcNow.Ticks - Utc.TimeBase);
	};
}
