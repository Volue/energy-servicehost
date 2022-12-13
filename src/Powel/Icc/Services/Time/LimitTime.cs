using System;

namespace Powel.Icc.Services.Time
{
	/// <summary>
	/// Summary description for LimitTime.
	/// </summary>
	public struct LimitTime : IComparable
	{
		UtcTime time;
		Limit limit;

		public LimitTime(UtcTime time, Limit limit)
		{
			this.time = time;
			this.limit = limit;
		}

		public LimitTime(UtcTime time)
		{
			this.time = time;
			this.limit = Limit.Point;
		}

		public UtcTime Time
		{
			get { return time; }
		}

		public Limit Limit
		{
			get { return limit; }
		}

		public override int GetHashCode()
		{
			return limit.GetHashCode() ^ time.GetHashCode();
		}

		public override bool Equals(object o) 
		{
			LimitTime time2 = (LimitTime)o;
			return this == time2;
		}

		public int CompareTo(object obj)
		{ 
			LimitTime other = (LimitTime)obj;

			if (time != other.time)
				return time.CompareTo(other.time);
			else if (limit == other.limit)
				return 0;
			else if (limit == Limit.LeftLimit)
				return -1;
			else if (limit == Limit.RightLimit)
				return 1;
			else if (other.limit == Limit.LeftLimit)
				return 1;
			else if (other.limit == Limit.RightLimit)
				return -1;
			else
				throw new ApplicationException("This is supposed to be unreachable code.");
		} 		
		
		public static bool operator<(LimitTime lhs, LimitTime rhs)
		{ 
			return ((IComparable)lhs).CompareTo(rhs) < 0;
		}

		public static bool operator<=(LimitTime lhs, LimitTime rhs)
		{ 
			return ((IComparable)lhs).CompareTo(rhs) <= 0;
		}

		public static bool operator>(LimitTime lhs, LimitTime rhs)
		{ 
			return ((IComparable)lhs).CompareTo(rhs) > 0;
		}

		public static bool operator>=(LimitTime lhs, LimitTime rhs)
		{ 
			return ((IComparable)lhs).CompareTo(rhs) >= 0;
		}

		public static bool operator==(LimitTime lhs, LimitTime rhs)
		{ 
			return lhs.time == rhs.time && lhs.limit == rhs.limit;
		}

		public static bool operator!=(LimitTime lhs, LimitTime rhs)
		{ 
			return !(lhs == rhs);
		}
	}

	public enum Limit : byte
	{
		Point,
		LeftLimit,
		RightLimit
	}
}
