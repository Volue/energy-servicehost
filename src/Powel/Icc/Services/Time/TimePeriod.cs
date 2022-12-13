using System.Collections.Generic;

namespace Powel.Icc.Services.Time
{
	using System;

	[Serializable]
	public class TimePeriod 
	{
		public TimePeriod() 
		{
			From = UtcTime.MinValue;
			Until = UtcTime.MaxValue;
		}

		public TimePeriod(UtcTime from, UtcTime until)
		{
			From = from;
			Until = until;
		}

        public TimePeriod(UtcTime from, TimeSpan dur)
        {
            From = from;
            Until = from + dur;
        }

        public TimePeriod(UtcTime from)
		{
			From = from;
			Until = UtcTime.MaxValue;
		}

	    //SIV. We do not use public setter, but public methods SetFrom and SetUntil to control their invocation.
        //Older implementation where From and Until were immutable encouraged sub-optimal
        //instantiation of a new TimePeriod each time its boundaries had to be changed
	    public UtcTime From { get; private set; }

	    public UtcTime Until { get; private set; }

	    public void SetFrom(UtcTime value)
	    {
	        From = value;
	    }
	    public void SetUntil(UtcTime value)
	    {
	        Until = value;
	    }

        public bool IsValid(bool allowOpenEnd = false, bool allowOpenStart = false, bool allowEqual = true)
        {
            if (allowOpenStart && allowOpenEnd)
                return true;

            var isFromValid = From.Ticks == Utc.Null || From.Ticks == Utc.MinValue ? allowOpenStart 
                : Math.Abs((UtcTime.Now - From).TotalDays) < 200 * 365; //SIV. No more than 200 years before or after today
            var isUntilValid = Until.Ticks == Utc.Null || Until.Ticks == Utc.MaxValue ? allowOpenEnd 
                : Math.Abs((UtcTime.Now - Until).TotalDays) < 200 * 365; //SIV. No more than 200 years before or after today

            if (!isFromValid || !isUntilValid)
            {
                return false;
            }

            if (allowEqual)
            {
                return From <= Until;
            }

            return From < Until;
        }

	    public bool IsInPeriod(UtcTime t)
	    {
	        return t >= From && t < Until;
	    }

        public List<TimePeriod> Split(TimeSpan resolution)
        {
            var result = new List<TimePeriod>();
            var nextFrom = From;
            var nextTo = From + resolution;

            if (nextTo >= Until)
            {
                result.Add(new TimePeriod(From, Until));
                return result;
            }

            while (nextTo < Until)
            {
                result.Add(new TimePeriod(nextFrom, nextTo));

                nextFrom = nextTo;
                nextTo = nextFrom + resolution;
            }

            result.Add(new TimePeriod(nextFrom, Until));

            return result;
        }


		public TimeSpan Duration => new TimeSpan(Until.Ticks - From.Ticks);

		public bool Intersects(TimePeriod period) 
		{
			if (period == null)
				return false;

			var p1 = (TimePeriod)MemberwiseClone();
			var p2 = (TimePeriod)period.MemberwiseClone();

            // ToDo: Checks for UtcTime.Null: This is not good, UtcTime.Null is 1. jan. 1970, and that is a valid date. UtcTime.Null should be obsolete.
            if (p1.From == UtcTime.Null)
                p1 = new TimePeriod(UtcTime.MinValue, p1.Until);
			if (p1.Until == UtcTime.Null)
			    p1 = new TimePeriod(p1.From);
			if (p2.From == UtcTime.Null)
			    p2 = new TimePeriod(UtcTime.MinValue, p2.Until);
			if (p2.Until == UtcTime.Null)
			    p2 = new TimePeriod(p2.From);

			return p1.From < p2.Until && p1.Until > p2.From || p1.From == p2.From;
		}

		public bool Intersects(UtcTime time)
		{
			return Intersects(new TimePeriod(time, time));
		}

		public TimePeriod Intersection(TimePeriod period) 
		{
			if (Intersects(period))
				return new TimePeriod(period.From > From ? period.From : From, period.Until < Until ? period.Until : Until);

		    return null;
		}

        public bool Touches(TimePeriod period)
        {
            return From == period.From || From == period.Until || Until == period.From || Until == period.Until;
        }

        /// <summary>
        /// Returns Min(From) and Max(Until) for the two periods if they Intersects or Touches.
        /// </summary>
        /// <param name="period">A valid time period. If not valid, this is returned</param>
        /// <returns>May return an invalid period if both are invalid or they neither intersects not touches</returns>
        public TimePeriod Union(TimePeriod period)
        {
            if (!period.IsValid())
                return this;
            if (!IsValid())
                return period;
            var res = new TimePeriod();
            if (Intersects(period) || Touches(period))
            {
                var from  = period.From  < From  ? period.From  : From;   // min(period.From,From)
                var until = period.Until > Until ? period.Until : Until;  // max(period.Until,Until)
                res = new TimePeriod(from,until);
            }
            return res;
        }

		public override int GetHashCode()
		{
			return From.GetHashCode() ^ Until.GetHashCode();
		}

		public override bool Equals(object o) 
		{
			var timePeriod2 = o as TimePeriod;

			return timePeriod2 != null && From.Equals(timePeriod2.From) && Until.Equals(timePeriod2.Until);
		}

		public override string ToString()
		{
            //return $"[{GetType().FullName}. From = {From}, Until = {Until}]";
		    return $"[{From}, {Until})";
        }

		public TimePeriod Trim(TimeSpan resolution)
	    {
	        return new TimePeriod(
                RegionalCalendar.UtcCalendar.trim(From, resolution),
                RegionalCalendar.UtcCalendar.trim(Until, resolution));
	    }
	}
}
