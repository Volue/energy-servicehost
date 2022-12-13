using System;
using System.Collections;
using System.Runtime.Serialization;
using LSCollections;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Interop;
//using Powel.Icc.Interop.Monitoring;
using Powel.Icc.Services.Time;

namespace Powel.Icc.TimeSeries
{
	/// <summary>
	/// Summary description for TimeSeries.
	/// </summary>
	[Serializable]
	public class TimeSeries : ICollection, IEnumerable, ISerializable, ICloneable//, IMonitorable
	{
		TimeResolution resolution = TimeResolution.Unconstrained;
		IDictionary points = new SkipList();
		int id;
		string name;
		string description;
		CurveType curveType;
		string unit;

		public TimeSeries()
		{
		}

		public TimeSeries(String name, TimeResolution resolution)
		{
			this.name = name;
			this.resolution = resolution;
		}

		public Tvq this[int index]
		{
			get
			{
				return ItemByIndex(index);
			}
		}

		public int ID
		{
			get { return id; }
			set { id = value; }
		}

		public string Name
		{
			get { return name; }
			set { name = value; }
		}

		public string Description
		{
			get { return description; }
			set { description = value; }
		}

		public TimeResolution Resolution
		{
			get { return resolution; }
			set { resolution = value; }
		}

		public CurveType CurveType
		{
			get { return curveType; }
			set { curveType = value; }
		}

        public string Unit
        {
            get { return unit; }
            set { unit = value; }
        }

        public bool UseRegistrationHandling
        {
            get; set;
        }

        public int Paraflag { get; set; }
        public UtcTime LastValidatedDate { get; set; }
        public UtcTime LastCorrectedDate { get; set; }

        public int Count
		{
			get { return points.Count; }
		}

		public ICollection Keys
		{
			get	{ return points.Keys; }
		}

		public ICollection Values
		{
			get { return points.Values; }
		}

		public bool IsReadOnly
		{
			get { return false; }
		}

		public bool IsFixedSize
		{
			get { return false; }
		}

		public object SyncRoot
		{
			get { return null; }
		}

		public bool IsSynchronized
		{
			get { return false; }
		}

		public int MonitorKey1
		{
			get { return this.id;}
			set {this.id = value;}
		}

		//TODO should reflect virtualCode. Null for now...
		//public int MonitorKey2
		//{
		//	get { return (int) Monitoring.ObjectKeyNull; }
		//	set { } 
		//}

		public int ObjectType
		{
			get { return (int) ObjectTypes.TimeSeries; }
		}

		public TimePeriod MonitorPeriod
		{
			get { return this.Span; }
			set { }
		}

		public Tvq GetTvq(LimitTime limitTime)
		{
			if (points.Contains(limitTime))
				return new Tvq(limitTime, (VQ)points[limitTime]);

			int index = IndexByTime(limitTime);

			// Before first point. Undefined.
			if (index == -1)
				return null;

			if (curveType == CurveType.Step || index == Count - 1)
				return new Tvq(limitTime, this[index].VQ);

			return new Tvq(limitTime, Interpolate(
				limitTime.Time, this[index], this[index+1]));
		}

		public Tvq GetTvq(UtcTime time)
		{
			return GetTvq(new LimitTime(time));
		}

		public double GetValue(LimitTime limitTime)
		{
			return GetTvq(limitTime).Value;
		}

		public double GetValue(UtcTime time)
		{
			return GetValue(new LimitTime(time));
		}

		Tvq ItemByIndex(int index)
		{
			int i=0;

			foreach (Tvq tvq in this)
			{
				if (i++ == index)
					return tvq;
			}

			throw new ArgumentOutOfRangeException();
		}

		int IndexByTime(LimitTime time)
		{
			int i=0;

			foreach (Tvq tvq in this)
			{
				if (tvq.LimitTime <= time)
					i++;
				else 
					break;
			}

			i--;

			return i;
		}

		double Interpolate(UtcTime time, Tvq tvq1, Tvq tvq2)
		{
			return tvq1.Value + (tvq2.Value - tvq1.Value) * 
				(time.Ticks - tvq1.Time.Ticks) / 
				(tvq2.Time.Ticks - tvq1.Time.Ticks);
		}

		public void SetValue(Tvq tvq, bool setModified)
		{
			if (setModified)
				tvq.Quality = tvq.Quality.SetModified(true);

			if (points.Contains(tvq.LimitTime))
				points[tvq.LimitTime] = tvq.VQ;
			else
				points.Add(tvq.LimitTime, tvq.VQ);
		}

		public void SetValue(Tvq tvq)
		{
			SetValue(tvq, true);
		}

		public void SetValue(LimitTime time, double value)
		{
			SetValue(new Tvq(time, value));
		}

		public void SetValue(UtcTime time, double value)
		{
			SetValue(time, value, true);
		}

		public void SetValue(UtcTime time, double value, bool setModified)
		{
			SetValue(new Tvq(time, value), setModified);
		}

		public void SetModified(LimitTime time, bool modified)
		{
			Tvq tvq = GetTvq(time);
			tvq.Quality = tvq.Quality.SetModified(modified);
		}

		public void SetModified(UtcTime time, bool modified)
		{
			SetModified(new LimitTime(time), modified);
		}

		public bool Contains(LimitTime time)
		{
			return points.Contains(time);
		}

		public bool Contains(UtcTime time)
		{
			return points.Contains(new LimitTime(time));
		}

		public void Clear()
		{
			points.Clear();
		}

		public void Remove(LimitTime time)
		{
		}

		public void CopyTo(Array array, int index)
		{
		}

		public object Clone()
		{
			return Clone(true);
		}

		public TimeSeries Clone(bool includeData)
		{
			TimeSeries ts = new TimeSeries(this.name, this.resolution);
			ts.description = this.description;
			ts.unit = this.unit;

			if (includeData)
			{
				foreach (Tvq tvq in this)
					ts.SetValue((Tvq)tvq.Clone(), false);
			}

			return ts;
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return GetEnumerator();
		}

		public TimeSeriesEnumerator GetEnumerator()
		{
			return new TimeSeriesEnumerator(this);
		}

		public void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			throw new InvalidOperationException("Serialization not yet supported.");
		}

		public TimePeriod Span
		{
			get
			{
				int count = this.Count;

				if (count == 0)
					return null;

				return new TimePeriod(this[0].Time, this[count-1].Time);
			}
		}

	    public class TimeSeriesEnumerator : IEnumerator
		{
			IDictionaryEnumerator internalEnumerator;

			public TimeSeriesEnumerator(TimeSeries ts)
			{
				internalEnumerator = ts.points.GetEnumerator();
			}

			object IEnumerator.Current
			{
				get { return Current; }
			}

			public Tvq Current
			{
				get 
				{ 
					return new Tvq(
						(LimitTime)internalEnumerator.Key, 
						(VQ)internalEnumerator.Value); 
				}
			}

			public bool MoveNext()
			{
				return internalEnumerator.MoveNext();
			}

			public void Reset()
			{
				internalEnumerator.Reset();
			}
		}
	}
}
