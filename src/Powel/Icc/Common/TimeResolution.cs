using System;
using System.ComponentModel;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Common
{
	/// <summary>
	/// Summary description for TimeResolution.
	/// </summary>
	public struct TimeResolution
	{
	    private readonly int _value;

        public TimeResolution(int value)
		{ 
			_value = value; 
		}

	    public int Value => _value;

        public byte Steptype { 
            get
            {
                switch (Value)
                {
                    case -1: return 2;	// Var int
                    case 23: return 7;	// Min 15
                    case 101: return 4;	// Hour
                    case 111: return 5;	// Day
                    case 121: return 6;	// Week
                    case 131: return 8;	// Month
                    case 141: return 9;	// Year
                }
	            throw new InvalidEnumArgumentException("Illegal TimeResolution.Value:" + Value);
	        } 
        }
		public static TimeResolution Unconstrained
		{
			get { return new TimeResolution(-1); }
		}

		public static TimeResolution Minute15
		{
			get { return new TimeResolution(23); }
		}

		public static TimeResolution Hour
		{
			get { return new TimeResolution(101); }
		}

		public static TimeResolution Day
		{
			get { return new TimeResolution(111); }
		}

		public static TimeResolution Week
		{
			get { return new TimeResolution(121); }
		}

		public static TimeResolution Month
		{
			get { return new TimeResolution(131); }
		}

		public static TimeResolution Year
		{
			get { return new TimeResolution(141); }
		}

		public override bool Equals(object obj)
		{
			TimeResolution other = (TimeResolution)obj;
			return (other == this);
		}

		public override int GetHashCode()
		{
			return _value.GetHashCode();
		}

		public static bool operator==(TimeResolution lhs, TimeResolution rhs)
		{ 
			return lhs._value == rhs._value;
		}

		public static bool operator!=(TimeResolution lhs, TimeResolution rhs)
		{ 
			return !(lhs == rhs);
		}

		public override string ToString()
		{
			if (this == TimeResolution.Minute15)
				return "Minute15";
			else if (this == TimeResolution.Hour)
				return "Hour";
			else if (this == TimeResolution.Day)
				return "Day";
			else if (this == TimeResolution.Week)
				return "Week";
			else if (this == TimeResolution.Month)
				return "Month";
			else if (this == TimeResolution.Year)
				return "Year";
			else if (this == TimeResolution.Unconstrained)
				return "Unconstrained";
			else
				throw new InvalidOperationException("This time resolution is undefined.");
		}

		public UtcTime Step(UtcTime t, int count, RegionalCalendar calendar)
		{
			if (this == TimeResolution.Minute15)
				return calendar.AddMinutes(t, 15 * count);
			else if (this == TimeResolution.Hour)
				return calendar.AddHours(t, count);
			else if (this == TimeResolution.Day)
				return calendar.AddDays(t, count);
			else if (this == TimeResolution.Week)
				return calendar.AddDays(t, 7 * count);
			else if (this == TimeResolution.Month)
				return calendar.AddMonths(t, count);
			else if (this == TimeResolution.Year)
				return calendar.AddYears(t, count);
			else
				throw new InvalidOperationException("This particular time resolution does not support stepping.");
		}
	}
}
