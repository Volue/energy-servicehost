using System;
using System.Collections;
using System.Data;
using System.Diagnostics;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Services.Time;

namespace Powel.Icc.TimeSeries
{
	/// <summary>
	/// Summary description for TimeSeriesAge.
	/// </summary>
	public class TimeSeriesAge
	{
		SortedList segments = new SortedList();
		TimeSeriesData.TimeSeriesInfo timeSeriesInfo;

		public TimeSeriesAge()
		{
		}

        public TimeSeriesAge(string timeSeriesName)
        {
            this.timeSeriesInfo = new TimeSeriesData.TimeSeriesInfo(timeSeriesName);
        }

        public TimeSeriesAge(DataTable dt)
		{
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			if (dt.Rows.Count == 0)
				throw new ArgumentException("The DataTable is empty.");

			foreach (DataRow dr in dt.Rows)
			{
                timeSeriesInfo = new TimeSeriesData.TimeSeriesInfo();
                timeSeriesInfo.tims_key = (int)dr["tims_key"];
                timeSeriesInfo.filename = (string)dr["filename"];
                timeSeriesInfo.tscode = (string)dr["tscode"];
				UtcTime from = calendar.ToUtcTime((string)dr["segment_from"]);
				UtcTime until = calendar.ToUtcTime((string)dr["segment_until"]);
				UtcTime age = calendar.ToUtcTime((string)dr["age"]);

				AddSegment(new TimePeriod(from, until), age);
			}
		}

        public string TimeSeriesName
		{
			get { return timeSeriesInfo.FullName; }
		}

        public AgeSegment[] Segments
		{
			get 
			{ 
				AgeSegment[] array = new AgeSegment[segments.Count];
				segments.Values.CopyTo(array, 0);
				return array;
			}
		}

		public UtcTime GetAge(UtcTime timeSeriesPoint)
		{
			timeSeriesPoint = timeSeriesPoint.TrimToMillisecondAccuracy();

			foreach (AgeSegment segment in segments.Values)
				if (segment.Period.Intersects(timeSeriesPoint))
					return segment.Age;

			return UtcTime.Null;
		}

		public bool IsOlder(UtcTime timeSeriesPoint, UtcTime sourceTimeStamp)
		{
			timeSeriesPoint = timeSeriesPoint.TrimToMillisecondAccuracy();
			sourceTimeStamp = sourceTimeStamp.TrimToMillisecondAccuracy();

			UtcTime age = GetAge(timeSeriesPoint);
			return age == UtcTime.Null ? false : (sourceTimeStamp < age);
		}

		/// <summary>
		/// This method adds segments as they are, assuming that the segments
		/// are valid and non-overlapping. It should not be exposed publicly, 
		/// but is needed when initializing from a database for performance
		/// reasons, as thousands of concurrent updates are too slow.
		/// </summary>
		public void AddSegment(TimePeriod period, UtcTime age)
		{
			segments.Add(period.From, new AgeSegment(period, age));
		}

		public void UpdateSegment(TimePeriod period, UtcTime age)
		{
			period = new TimePeriod(
				period.From.TrimToMillisecondAccuracy(),
				period.Until.TrimToMillisecondAccuracy());

			age = age.TrimToMillisecondAccuracy();

			if (period.Duration == TimeSpan.Zero)
				throw new ArgumentException("TimeSeriesAge cannot contain segments of zero duration.", "period");

			// At the moment there is just one new segment, but it could be 
			// split in several sub-segments after the following trimmings.
			SortedList newSegments = new SortedList();
			newSegments.Add(period.From, new AgeSegment(period, age));

			// First trim away (and possibly split) the overlapping parts
			// of the new segment that are older than existing segments.
			// When age is equal, the most recently received data wins.
			TrimOverlappingAndOlder(newSegments, segments, false);

			// Then make room for the new sub-segments by trimming away the
			// overlapping parts of the existing segments that are older
			// than the new one.
			// When age is equal, the most recently received data wins.
			TrimOverlappingAndOlder(segments, newSegments, true);

			foreach (AgeSegment newSegment in newSegments.Values)
				segments.Add(newSegment.Period.From, newSegment);

			MergeAdjacentSegments();
		}

		public TimePeriod[] TrimOverlappingAndOlder(TimePeriod periodToTrim, UtcTime age)
		{
			periodToTrim = new TimePeriod(
				periodToTrim.From.TrimToMillisecondAccuracy(),
				periodToTrim.Until.TrimToMillisecondAccuracy());

			age = age.TrimToMillisecondAccuracy();

			SortedList segmentsToTrim = new SortedList();
			SortedList segmentsToCompareWith = new SortedList();

			segmentsToTrim.Add(periodToTrim.From, new AgeSegment(periodToTrim, age));

			foreach (AgeSegment segment in this.Segments)
				segmentsToCompareWith.Add(segment.Period.From, segment);

			TrimOverlappingAndOlder(segmentsToTrim, segmentsToCompareWith, false);

			TimePeriod[] result = new TimePeriod[segmentsToTrim.Count];

			for (int i=0; i<result.Length; i++)
				result[i] = ((AgeSegment)segmentsToTrim.GetByIndex(i)).Period;

			return result;
		}

