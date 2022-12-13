using System;
#if TSS_Availability
using Powel.Status;
#else
using Powel.Icc.Services.DataContracts;
#endif
using Powel.Icc.Services.Time;

namespace Powel.Icc.TimeSeries
{
	public class Tvq : ICloneable
	{
		LimitTime time;
		VQ vq;

		internal Tvq(LimitTime time, VQ vq)
		{
			this.time = time;
			this.vq = vq;
		}

		public Tvq(LimitTime time, double value)
		{
			this.time = time;
			this.vq = new VQ(value);
		}

		internal Tvq(UtcTime time, VQ vq)
		{
			this.time = new LimitTime(time, Limit.Point);
			this.vq = vq;
		}

		public Tvq(UtcTime time, double value)
		{
			this.time = new LimitTime(time, Limit.Point);
			this.vq = new VQ(value);
		}

		public Tvq(UtcTime time, double value, Quality quality)
		{
			this.time = new LimitTime(time, Limit.Point);
			this.vq = new VQ(value, quality);
		}

		public LimitTime LimitTime 
		{
			get { return time; }
		}

		public UtcTime Time 
		{
			get { return time.Time; }
		}

		public Limit Limit
		{
			get { return time.Limit; }
		}

		internal VQ VQ
		{
			get { return vq; }
			set { this.vq = value; }
		}

		public double Value 
		{
			get { return vq.Value; }
			set { vq = new VQ(value, vq.Quality.SetModified(true)); }
		}

		public Quality Quality
		{
			get { return vq.Quality; }
			set { vq = new VQ(vq.Value, value); }
		}

		public override int GetHashCode()
		{
			return time.GetHashCode() ^ vq.GetHashCode();
		}

		public object Clone()
		{
			return new Tvq(this.time, this.vq);
		}

		public override bool Equals(object o) 
		{
			Tvq tvq2 = o as Tvq;

			if (tvq2 == null)
				return false;

			if (!time.Equals(tvq2.time))
				return false;
			
			return vq.Equals(tvq2.vq);
		}
	}

	[Obsolete("Use the new Tvq class that is based on LimitTime.")]
	public class ObsoleteTvq : IComparable
	{
		private DateTime time;
		private double value;
		private Quality quality;

		public ObsoleteTvq()
		{
		}

		public ObsoleteTvq(DateTime dt)
		{
			time = dt;
			value = Double.NaN;
			quality = new Quality();
		}

		public ObsoleteTvq(DateTime t, double v)
		{
			time = t;
			value = v;
			quality = new Quality();
		}

		public ObsoleteTvq(DateTime t, double v, Quality q)
		{
			time = t;
			value = v;
			quality = q;
		}

		public DateTime Time 
		{
			get { return time; }
			set { time = value; }
		}
		public double Value 
		{
			get { return value; }
			set { this.value = value; }
		}
		public Quality Quality 
		{
			get { return quality; }
			set { quality = value; }
		}

//		public override int GetHashCode()
//		{
//			return time.Ticks;
//		}

		public int CompareTo(object o)
		{
			if (!(o is ObsoleteTvq))
				throw new InvalidOperationException("Can not compare a ObsoleteTvq to anything but another ObsoleteTvq.");

			ObsoleteTvq other = (ObsoleteTvq)o;
			return time.CompareTo(other.time);
		}
	}
}