		void TrimOverlappingAndOlder(SortedList segmentsToTrim, SortedList segmentsToCompareWith, bool trimWhenSameAge)
		{
			// We will trim/split any segments that overlap with any of the
			// segments to compare with, provided they are older. A segment 
			// may be split in two if it extends beyond the new one in both 
			// directions, or it may disappear if the new one overlaps it 
			// completely, hence TrimOverlap may return zero, one or two 
			// new segments. These will be inserted at the beginning of the 
			// collection, while the original is removed.

			int segmentsToCompareWithIndex = 0;

			for (int i=0; i<segmentsToTrim.Count; i++)
			{
				AgeSegment segmentToTrim = (AgeSegment)segmentsToTrim.GetByIndex(i);

				// Skip past all non-overlapping segments on the left hand side of segmentsToCompareWith
				while (segmentsToCompareWithIndex < segmentsToCompareWith.Count &&
					((AgeSegment)segmentsToCompareWith.GetByIndex(segmentsToCompareWithIndex)).Period.Until <= segmentToTrim.Period.From)
					segmentsToCompareWithIndex++;

				// We might be finished now
				if (segmentsToCompareWithIndex == segmentsToCompareWith.Count)
					return;

				// Check all overlapping segments
				while (segmentsToCompareWithIndex < segmentsToCompareWith.Count &&
					((AgeSegment)segmentsToCompareWith.GetByIndex(segmentsToCompareWithIndex)).Period.From < segmentToTrim.Period.Until)
				{
					AgeSegment compareSegment = (AgeSegment)segmentsToCompareWith.GetByIndex(segmentsToCompareWithIndex);

					if ((trimWhenSameAge && compareSegment.Age >= segmentToTrim.Age) ||
						(!trimWhenSameAge && compareSegment.Age > segmentToTrim.Age))
					{
						segmentsToTrim.RemoveAt(i--); // Now decrementing i, to make the for-loop iterate to the correct compareSegment.

						foreach (AgeSegment trimmedSegment in segmentToTrim.TrimOverlapWith(compareSegment.Period))
						{
							segmentsToTrim.Add(trimmedSegment.Period.From, trimmedSegment);
							i++; // To make the for-loop index point to the same compareSegment as before
						}
					}

					// We might be finished now
					if (segmentsToTrim.Count == 0)
						return;

					segmentToTrim = (AgeSegment)segmentsToTrim.GetByIndex((int)Math.Max(i, 0));
					segmentsToCompareWithIndex++;
				}

				// The next segmentToTrim might need to compare with this last compareSegment also.
				if (segmentsToCompareWithIndex > 0)
					segmentsToCompareWithIndex--;
			}
		}

		void MergeAdjacentSegments()
		{
			for (int i=0; i<segments.Count - 1; i++)
			{
				AgeSegment segment1 = (AgeSegment)segments.GetByIndex(i);
				AgeSegment segment2 = (AgeSegment)segments.GetByIndex(i+1);

				if (segment1.Age == segment2.Age && segment1.Period.Until == segment2.Period.From)
				{
					segment1.Period = new TimePeriod(segment1.Period.From, segment2.Period.Until);
					segments.RemoveAt(i+1);
				}
			}
		}

        /// Removed internal because tests are now external in Powel.Icc.Test
        /*internal*/
        public class AgeSegment
		{
			TimePeriod period;
			UtcTime age;

			public AgeSegment(TimePeriod period, UtcTime age)
			{
				this.period = new TimePeriod(period.From.TrimToMillisecondAccuracy(), period.Until.TrimToMillisecondAccuracy());
				this.age = age.TrimToMillisecondAccuracy();
			}

			public TimePeriod Period
			{
				get { return period; }
				set { period = value; }
			}

			public UtcTime Age
			{
				get { return age; }
			}

			public AgeSegment[] TrimOverlapWith(TimePeriod period)
			{
				period = new TimePeriod(
					period.From.TrimToMillisecondAccuracy(),
					period.Until.TrimToMillisecondAccuracy());

				Debug.Assert(period.Duration > TimeSpan.Zero);

				SortedList newSegments = new SortedList();

				if (!this.period.Intersects(period))
					return new AgeSegment[] { this };

				if (this.period.From < period.From)
					newSegments.Add(this.period.From, new AgeSegment(
						new TimePeriod(this.period.From, period.From), 
						this.age));

				if (this.period.Until > period.Until)
					newSegments.Add(period.Until, new AgeSegment(
						new TimePeriod(period.Until, this.period.Until), 
						this.age));

				AgeSegment[] array = new AgeSegment[newSegments.Count];
				newSegments.Values.CopyTo(array, 0);
				return array;
			}

			public override int GetHashCode()
			{
				return period.GetHashCode() ^ age.GetHashCode();
			}

			public override bool Equals(object o) 
			{
				AgeSegment ageSegment2 = o as AgeSegment;

				if (ageSegment2 == null)
					return false;

				return period.Equals(ageSegment2.period) && age.Equals(ageSegment2.age);
			}
		}
	}
}
