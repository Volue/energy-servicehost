using System;
using System.Data;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
#if !TSS_Availability
using Powel.Icc.Interop;
using Powel.Icc.Services.Time;
#else
using Powel.Time;
#endif
using log4net;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for TimeSeriesData.
	/// </summary>
	public delegate string CreateTsNameDelegate(RestrictionData.RestrCompCon con);
	public sealed class STEPTYPE 
	{
		public const int BREAKPOINT = 2;
		public const int MIN15 = 7;
		public const int HOUR = 4;
		public const int DAY = 5;
		public const int WEEK = 6;
		public const int MONTH = 8;
		public const int YEAR = 9;
	}
	
	public class TsUtil 
	{
		public const int TS_VL_MISSING = 0x4000000;
		
		/// <summary>
		/// Sorry.. yet another place needed:
		/// </summary>
		/// <param name="timeSpan"></param>
		/// <returns></returns>
		public static int TimeSpanToStepType(TimeSpan timeSpan) {
			if (timeSpan.Ticks == 0) return STEPTYPE.BREAKPOINT;
			else if (timeSpan.Ticks == 15 * Utc.Minute) return STEPTYPE.MIN15;
			else if (timeSpan.Ticks == Utc.Hour) return STEPTYPE.HOUR;
			else if (timeSpan.Ticks == Utc.Day) return STEPTYPE.DAY;
			else if (timeSpan.Ticks == Utc.Week) return STEPTYPE.WEEK;
			else if (timeSpan.Ticks == Utc.Month) return STEPTYPE.MONTH;
			else if (timeSpan.Ticks == Utc.Year) return STEPTYPE.YEAR;
			return STEPTYPE.BREAKPOINT;
			//throw new NotImplementedException();
		}
		public static TimeSpan StepTypeToTimeSpan(int stepType) {
			if (stepType == STEPTYPE.BREAKPOINT) return new TimeSpan(0);
			else if (stepType == STEPTYPE.MIN15) return new TimeSpan(15 * Utc.Minute);
			else if (stepType == STEPTYPE.HOUR) return new TimeSpan(Utc.Hour);
			else if (stepType == STEPTYPE.DAY) return new TimeSpan(Utc.Day);
			else if (stepType == STEPTYPE.WEEK) return new TimeSpan(Utc.Week);
			else if (stepType == STEPTYPE.MONTH) return new TimeSpan(Utc.Month);
			else if (stepType == STEPTYPE.YEAR) return new TimeSpan(Utc.Year);
			return new TimeSpan(0);
		}
	}
	/// <summary>
	///  Minimal Interface for an item that specifies what portion of a TS to read
	/// </summary>
	public interface ITsReadItem {
		int TimsKey { get;}
		int VlCode { get;}
		TimeSpan deltaT {get;}
		TimePeriod timePeriod {get;}
	};
	/// <summary>
	/// Concrete useful implementation of ITsReadItem that we can use
	/// </summary>
	public class TsReadItem : ITsReadItem,IComparable<TsReadItem>
    {
		public TsReadItem(int tk,int vlc,TimeSpan dT,TimePeriod p)
        {
			TimsKey = tk; VlCode = vlc; deltaT = dT; timePeriod = p;
		}
		public int TimsKey { get; set; }
		public int VlCode { get; set; }
		public TimeSpan deltaT { get; set; }
		public TimePeriod timePeriod { get; set; }
		public int CompareTo(TsReadItem x) {
			if (deltaT < x.deltaT)
				return -1;
			if (deltaT > x.deltaT)
				return 1;
			if (TimsKey < x.TimsKey)
				return -1;
			if (TimsKey > x.TimsKey)
				return 1;
			if (VlCode < x.VlCode)
				return -1;
			if (VlCode > x.VlCode)
				return 1;
			if (timePeriod.From < x.timePeriod.From)
				return -1;
			if (timePeriod.From > x.timePeriod.From)
				return 1;
			// same dt,tk,vlc,tfrom..
			return 0;
		}
	}
	/// <summary>
	/// Interface for a container of TsReadItems.
	/// We want to read many items in one batch.
	/// </summary>
	public interface ITsReadItems
	{
		int Length { get; }
		ITsReadItem item(int i);
	}
	/// <summary>
	/// Simple and hopefully useful utility class that makes a List<TsReadItem> vx </TsReadItem>
	/// look like the required interface ITsReadItems
	/// </summary>
	public class TsReadItemsAsArray : ITsReadItems {
		private List<TsReadItem> v;
		public TsReadItemsAsArray(List<TsReadItem> vx)
		{
			v = vx;
		}
		public int Length { get { return v.Count; } }
		public ITsReadItem item(int i) {
			return v[i];
		}
	}
	/// <summary>
	/// Interface for the class that will receive the result when calling the ITsReader readValues interface.
	/// </summary>
	public interface ITsReadResult {
		void add(int timsKey, int vlCode, UtcTime T, double V, int Q);
		void add(int timsKey, int vlCode, UtcTime T, double V, int Q, int readKey, int reasKey, int reptKey);
	}
	/// <summary>
	/// Simple and maybe useful implementation of the ITsReadResult.
	/// Collecting the read result as dynamically growing List<> with an
	/// initial capacity.
	/// The elements are in the order as received from the ITsReader, that might be
	/// in random order (it's really tims_key,vlc,date, but due to Oracle, but you can't rely on it).
	/// </summary>
	public class TsReadResultAsSeparateArrays : ITsReadResult {
		public List<int> tk;
		public List<int> vlc;
		public List<UtcTime> T;
		public List<double> V;
		public List<int> Q;
		public List<int> readK;
		public List<int> reasK;
		public List<int> reptK;
		public TsReadResultAsSeparateArrays(int initialCapacity) {
			tk = new List<int>(initialCapacity);
			vlc = new List<int>(initialCapacity);
			Q = new List<int>(initialCapacity);
			V = new List<double>(initialCapacity);
			T = new List<UtcTime>(initialCapacity);
			readK = new List<int>(initialCapacity);
			reasK = new List<int>(initialCapacity);
			reptK = new List<int>(initialCapacity);
		}
		public void add(int timsKey, int vlCode, UtcTime Tx, double Vx, int Qx) {
			add(timsKey, vlCode, Tx, Vx, Qx, 0, 0, 0);
		}
		public void add(int timsKey, int vlCode, UtcTime Tx, double Vx, int Qx, int readKey, int reasKey, int reptKey) {
			tk.Add(timsKey); vlc.Add(vlCode); T.Add(Tx); V.Add(Vx); Q.Add(Qx); readK.Add(readKey); reasK.Add(reasKey); reptK.Add(reptKey);
		}
		/// <summary>
		/// Returns a pivot table,ix, where the indicies adresses the returned result
		/// in the order of ascending tims_key,vlcode,datetime.
		/// Could be useful when using the result..
		/// </summary>
		/// <returns></returns>
		public int[] getPivotVectorOrderByTimsKeyVlCodeTime() {
			int[] ix = new int[tk.Count];
			for (int i = 0; i < ix.Length; ++i) ix[i] = i;
			Array.Sort<int>(ix, delegate (int a,int b) {
				if (tk[a] < tk[b]) return -1;
				if (tk[a] > tk[b]) return 1;
				if (vlc[a] < vlc[b]) return -1;
				if (vlc[a] > vlc[b]) return 1;
				if (T[a] < T[b]) return -1;
				if (T[a] > T[b]) return 1;
				return 0;
			});
			return ix;
		}
	}
	/// <summary>
	/// Interface for the class that will receive the result when calling the ITsReader readRtpValues interface.
	/// </summary>
	public interface ITsRtpReadResult
	{
		void add(int timsKey, int vlCode, UtcTime T, UtcTime R, UtcTime C);
	}

	/// <summary>
	/// Simple and maybe useful implementation of the ITsRtpReadResult.
	/// Collecting the read result as dynamically growing List<> with an
	/// initial capacity.
	/// The elements are in the order as received from the ITsReader, that might be
	/// in random order (it's really tims_key,vlc,date, but due to Oracle, but you can't rely on it).
	/// </summary>
	public class TsRtpReadResultAsSeparateArrays : ITsRtpReadResult
	{
		public List<int> tk;
		public List<int> vlc;
		public List<UtcTime> T;
		public List<UtcTime> Trtp;
		public List<UtcTime> Tchange;
		public TsRtpReadResultAsSeparateArrays(int initialCapacity)
		{
			tk = new List<int>(initialCapacity);
			vlc = new List<int>(initialCapacity);
			T = new List<UtcTime>(initialCapacity);
			Trtp = new List<UtcTime>(initialCapacity);
			Tchange = new List<UtcTime>(initialCapacity);
		}
		public void add(int timsKey, int vlCode, UtcTime Tx, UtcTime Rx, UtcTime Cx)
		{
			tk.Add(timsKey); vlc.Add(vlCode); T.Add(Tx); Trtp.Add(Rx); Tchange.Add(Cx);
		}
		/// <summary>
		/// Returns a pivot table,ix, where the indicies adresses the returned result
		/// in the order of ascending tims_key,vlcode,datetime.
		/// Could be useful when using the result..
		/// </summary>
		/// <returns></returns>
		public int[] getPivotVectorOrderByTimsKeyVlCodeTime()
		{
			int[] ix = new int[tk.Count];
			for (int i = 0; i < ix.Length; ++i) ix[i] = i;
			Array.Sort<int>(ix, delegate(int a, int b)
			{
				if (tk[a] < tk[b]) return -1;
				if (tk[a] > tk[b]) return 1;
				if (vlc[a] < vlc[b]) return -1;
				if (vlc[a] > vlc[b]) return 1;
				if (T[a] < T[b]) return -1;
				if (T[a] > T[b]) return 1;
				return 0;
			});
			return ix;
		}
	}

	/// <summary>
	/// Basic interface for a ts reader, accepts a readSpecification,
	/// and reads from db using TzDb for any timestamps. Results are
	/// added to the ITsReadResult readResult parameter.
	/// Returns number of items read.
	/// </summary>
	public interface ITsReader {
		int readValues(ITsReadItems readSpecification, ITsReadResult readResult,IDbConnection db,RegionalCalendar TzDb);
		int readRTP(ITsReadItems readSpecification, ITsRtpReadResult readResult, IDbConnection db, RegionalCalendar TzDb);
	}
	/// <summary>
	/// Basic interface for a ts reader, accepts a readSpecification,
	/// and reads from db using TzDb for any timestamps. Results are
	/// added to the ITsReadResult readResult parameter.
	/// Returns number of items read.
	/// </summary>
	public interface ITsReaderR92 : ITsReader {
		//int readValues(ITsReadItems readSpecification, ITsReadResult readResult, IDbConnection db, RegionalCalendar TzDb);
		//int readRTP(ITsReadItems readSpecification, ITsRtpReadResult readResult, IDbConnection db, RegionalCalendar TzDb);
	}
	/// <summary>
	/// The ITsWriteItems interface encapsulates information we need when
	/// writing timeseries of points at the DB-layer level.
	/// Currently only breakpoint ts need to have rexxKey(i) and
	/// changedPeriodXXX methods.
	/// Fixed interval series uses point information to deduce changedPeriod,
	/// (which is wrong), so we recommend to always support changed Period
	/// along with the points.
	/// </summary>
	public interface ITsWriteItems {
		/// <summary>
		/// This method should return the number of points that are supplied.
		/// The interface should be able to efficiently answer to 
		/// access of 0.. n-1 for point get methods, dt,timsKey.. etc..
		/// </summary>
		int numberOfPoints { get; }
		TimeSpan dt(int i);
		int timsKey(int i);
		int vlCode(int i);
		UtcTime T(int i);
		double V(int i);
		int Q(int i);
		int readKey(int i);// break points only
		int reptKey(int i);
		int reasKey(int i);
		UtcTime rtp(int i);

		/// <summary>
		/// numberOfPeriods needs to be supplied for all breakpoint series
		/// A a minimum, there should be minimum ONE changedPeriod entry for
		/// each unique timsKey in the point list. The periods should span the
		/// range of the points. There might be several change periods pr. set of 
		/// Points stored.
		/// There might be changedPeriods without points, which merely means that
		/// any existing points in that range will be removed as a part of the storage
		/// process.
		/// </summary>
		int numberOfPeriods { get; }// currently, break points only
		int changedPeriodTimsKey(int i);
		// not yet(implicit break point): TimeSpan changedDt(int i);
		int changedPeriodVlCode(int i);
		TimePeriod changedPeriod(int i);
	}
	/// <summary>
	/// Helper class for the old static write rotine.
	/// </summary>
	public struct TsWriteItemsFromArrays:ITsWriteItems {
		int[] stepTypes;
		int[] timsKeys; int[] vlcodes; long[] Ts; double[] Vs;
		int[] Qs; int?[] readKeys; int?[] reptKeys; int?[] reasKeys;
		UtcTime[] rtps; int[] paraFlags; UtcTime[] lcDates; int[] deleteTimsKeys;
		int[] deleteVlcodes; string[] deleteFromDate17s; string[] deleteUntilDate17s;
		RegionalCalendar TzDb;
		public TsWriteItemsFromArrays(int[] stepTypes_,
			int[] timsKeys_, int[] vlcodes_, long[] Ts_, double[] Vs_,
			int[] Qs_, int?[] readKeys_, int?[] reptKeys_, int?[] reasKeys_, int[] deleteTimsKeys_,
			int[] deleteVlcodes_, string[] deleteFromDate17s_, string[] deleteUntilDate17s_,
			RegionalCalendar TzDb_)
		{
			stepTypes = stepTypes_;
			timsKeys = timsKeys_;
			vlcodes = vlcodes_;
			Ts = Ts_;
			Vs = Vs_;
			Qs = Qs_;
			readKeys = readKeys_;
			reptKeys = reptKeys_;
			reasKeys = reasKeys_;
			rtps = null;
			paraFlags = null;
			lcDates = null;
			deleteTimsKeys = deleteTimsKeys_;
			deleteVlcodes = deleteVlcodes_;
			deleteFromDate17s = deleteFromDate17s_;
			deleteUntilDate17s = deleteUntilDate17s_;
			TzDb = TzDb_;
		}
		public TsWriteItemsFromArrays(int[] stepTypes_,
			int[] timsKeys_, int[] vlcodes_, long[] Ts_, double[] Vs_,
			int[] Qs_, int?[] readKeys_, int?[] reptKeys_, int?[] reasKeys_, UtcTime[] rtps_,
			int[] deleteTimsKeys_, int[] deleteVlcodes_, string[] deleteFromDate17s_, string[] deleteUntilDate17s_,
			RegionalCalendar TzDb_)
		{
			stepTypes = stepTypes_;
			timsKeys = timsKeys_;
			vlcodes = vlcodes_;
			Ts = Ts_;
			Vs = Vs_;
			Qs = Qs_;
			readKeys = readKeys_;
			reptKeys = reptKeys_;
			reasKeys = reasKeys_;
			rtps = rtps_;
			paraFlags = null;
			lcDates = null;
			deleteTimsKeys = deleteTimsKeys_;
			deleteVlcodes = deleteVlcodes_;
			deleteFromDate17s = deleteFromDate17s_;
			deleteUntilDate17s = deleteUntilDate17s_;
			TzDb = TzDb_;
		}
		public TsWriteItemsFromArrays(int[] stepTypes_,
			int[] timsKeys_, int[] vlcodes_, long[] Ts_, double[] Vs_,
			int[] Qs_, int?[] readKeys_, int?[] reptKeys_, int?[] reasKeys_, UtcTime[] rtps_,
			int[] paraFlags_, UtcTime[] lcDates_, int[] deleteTimsKeys_,
			int[] deleteVlcodes_, string[] deleteFromDate17s_, string[] deleteUntilDate17s_,
			RegionalCalendar TzDb_, bool appendOrigSeries, bool protectCorrectedValues)
		{
			stepTypes = stepTypes_;
			timsKeys = timsKeys_;
			vlcodes = vlcodes_;
			Ts = Ts_;
			Vs = Vs_;
			Qs = Qs_;
			readKeys = readKeys_;
			reptKeys = reptKeys_;
			reasKeys = reasKeys_;
			rtps = rtps_;
			paraFlags = paraFlags_;
			lcDates = lcDates_;
			deleteTimsKeys = deleteTimsKeys_;
			deleteVlcodes = deleteVlcodes_;
			deleteFromDate17s = deleteFromDate17s_;
			deleteUntilDate17s = deleteUntilDate17s_;
			TzDb = TzDb_;
			if (appendOrigSeries)
			{
				appendOrigSeriesPointsAndPeriods(ref stepTypes, ref timsKeys,
					ref vlcodes, ref Ts, ref Vs, ref Qs, ref readKeys, ref reptKeys,
					ref reasKeys, ref rtps, ref paraFlags, ref lcDates, ref deleteTimsKeys,
					ref deleteVlcodes, ref deleteFromDate17s, ref deleteUntilDate17s);
			}
			if (protectCorrectedValues)
			{
				removePointsProtectCorrectedValuesMode(ref stepTypes, ref timsKeys,
					ref vlcodes, ref Ts, ref Vs, ref Qs, ref readKeys, ref reptKeys,
					ref reasKeys, ref rtps, ref paraFlags, ref lcDates, ref deleteTimsKeys,
					ref deleteVlcodes, ref deleteFromDate17s, ref deleteUntilDate17s);
			}
		}
		public int numberOfPoints { get { return stepTypes == null ? 0 : stepTypes.Length; } }
		public TimeSpan dt(int i) { return TsUtil.StepTypeToTimeSpan(stepTypes[i]); }
		public int timsKey(int i) { return timsKeys[i]; }
		public int vlCode(int i) { return vlcodes[i]; }
		public UtcTime T(int i) { return UtcTime.CreateFromPdtime(Ts[i]); }
		public double V(int i) { return Vs[i]; }
		public int Q(int i) { return Qs[i]; }
		public int readKey(int i) { return readKeys == null || readKeys[i] == null ? 0 : (int)readKeys[i]; }// break points only
		public int reptKey(int i) { return reptKeys == null || reptKeys[i] == null ? 0 : (int)reptKeys[i]; }
		public int reasKey(int i) { return reasKeys == null || reasKeys[i] == null ? 0 : (int)reasKeys[i]; }
		//public UtcTime rtp(int i) { return i >= rtps.Length ? new UtcTime() : rtps[i]; }
		public UtcTime rtp(int i) { return rtps == null || rtps[i] == null ? new UtcTime() : rtps[i]; }
		public int paraFlag(int i) { return paraFlags == null ? 0 : paraFlags[i]; }
		public UtcTime lcDate(int i) { return lcDates == null || lcDates[i] == null ? new UtcTime() : lcDates[i]; }
		public int numberOfPeriods { get { return deleteTimsKeys == null ? 0 : deleteTimsKeys.Length; } }// currently, break points only
		public int changedPeriodTimsKey(int i) { return deleteTimsKeys[i]; }
		// not yet(implicit break point): TimeSpan changedDt(int i);
		public int changedPeriodVlCode(int i) { return deleteVlcodes[i]; }
		public TimePeriod changedPeriod(int i) { return new TimePeriod(TzDb.ToUtcTime(deleteFromDate17s[i]), TzDb.ToUtcTime(deleteUntilDate17s[i])); }

		internal void appendOrigSeriesPointsAndPeriods(ref int[] stepTypes, ref int[] timsKeys,
			ref int[] vlcodes, ref long[] Ts, ref double[] Vs, ref int[] Qs, ref int?[] readKeys,
			ref int?[] reptKeys, ref int?[] reasKeys, ref UtcTime[] rtps, ref int[] paraFlags,
			ref UtcTime[] lcDates, ref int[] deleteTimsKeys, ref int[] deleteVlcodes,
			ref string[] deleteFromDate17s, ref string[] deleteUntilDate17s)
		{
			if (paraFlags == null) return;
			List<int> stepTypeL = new List<int>(stepTypes);
			List<int> timsKeyL = new List<int>(timsKeys);
			List<int> vlcodeL = new List<int>(vlcodes);
			List<long> TL = new List<long>(Ts);
			List<double> VL = new List<double>(Vs);
			List<int> QL = new List<int>(Qs);
			List<int?> readKeyL = new List<int?>(readKeys);
			List<int?> reptKeyL = new List<int?>(reptKeys);
			List<int?> reasKeyL = new List<int?>(reasKeys);
			List<UtcTime> rtpL = rtps == null ? null : new List<UtcTime>(rtps);
			List<int> paraFlagL = paraFlags == null ? null : new List<int>(paraFlags);
			List<UtcTime> lcDateL = lcDates == null ? null : new List<UtcTime>(lcDates);
			List<int> deleteTimsKeyL = new List<int>(deleteTimsKeys);
			List<int> deleteVlcodeL = new List<int>(deleteVlcodes);
			List<string> deleteFromDate17L = new List<string>(deleteFromDate17s);
			List<string> deleteUntilDate17L = new List<string>(deleteUntilDate17s);
			int nPnt = stepTypes.Length;
			for (int i = 0; i < nPnt; ++i) //loop over point lists and append orig series points
			{
				if (paraFlags[i] != 1) continue;
				if (vlcodes[i] != 0) continue;
				stepTypeL.Add(stepTypes[i]);
				timsKeyL.Add(timsKeys[i]);
				vlcodeL.Add(100); //orig series
				TL.Add(Ts[i]);
				VL.Add(Vs[i]);
				QL.Add(Qs[i]);
				readKeyL.Add(readKeys[i]);
				reptKeyL.Add(reptKeys[i]);
				reasKeyL.Add(reasKeys[i]);
				if (rtps != null) rtpL.Add(rtps[i]);
				paraFlagL.Add(paraFlags[i]);
				if (lcDates != null) lcDateL.Add(lcDates[i]);
			}
			int nPer = deleteTimsKeys.Length;
			for (int i = 0; i < nPer; ++i) //loop over period lists and append orig series periods
			{
				if (deleteVlcodes[i] != 0) continue;
				deleteTimsKeyL.Add(deleteTimsKeys[i]);
				deleteVlcodeL.Add(100); //orig series
				deleteFromDate17L.Add(deleteFromDate17s[i]);
				deleteUntilDate17L.Add(deleteUntilDate17s[i]);
			}
			stepTypes = stepTypeL.ToArray();
			timsKeys = timsKeyL.ToArray();
			vlcodes = vlcodeL.ToArray();
			Ts = TL.ToArray();
			Vs = VL.ToArray();
			Qs = QL.ToArray();
			readKeys = readKeyL.ToArray();
			reptKeys = reptKeyL.ToArray();
			reasKeys = reasKeyL.ToArray();
			if (rtps != null) rtps = rtpL.ToArray();
			if (paraFlags != null) paraFlags = paraFlagL.ToArray();
			if (lcDates != null) lcDates = lcDateL.ToArray();
			deleteTimsKeys = deleteTimsKeyL.ToArray();
			deleteVlcodes = deleteVlcodeL.ToArray();
			deleteFromDate17s = deleteFromDate17L.ToArray();
			deleteUntilDate17s = deleteUntilDate17L.ToArray();
		}

		internal void removePointsProtectCorrectedValuesMode(ref int[] stepTypes, ref int[] timsKeys,
			ref int[] vlcodes, ref long[] Ts, ref double[] Vs, ref int[] Qs, ref int?[] readKeys,
			ref int?[] reptKeys, ref int?[] reasKeys, ref UtcTime[] rtps, ref int[] paraFlags,
			ref UtcTime[] lcDates, ref int[] deleteTimsKeys, ref int[] deleteVlcodes,
			ref string[] deleteFromDate17s, ref string[] deleteUntilDate17s)
		{
			if (lcDates == null) return;
			List<int> stepTypeL = new List<int>(stepTypes);
			List<int> timsKeyL = new List<int>(timsKeys);
			List<int> vlcodeL = new List<int>(vlcodes);
			List<long> TL = new List<long>(Ts);
			List<double> VL = new List<double>(Vs);
			List<int> QL = new List<int>(Qs);
			List<int?> readKeyL = new List<int?>(readKeys);
			List<int?> reptKeyL = new List<int?>(reptKeys);
			List<int?> reasKeyL = new List<int?>(reasKeys);
			List<UtcTime> rtpL = rtps == null ? null : new List<UtcTime>(rtps);
			List<int> paraFlagL = paraFlags == null ? null : new List<int>(paraFlags);
			List<UtcTime> lcDateL = lcDates == null ? null : new List<UtcTime>(lcDates);
			List<int> deleteTimsKeyL = new List<int>(deleteTimsKeys);
			List<int> deleteVlcodeL = new List<int>(deleteVlcodes);
			List<string> deleteFromDate17L = new List<string>(deleteFromDate17s);
			List<string> deleteUntilDate17L = new List<string>(deleteUntilDate17s);
			int nPnt = stepTypes.Length;
			int nPer = deleteTimsKeys.Length;
			for (int i = nPnt - 1; i >= 0; --i) //loop over point lists and remove corrected points
			{
				if (vlcodes[i] != 0) continue;
				if (lcDates[i].Ticks == Utc.Null) continue;
				if (lcDates[i] < T(i)) continue;
				// remove the point
				stepTypeL.RemoveAt(i);
				timsKeyL.RemoveAt(i);
				vlcodeL.RemoveAt(i);
				TL.RemoveAt(i);
				VL.RemoveAt(i);
				QL.RemoveAt(i);
				readKeyL.RemoveAt(i);
				reptKeyL.RemoveAt(i);
				reasKeyL.RemoveAt(i);
				if (rtps != null) rtpL.RemoveAt(i);
				paraFlagL.RemoveAt(i);
				lcDateL.RemoveAt(i);
			}
			nPnt = stepTypeL.Count;
			// loop over period lists and update from date to match possibly reduced point lists
			for (int j = nPer - 1; j >= 0; --j)
			{
				if (deleteVlcodes[j] != 0) continue;
				// get info from this period
				int timsKey = deleteTimsKeys[j];
				UtcTime fromDateDel = TzDb.ToUtcTime(deleteFromDate17s[j]);
				UtcTime untilDateDel = TzDb.ToUtcTime(deleteUntilDate17s[j]);
				// default new from date is initialized to until date plus one second
				UtcTime newFromDateDel = TzDb.AddSeconds(untilDateDel, 1);
				// loop over reduced point lists to find new from date (will be found if any points
				// left belonging to the period)
				for (int i = nPnt - 1; i >= 0; --i)
				{
					if (vlcodeL[i] != 0) continue;
					if (timsKeyL[i] != timsKey) continue;
					UtcTime t = UtcTime.CreateFromPdtime(TL[i]);
					if (t >= fromDateDel && t <= untilDateDel && t < newFromDateDel)
						newFromDateDel = t;
				}
				// if a new from date was found, update from date of period, otherwise remove the period
				// as all points belonging to it must have been removed above
				if (newFromDateDel <= untilDateDel)
				{
					deleteFromDate17L[j] = TzDb.ToDatabaseString(newFromDateDel);
				}
				else
				{
					deleteTimsKeyL.RemoveAt(j);
					deleteVlcodeL.RemoveAt(j);
					deleteFromDate17L.RemoveAt(j);
					deleteUntilDate17L.RemoveAt(j);
				}
			}
			stepTypes = stepTypeL.ToArray();
			timsKeys = timsKeyL.ToArray();
			vlcodes = vlcodeL.ToArray();
			Ts = TL.ToArray();
			Vs = VL.ToArray();
			Qs = QL.ToArray();
			readKeys = readKeyL.ToArray();
			reptKeys = reptKeyL.ToArray();
			reasKeys = reasKeyL.ToArray();
			if (rtps != null) rtps = rtpL.ToArray();
			if (paraFlags != null) paraFlags = paraFlagL.ToArray();
			if (lcDates != null) lcDates = lcDateL.ToArray();
			deleteTimsKeys = deleteTimsKeyL.ToArray();
			deleteVlcodes = deleteVlcodeL.ToArray();
			deleteFromDate17s = deleteFromDate17L.ToArray();
			deleteUntilDate17s = deleteUntilDate17L.ToArray();
		}
	}

	/// <summary>
	///  Class that supports the generic interface to a ts.db supporting writeItems interface.
	///  Uses supplied db connection(and transaction context) and TzDb to do any needed conversions
	///  from UtcTime to database time calendar.
	/// </summary>
	public interface ITsWriter
    {
		int writeValues(ITsWriteItems writeItems, IDbConnection db, RegionalCalendar TzDb, IccProgram iccProgram,int validateOnStoreOption);
	}
	/// <summary>
	/// Implementation of ITsWriter
	/// name convention; T.V.Q means Time Value Quality
	/// </summary>
	public class TsWriter:ITsWriter {
		/// <summary>
		/// Db package name used for writing ts values.
		/// For the 7.5 backport we use TS_UTILITY_92X
		/// For 8.x, 9.x etc. we use ICC_TSSTORE
		/// </summary>
		string dbPackageName;
		private int meterVlAuditMode;
		private int updateOCmode;
		private string _M2V_LICENCE;
		private void updateConfigurableAuditAndOCMode(IDbConnection db) {
			meterVlAuditMode = TimeSeriesData.GetPDDBEnv("ICC_DB_USE_METER_VL_AUDIT", "false", db).ToLower() == "true" ? 1 : 0;
			updateOCmode = TimeSeriesData.GetPDDBEnv("ICC_USE_OBJECT_CHANGED", "true", db).ToLower() == "true" ? 1 : 0;
			_M2V_LICENCE = TimeSeriesData.GetPDDBEnv("_M2V_LICENCE", "FALSE", db);

		}
		public TsWriter() {
			dbPackageName = "TS_UTILITY"; //useBackport75Package ? "TS_UTILITY_92X" : "TS_UTILITY";
		}
		/// <summary>
		/// Db routine requires the point sent in in this order:
		/// (it's to get maximum efficiency at database server).
		/// </summary>
		/// <param name="r"></param>
		/// <returns></returns>
		private static int[] getPointVectorOrderByTimsKeyVlCodeTime(ITsWriteItems r) {
			int[] ix = new int[r.numberOfPoints];
			for (int i = 0; i < ix.Length; ++i)
				ix[i] = i;
			Array.Sort<int>(ix, delegate(int a, int b) {
				if (r.timsKey(a) < r.timsKey(b)) return -1;
				if (r.timsKey(a) > r.timsKey(b)) return 1;
				if (r.vlCode(a) < r.vlCode(b)) return -1;
				if (r.vlCode(a) > r.vlCode(b)) return 1;
				if (r.T(a) < r.T(b)) return -1;
				if (r.T(a) > r.T(b)) return 1;
				return 0;
			}
			);
			return ix;
		}
		public int writeValues(ITsWriteItems writeItems, IDbConnection db, RegionalCalendar TzDb, IccProgram iccProgram, int validateOnStoreOption)
		{
			//sort arrays by  timsKeys, vlcodes, pdtimeDatetimes, values
			int[] ix = getPointVectorOrderByTimsKeyVlCodeTime(writeItems);
			// needs to make arrays to send over to db.. goes here:
			updateConfigurableAuditAndOCMode(db);
			int 
				checkTSAccessMode = 1,
				updateLEmode = 0,
				updateLVmode = 1,
				mergeMode = 0;
			int n = writeItems.numberOfPoints;
			int[] stepTypes = new int[n];
			int[] timsKeys = new int[n];
			int[] vlcodes = new int[n];
			long[] T = new long[n];
			OracleDecimal[] V = new OracleDecimal[n];
			int[] Q = new int[n];
			int[] readKeys = new int[n];
			int[] reptKeys = new int[n];
			int[] reasKeys = new int[n];
			OracleDecimal[] rtpDate = new OracleDecimal[n];

			for (int i = 0; i < n; ++i)
			{
				int j = ix[i];
				stepTypes[i] = TsUtil.TimeSpanToStepType(writeItems.dt(j));
				timsKeys[i] = writeItems.timsKey(j);
				vlcodes[i] = writeItems.vlCode(j);
				T[i] = writeItems.T(j).ToPdtime();
				Q[i] = writeItems.Q(j);
				double x = writeItems.V(j);
				if (Double.IsNaN(x))
				{
					V[i] = OracleDecimal.Null;
					Q[i] |= TsUtil.TS_VL_MISSING;// tick on the missing bit here, to avoid trouble later.
				}
				else if ((Q[i] & TsUtil.TS_VL_MISSING) == TsUtil.TS_VL_MISSING)
					V[i] = OracleDecimal.Null;
				else
					V[i] = (OracleDecimal)x;

				readKeys[i] = writeItems.readKey(j);
				reptKeys[i] = writeItems.reptKey(j);
				reasKeys[i] = writeItems.reasKey(j);
				rtpDate[i] = writeItems.rtp(j) == UtcTime.Null ? OracleDecimal.Null : writeItems.rtp(j).ToPdtime(); // OracleDecimal.Null;
			}
			n = writeItems.numberOfPeriods;
			int[] changedTimsKeys = new int[n];
			int[] changedVlCodes = new int[n];
			OracleDecimal[] changedFromDate = new OracleDecimal[n];
			OracleDecimal[] changedUntilDate = new OracleDecimal[n];
			for (int i = 0; i < n; ++i)
			{
				changedTimsKeys[i] = writeItems.changedPeriodTimsKey(i);
				changedVlCodes[i] = writeItems.changedPeriodVlCode(i);
				changedFromDate[i] = writeItems.changedPeriod(i).From.ToPdtime();
				changedUntilDate[i] = writeItems.changedPeriod(i).Until.ToPdtime();
			}

			using (OracleCommand cmd = new OracleCommand())
			{
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = dbPackageName + ".BULKWRITE";

				OracleParameter par;

				par = cmd.Parameters.Add("p_timsKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, timsKeys);

				par = cmd.Parameters.Add("p_vlcode", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, vlcodes);

				par = cmd.Parameters.Add("p_datetime", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, T);

				par = cmd.Parameters.Add("p_tsvalues", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, V);

				par = cmd.Parameters.Add("p_tsstatus", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, Q);

				par = cmd.Parameters.Add("p_readKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, readKeys);

				par = cmd.Parameters.Add("p_reptKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, reptKeys);

				par = cmd.Parameters.Add("p_reasKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, reasKeys);

				par = cmd.Parameters.Add("p_rtpdate", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, rtpDate);

				par = cmd.Parameters.Add("p_changedTimsKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, changedTimsKeys);

				par = cmd.Parameters.Add("p_changedVlCode", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, changedVlCodes);

				par = cmd.Parameters.Add("p_changedPeriodStart", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, changedFromDate);

				par = cmd.Parameters.Add("changedPeriodEnd", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, changedUntilDate);

				par = cmd.Parameters.Add("p_meterVlAuditMode", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = meterVlAuditMode;

				par = cmd.Parameters.Add("p_checkTSAccessMode", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = checkTSAccessMode;

				par = cmd.Parameters.Add("p_updateLEMode", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = updateLEmode;

				par = cmd.Parameters.Add("p_updateLVMode", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = updateLVmode;

				par = cmd.Parameters.Add("p_updateOCMode", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = updateOCmode;

				par = cmd.Parameters.Add("p_mergeMode", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = mergeMode;

				par = cmd.Parameters.Add("p_progKey", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = iccProgram;

                par = cmd.Parameters.Add("p_overwriteFlag", OracleDbType.Decimal);
                par.Direction = ParameterDirection.Input;
                par.Value = 0;

				par = cmd.Parameters.Add("p_doTestOnStore", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = IccConfiguration.Metering.TimeSeriesValidationOnSave(_M2V_LICENCE);
				
				Util.ExecuteCommand(cmd, db);
			}
			return writeItems.numberOfPoints;
		}

	}

	/// <summary>
	///  Class that supports the generic interface to a ts.db supporting writeItems interface.
	///  Uses supplied db connection(and transaction context) and TzDb to do any needed conversions
	///  from UtcTime to database time calendar.
	/// </summary>
	public interface ITsWriterR92 : ITsWriter
	{
		//int writeValues(ITsWriteItems writeItems, IDbConnection db, RegionalCalendar TzDb, IccProgram iccProgram, int validateOnStoreOption);
	}
	/// <summary>
	/// Implementation of ITsWriter
	/// name convention; T.V.Q means Time Value Quality
	/// </summary>
	public class TsWriterR92 : ITsWriterR92 {
		/// <summary>
		/// Db package name used for writing ts values.
		/// For the 7.5 backport we use TS_UTILITY_92X
		/// For 8.x, 9.x etc. we use ICC_TSSTORE
		/// </summary>
		string dbPackageName;
		public TsWriterR92() {
			dbPackageName = "ICC_TSSTORE"; //useBackport75Package ? "TS_UTILITY_92X" : "ICC_TSSTORE";
		}
		/// <summary>
		/// Db routine requires the point sent in in this order:
		/// (it's to get maximum efficiency at database server).
		/// </summary>
		/// <param name="r"></param>
		/// <returns></returns>
		private static int[] getPointVectorOrderByResolutionTimsKeyVlCodeTime(ITsWriteItems r) {
			int[] ix = new int[r.numberOfPoints];
			for (int i = 0; i < ix.Length; ++i)
				ix[i] = i;
			Array.Sort<int>(ix, delegate(int a, int b) {
				if (r.dt(a) < r.dt(b)) return -1;
				if (r.dt(a) > r.dt(b)) return 1;
				if (r.timsKey(a) < r.timsKey(b)) return -1;
				if (r.timsKey(a) > r.timsKey(b)) return 1;
				if (r.vlCode(a) < r.vlCode(b)) return -1;
				if (r.vlCode(a) > r.vlCode(b)) return 1;
				if (r.T(a) < r.T(b)) return -1;
				if (r.T(a) > r.T(b)) return 1;
				return 0;
			}
			);
			return ix;
		}

		public int writeValues(ITsWriteItems writeItems, IDbConnection db, RegionalCalendar TzDb, IccProgram iccProgram, int validateOnStoreOption)
		{
			//sort arrays by tsResolutions, timsKeys, vlcodes, pdtimeDatetimes, values
			int[] ix = getPointVectorOrderByResolutionTimsKeyVlCodeTime(writeItems);
			// needs to make arrays to send over to db.. goes here:
			int lastTimsKey = 0;
			int totalNumberOfSeries = 0;
			int numberOfBreakPointSeries = 0;
			int numberOfBreakPointValues = 0;
			int n = writeItems.numberOfPoints;
			int[] stepTypes = new int[n];
			int[] timsKeys = new int[n];
			int[] vlcodes = new int[n];
			long[] T = new long[n];
			OracleDecimal[] V = new OracleDecimal[n];
			int[] Q = new int[n];
			int[] reasKeys = new int[n];
			int[] reptKeys = new int[n];
			int[] readKeys = new int[n];

			for (int i = 0; i < n; ++i) {
				int j = ix[i];
				stepTypes[i] = TsUtil.TimeSpanToStepType(writeItems.dt(j));
				timsKeys[i] = writeItems.timsKey(j);
				vlcodes[i] = writeItems.vlCode(j);
				T[i] = writeItems.T(j).ToPdtime();
				Q[i] = writeItems.Q(j);
				double x = writeItems.V(j);
				if (Double.IsNaN(x)) {
					V[i] = OracleDecimal.Null;
					Q[i] |= TsUtil.TS_VL_MISSING;// tick on the missing bit here, to avoid trouble later.
				} else
					V[i] = (OracleDecimal)x;

				readKeys[i] = writeItems.readKey(j);
				reasKeys[i] = writeItems.reasKey(j);
				reptKeys[i] = writeItems.reptKey(j);
				if (timsKeys[i] != lastTimsKey) {
					totalNumberOfSeries++;
					if (stepTypes[i] == STEPTYPE.BREAKPOINT) numberOfBreakPointSeries++;
					lastTimsKey = timsKeys[i];
				}
				if (stepTypes[i] == STEPTYPE.BREAKPOINT) numberOfBreakPointValues++;
			}
			n = writeItems.numberOfPeriods;
			int[] changedTimsKeys = new int[n];
			int[] changedVlCodes = new int[n];
			string[] changedFromDate17s = new string[n];
			string[] changedUntilDate17s = new string[n];
			for (int i = 0; i < n; ++i) {
				changedTimsKeys[i] = writeItems.changedPeriodTimsKey(i);
				changedVlCodes[i] = writeItems.changedPeriodVlCode(i);
				changedFromDate17s[i] = TzDb.ToDatabaseString(writeItems.changedPeriod(i).From);
				changedUntilDate17s[i] = TzDb.ToDatabaseString(writeItems.changedPeriod(i).Until);
			}

			using (OracleCommand cmd = new OracleCommand()) 
			{
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = dbPackageName + ".WRITETSVALUESOPTIMIZED";

				OracleParameter par;

				par = cmd.Parameters.Add("p_totalNumberOfSeries", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = totalNumberOfSeries;

				par = cmd.Parameters.Add("p_numberOfBreakPointSeries", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = numberOfBreakPointSeries;

				par = cmd.Parameters.Add("p_numberOfBreakPointValues", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = numberOfBreakPointValues;

				par = cmd.Parameters.Add("p_timsKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, timsKeys);

				par = cmd.Parameters.Add("p_vlcode", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, vlcodes);

				par = cmd.Parameters.Add("p_datetime", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, T);

				par = cmd.Parameters.Add("p_steptype", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, stepTypes);

				par = cmd.Parameters.Add("p_tsvalues", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, V);

				par = cmd.Parameters.Add("p_tsstatus", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, Q);

				par = cmd.Parameters.Add("p_readKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, readKeys);

				par = cmd.Parameters.Add("p_reptKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, reptKeys);

				par = cmd.Parameters.Add("p_reasKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, reasKeys);

				par = cmd.Parameters.Add("p_deleteTimsKey", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, changedTimsKeys);

				par = cmd.Parameters.Add("p_deleteVlcode", OracleDbType.Decimal);
				Util.SetPLSQLAssociativeArrayValue(par, changedVlCodes);

				par = cmd.Parameters.Add("p_deletePeriodStart", OracleDbType.Varchar2);
				Util.SetPLSQLAssociativeArrayValue(par, changedFromDate17s);

				par = cmd.Parameters.Add("p_deletePeriodEnd", OracleDbType.Varchar2);
				Util.SetPLSQLAssociativeArrayValue(par, changedUntilDate17s);

				par = cmd.Parameters.Add("p_doValidateOnStoreOption", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = validateOnStoreOption;

				par = cmd.Parameters.Add("progKey", OracleDbType.Decimal);
				par.Direction = ParameterDirection.Input;
				par.Value = iccProgram;

				Util.ExecuteCommand(cmd, db);
			}
			return writeItems.numberOfPoints;
		}

	}

	/// <summary>
	/// Class that implements ITsReader
	/// </summary>
	public class TsReader: ITsReader {
		/// <summary>
		/// Db package name used for writing ts values.
		/// For the 7.5 backport we use TS_UTILITY_92X
		/// For 8.x, 9.x etc. we use TS_UTILITY
		/// </summary>
		string dbPackageName;
		public TsReader() {
			dbPackageName = "TS_UTILITY"; //useBackport75Package ? "TS_UTILITY_92X" : "TS_UTILITY";
		}
		/// <summary>
		/// Make ordered by resolution,timsKey,vlcode,datetime pivot vector for
		/// ITsReadItems. Needed by the read routine to efficiently call one
		/// reader for each resolution.
		/// Update 10.0 version: Not needed.. we leave pivot vector filled with 0..n-1
		/// Reasons for keeping it: If we need to support both 9.2 and 10 db-layer for the replicationservice in fortum.
		/// </summary>
		/// <param name="r"> reference to read items, not modified</param>
		/// <returns> ordered vector of indexes </returns>
		private static int[] makePivotVector(ITsReadItems r) {
			int[] ix = new int[r.Length];
			for (int i = 0; i < ix.Length; ++i)
				ix[i] = i;
			//Array.Sort<int>(ix, delegate (int a, int b) {
			//	ITsReadItem ra= r.item(a), rb=r.item(b);
			//	if (ra.deltaT < rb.deltaT) return -1;
			//	if (ra.deltaT > rb.deltaT) return 1;
			//	if (ra.TimsKey < rb.TimsKey) return -1;
			//	if (ra.TimsKey > rb.TimsKey) return  1;
			//	if (ra.VlCode < rb.VlCode) return -1;
			//	if (ra.VlCode > rb.VlCode) return 1;
			//	if (ra.timePeriod.From < rb.timePeriod.From) return -1;
			//	if (ra.timePeriod.From > rb.timePeriod.From) return 1;
			//	return 0;
			//}
			//);
			return ix;
		}
		/// <summary>
		/// Implementation of the interface method. Ref. interface spec. for semantics.
		/// In this implementation we use the ts_utility bulk reader functions, but without
		/// surrounding reads (currently) for break point.
		/// Replication does not need surrounding reads.
		/// </summary>
		/// <param name="readSpecification"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		public int readValues(ITsReadItems readSpecification, ITsReadResult readResult, IDbConnection db, RegionalCalendar TzDb)
		{
			// 1. construct pivot ix for readSpecification Items, ordered by timeStep.tk,vlc, period. from time..
			int[] ix = makePivotVector(readSpecification);
			int n = readMeterVlValues(ix, readSpecification, readResult, db, TzDb);
			return n;
		}

		/// <summary>
		/// Implementation of the interface method. Ref. interface spec. for semantics.
		/// In this implementation we use the ts_utility bulk reader functions, but without
		/// surrounding reads (currently) for break point.
		/// Replication does not need surrounding reads.
		/// </summary>
		/// <param name="readSpecification"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		public int readRTP(ITsReadItems readSpecification, ITsRtpReadResult readResult, IDbConnection db, RegionalCalendar TzDb)
		{
			// 1. construct pivot ix for readSpecification Items, ordered by timeStep.tk,vlc, period. from time..
			int[] ix = makePivotVector(readSpecification);
			int n = readMeterVlRtpValues(ix, readSpecification, readResult, db, TzDb);
			return n;
		}

		/// <summary>
		/// Generate Oracle Date based on UtcTime and a calendar for the database.
		/// Needs to be max performance.
		/// </summary>
		/// <param name="TzDb"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		private static OracleDate OracleDateFromUtcTime(RegionalCalendar TzDb,UtcTime t) {
			int Y, M, D, h, m, s, ms; TzDb.asYMDhms_ms(t, out Y, out M, out D, out h, out m, out s, out ms);
			return new OracleDate(Y,M,D,h,m,s);
		}
		private static UtcTime UtcTimeFromOracleDate(OracleDate dt,RegionalCalendar TzDb) {
			return TzDb.ToUtcTime(dt.Value);
		}
		/// <summary>
		/// Read break point values, uses the ix pivot table to pick out the break point read items
		/// in perfect reading order.
		/// </summary>
		/// <param name="ix"></param>
		/// <param name="rs"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		private int readMeterVlValues(int[] ix, ITsReadItems rs, ITsReadResult readResult, IDbConnection db, RegionalCalendar TzDb)
		{
			//-- count up metervl read specs so we know sizes of vectors.
			int n = ix.Length;
			//for (int i = 0; i < ix.Length && rs.item(ix[i]).deltaT.Ticks == 0; ++i)
			//	n++;
			//if(n==0)
			//	return 0;
			int[] tk = new int[n];
			int[] vlc = new int[n];
			OracleDate[] Tfrom = new OracleDate[n];
			OracleDate[] Tto = new OracleDate[n];
			for (int i = 0; i < n; ++i)
			{
				ITsReadItem ri = rs.item(ix[i]);
				tk[i] = ri.TimsKey;
				vlc[i] = ri.VlCode;
				Tfrom[i] = OracleDateFromUtcTime(TzDb, ri.timePeriod.From);
				UtcTime until = ri.timePeriod.Until;
				if (ri.deltaT.Ticks > 0)
					until.Ticks += ri.deltaT.Ticks; // add extra interval
				else
					until.Ticks += Utc.Second; // add extra second
				Tto[i] = OracleDateFromUtcTime(TzDb, until);
			}

			using (OracleCommand cmd = new OracleCommand())
			{
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = dbPackageName + ".BulkRead";

				OracleParameter par = cmd.Parameters.Add("retval", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.ReturnValue;
				par = cmd.Parameters.Add("iTimsKey", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = tk;
				par = cmd.Parameters.Add("iVlCode", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = vlc;

				par = cmd.Parameters.Add("iTfrom", OracleDbType.Date);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = Tfrom;

				par = cmd.Parameters.Add("iTto", OracleDbType.Date);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = Tto;

				Util.ExecuteCommand(cmd, db);
				using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
				{
					int count = 0;
					using (OracleDataReader reader = cursor.GetDataReader())
					{
						int idxTimsKey = 0;// reader.GetOrdinal("tims_key");
						int idxVlc = 1;
						int idxTime = 2; //reader.GetOrdinal("datetim");
						int idxValue = 3; //reader.GetOrdinal("tsvalue");
						int idxQuality = 4;// reader.GetOrdinal("tsstatus");
						int idxReadKey = 5;
						int idxReptKey = 6;
						int idxReasKey = 7;

						while (reader.Read())
						{
							double V = reader.IsDBNull(idxValue) ? Double.NaN : reader.GetOracleDecimal(idxValue).ToDouble();
							int Q = reader.IsDBNull(idxQuality) ? 0 : reader.GetOracleDecimal(idxQuality).ToInt32();
							if (Double.IsNaN(V)) // tick on TS_VL_MISSING for all NAN values, regardless.
								Q = Q | TsUtil.TS_VL_MISSING;
							else if ((Q & TsUtil.TS_VL_MISSING) != 0)
								V = Double.NaN;
							int readKey = reader.IsDBNull(idxReadKey) ? 0 : reader.GetOracleDecimal(idxReadKey).ToInt32();
							int reasKey = reader.IsDBNull(idxReasKey) ? 0 : reader.GetOracleDecimal(idxReasKey).ToInt32();
							int reptKey = reader.IsDBNull(idxReptKey) ? 0 : reader.GetOracleDecimal(idxReptKey).ToInt32();
							readResult.add(reader.GetInt32(idxTimsKey), reader.GetInt32(idxVlc),
								TzDb.ToUtcTime(reader.GetDateTime(idxTime)), V, Q, readKey, reasKey, reptKey
								);
							++count;
						}
					}

					return count;
				}
			}
		}
		/// <summary>
		/// Generate a key from timsKey.VlCode we can efficiently use in a dictionary.
		/// We utilize the fact that tk,vlc are 32 bits, and generates a unique (need that)
		/// 64 bits number like this (vlc<<32) | tk
		/// </summary>
		/// <param name="tk"></param>
		/// <param name="vlc"></param>
		/// <returns></returns>
		private static long mkTkVlc(int tk,int vlc) {
			return (long)((((ulong)vlc) << 32) | (ulong)((uint)tk));
		}

		/// <summary>
		/// Read RTP info for break point values, uses the ix pivot table to pick out the break point read items
		/// in perfect reading order.
		/// </summary>
		/// <param name="ix"></param>
		/// <param name="rs"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		private int readMeterVlRtpValues(int[] ix, ITsReadItems rs, ITsRtpReadResult readResult, IDbConnection db, RegionalCalendar TzDb)
		{
			//-- count up metervl read specs so we know sizes of vectors.
			int n = ix.Length;
			//for (int i = 0; i < ix.Length && rs.item(ix[i]).deltaT.Ticks == 0; ++i)
			//	n++;
			//if(n==0)
			//	return 0;
			int[] tk = new int[n];
			int[] vlc = new int[n];
			OracleDate[] Tfrom = new OracleDate[n];
			OracleDate[] Tto = new OracleDate[n];
			for (int i = 0; i < n; ++i)
			{
				ITsReadItem ri = rs.item(ix[i]);
				tk[i] = ri.TimsKey;
				vlc[i] = ri.VlCode;
				Tfrom[i] = OracleDateFromUtcTime(TzDb, ri.timePeriod.From);
				Tto[i] = OracleDateFromUtcTime(TzDb, ri.timePeriod.Until);
			}

			using (OracleCommand cmd = new OracleCommand())
			{
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = dbPackageName + ".BulkReadRTPMeterVl";

				OracleParameter par = cmd.Parameters.Add("retval", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.ReturnValue;
				par = cmd.Parameters.Add("iTimsKey", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = tk;
				par = cmd.Parameters.Add("iVlCode", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = vlc;

				par = cmd.Parameters.Add("iFrom", OracleDbType.Date);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = Tfrom;

				par = cmd.Parameters.Add("iTo", OracleDbType.Date);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = Tto;

				Util.ExecuteCommand(cmd, db);
				using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
				{
					int count = 0;
					using (OracleDataReader reader = cursor.GetDataReader())
					{
						int idxTimsKey = 0;// reader.GetOrdinal("tims_key");
						int idxVlc = 1;
						int idxTime = 2; //reader.GetOrdinal("datetim");
						int idxTreg = 3; //reader.GetOrdinal("treg");
						int idxTchange = 4;// reader.GetOrdinal("tchange");

						while (reader.Read())
						{
                            var timsKey = reader.GetInt32(idxTimsKey);
                            var vlCode = reader.GetInt32(idxVlc);
                            var regTimeDb = reader.IsDBNull(idxTreg) ? default(DateTime) : reader.GetDateTime(idxTreg);
                            var regTimeDbUtc = regTimeDb == default(DateTime) ? new UtcTime() : TzDb.ToUtcTime(regTimeDb);
							var changeTimeUtc = reader.IsDBNull(idxTchange) ? new UtcTime() : TzDb.ToUtcTime(reader.GetDateTime(idxTchange));
                            var timeUtc = TzDb.ToUtcTime(reader.GetDateTime(idxTime));
                            readResult.add(timsKey, vlCode, timeUtc, regTimeDbUtc, changeTimeUtc);
							++count;
						}
					}

					return count;
				}
			}
		}

	}

	/// <summary>
	/// Class that implements ITsReaderR92. Used in replication with a remote database on version 9.2.
	/// </summary>
	public class TsReaderR92 : ITsReaderR92 {
		/// <summary>
		/// Db package name used for writing ts values.
		/// For the 7.5 backport we use TS_UTILITY_92X
		/// For 8.x, 9.x etc. we use TS_UTILITY
		/// </summary>
		string dbPackageName;
		public TsReaderR92() {
			dbPackageName = "TS_UTILITY"; //useBackport75Package ? "TS_UTILITY_92X" : "TS_UTILITY";
		}
		/// <summary>
		/// Make ordered by resolution,timsKey,vlcode,datetime pivot vector for
		/// ITsReadItems. Needed by the read routine to efficiently call one
		/// reader for each resolution.
		/// </summary>
		/// <param name="r"> reference to read items, not modified</param>
		/// <returns> ordered vector of indexes </returns>
		private static int[] makePivotVector(ITsReadItems r)
		{
			int[] ix = new int[r.Length];
			for (int i = 0; i < ix.Length; ++i)
				ix[i] = i;
			Array.Sort<int>(ix, delegate(int a, int b) {
				ITsReadItem ra = r.item(a), rb = r.item(b);
				if (ra.deltaT < rb.deltaT) return -1;
				if (ra.deltaT > rb.deltaT) return 1;
				if (ra.TimsKey < rb.TimsKey) return -1;
				if (ra.TimsKey > rb.TimsKey) return 1;
				if (ra.VlCode < rb.VlCode) return -1;
				if (ra.VlCode > rb.VlCode) return 1;
				if (ra.timePeriod.From < rb.timePeriod.From) return -1;
				if (ra.timePeriod.From > rb.timePeriod.From) return 1;
				return 0;
			}
			);
			return ix;
		}
		/// <summary>
		/// Used in replication with a remote database on version 9.2.
		/// </summary>
		/// <param name="readSpecification"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		public int readValues(ITsReadItems readSpecification, ITsReadResult readResult, IDbConnection db, RegionalCalendar TzDb)
		{
			// 1. construct pivot ix for readSpecification Items, ordered by timeStep.tk,vlc, period. from time..
			int[] ix = makePivotVector(readSpecification);
			int n = readMeterVlValues(ix, readSpecification, readResult, db, TzDb)
				+   readFixedVlValues(ix, readSpecification, readResult, db, TzDb);
			return n;
		}
		/// <summary>
		/// Dummy.
		/// </summary>
		public int readRTP(ITsReadItems readSpecification, ITsRtpReadResult readResult, IDbConnection db, RegionalCalendar TzDb) {
			return 0;
		}
		/// <summary>
		/// Generate Oracle Date based on UtcTime and a calendar for the database.
		/// Needs to be max performance.
		/// </summary>
		/// <param name="TzDb"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		private static OracleDate OracleDateFromUtcTime(RegionalCalendar TzDb, UtcTime t) {
			int Y, M, D, h, m, s, ms; TzDb.asYMDhms_ms(t, out Y, out M, out D, out h, out m, out s, out ms);
			return new OracleDate(Y, M, D, h, m, s);
		}
		/// <summary>
		/// Read break point values, uses the ix pivot table to pick out the break point read items
		/// in perfect reading order.
		/// </summary>
		/// <param name="ix"></param>
		/// <param name="rs"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		private int readMeterVlValues(int[] ix, ITsReadItems rs, ITsReadResult readResult, IDbConnection db, RegionalCalendar TzDb) {
			//-- count up metervl read specs so we know sizes of vectors.
			int n = 0;
			for (int i = 0; i < ix.Length && rs.item(ix[i]).deltaT.Ticks == 0; ++i)
				n++;
			if (n == 0)
				return 0;
			int[] tk = new int[n];
			int[] vlc = new int[n];
			OracleDate[] Tfrom = new OracleDate[n];
			OracleDate[] Tto = new OracleDate[n];
			for (int i = 0; i < n; ++i) {
				ITsReadItem ri = rs.item(ix[i]);
				tk[i] = ri.TimsKey;
				vlc[i] = ri.VlCode;
				Tfrom[i] = OracleDateFromUtcTime(TzDb, ri.timePeriod.From);
				Tto[i] = OracleDateFromUtcTime(TzDb, ri.timePeriod.Until);
			}

			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = dbPackageName + ".BulkRead";

				OracleParameter par = cmd.Parameters.Add("retval", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.ReturnValue;
				par = cmd.Parameters.Add("iTimsKey", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = tk;
				par = cmd.Parameters.Add("iVlCode", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = vlc;

				par = cmd.Parameters.Add("iFrom", OracleDbType.Date);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = Tfrom;

				par = cmd.Parameters.Add("iTo", OracleDbType.Date);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = Tto;

				Util.ExecuteCommand(cmd, db);
				using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value) {
					int count = 0;
					using (OracleDataReader reader = cursor.GetDataReader()) {
						int idxTimsKey = 0;// reader.GetOrdinal("tims_key");
						int idxVlc = 1;
						int idxTime = 2; //reader.GetOrdinal("datetim");
						int idxValue = 3; //reader.GetOrdinal("tsvalue");
						int idxQuality = 4;// reader.GetOrdinal("tsstatus");
						int idxReadKey = 5;
						int idxReptKey = 6;
						int idxReasKey = 7;

						while (reader.Read()) {
							double V = reader.IsDBNull(idxValue) ? Double.NaN : reader.GetOracleDecimal(idxValue).ToDouble();
							int Q = reader.IsDBNull(idxQuality) ? 0 : reader.GetOracleDecimal(idxQuality).ToInt32();
							if (Double.IsNaN(V)) // tick on TS_VL_MISSING for all NAN values, regardless.
								Q = Q | TsUtil.TS_VL_MISSING;
							else if ((Q & TsUtil.TS_VL_MISSING) != 0)
								V = Double.NaN;
							int readKey = reader.IsDBNull(idxReadKey) ? 0 : reader.GetOracleDecimal(idxReadKey).ToInt32();
							int reasKey = reader.IsDBNull(idxReasKey) ? 0 : reader.GetOracleDecimal(idxReasKey).ToInt32();
							int reptKey = reader.IsDBNull(idxReptKey) ? 0 : reader.GetOracleDecimal(idxReptKey).ToInt32();
							readResult.add(reader.GetInt32(idxTimsKey), reader.GetInt32(idxVlc),
								TzDb.ToUtcTime(reader.GetDateTime(idxTime)), V, Q, readKey, reasKey, reptKey
								);
							++count;
						}
					}

					return count;
				}
			}
		}
		/// <summary>
		/// Generate a key from timsKey.VlCode we can efficiently use in a dictionary.
		/// We utilize the fact that tk,vlc are 32 bits, and generates a unique (need that)
		/// 64 bits number like this (vlc<<32) | tk
		/// </summary>
		/// <param name="tk"></param>
		/// <param name="vlc"></param>
		/// <returns></returns>
		private static long mkTkVlc(int tk, int vlc)
		{
			return (long)((((ulong)vlc) << 32) | (ulong)((uint)tk));
		}
		/// <summary>
		/// As the name indicate, figure out what bulk-read procedure to use,
		/// and how the return ref.cursor is formatted. (multiple values pr. row).
		/// </summary>
		/// <param name="currentResolutionTicks"></param>
		/// <param name="readMethod"></param>
		/// <param name="nColumns"></param>
		/// <param name="trimDt"></param>
		/// <param name="t17cLength"></param>
		/// <returns></returns>
		private static bool FigureOutQueryParameters(long currentResolutionTicks, out string readMethod, out int nColumns, out TimeSpan trimDt, out int t17cLength) {
			if (currentResolutionTicks == 15 * Utc.Minute) {
				readMethod = "BulkReadMin15Vl"; nColumns = 4 * 24; trimDt = new TimeSpan(Utc.Day); t17cLength = 17;
			} else if (currentResolutionTicks == Utc.Hour) {
				readMethod = "BulkReadHourVl"; nColumns = 24; trimDt = new TimeSpan(Utc.Day); t17cLength = 17;
			} else if (currentResolutionTicks == Utc.Day) {
				readMethod = "BulkReadDayVl"; nColumns = 31; trimDt = new TimeSpan(Utc.Month); t17cLength = 6;
			} else if (currentResolutionTicks == Utc.Week) {
				readMethod = "BulkReadWeekVl"; nColumns = 53; trimDt = new TimeSpan(Utc.Year); t17cLength = 6;
			} else if (currentResolutionTicks == Utc.Month) {
				readMethod = "BulkReadMonthVl"; nColumns = 1; trimDt = new TimeSpan(Utc.Month); t17cLength = 6;
			} else if (currentResolutionTicks == Utc.Year) {
				readMethod = "BulkReadYearVl"; nColumns = 1; trimDt = new TimeSpan(Utc.Year); t17cLength = 4;
			} else {
				readMethod = "NotSupported"; nColumns = 0; trimDt = new TimeSpan(0); t17cLength = 0;
			}
			return nColumns > 0;
		}
		/// <summary>
		/// Read from the mysterious fixedVl tables, there are 6 of them,
		/// min15_vl,hour_vl,day_vl,week_vl,month_vl and (puhh) year_vl.
		/// Filters out values that are outside the read-specification, this might be time consuming
		/// if we are unlucky with the result-set returned from db.
		/// The current implementation uses a dictionary of tims_key.vlcode vs. readspecification
		/// and the lookup here at worst case could be one for each value and row returned.
		/// Better strategies could be implemented if needed (read all values first, then sort, then filter).
		/// </summary>
		/// <param name="ix"></param>
		/// <param name="rs"></param>
		/// <param name="readResult"></param>
		/// <param name="db"></param>
		/// <param name="TzDb"></param>
		/// <returns></returns>
		private int readFixedVlValues(int[] ix, ITsReadItems rs, ITsReadResult readResult, IDbConnection db, RegionalCalendar TzDb)
		{
			// 1. find first non break point index
			// then loop find next resolution index
			// for values in current range (from.. to)
			//  construct pl/sql vectors, execute statement and collect values into readResult.
			int count = 0;
			int rangeTo = 0;
			for (; rangeTo < ix.Length && rs.item(ix[rangeTo]).deltaT.Ticks == 0; ++rangeTo)
				;
			if (rangeTo == ix.Length || rs.item(ix[rangeTo]).deltaT.Ticks == 0)
				return 0;
			for (int rangeFrom = rangeTo; rangeFrom < ix.Length; rangeFrom = rangeTo) {
				long currentResolutionTicks = rs.item(ix[rangeFrom]).deltaT.Ticks;
				rangeTo = rangeFrom;// search forward until end or next resolution to read.
				while (rangeTo < ix.Length && rs.item(ix[rangeTo]).deltaT.Ticks == currentResolutionTicks)
					++rangeTo;
				//-- figure out query and procedure parameters to use.
				string readMethod;
				TimeSpan trimDt;
				int t17cLength;// length of 17c string i db, e.g. '20070102030405' or '200701'
				int nColumns;
				bool okCfg = FigureOutQueryParameters(currentResolutionTicks, out readMethod, out nColumns, out trimDt, out t17cLength);
				//-- now genereate pl/sql arrays, and the map we need to filter out values before and after valid time range
				//   remember, data is represented as multiple values pr. row. Not all value read from db are requested.
				//   so we filter them out here.
				int n = (rangeTo - rangeFrom);
				int[] tk = new int[n];
				int[] vlc = new int[n];
				string[] Tfrom = new string[n];
				string[] Tto = new string[n];
				Dictionary<long, List<TimePeriod>> tkPeriodMap = new Dictionary<long, List<TimePeriod>>(n);
				for (int i = 0; i < n; ++i) {
					ITsReadItem ri = rs.item(ix[rangeFrom + i]);
					tk[i] = ri.TimsKey;
					vlc[i] = ri.VlCode;
					Tfrom[i] = TzDb.ToDatabaseString(TzDb.trim(ri.timePeriod.From, trimDt)).Substring(0, t17cLength);
					Tto[i] = TzDb.ToDatabaseString(TzDb.trim(TzDb.Add(ri.timePeriod.Until, trimDt, 1) - new TimeSpan(1), trimDt)).Substring(0, t17cLength);
					//when reading rows back, we need to filter out not wanted values, 
					// we need a efficient map etc. to do this.
					long tkvlc = mkTkVlc(ri.TimsKey, ri.VlCode);
					if (!tkPeriodMap.ContainsKey(tkvlc))
						tkPeriodMap.Add(tkvlc, new List<TimePeriod>(2));
					tkPeriodMap[tkvlc].Add(ri.timePeriod);
				}

				using (OracleCommand cmd = new OracleCommand()) {
					cmd.CommandType = CommandType.StoredProcedure;
					cmd.CommandText = dbPackageName + "." + readMethod;

					OracleParameter par = cmd.Parameters.Add("retval", OracleDbType.RefCursor);
					par.Direction = ParameterDirection.ReturnValue;
					par = cmd.Parameters.Add("iTimsKey", OracleDbType.Decimal);
					par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
					par.Value = tk;
					par = cmd.Parameters.Add("iVlCode", OracleDbType.Decimal);
					par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
					par.Value = vlc;

					par = cmd.Parameters.Add("iFrom", OracleDbType.Varchar2);
					par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
					par.Value = Tfrom;

					par = cmd.Parameters.Add("iTo", OracleDbType.Varchar2);
					par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
					par.Value = Tto;

					Util.ExecuteCommand(cmd, db);
					using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
					{
						using (OracleDataReader reader = cursor.GetDataReader())
						{
							int idxTimsKey = 0;// reader.GetOrdinal("tims_key");
							int idxVlc = 1;
							int idxTime = 2; //reader.GetOrdinal("datetim");
							TimeSpan deltaT = new TimeSpan(currentResolutionTicks);
							long currentTkVlc = mkTkVlc(0, 0);
							List<TimePeriod> currentPeriods = null;
							while (reader.Read()) {
								int timsKey = reader.GetInt32(idxTimsKey);
								int vlCode = reader.GetInt32(idxVlc);
								long tkVlc = mkTkVlc(timsKey, vlCode);
								if (tkVlc != currentTkVlc) { //since oracle and index is organized tk,vlc,timeperiod, we get back rows in this order..
									if (!tkPeriodMap.TryGetValue(tkVlc, out currentPeriods)) {
										currentPeriods = null;// should really never happen... we get back something we did not ask for.
									}
									currentTkVlc = tkVlc;
								}
								string rowTimeStr = reader.GetString(idxTime);
								UtcTime t;
								if (currentResolutionTicks == Utc.Week) { // pick out year component, supports only dates after year 0.
									int y = 1000 * (rowTimeStr[0] - '0') + 100 * (rowTimeStr[1] - '0') + 10 * (rowTimeStr[2] - '0') + 1 * (rowTimeStr[3] - '0');
									UtcTime tw1 = TzDb.trimWeek(TzDb.ToUtcTime(y, 2, 1, 0, 0, 0, 0));// just pick a week well into the year
									int Yw, Wn;
									TzDb.asYW(tw1, out Yw, out Wn);// figure out the week number (what ever standard Db uses)
									t = TzDb.Add(tw1, new TimeSpan(Utc.Week), 1 - Wn);// go Wn number of weeks back to get start of week year.
								} else
									t = TzDb.ToUtcTime(rowTimeStr);
								if (currentPeriods != null) {
									bool rowWithinRequestedRange = false;
									UtcTime te = TzDb.Add(t, deltaT, nColumns - 1);// last point row is stored at Tstart+ (nColumns-1)*deltaT.
									for (int r = 0; r < currentPeriods.Count && !rowWithinRequestedRange; ++r) {
										rowWithinRequestedRange = currentPeriods[r].From <= t && te <= currentPeriods[r].Until;
									}
									int idxValue = 3; //reader.GetOrdinal("tsvalue");
									int idxQuality = 4;// reader.GetOrdinal("tsstatus");

									for (int c = 0; c < nColumns; ++c) {
										bool valueWithinRequestedRange = rowWithinRequestedRange;
										for (int r = 0; r < currentPeriods.Count && !valueWithinRequestedRange; ++r) {
											valueWithinRequestedRange = currentPeriods[r].From <= t && t <= currentPeriods[r].Until;
										}
										if (valueWithinRequestedRange) {
											double V = reader.IsDBNull(idxValue) ? Double.NaN : reader.GetOracleDecimal(idxValue).ToDouble();
											int Q = reader.IsDBNull(idxQuality) ? 0 : reader.GetOracleDecimal(idxQuality).ToInt32();
											if (!(Double.IsNaN(V) && reader.IsDBNull(idxQuality))) {// if both are null, we just skip it.
												if (Double.IsNaN(V)) // tick on TS_VL_MISSING for all NAN values, regardless.
													Q = Q | TsUtil.TS_VL_MISSING;
												else if ((Q & TsUtil.TS_VL_MISSING) != 0)
													V = Double.NaN;
												readResult.add(timsKey, vlCode, t, V, Q);
												++count;
											}
										}
										idxValue += 2;
										idxQuality += 2;
										t = TzDb.Add(t, deltaT, 1);
									}
								}
							}
						}
					}
				}
			}
			return count;
		}

	}

	/// <summary>
	/// Minimal start for the TsAdmin interface that
	/// supports meta-info administration for timeseries.
	/// That is, create/modify/delete ts (TIMESER in icc)
	/// Access rights etc.
	/// Created to support multiple db connections with different
	/// versions in the Replication service.
	/// </summary>
	public interface ITsAdmin {
		void setBackPort92Version(bool backPort);
		int createTimeSeries(TimeSeriesData.TimeSeriesInfo info, IDbConnection db);
		int createTimeSeries(TimeSeriesData.TimeSeriesInfo info, string expression, IDbConnection db);
		TimeSeriesData.TimeSeriesInfo fetch(TimeSeriesData.TimeSeriesInfo queryInfo, IDbConnection db);
		TimeSeriesData.TimeSeriesInfo[] fetch(int[] timsKeys, IDbConnection db);
		IDictionary fetchAccess(int[]timsKeys,IDbConnection db);
		int fetchExpressionKeyForVTs(int tims_key, IDbConnection db);
		string fetchExpressionForVTs(int tims_key, IDbConnection db);
		string getTimeSeriesPath(int timsKey, IDbConnection db);
		Hashtable getTimeSeriesIDs(string[] paths, IDbConnection db);
		void deleteTimeSeries(int timsKey, IDbConnection db);
		void deleteTimeSeries(string tscode, IDbConnection db);
		void deleteVirtualTimeserCalcdef(int tims_key, IDbConnection db);
		void deleteRC(int timsKey,IDbConnection db);
		bool exists(int timsKey, IDbConnection db);
		bool exists(string tsCode, IDbConnection db);
		string ensureUniqeName(string suggestion, IDbConnection db);
	}
	/// <summary>
	/// Straight forward implementation, delegate to the TimeSeriesData
	/// class. 
	/// </summary>
	public class TsAdmin:ITsAdmin {
		bool backPort92Version;
		public TsAdmin(bool backPortTo92Version) {
			backPort92Version = backPortTo92Version;
		}
		public TsAdmin():this(false) {}
		public void setBackPort92Version(bool backPort) {
			backPort92Version = backPort;
		}
		public int createTimeSeries(TimeSeriesData.TimeSeriesInfo info, IDbConnection db) {
			return backPort92Version?TimeSeriesData.CreateTimeSeries92(info, db):TimeSeriesData.CreateTimeSeries(info, db);
		}
		public int createTimeSeries(TimeSeriesData.TimeSeriesInfo info, string expression, IDbConnection db) {
			return backPort92Version?TimeSeriesData.CreateTimeSeries92(info, expression, db):TimeSeriesData.CreateTimeSeries(info, expression, db);
		}

		public TimeSeriesData.TimeSeriesInfo fetch(TimeSeriesData.TimeSeriesInfo queryInfo, IDbConnection db) {
			return  TimeSeriesData.Fetch(queryInfo, db, backPort92Version);
		}
		public TimeSeriesData.TimeSeriesInfo[] fetch(int[] timsKeys, IDbConnection db) {
			return TimeSeriesData.Fetch(timsKeys, db, backPort92Version);
		}
		public IDictionary fetchAccess(int[] timsKeys, IDbConnection db) {
			return TimeSeriesData.FetchAccess(timsKeys, db);
		}
		public int fetchExpressionKeyForVTs(int tims_key, IDbConnection db) {
			return TimeSeriesData.FetchExpressionKeyForVTs(tims_key, db);
		}
		public string fetchExpressionForVTs(int tims_key, IDbConnection db) {
			return TimeSeriesData.FetchExpression(tims_key, db);
		}
		public string getTimeSeriesPath(int timsKey, IDbConnection db) {
			return TimeSeriesData.GetTimeSeriesPath(timsKey, db);
		}
		public Hashtable getTimeSeriesIDs(string[] paths, IDbConnection db) {
			return TimeSeriesData.GetTimeSeriesIDs(paths, db);
		}
		public void deleteTimeSeries(int timsKey, IDbConnection db) {
			TimeSeriesData.DeleteTimeSeries(timsKey, db);
		}
		public void deleteTimeSeries(string tscode, IDbConnection db) {
			//if (backPort92Version)
			//	TimeSeriesData.DeleteTimeSeries75(tscode, db);
			//else
			//	TimeSeriesData.DeleteTimeSeries(tscode, db);
			TimeSeriesData.DeleteTimeSeries(tscode, db);
		}
		public void deleteRC(int timsKey, IDbConnection db) {
			TimeSeriesData.DeleteRC(timsKey, db);
		}
		public void deleteVirtualTimeserCalcdef(int tims_key, IDbConnection db) {
			TimeSeriesData.DeleteVirtualTimeserCalcdef(tims_key, db);
		}
		public bool exists(int timsKey, IDbConnection db) {
			return TimeSeriesData.Exists(timsKey, db);
		}
		public bool exists(string tsCode, IDbConnection db) {
			return TimeSeriesData.Exists(tsCode, db);
		}
		public string ensureUniqeName(string suggestion,IDbConnection db) {
			return TimeSeriesData.EnsureUniqeName(suggestion,db);
		}
	}
	public class TimeSeriesData: TimeSeriesDataRO
    {
		private static ILog m_l4n = LogManager.GetLogger(typeof(TimeSeriesData));

		private TimeSeriesData()
		{
		}

		public static CreateTsNameDelegate CreateTsNameCB 
		{
			get; 
			set;
		}

		public static bool Exists(string fullname) 
		{
			return Exists(fullname, null);
		}

		public static bool Exists(string fullname, IDbConnection connection) 
		{
			using (var cmd = new OracleCommand()) 
			{
				if (!fullname.StartsWith("/"))
					fullname = "/" + fullname;
				cmd.CommandText = "select tims_key from timeser where filename||tscode = :1";
				cmd.Parameters.Add(null, fullname);
				var o = Util.CommandToScalar(cmd, connection);
				return o != null;
			}
		}

		public static int FindTsByFullName(string stFullName, IDbConnection cn)
		{
			int timsKey;
			OracleCommand cmd = new OracleCommand();

			cmd.CommandText = "SELECT Tims_Key FROM TIMESER WHERE FileName || TSCode = :stFullName";

			cmd.Parameters.Add(null, stFullName);

			DataTable dt = Powel.Icc.Data.Util.CommandToDataTable(cmd, cn);

			timsKey = dt.Rows.Count == 0 ? 0 : Convert.ToInt32(dt.Rows[0]["Tims_Key"]);

			return timsKey;
		}
		
		public static bool Exists(int timsKey) 
		{
			return Exists(timsKey, null);
		}

		public static bool Exists(int timsKey, IDbConnection connection) 
		{
			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandText = "select tims_key from timeser where tims_key = :1";
				cmd.Parameters.Add(null, timsKey);
				object o = Util.CommandToScalar(cmd, connection);
				return o != null;
			}
		}

		public static Hashtable Exists(int[] timsKeys) 
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				return Exists(timsKeys, connection);
			}
		}

		public static Hashtable Exists(int[] timsKeys, IDbConnection connection) 
		{
			Hashtable hashtable = new Hashtable();

			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = "icc_tsstore.tims_keys_exist";

				OracleParameter par;

				par = cmd.Parameters.Add("p_timsKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = timsKeys;

				par = cmd.Parameters.Add("c", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, connection);

				using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters["c"].Value) {
					using (OracleDataReader reader = cursor.GetDataReader()) {
						while (reader.Read())
							hashtable.Add(reader.GetInt32(0), null);
					}
				}

				return hashtable;
			}
		}


		//public static CurveType GetCurveType(int timsKey) 
		//{
  //          m_l4n.Info($"Enter\n\ttimsKey = {timsKey}");
  //          using (var oCmd = new OracleCommand())
  //          {
  //              oCmd.CommandText = "select curvetyp from timeser where tims_key = :1";
  //              oCmd.Parameters.Add(null, timsKey);
  //              object o = Util.CommandToScalar(oCmd);
  //              int ct = System.Convert.ToInt32(o);
  //              m_l4n.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}\n\tct (CurveType) = {ct}");
  //              return (CurveType)ct;
  //          }
		//}
		public static string GetTimeSeriesPath(int id)
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				return GetTimeSeriesPath(id, connection);
			}
		}
		public static string GetTimeSeriesPath(int id,IDbConnection connection)
		{
			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = "pdmisc.tims_key_to_full_name";

				OracleParameter par = cmd.Parameters.Add("retval", OracleDbType.Varchar2, 4096);
				par.Direction = ParameterDirection.ReturnValue;

				cmd.Parameters.Add("p_tims_key", id);

				Util.ExecuteCommand(cmd, connection);

				return cmd.Parameters[0].Value.ToString();
			}
		}
		public static int GetTimeSeriesID(string fullName)
		{
			if (fullName.Substring(0,1) != "/")
				fullName = "/" + fullName;
			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandText = "select tims_key from timeser where filename||tscode = :1";
				cmd.Parameters.Add(null, fullName);
				object o = Util.CommandToScalar(cmd);
				if (o != null)
					return Convert.ToInt32(o);
				else return 0;
			}
		}
		public static Hashtable GetTimeSeriesIDs(string[] fullName, IDbConnection connection)
		{
			Hashtable hashtable = new Hashtable();

			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = "icc_tsstore.get_tims_keys";

				OracleParameter par;

				par = cmd.Parameters.Add("p_paths", OracleDbType.Varchar2);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = fullName;

				par = cmd.Parameters.Add("c", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, connection);

				using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters["c"].Value) {
					using (OracleDataReader reader = cursor.GetDataReader()) {
						while (reader.Read())
							hashtable.Add(reader.GetString(1), reader.GetInt32(0));
					}
				}
			}
			return hashtable;
		}

		public static int[] GetKeysForMeasurePoint(string measurePointId, UtcTime time, IDbConnection connection)
		{
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandText = "select tims_key from meas_ts, meas_pnt where meas_ts.meas_key = meas_pnt.meas_key and meas_pnt.name = :1 and meas_pnt.valftime < :2 and meas_pnt.valttime > :3";
				cmd.Parameters.Add(":1", measurePointId);
				cmd.Parameters.Add(":2", Util.ConvertToIccDateTime(calendar.ToDateTime(time)));
				cmd.Parameters.Add(":3", Util.ConvertToIccDateTime(calendar.ToDateTime(time)));
				DataTable keyTable = Util.CommandToDataTable(cmd, connection);

				int numKeys = keyTable.Rows.Count;
				int[] keys = new int[numKeys];

				for (int i = 0; i < numKeys; i++) {
					keys[i] = Util.GetInt32(keyTable.Rows[i], "tims_key");
				}
				return keys;			
			}
		}

	    public static string CreateName(RestrictionData.RestrCompCon con)
	    {
	        var suggestion = CreateTsNameCB == null 
                ? string.Format("RRS_{0:000000}_{1:000000}", con.hyd_key, con.oprt_key)
                : CreateTsNameCB(con);
	        using (var db = Util.OpenConnection())
	        {
	            return EnsureUniqeName(suggestion, db);
	        }
	    }

        public static string EnsureUniqeName(string suggestion, IDbConnection db)
        {
			var tscode = suggestion;
			if (!Exists(tscode, db))
                return tscode;

            var n = suggestion.Length;
			if (n > 37)
                tscode = suggestion.Remove(37, n - 37);// Make room for a _nn at end (_01 to _99)
			for (var i = 1; i < 99; i++)
            {
				suggestion = String.Format("{0}{1:_00}", tscode, i);

                if (Exists(suggestion, db))
                    continue;

                tscode = suggestion;
				break;
			}
			return tscode;
		}

		//[Obsolete("Please use the version with IDBConnection, or better use the ITsAdmin interface")]	
		//public static string EnsureUniqeName(string suggestion) 
		//{
		//	string res;
		//	using (IDbConnection db = Util.OpenConnection()) {
		//		res = EnsureUniqeName(suggestion,db);
		//	}
		//	return res;
		//}

		public static Version CheckVersion(Version codeVersion, IDbTransaction transaction)
		{
			using (OracleCommand cmd = new OracleCommand("icc_tsstore.checkVersion")) {
				cmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;

				cmd.Parameters.Add("p_applicationVersion", codeVersion.Major);
				cmd.Parameters.Add("p_applicationRelease", codeVersion.Minor);
				cmd.Parameters.Add("p_abortIfFailure", Convert.ToDecimal(0));

				par = cmd.Parameters.Add("p_packageVersion", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;
				par = cmd.Parameters.Add("p_packageRelease", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;

				par = new OracleParameter();
				par.ParameterName = "p_failureMessage";
				par.DbType = DbType.String;
				par.Size = Util.DefaultParameterSize;
				par.Direction = ParameterDirection.Output;
				cmd.Parameters.Add(par);

				par = new OracleParameter();
				par.ParameterName = "p_versionMessage";
				par.DbType = DbType.String;
				par.Size = Util.DefaultParameterSize;
				par.Direction = ParameterDirection.Output;
				cmd.Parameters.Add(par);

				par = cmd.Parameters.Add("p_compatible", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, transaction);

				string errorMessage = null;
				if (cmd.Parameters["p_failureMessage"].Value != DBNull.Value)
					errorMessage = ((string)cmd.Parameters["p_failureMessage"].Value).Trim();
				if (errorMessage == null || errorMessage.Length == 0)
					errorMessage = "The running code is not compatible with the database schema.";

				if (Util.GetInt32(cmd.Parameters["p_compatible"]) == 0)
					throw new ApplicationException(errorMessage);

				return new Version(
					Util.GetInt32(cmd.Parameters["p_packageVersion"]),
					Util.GetInt32(cmd.Parameters["p_packageRelease"]));
			}
		}

		public static void SetDebug(bool debug, IDbTransaction transaction)
		{
			using (OracleCommand cmd = new OracleCommand("icc_tsstore.setDebug")) {
				cmd.CommandType = CommandType.StoredProcedure;
				//cmd.Parameters.Add("onOrOff", debug);
				Util.ExecuteCommand(cmd, transaction);
			}
		}

		public static void InitializeTsStore(IccProgram iccProgram, int timeSeriesID, int virtualCode, string startDate, IDbTransaction transaction)
		{
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand("icc_tsstore.initialize")) {
				cmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;

				par = cmd.Parameters.Add("p_mvs", 1);
				par = cmd.Parameters.Add("p_progKey", (int)iccProgram);
				par = cmd.Parameters.Add("p_timsKey", timeSeriesID);
				par = cmd.Parameters.Add("p_vlcode", virtualCode);
				par = cmd.Parameters.Add("p_startDate", startDate);

				par = cmd.Parameters.Add("ret", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, transaction);
			}
		}

		public static void StoreTimeSeriesValues(
			int[] startPositions, 
			string[] dates, 
			double[] values, 
			int[] valueIndicators, 
			int[] statuses, 
			int[] numberOfValues, 
			IDbTransaction transaction)
		{
			using (OracleCommand cmd = new OracleCommand("icc_tsstore.tsStore")) {
				cmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;

				int[] statusIndicators = new int[values.Length];

				par = cmd.Parameters.Add("p_startPosition", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = startPositions;

				par = cmd.Parameters.Add("p_datetime", OracleDbType.Varchar2);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = dates;

				par = cmd.Parameters.Add("p_tsValues", OracleDbType.Double);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = values;

				par = cmd.Parameters.Add("p_valueIndicator", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = valueIndicators;

				par = cmd.Parameters.Add("p_tsStatus", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = statuses;

				par = cmd.Parameters.Add("p_statusIndicator", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = statusIndicators;

				par = cmd.Parameters.Add("p_noValues", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = numberOfValues;

				par = cmd.Parameters.Add("ret", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, transaction);
			}
		}

		public static void FinishTsStore(UtcTime end, IDbTransaction transaction)
		{
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand("icc_tsstore.finish")) {
				cmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;

				par = cmd.Parameters.Add("p_endDate", Util.ConvertToIccDateTime(calendar.ToDateTime(end)));

				par = cmd.Parameters.Add("ret", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, transaction);
			}
		}

		public static void StoreAllInOne(
			bool deleteBeforeStore, 
			int timeSeriesID, 
			int virtualCode, 
			string[] dates, 
			double[] values, 
			int[] valueIndicators, 
			int[] statuses, 
			int numberOfValues, 
			IDbTransaction transaction)
		{
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand("icc_tsstore.tsStoreAllInOne")) {
				cmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;

				int[] timsKeys = new int[numberOfValues];
				int[] virtualCodes = new int[numberOfValues];
				int[] readKeys = new int[numberOfValues];
				int[] reptKeys = new int[numberOfValues];
				int[] reasKeys = new int[numberOfValues];
				int[] statusIndicators = new int[numberOfValues];

				for (int i = 0; i < numberOfValues; i++) {
					timsKeys[i] = timeSeriesID;
					virtualCodes[i] = virtualCode;
				}

				par = cmd.Parameters.Add("p_deleteBeforeStore", Convert.ToInt32(deleteBeforeStore));

				par = cmd.Parameters.Add("p_timsKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = timsKeys;

				par = cmd.Parameters.Add("p_vlcode", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = virtualCodes;

				par = cmd.Parameters.Add("p_datetime", OracleDbType.Varchar2);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = dates;

				par = cmd.Parameters.Add("p_tsValues", OracleDbType.Double);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = values;

				par = cmd.Parameters.Add("p_valueIndicator", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = valueIndicators;

				par = cmd.Parameters.Add("p_tsStatus", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = statuses;

				par = cmd.Parameters.Add("p_statusIndicator", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = statusIndicators;

				par = cmd.Parameters.Add("p_readKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = readKeys;

				par = cmd.Parameters.Add("p_reptKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = reptKeys;

				par = cmd.Parameters.Add("p_reasKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = reasKeys;

				par = cmd.Parameters.Add("p_noValues", numberOfValues);

				par = cmd.Parameters.Add("ret", OracleDbType.Int32);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, transaction);
			}
		}
		public static bool UpdateTsInfo(TimeSeriesInfo tsInfo) 
		{
			m_l4n.Debug("UpdateTsInfo");
			m_l4n.Debug("..tsInfo = " + tsInfo);
			using (OracleCommand cmd = new OracleCommand()) 
			{
				cmd.CommandText = 
						"UPDATE TIMESER SET " +
								 "FILENAME = :1" + 
								",TSCODE = :2" +
								",TSIN_KEY = :3" +
								",UNME_KEY = :4" +
								",CURVETYP = :5" +
								",STEPTYPE = :6" +
								",DESCRIPT = :7" +
								",TSER_KEY = :8" +
								",TSOR_KEY = :9" +
								",TSPR_KEY = :10" +
								",NAME = :11" +
								",LVDATE = :12" +
								",VLTP_KEY = :13" +
								",VLTP_VALFTIME = :14" +
							"WHERE TIMS_KEY = :15"
						;
				cmd.Parameters.Add(null, tsInfo.filename);
				cmd.Parameters.Add(null, tsInfo.tscode);
				cmd.Parameters.Add(null, tsInfo.resolution.Value);
				cmd.Parameters.Add(null, tsInfo.unme_key);
				cmd.Parameters.Add(null, (short)tsInfo.CurveType);
				cmd.Parameters.Add(null, tsInfo.Steptype);
				cmd.Parameters.Add(null, tsInfo.descript);
				cmd.Parameters.Add(null, tsInfo.tser_key);
				cmd.Parameters.Add(null, tsInfo.tsor_key);
				cmd.Parameters.Add(null, tsInfo.tspr_key);
				cmd.Parameters.Add(null, tsInfo.name);
				cmd.Parameters.Add(null, tsInfo.lvdate);

				m_l4n.Debug("..Add(ValueType); Convert.ToInt32(tsInfo.ValueType) = " + Convert.ToInt32(tsInfo.ValueType));
				cmd.Parameters.Add(null, Convert.ToInt32(tsInfo.ValueType));

				m_l4n.Debug("..tsInfo.ValueTypeValidFrom = " + tsInfo.ValueTypeValidFrom);
				RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
				DateTime dValueTypeValidFrom = cal.ToDateTime(tsInfo.ValueTypeValidFrom);
				m_l4n.Debug("..dValueTypeValidFrom = " + dValueTypeValidFrom);

				cmd.Parameters.Add(null, dValueTypeValidFrom);
				m_l4n.Debug("..Add(tims_key)");
				cmd.Parameters.Add(null, tsInfo.tims_key);

				Util.CommandToScalar(cmd);
		  }

			m_l4n.Debug("UpdateTsInfo. Exit");

			return true;
		}
		public static int CreateTimeSeries(TimeSeriesData.TimeSeriesInfo timeser)
		{
			return CreateTimeSeries(timeser, (IDbConnection)null);
		}

		public static int CreateTimeSeries(TimeSeriesData.TimeSeriesInfo timeser, IDbTransaction transaction)
		{
			return CreateTimeSeries(timeser, transaction.Connection);
		}

		public static int CreateTimeSeries(TimeSeriesData.TimeSeriesInfo timeser, IDbConnection connection)
		{
			if ( timeser.tims_key == 0 ) 
			{
				using (OracleCommand cmdWriteTsInfo = new OracleCommand())
				{
					bool bUseICC_TsStore_WriteTsInfo = true;
					string stSQL;
					// Create timeseries and then create a virtual ts definition 
					// Virtual series definiton is based on PL/SQL code found in trigger code for new_virtual_calcset
					if (bUseICC_TsStore_WriteTsInfo)
					{
						stSQL = @"
							DECLARE 
								nTims_Key TIMESER.Tims_Key%TYPE;
								no INTEGER;
								iccSender INTEGER := NULL;
								rcName rc_set.name%TYPE;
								stDuplTSCode TIMESER.TSCode%TYPE;
								stDuplEstTime TIMESER.EstTime%TYPE;
							BEGIN
            		nTims_Key := ICC_TsStore.WriteTsInfo(
									 iTimsKey => NULL
									,iTsCode => :TsCode
									,iTsName => :TsName
									,iDescript => :Descript
									,iStepType => :StepType
									,iCurveType => :CurveType
									,iValueType => :ValueType
									,iVersion => NULL
									,iReference => NULL
									,iFilename => :Filename
									,iParaFlag => :ParaFlag
									,iPrdKey => NULL
									,iTsProducerKey => :TsProducerKey
									,iTsIntervalKey => :TsIntervalKey
									,iTsOriginKey => :TsOriginKey
									,iTsErKey => :TsErKey
									,iUnmeKey => :UnmeKey
									,iRCKey => NULL
									,iConvertusingLocaltime => NULL
									,iRTP => :UseRegistrationHandling
									,iVltpValftime => NULL
									-- To avoid unintended UPDATE:
									,nTryUpdateIfTimsKeyNULL => 0
            			);
								:NewTims_Key := nTims_Key;
							END;"
							;
						cmdWriteTsInfo.CommandText = stSQL;

						cmdWriteTsInfo.Parameters.Add("TsCode", OracleDbType.Varchar2, 40, timeser.tscode == null ? "" : timeser.tscode, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsName", OracleDbType.Varchar2, 40, timeser.name == null ? "" : timeser.name, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("Descript", OracleDbType.Varchar2, 255, timeser.descript == null ? "" : timeser.descript, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("StepType", OracleDbType.Decimal, timeser.Steptype, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("CurveType", OracleDbType.Decimal, timeser.CurveType, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("ValueType", OracleDbType.Decimal, timeser.ValueType, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("FileName", OracleDbType.Varchar2, 512, timeser.filename != null ? timeser.filename : "/", ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("ParaFlag", OracleDbType.Decimal, timeser.paraflag, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsProducerKey", OracleDbType.Decimal, timeser.tspr_key == 0 ? (int)TsProducer.Undefined : timeser.tspr_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsIntervalKey", OracleDbType.Decimal, timeser.Resolution.Value, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsOriginKey", OracleDbType.Decimal, timeser.tsor_key == 0 ? (int)TsOrigin.Undefined : timeser.tsor_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsErKey", OracleDbType.Decimal, timeser.tser_key == 0 && !timeser.isPrecipitation ? (int)TsType.Undefined : timeser.tser_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("UnmeKey", OracleDbType.Decimal, timeser.unme_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("UseRegistrationHandling", OracleDbType.Decimal, timeser.use_registration_handling, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("NewTims_Key", OracleDbType.Decimal, DBNull.Value, ParameterDirection.Output);
					}
					else
					{
						stSQL = @"
declare 
	timskey TIMESER.Tims_Key%TYPE;
begin
	-- select nvl(max(tims_key),0)+1 into timskey from timeser;
	timskey := TS_Key.GetAdvancedTimsKey;
	:1 := timskey;
	insert into TIMESER 
		(
			 tims_key,filename,tscode,tsin_key,unme_key,curvetyp,steptype
			,descript,tser_key,tsor_key,tspr_key,name
			,paraflag,use_registration_handling,vltp_key
		) 
		values 
		(
			timskey,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15
		);
end;";
						cmdWriteTsInfo.CommandText = stSQL;
						cmdWriteTsInfo.Parameters.Add(null, OracleDbType.Int32, ParameterDirection.Output);
						cmdWriteTsInfo.Parameters.Add(null, timeser.filename);
						cmdWriteTsInfo.Parameters.Add(null, timeser.tscode);
						cmdWriteTsInfo.Parameters.Add(null, timeser.Resolution.Value);
						cmdWriteTsInfo.Parameters.Add(null, timeser.unme_key);
						cmdWriteTsInfo.Parameters.Add(null, (short)timeser.CurveType);
						cmdWriteTsInfo.Parameters.Add(null, timeser.Steptype);
						cmdWriteTsInfo.Parameters.Add(null, timeser.descript == null ? "" : timeser.descript);
						cmdWriteTsInfo.Parameters.Add(null, timeser.tser_key == 0 && !timeser.isPrecipitation ? (int)TsType.Undefined : timeser.tser_key);
						cmdWriteTsInfo.Parameters.Add(null, timeser.tsor_key == 0 ? (int)TsOrigin.Undefined : timeser.tsor_key);
						cmdWriteTsInfo.Parameters.Add(null, timeser.tspr_key == 0 ? (int)TsProducer.Undefined : timeser.tspr_key);
						cmdWriteTsInfo.Parameters.Add(null, timeser.name);
						cmdWriteTsInfo.Parameters.Add(null, timeser.paraflag);
						cmdWriteTsInfo.Parameters.Add(null, timeser.use_registration_handling);
						cmdWriteTsInfo.Parameters.Add(null, (short)timeser.ValueType);
					}
					{
						Util.ExecuteCommand(cmdWriteTsInfo, connection);
						timeser.tims_key = Int32.Parse(cmdWriteTsInfo.Parameters["NewTims_Key"].Value.ToString());
					}
				}
			}
			return timeser.tims_key;
		}
		public static int CreateTimeSeries92(TimeSeriesData.TimeSeriesInfo timeser, IDbConnection connection) {
			if (timeser.tims_key == 0)
			{
				// Insert
				using (OracleCommand oCmd = new OracleCommand())
				{
					oCmd.CommandText = @"
declare 
	timskey TIMESER.Tims_Key%TYPE;
begin
	-- select max(tims_key)+1 into timskey from timeser;
	timskey := TS_Key.GetAdvancedTimsKey;
	:1 := timskey;
	insert into TIMESER (tims_key,filename,tscode,tsin_key,unme_key,curvetyp,steptype,descript,tser_key,tsor_key,tspr_key,name,paraflag,use_registration_handling) values (timskey,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14);
end;";
					oCmd.Parameters.Add(null, OracleDbType.Int32, ParameterDirection.Output);
					oCmd.Parameters.Add(null, timeser.filename);
					oCmd.Parameters.Add(null, timeser.tscode);
					oCmd.Parameters.Add(null, timeser.Resolution.Value);
					oCmd.Parameters.Add(null, timeser.unme_key);
					oCmd.Parameters.Add(null, (short)timeser.CurveType);
					oCmd.Parameters.Add(null, timeser.Steptype);
					oCmd.Parameters.Add(null, timeser.descript == null ? "" : timeser.descript);
					oCmd.Parameters.Add(null, timeser.tser_key == 0 ? (int)TsType.Undefined : timeser.tser_key);
					oCmd.Parameters.Add(null, timeser.tsor_key == 0 ? (int)TsOrigin.Undefined : timeser.tsor_key);
					oCmd.Parameters.Add(null, timeser.tspr_key == 0 ? (int)TsProducer.Undefined : timeser.tspr_key);
					oCmd.Parameters.Add(null, timeser.name);
					oCmd.Parameters.Add(null, timeser.paraflag);
					oCmd.Parameters.Add(null, timeser.use_registration_handling);
					Util.ExecuteCommand(oCmd, connection);
					timeser.tims_key = Util.GetInt32(oCmd.Parameters[0]);
				}
			}
			return timeser.tims_key;
		}

		[Obsolete("This function needs connection arguments etc.")]
		public static int CreateTimeSeries(TimeSeriesData.TimeSeriesInfo timeser, string expression) {
			int res=0;
			using (IDbConnection connection = Util.OpenConnection()) {
				using (IDbTransaction tx = Util.OpenTransaction(connection)) {
					res = CreateTimeSeries(timeser, expression, connection);
					if (res > 0)
						tx.Commit();
					else
						tx.Rollback();
				}
			}
			return res;
		}
		
		public static int CreateTimeSeries(TimeSeriesData.TimeSeriesInfo timeser, string expression, IDbConnection connection)
		{
			if (timeser.tims_key == 0) 
			{
				// Insert
				using (OracleCommand cmdWriteTsInfo = new OracleCommand())
				{
					bool bUseICC_TsStore_WriteTsInfo = true;
					StringBuilder sb;
					// Create timeseries and then create a virtual ts definition 
					// Virtual series definiton is based on PL/SQL code found in trigger code for new_virtual_calcset
					if (bUseICC_TsStore_WriteTsInfo)
					{
						sb = new StringBuilder(@"
							DECLARE 
								nTims_Key TIMESER.Tims_Key%TYPE;
								no INTEGER;
								iccSender INTEGER := NULL;
								rcName rc_set.name%TYPE;
								stDuplTSCode TIMESER.TSCode%TYPE;
								stDuplEstTime TIMESER.EstTime%TYPE;
							BEGIN
            		nTims_Key := ICC_TsStore.WriteTsInfo(
									 iTimsKey => NULL
									,iTsCode => :TsCode
									,iTsName => :TsName
									,iDescript => :Descript
									,iStepType => :StepType
									,iCurveType => :CurveType
									,iValueType => :ValueType
									,iVersion => NULL
									,iReference => NULL
									,iFilename => :Filename
									,iParaFlag => :ParaFlag
									,iPrdKey => NULL
									,iTsProducerKey => :TsProducerKey
									,iTsIntervalKey => :TsIntervalKey
									,iTsOriginKey => :TsOriginKey
									,iTsErKey => :TsErKey
									,iUnmeKey => :UnmeKey
									,iRCKey => NULL
									,iConvertusingLocaltime => NULL
									,iRTP => :UseRegistrationHandling
									,iVltpValftime => NULL
									-- To avoid unintended UPDATE:
									,nTryUpdateIfTimsKeyNULL => 0
            			);
								:NewTims_Key := nTims_Key;
							
								iccSender := pdmisc.getenv( 'ICC_SENDER', iccSender );
								IF iccSender IS NULL THEN
									pdlog.reportError( 20105,'ICC_SENDER');
								END IF;
								rcName := 'VTS#RRS#' || :RCNamePostfix;
								select count(*) into no from rc_set  where name = rcName;
								IF no > 0 THEN
									pdlog.reportError(20108, 'RC_SET', 'NAME', rcName);
								END IF;
								-- pdlog.set_prog_number(prognumber);
								-- pdlog.reportError(20112,pdmisc.get_prog_number);

								INSERT INTO RC_Set (opun_key,name,object_type) values (iccSender,rcName,5);
								INSERT INTO RC_Head (rc_key,seqno,name,svalue) values (rc_set_seq.currval,1,'TIMESTEP',:RCHeadTimeStep);
								INSERT INTO RC_Column (rc_key,seqno,object_type,tims_key) values ( rc_set_seq.currval,1,1,nTims_Key);
								INSERT INTO C_Definition (calc_key,seqno,valftime,valttime) values (rc_column_seq.currval,1,'00000000000000000','99999999999999999');
							END;"
							);
						cmdWriteTsInfo.CommandText = sb.ToString();

						cmdWriteTsInfo.Parameters.Add("TsCode", OracleDbType.Varchar2, 40, timeser.tscode == null ? "" : timeser.tscode, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsName", OracleDbType.Varchar2, 40, timeser.name == null ? "" : timeser.name, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("Descript", OracleDbType.Varchar2, 255, timeser.descript == null ? "" : timeser.descript, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("StepType", OracleDbType.Decimal, timeser.Steptype, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("CurveType", OracleDbType.Decimal, timeser.CurveType, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("ValueType", OracleDbType.Decimal, timeser.ValueType, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("FileName", OracleDbType.Varchar2, 512, timeser.filename != null ? timeser.filename : "/", ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("ParaFlag", OracleDbType.Decimal, timeser.paraflag, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsProducerKey", OracleDbType.Decimal, timeser.tspr_key == 0 ? (int)TsProducer.Undefined : timeser.tspr_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsIntervalKey", OracleDbType.Decimal, timeser.Resolution.Value, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsOriginKey", OracleDbType.Decimal, timeser.tsor_key == 0 ? (int)TsOrigin.Undefined : timeser.tsor_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("TsErKey", OracleDbType.Decimal, timeser.tser_key == 0 && !timeser.isPrecipitation ? (int)TsType.Undefined : timeser.tser_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("UnmeKey", OracleDbType.Decimal, timeser.unme_key, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("UseRegistrationHandling", OracleDbType.Decimal, timeser.use_registration_handling, ParameterDirection.Input);
						cmdWriteTsInfo.Parameters.Add("NewTims_Key", OracleDbType.Decimal, DBNull.Value, ParameterDirection.Output);

						cmdWriteTsInfo.Parameters.Add("RCNamePostfix", OracleDbType.Varchar2, 40, timeser.tscode == null ? "" : timeser.tscode, ParameterDirection.Input);
						string stRCHeadTimeStep = timeser.TimeStepAttribute;
						cmdWriteTsInfo.Parameters.Add("RCHeadTimeStep", OracleDbType.Varchar2, 255, stRCHeadTimeStep == null ? "" : stRCHeadTimeStep, ParameterDirection.Input);
					}
					else
					{
						sb = new StringBuilder(@"
							DECLARE 
								nTims_Key TIMESER.Tims_Key%TYPE;
								no INTEGER;
								iccSender INTEGER := NULL;
								rcName rc_set.name%TYPE;
								stDuplTSCode TIMESER.TSCode%TYPE;
								stDuplEstTime TIMESER.EstTime%TYPE;
								-- prognumber INTEGER := 65;
							BEGIN
								-- select max(tims_key)+1 into nTims_Key from timeser;
								nTims_Key := TS_Key.GetAdvancedTimsKey;
								:NewTims_Key := nTims_Key;
								BEGIN
									INSERT INTO TIMESER 
										(tims_key, filename, tscode, tsin_key,unme_key,curvetyp,steptype,descript,tser_key,tsor_key,tspr_key,name,use_registration_handling,vltp_key) 
										VALUES 
										(nTims_Key, :2, :3, :4,:5,:6,:7,:8,:9,:10,:11,:12, 0, 0);
								EXCEPTION
									WHEN OTHERS THEN
										SELECT 
													MAX(TSCode), MAX(EstTime)
											INTO 
													stDuplTSCode, stDuplEstTime
											FROM TIMESER 
											WHERE Tims_Key = nTims_Key
											;
										raise_application_error(-20000, 'Exception after attempt to INSERT INTO TIMESER; nTims_Key = ' || nTims_Key || 
											'; existing TS TSCode = ' || stDuplTSCode || '; existing TS EstTime = ' || stDuplEstTime ||
											'; Exception: ' || SQLERRM);
								END;
								iccSender := pdmisc.getenv( 'ICC_SENDER', iccSender );
								if ( iccSender is null ) then
									pdlog.reportError( 20105,'ICC_SENDER');
								end if;
								rcName := 'VTS#RRS#'||:3;
								select count(*) into no from rc_set  where name = rcName;
								if ( no > 0 ) then
									pdlog.reportError(20108, 'RC_SET', 'NAME', rcName);
								end if;
								-- pdlog.set_prog_number(prognumber);
								-- pdlog.reportError(20112,pdmisc.get_prog_number);

								insert into rc_set (opun_key,name,object_type) values (iccSender,rcName,5);
								insert into rc_head (rc_key,seqno,name,svalue) values (rc_set_seq.currval,1,'TIMESTEP',:13);
								insert into rc_column (rc_key,seqno,object_type,tims_key) values ( rc_set_seq.currval,1,1,nTims_Key);
								insert into c_definition (calc_key,seqno,valftime,valttime) values (rc_column_seq.currval,1,'00000000000000000','99999999999999999');
							END;"
							);
						cmdWriteTsInfo.CommandText = sb.ToString();
						cmdWriteTsInfo.Parameters.Add("NewTims_Key", OracleDbType.Int32, ParameterDirection.Output);
						cmdWriteTsInfo.Parameters.Add(":2", timeser.filename != null ? timeser.filename : "/");
						cmdWriteTsInfo.Parameters.Add(":3", timeser.tscode);
						cmdWriteTsInfo.Parameters.Add(":4", timeser.Resolution.Value);
						cmdWriteTsInfo.Parameters.Add(":5", timeser.unme_key);
						cmdWriteTsInfo.Parameters.Add(":6", (short)timeser.CurveType);
						cmdWriteTsInfo.Parameters.Add(":7", timeser.Steptype);
						cmdWriteTsInfo.Parameters.Add(":8", timeser.descript == null ? "" : timeser.descript);
						cmdWriteTsInfo.Parameters.Add(null, timeser.tser_key == 0 && !timeser.isPrecipitation ? (int)TsType.Undefined : timeser.tser_key);
						cmdWriteTsInfo.Parameters.Add(":10", timeser.tsor_key == 0 ? (int)TsOrigin.Undefined : timeser.tsor_key);
						cmdWriteTsInfo.Parameters.Add(":11", timeser.tspr_key == 0 ? (int)TsProducer.Undefined : timeser.tspr_key);
						cmdWriteTsInfo.Parameters.Add(":12", timeser.name);
						string resolution = timeser.TimeStepAttribute;
						cmdWriteTsInfo.Parameters.Add(":13", resolution);
						//oCmd.Parameters.Add(":14", (short)timeser.ValueType);
					}
					{
						Util.ExecuteCommand(cmdWriteTsInfo, connection);
						timeser.tims_key = Int32.Parse(cmdWriteTsInfo.Parameters["NewTims_Key"].Value.ToString());

						if (timeser.tims_key > 0) 
						{
							// Split expression into parts and insert into c_expression
							int i = 0;
							string ls = String.Format("%#{0}#", timeser.tims_key);
							expression = expression.Replace("$<timskey>", timeser.tims_key.ToString());	// Expand macros part of expression
							foreach (string exp in expression.Replace("\r", "").Split('\n')) {
								using (OracleCommand eCmd = new OracleCommand()) {
                                    
                                    sb.Remove(0, sb.Length);
									sb.AppendFormat("insert into c_expression (expression_key,seqno,expression) values (c_expression_seq.currval,{0},:1)", ++i);
									eCmd.CommandText = sb.ToString();
									eCmd.Parameters.Add(":1", exp.Replace("##", ls));
									Util.ExecuteCommand(eCmd, connection);
								}
							}
							// Set rc_key in timeser
							using (OracleCommand uCmd = new OracleCommand()) 
							{
								sb.Remove(0, sb.Length);
								sb.Append("update timeser set rc_key = rc_set_seq.currval where tims_key = :1");
								uCmd.CommandText = sb.ToString();
								uCmd.Parameters.Add(":1", timeser.tims_key);
								Util.ExecuteCommand(uCmd, connection);
							}
						}
					}
				}
			}
			return timeser.tims_key;
		}
		public static int CreateTimeSeries92(TimeSeriesData.TimeSeriesInfo timeser, string expression, IDbConnection connection) {
			if (timeser.tims_key == 0)
			{
				// Insert
				using (OracleCommand oCmd = new OracleCommand())
				{
					// Create timeseries and then create a virtual ts definition 
					// Virtual series definiton is based on PL/SQL code found in trigger code for new_virtual_calcset
					StringBuilder sb = new StringBuilder(@"
declare 
	timskey TIMESER.Tims_Key%TYPE;
	no INTEGER;
	iccSender INTEGER := NULL;
	rcName rc_set.name%TYPE;
	-- prognumber INTEGER := 65;
begin
	-- select max(tims_key)+1 into timskey from timeser;
	timskey := TS_Key.GetAdvancedTimsKey;
	:1 := timskey;
	insert into timeser (tims_key,filename,tscode,tsin_key,unme_key,curvetyp,steptype,descript,tser_key,tsor_key,tspr_key,name,use_registration_handling) values (timskey,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,0);
	iccSender := pdmisc.getenv( 'ICC_SENDER', iccSender );
	if ( iccSender is null ) then
		pdlog.reportError( 20105,'ICC_SENDER');
	end if;
	rcName := 'VTS#RRS#'||:3;
	select count(*) into no from rc_set  where name = rcName;
	if ( no > 0 ) then
		pdlog.reportError(20108, 'RC_SET', 'NAME', rcName);
	end if;
	-- pdlog.set_prog_number(prognumber);
	-- pdlog.reportError(20112,pdmisc.get_prog_number);

	insert into rc_set (opun_key,name,object_type) values (iccSender,rcName,5);
	insert into rc_head (rc_key,seqno,name,svalue) values (rc_set_seq.currval,1,'TIMESTEP',:13);
	insert into rc_column (rc_key,seqno,object_type,tims_key) values ( rc_set_seq.currval,1,1,timskey);
	insert into c_definition (calc_key,seqno,valftime,valttime) values (rc_column_seq.currval,1,'00000000000000000','99999999999999999'); end;");


					{
						oCmd.CommandText = sb.ToString();
						oCmd.Parameters.Add(":1", OracleDbType.Int32, ParameterDirection.Output);
						oCmd.Parameters.Add(":2", timeser.filename != null ? timeser.filename : "/");
						oCmd.Parameters.Add(":3", timeser.tscode);
						oCmd.Parameters.Add(":4", timeser.Resolution.Value);
						oCmd.Parameters.Add(":5", timeser.unme_key);
						oCmd.Parameters.Add(":6", (short)timeser.CurveType);
						oCmd.Parameters.Add(":7", timeser.Steptype);
						oCmd.Parameters.Add(":8", timeser.descript == null ? "" : timeser.descript);
						oCmd.Parameters.Add(":9", timeser.tser_key == 0 ? (int)TsType.Undefined : timeser.tser_key);
						oCmd.Parameters.Add(":10", timeser.tsor_key == 0 ? (int)TsOrigin.Undefined : timeser.tsor_key);
						oCmd.Parameters.Add(":11", timeser.tspr_key == 0 ? (int)TsProducer.Undefined : timeser.tspr_key);
						oCmd.Parameters.Add(":12", timeser.name);
						string resolution = timeser.TimeStepAttribute;
						oCmd.Parameters.Add(":13", resolution);

						Util.ExecuteCommand(oCmd, connection);
						timeser.tims_key = Util.GetInt32(oCmd.Parameters[":1"]);

						if (timeser.tims_key > 0)
						{
							// Split expression into parts and insert into c_expression
							int i = 0;
							string ls = String.Format("%#{0}#", timeser.tims_key);
							expression = expression.Replace("$<timskey>", timeser.tims_key.ToString());	// Expand macros part of expression
							foreach (string exp in expression.Replace("\r", "").Split('\n'))
							{
								using (OracleCommand eCmd = new OracleCommand())
								{
									sb.Remove(0, sb.Length);
									sb.AppendFormat("insert into c_expression (expression_key,seqno,expression) values (c_expression_seq.currval,{0},:1)", ++i);
									eCmd.CommandText = sb.ToString();
									eCmd.Parameters.Add(":1", exp.Replace("##", ls));
									Util.ExecuteCommand(eCmd, connection);
								}
							}
							// Set rc_key in timeser
							using (OracleCommand uCmd = new OracleCommand())
							{
								sb.Remove(0, sb.Length);
								sb.Append("update timeser set rc_key = rc_set_seq.currval where tims_key = :1");
								uCmd.CommandText = sb.ToString();
								uCmd.Parameters.Add(":1", timeser.tims_key);
								Util.ExecuteCommand(uCmd, connection);
							}
						}
					}
				}
			}
			return timeser.tims_key;
		}
		[Obsolete("This function needs connection arguments etc, use alternative with dbconnection")]
		public static void DeleteTimeSeries(int tims_key) 
		{
			DeleteTimeSeries(tims_key, null);
		}

		/// <summary>
		/// Delete a timeseries identified by the tims_key
		/// If there are a calcset associated with this series (i.e. a virtual time series), this definiton is 
		/// removed from the RC_SET hieararchy
		/// </summary>
		/// <param name="tims_key">The primary key for a time series</param>
		public static void DeleteTimeSeries(int tims_key, IDbConnection connection) 
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = @"
declare 
	rckey INTEGER := 0;
	status INTEGER;
begin
	select rc_key into rckey from timeser where tims_key=:1;
	if ( rckey > 0 ) then
		icc_rc.deletercdefinition(rckey,status);
	end if;
	delete from timeser where tims_key=:1;
	delete from meter_vl where tims_key=:1;
	
end;";
				oCmd.Parameters.Add(":1", tims_key);
				Util.ExecuteCommand(oCmd, connection);
			}
			return;
		}

		[Obsolete("This function needs connection arguments etc, use alternative with dbconnection")]
		public static void DeleteRC(int tims_key) {
			using( IDbConnection db=Util.OpenConnection()) {
				DeleteRC(tims_key, db);
			}
		}

		public static void DeleteRC(int tims_key,IDbConnection db)
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "select rc_key from rc_column where tims_key = :1";
				oCmd.Parameters.Add(null, tims_key);
				int rc_key = (int)Util.CommandToScalar(oCmd, db);

				OracleCommand oCmd1 = new OracleCommand();
				oCmd1.CommandText = "delete from rc_column where rc_key = :1";
				oCmd1.Parameters.Add(null, rc_key);
				Util.ExecuteCommand(oCmd1, db);

				oCmd1.CommandText = "delete from rc_set where rc_key = :1";
				Util.ExecuteCommand(oCmd1, db);
			}
		}
		[Obsolete("This function needs connection arguments etc, use alternative with dbconnection")]
		public static void DeleteTimeSeries(string tscode)
		{
			DeleteTimeSeries(tscode, null);
		}

		public static void DeleteTimeSeries(string fullname, IDbConnection connection)
		{
			TimeSeriesInfo info = new TimeSeriesInfo();
			info.tscode = fullname;
			info = Fetch(info, connection);

			if( info != null)
				DeleteTimeSeries(info.tims_key, connection);
		}
		//public static void DeleteTimeSeries75(string tscode, IDbConnection connection) {
		//	TimeSeriesInfo info = new TimeSeriesInfo();
		//	info.tscode = tscode;
		//	info = Fetch(info, connection,true);

		//	if (info != null)
		//		DeleteTimeSeries(info.tims_key, connection);
		//}

		public static void DeleteTimeSeries(string tscode, UtcTime from, UtcTime to, IDbConnection connection)
		{
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;
			int key = 0;
			int status = 0;
			const string sValuesOnly = "VALUES"; //deletes only time series values. Preserves header. Opposite to "HEADER".

			TimeSeriesInfo info = new TimeSeriesInfo();
			info.tscode = tscode;
			info = Fetch(info, connection);
			if( info == null)
				return;

			using (OracleCommand cmd = new OracleCommand("icc_tsadm.create_search_key_list")) {
				cmd.CommandType = CommandType.StoredProcedure;

				OracleParameter searchKeyPar = cmd.Parameters.Add("p_tims_key", info.tims_key);
				searchKeyPar = cmd.Parameters.Add("selected_key_list", OracleDbType.Int32, ParameterDirection.Output);
				searchKeyPar = cmd.Parameters.Add("status", OracleDbType.Int32, ParameterDirection.Output);
				searchKeyPar = cmd.Parameters.Add("message", OracleDbType.Varchar2, ParameterDirection.Output);
				Util.ExecuteCommand(cmd, connection);
				status = ((OracleDecimal)cmd.Parameters["status"].Value).ToInt32();
				key = ((OracleDecimal)cmd.Parameters["selected_key_list"].Value).ToInt32();

				if (status != 0)
					throw new ApplicationException("Error before deleting time series interval: " + (string)cmd.Parameters["message"].Value);

				using (OracleCommand cmdx = new OracleCommand("icc_tsadm.delete_ts")) {
					cmdx.CommandType = CommandType.StoredProcedure;
					OracleParameter deletePar = cmdx.Parameters.Add("selected_key_list", key);
					deletePar = cmdx.Parameters.Add("delete_option", sValuesOnly); //deletes only values, preserves header
					deletePar = cmdx.Parameters.Add("first_date", calendar.ToDateTime(from));
					deletePar = cmdx.Parameters.Add("last_date", calendar.ToDateTime(to));
					deletePar = cmdx.Parameters.Add("status", OracleDbType.Int32, ParameterDirection.Output);
					deletePar = cmdx.Parameters.Add("message", OracleDbType.Varchar2, ParameterDirection.Output);
					Util.ExecuteCommand(cmdx, connection);
					status = ((OracleDecimal)cmdx.Parameters["status"].Value).ToInt32();
					if (status != 0)
						throw new ApplicationException("Error deleting time series interval: " + (string)cmdx.Parameters["message"].Value);
				}
			}

		}

		/// <summary>
		/// Delete the calcset related to a virtual series
		/// </summary>
		/// <param name="rc_key">The time series key</param>
		public static void DeleteVirtualTimeserCalcdef(int tims_key,IDbConnection db) 
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = @"
declare 
	rckey INTEGER := 0;
	status INTEGER;
begin
	select rc_key into rckey from timeser where tims_key=:1;
	if ( rckey>0 ) then
		icc_rc.deletercdefinition(rckey,status);
	end if;
	update timeser set rc_key=null where tims_key=:1;
end;";
				oCmd.Parameters.Add(":1", tims_key);
				Util.ExecuteCommand(oCmd, db);
			}
			return;

		}
		public static void DeleteVirtualTimeserCalcdef(int tims_key) {
			using(IDbConnection db=Util.OpenConnection()) {
				DeleteVirtualTimeserCalcdef(tims_key, db);
			}
		}
		public static TimeSeriesInfo[] Fetch(int[] timsKeys) 
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				return Fetch(timsKeys, connection);
			}
		}
		public static TimeSeriesInfo[] Fetch(int[] timsKeys, IDbConnection connection) {
			return Fetch(timsKeys, connection, false);
		}
		public static TimeSeriesInfo[] Fetch(int[] timsKeys, IDbConnection connection, bool backPort92)
		{
			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = "icc_tsstore.get_timeseries_info";

				OracleParameter par;

				par = cmd.Parameters.Add("p_timsKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = timsKeys;

				par = cmd.Parameters.Add("c", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.Output;

				ArrayList list = new ArrayList();

				foreach (DataRow dr in Util.CommandToDataTable(cmd, connection).Rows)
					list.Add(new TimeSeriesInfo(dr, backPort92));

				return (TimeSeriesInfo[])list.ToArray(typeof(TimeSeriesInfo));
			}
		}
				
		public static TimeSeriesInfo Fetch(TimeSeriesInfo timeser) 
		{
			return Fetch(timeser, null);
		}
		public static TimeSeriesInfo Fetch(TimeSeriesInfo timeser, IDbConnection connection) {
			return Fetch(timeser, connection, false);
		}

		public static TimeSeriesInfo Fetch(TimeSeriesInfo timeser, IDbConnection connection, bool backPort92) 
		{
			using (OracleCommand oCmd = new OracleCommand()) {

				if (timeser.tims_key > 0) {
					oCmd.CommandText = "select * from timeser where tims_key=:1";
					oCmd.Parameters.Add(":1", timeser.tims_key);
				} else if (timeser.tscode != null && (timeser.tscode.Length) > 0) {
					oCmd.CommandText = "select * from timeser where filename||tscode=:1";
					oCmd.Parameters.Add(":1", timeser.FullName);
				} else
					return null;

				DataTable tbl = Util.CommandToDataTable(oCmd, connection);
				if (tbl.Rows.Count == 0) return null;

				// Fill and return
				return new TimeSeriesInfo(tbl.Rows[0], backPort92);
			}
		}

		public static IDictionary FetchAccess(int[] timsKeys) 
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				return FetchAccess(timsKeys, connection);
			}
		}

		public static IDictionary FetchAccess(int[] timsKeys, IDbConnection connection)
		{
			Hashtable hashtable = new Hashtable();

			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.CommandText = "icc_tsstore.get_timeseries_access";

				OracleParameter par;

				par = cmd.Parameters.Add("p_timsKey", OracleDbType.Int32);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = timsKeys;

				par = cmd.Parameters.Add("c", OracleDbType.RefCursor);
				par.Direction = ParameterDirection.Output;

				Util.ExecuteCommand(cmd, connection);

				using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters["c"].Value) {
					using (OracleDataReader reader = cursor.GetDataReader()) {
						while (reader.Read())
							hashtable.Add(reader.GetInt32(0), reader.GetInt32(1));
					}
				}

				return hashtable;
			}
		}
		

		public static DataTable FetchUnconstrainedValues(
			int timsKey, int vlCode, UtcTime beginning, UtcTime end, 
			IDbTransaction transaction)
		{
			if (end == UtcTime.Null)
				return FetchValues(
					"tims_key, vlcode, datetim, trunc(tsvalue, 28) tsvalue, tsstatus",
					"meter_vl", timsKey, vlCode, transaction);
			else
			{
				// Adjust beginning so that we get the last point before the
				// desired interval.
				beginning = GetLastTimeBefore(
					timsKey, vlCode, beginning, transaction);

				if (beginning == UtcTime.Null)
					return FetchValues(
						"tims_key, vlcode, datetim, trunc(tsvalue, 28) tsvalue, tsstatus",
						"meter_vl", "datetim", false, timsKey, vlCode, 
						end, transaction);
				else
					return FetchValues(
						"tims_key, vlcode, datetim, trunc(tsvalue, 28) tsvalue, tsstatus",
						"meter_vl", "datetim", false, timsKey, vlCode, 
						beginning, end, transaction);
			}
		}

		static UtcTime GetLastTimeBefore(int timsKey, int vlCode, UtcTime time,
			IDbTransaction transaction)
		{
			if (time == UtcTime.Null)
				return UtcTime.Null;

			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand(
@"select max(datetim) from meter_vl 
where tims_key = :1 and vlcode = :2 and datetim <= :3")) {
				cmd.Parameters.Add(null, timsKey);
				cmd.Parameters.Add(null, vlCode);
				cmd.Parameters.Add(null, calendar.ToDateTime(time));

				object o = Util.CommandToScalar(cmd, transaction);

				if (o == DBNull.Value)
					return UtcTime.Null;

				return calendar.ToUtcTime((DateTime)o);
			}
		}

		
		static DataTable FetchValues(string columns, string tableName, 
			int tims_key, int vlCode, IDbTransaction transaction)
		{
			string sql = String.Format(
				"select {0} from {1} where tims_key = :1 and vlcode = :2",
				columns, tableName);

			using (OracleCommand cmd = new OracleCommand(sql)) {
				cmd.Parameters.Add(null, tims_key);
				cmd.Parameters.Add(null, vlCode);
				return Util.CommandToDataTable(cmd, transaction);
			}
		}

		static DataTable FetchValues(string columns, string tableName, 
			string dateTimeName, bool isDateTimeString, 
			int tims_key, int vlCode, UtcTime end, 
			IDbTransaction transaction)
		{
			string sql = String.Format(
				@"select {0} from {1} where tims_key = :1 and vlcode = :2
and {2} < :3",
				columns, tableName, dateTimeName);

			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand(sql)) {
				cmd.Parameters.Add(null, tims_key);
				cmd.Parameters.Add(null, vlCode);

				if (isDateTimeString)
					cmd.Parameters.Add(null, calendar.ToDateTime(end));
				else
					cmd.Parameters.Add(null, calendar.ToDateTime(end));

				return Util.CommandToDataTable(cmd, transaction);
			}
		}

		static DataTable FetchValues(string columns, string tableName, 
			string dateTimeName, bool isDateTimeString, 
			int tims_key, int vlCode, 
			UtcTime beginning, UtcTime end, IDbTransaction transaction)
		{
			string sql = String.Format(
				@"select {0} from {1} where tims_key = :1 and vlcode = :2
and {2} >= :3 and {2} < :4",
				columns, tableName, dateTimeName);

			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

			using (OracleCommand cmd = new OracleCommand(sql)) {
				cmd.Parameters.Add(null, tims_key);
				cmd.Parameters.Add(null, vlCode);

				if (isDateTimeString) {
					cmd.Parameters.Add(null, calendar.ToDatabaseString(beginning));
					cmd.Parameters.Add(null, calendar.ToDatabaseString(end));
				} else {
					cmd.Parameters.Add(null, calendar.ToDateTime(beginning));
					cmd.Parameters.Add(null, calendar.ToDateTime(end));
				}

				return Util.CommandToDataTable(cmd, transaction);
			}
		}

		public static DataTable FetchTsintTP() 
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "select tsin_key,code,name from tsint_tp";
				return Util.CommandToDataTable(oCmd);
			}
		}

		public static DataTable FetchTseriTP() 
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "select tser_key,code,name from tseri_tp";
				return Util.CommandToDataTable(oCmd);
			}
		}

		public static DataTable FetchTsGroups() 
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "select tsgr_key,name from tsgroup";
				return Util.CommandToDataTable(oCmd);
			}
		}

		public static bool UpdateRcHead(int rc_key, string name, string svalue) {
			using (OracleCommand cmd = new OracleCommand()) {
				cmd.CommandText = "select count(*) from rc_head where rc_key=:1 and name=:2";
				cmd.Parameters.Add(null, rc_key);
				cmd.Parameters.Add(null, name);
				object o = Util.CommandToScalar(cmd);
				if (o == null) return false;
				if (System.Convert.ToInt32(o) == 0) return false;
				cmd.Parameters.Clear();
				cmd.CommandText = "update rc_head set svalue=:1 where rc_key=:2 and name=:3";
				cmd.Parameters.Add(null, svalue);
				cmd.Parameters.Add(null, rc_key);
				cmd.Parameters.Add(null, name);
				o = Util.CommandToScalar(cmd);
				return true;
			}
		}
		public static void SaveIntoObjectChangedForVTS(int rc_key)
		{
            using (OracleCommand oCmd = new OracleCommand())
            {
                oCmd.CommandText = "insert into object_changed (object_key,object_type,prog_key) values (:1,:2,:3)";
                short object_type = 5;  // PD_VIRTUAL_TIMES_SERIES_OBJECT_TYPE = 5
                var prog_key = (int)IccProgram.P_AVAILABILITY;
                oCmd.Parameters.Add(null, rc_key);
                oCmd.Parameters.Add(null, object_type);
                oCmd.Parameters.Add(null, prog_key);
                // Let fromtime - totime be undefined
                Util.ExecuteCommand(oCmd);
            }
		}
        public static bool DoesTsExist(int tims_key)
        {
            // Does ts realy exist?
            OracleCommand oCmdts = new OracleCommand();
            oCmdts.CommandText = "select tims_key from timeser where tims_key=:1";
            oCmdts.Parameters.Add(":1", OracleDbType.Int32, tims_key, ParameterDirection.Input);
            DataTable tblts = Util.CommandToDataTable(oCmdts);
            if (tblts.Rows.Count < 1) return false;
            return true;
        }
        public static void SaveIntoObjectChangedForVTSFromTimsKey(int tims_key)
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "insert into object_changed (object_key,object_type,prog_key) values ";
				oCmd.CommandText += "((select rc_key from timeser where tims_key =:1),:2,:3)";
				short object_type = 5;	// PD_VIRTUAL_TIMES_SERIES_OBJECT_TYPE = 5
				int prog_key = (int)IccProgram.P_AVAILABILITY;
				oCmd.Parameters.Add(null, tims_key);
				oCmd.Parameters.Add(null, object_type);
				oCmd.Parameters.Add(null, prog_key);
				// Let fromtime - totime be undefined
				Util.ExecuteCommand(oCmd);
			}
		}
		public static void SaveIntoObjectChangedForVTSFromCompKey(int comp_key)
		{
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "select a.rc_key from timeser a, restr_comp_con b where a.tims_key=b.tims_key and b.comp_key=:1";
				oCmd.Parameters.Add(null, comp_key);
				DataTable dt = Util.CommandToDataTable(oCmd);
				short object_type = 5;	// PD_VIRTUAL_TIMES_SERIES_OBJECT_TYPE = 5
				int prog_key = (int)IccProgram.P_AVAILABILITY;
				foreach (DataRow r in dt.Rows) {
					oCmd.Parameters.Clear();
					oCmd.CommandText = "insert into object_changed (object_key,object_type,prog_key) values ";
					oCmd.CommandText += "(:1,:2,:3)";
					int rckey = (int)r["RC_KEY"];
					oCmd.Parameters.Add(null, rckey);
					oCmd.Parameters.Add(null, object_type);
					oCmd.Parameters.Add(null, prog_key);
					Util.ExecuteCommand(oCmd);
				}
			}
			// Let fromtime - totime be undefined
		}

        public static void ReplaceExpressionForVTs(int timsKey, string expression)
        {
            var expression_key = FetchExpressionKeyForVTs(timsKey);
            if (expression_key > 0)
            {
                ReplaceExpressionForVTsInternal(expression_key, expression);
                SaveIntoObjectChangedForVTSFromTimsKey(timsKey);
            }
        }

        public static int FetchExpressionKeyForVTs(int timsKey)
        {
            int res;
            using (var db = Util.OpenConnection())
            {
                res = FetchExpressionKeyForVTs(timsKey, db);
            }
            return res;
        }

        /// <summary>
        /// Get expression_key vts definiton related to input tims_key.
        /// Assume that expression is placed on unlimited period/first periode, i.e. seqno=1 in c_definiton
        /// </summary>
        /// <param name="timsKey"></param>
        /// <param name="db"></param>
        /// <returns></returns>
        public static int FetchExpressionKeyForVTs(int timsKey, IDbConnection db)
        {
            if (timsKey <= 0)
                return 0;   // Throw an exception

            using (var oCmd = new OracleCommand())
            {
                // Virtual series definiton is based on PL/SQL code found in trigger code for new_virtual_calcset
                StringBuilder sb = new StringBuilder(@"
                    declare 
	                    exprkey c_definition.expression_key%type := 0;
                    begin
	                    select cd.expression_key into exprkey from c_definition cd, rc_column rc ,timeser ts where ts.rc_key=rc.rc_key and ts.tims_key= :1  and rc.tims_key=:1 and cd.calc_key=rc.calc_key and cd.seqno=1;
	                    :2 := nvl(exprkey,0);
                    end;"
                );
                oCmd.CommandText = sb.ToString();
                oCmd.Parameters.Add(":1", timsKey);
                oCmd.Parameters.Add(":2", OracleDbType.Int32, ParameterDirection.Output);
                Util.ExecuteCommand(oCmd, db);
                return Util.GetInt32(oCmd.Parameters[":2"]);//
            }
        }

        public static void ReplaceExpressionForVTsInternal(int expression_key, string expression)
        {
            if (expression_key == 0)
                return;    // Throw an exception
            // Delete all C_EXPRESSION rows related to actual expression, and then insert new expression with
            // reference to same expression_key

            using (IDbConnection connection = Util.OpenConnection())
            {
                IDbTransaction transaction = Util.OpenTransaction(connection);
                StringBuilder sb = new StringBuilder();
                using (OracleCommand oCmd = new OracleCommand())
                {
                    oCmd.CommandText = "delete from c_expression where expression_key=:1";
                    oCmd.Parameters.Add(":1", expression_key);
                    Util.ExecuteCommand(oCmd, transaction);
                    int i = 0;
                    foreach (string exp in expression.Replace("\r", "").Split('\n'))
                    {
                        using (OracleCommand eCmd = new OracleCommand())
                        {
                            sb.Remove(0, sb.Length);
                            sb.AppendFormat("insert into c_expression (expression_key,seqno,expression) values ({0},{1},:1)", expression_key, ++i);
                            eCmd.CommandText = sb.ToString();
                            eCmd.Parameters.Add(":1", exp);
                            Util.ExecuteCommand(eCmd, transaction);
                        }
                    }
                    Util.Commit(transaction);
                }
            }
        }

        public static string FetchExpression(int expression_key,IDbConnection db)
		{
			if ( expression_key == 0 ) return String.Empty;	// Throw an exception
			using (OracleCommand oCmd = new OracleCommand()) {
				oCmd.CommandText = "select expression from c_expression where expression_key=:1 order by seqno";
				oCmd.Parameters.Add(":1", expression_key);
				DataTable tbl = Util.CommandToDataTable(oCmd, db);
				StringBuilder sb = new StringBuilder();
				foreach (DataRow r in tbl.Rows) {
					sb.AppendFormat("{0}\n", r[0].ToString());
				}
				return sb.ToString();
			}
		}
		[Obsolete("This function needs connection arguments etc, use alternative with dbconnection")]
		public static string FetchExpression(int expression_key) {
			string res;
			using(IDbConnection db=Util.OpenConnection()) {
				res = FetchExpression(expression_key, db);
			}
			return res;
		}

		// GetPDDBEnv
		public static string GetPDDBEnv(string name, string defaultValue, IDbConnection connection)
		{
			string value = string.Empty;
			string query = string.Format("SELECT VALUE FROM PDDBENV WHERE NAME = '{0}'", name);

			DataTable resultDT = Util.SqlToDataTable(query, connection);

			if (resultDT.Rows.Count > 0)
				value = Convert.ToString(resultDT.Rows[0]["VALUE"]);
			if (string.IsNullOrEmpty(value))
			{
				value = defaultValue;
			}

			return value;

        }
        //-----------------------------------------------------------------------
        public static DataTable FetchPrdKeyForGroup(string sysGroupName)
        {
            using (IDbConnection connection = Util.OpenConnection())
            {
                OracleCommand oCmd = new OracleCommand();
                oCmd.CommandText = "select prd_key from SIMSYSDA where name=:1";
                oCmd.Parameters.Add(null, sysGroupName);
                DataTable tbl = Util.CommandToDataTable(oCmd);
                return tbl;
               /* if (tbl.Rows.Count == 0) return -1;
                return (int)tbl.Rows[0][0];*/
            }
        }
        public static DataTable FetchRestrictionsTimsKeyInActivUse(string sysGroupName)
        {
            using (IDbConnection connection = Util.OpenConnection())
            {
                OracleCommand oCmd = new OracleCommand();
                oCmd.CommandText = "select distinct a.tims_key from view_restr_comp_con a, view_hydcomp_in_simsysda c,restrictions b where a.owner > -1 and a.comp_key = b.comp_key and c.hydt_key in (2,4,5,6,7,8) and a.hyd_key = c.hyd_key  and c.sname = :1";
                oCmd.Parameters.Add(null, sysGroupName);
                return Util.CommandToDataTable(oCmd);
            }
        }
        public static DataTable FetchAllRestrictionsTimsKey(int prdKey)
        {
            string tsDum = "select tims_key from TIMESER where tscode like'RRS.S";
            tsDum += prdKey.ToString();
            tsDum += "%'";
            OracleCommand oCmd = new OracleCommand();
            oCmd.CommandText = tsDum;
            return Util.CommandToDataTable(oCmd);
        }
        public static DataTable FetchAllRestrictionsTimsKeyAndTscode(int prdKey)
        {
            string tsDum = "select tims_key,tscode from TIMESER where tscode like'RRS.S";
            tsDum += prdKey.ToString();
            tsDum += "%'";
            OracleCommand oCmd = new OracleCommand();
            oCmd.CommandText = tsDum;
            return Util.CommandToDataTable(oCmd);
        }
        public static DataTable FetchAllRestrictionsTimsKey(string sysGroupName)
        {
            using (IDbConnection connection = Util.OpenConnection())
            {

                DataTable prdTab = FetchPrdKeyForGroup(sysGroupName);
                DataTable tstable = new DataTable();
                tstable.Columns.Add("timsKey", typeof(int));
                foreach (DataRow r in prdTab.Rows)
                {
                    DataTable tsFprd = FetchAllRestrictionsTimsKey((int)r[0]);
                    foreach (DataRow ro in tsFprd.Rows)
                    {
                        tstable.Rows.Add(ro[0]);
                    }
                }
                return tstable;
            }
        }
        public static DataTable FetchAllRestrictionsTimsKeyAndTscode(string sysGroupName)
        {
            using (IDbConnection connection = Util.OpenConnection())
            {
                DataTable prdTab = FetchPrdKeyForGroup(sysGroupName);
                DataTable tstable = new DataTable();
                tstable.Columns.Add("timsKey", typeof(int));
                tstable.Columns.Add("tscode", typeof(string));
                foreach (DataRow r in prdTab.Rows)
                {
                    DataTable tsFprd = FetchAllRestrictionsTimsKeyAndTscode((int)r[0]);
                    foreach (DataRow ro in tsFprd.Rows)
                    {
                        tstable.Rows.Add((int)ro[0], (string)ro[1]);
                    }
                }
                return tstable;
            }
        }
        public static void DeleteRestriction(string sysGroupName)
        {
          DataTable timsKTable = FetchRestrictionsTimsKeyInActivUse(sysGroupName);
            foreach (DataRow r in timsKTable.Rows)
            {
                int tsKey = (int)r[0];
                DeleteRestriction(tsKey);
            }
            DataTable timsKTable2 = FetchAllRestrictionsTimsKey(sysGroupName);
            foreach (DataRow r in timsKTable2.Rows)
            {
                int tsKey = (int)r[0];
                DeleteRestriction(tsKey);
            }
        }
        public static void DeleteRestrictionPrd(int prdKey)
        {
            DataTable timsKTable2 = FetchAllRestrictionsTimsKey(prdKey);
            foreach (DataRow r in timsKTable2.Rows)
            {
                int tsKey = (int)r[0];
                DeleteRestriction(tsKey);
            }
        }
        public static void DeleteRestriction(int tims_key)
        {
           // DeleteC_expression(tims_key);
            using (IDbConnection connection = Util.OpenConnection())
            {
                IDbTransaction transaction = Util.OpenTransaction(connection);
                StringBuilder sb = new StringBuilder();
               
				DeleteVirtualTimeserCalcdef(tims_key, connection);
			
                using (OracleCommand oCmd = new OracleCommand())
                {
                    oCmd.CommandText = "delete from restr_dynamic where oprs_key in (select oprs_key from restrictions_tab where comp_key in (select comp_key from restr_comp_con where tims_key=:1))";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
                using (OracleCommand oCmd = new OracleCommand())
                {
                    oCmd.CommandText = "delete from restr_dynamic where tims_key = :1";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
                using (OracleCommand oCmd = new OracleCommand())
                {
                    oCmd.CommandText = "delete from restr_static where oprs_key in (select oprs_key from restrictions_tab where comp_key in (select comp_key from restr_comp_con where tims_key =:1))";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
               using (OracleCommand oCmd = new OracleCommand())
               {
                    oCmd.CommandText = "delete from restr_attributes where oprs_key in (select oprs_key from restrictions_tab where comp_key in (select comp_key from restr_comp_con where tims_key =:1))";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
                using (OracleCommand oCmd = new OracleCommand())
                {
                    oCmd.CommandText = "delete from restrictions_tab where comp_key in (select comp_key from restr_comp_con where tims_key =:1)";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
                using (OracleCommand oCmd = new OracleCommand())
                {


                    oCmd.CommandText = "delete from restrictions_audit where comp_key in (select comp_key from restr_comp_con where tims_key =:1)";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
                using (OracleCommand oCmd = new OracleCommand())
                {

                    oCmd.CommandText = "delete from restr_comp_con where tims_key =:1";
                    oCmd.Parameters.Add(":1", tims_key);
                    Util.ExecuteCommand(oCmd, transaction);
                }
                DeleteTimeSeries(tims_key, connection);
                Util.Commit(transaction);
            }
           
        }
        //public static void DeleteC_expression(int tims_key)
        //{
        //    using (IDbConnection connection = Util.OpenConnection())
        //    {
        //        IDbTransaction transaction = Util.OpenTransaction(connection);
        //        StringBuilder sb = new StringBuilder();
        //        int expression_key = FetchExpressionKeyForVTs(tims_key);
        //        using (OracleCommand oCmd = new OracleCommand())
        //        {
        //            oCmd.CommandText = "delete from c_expression where expression_key=:1";
        //            oCmd.Parameters.Add(":1", expression_key);
        //            Util.ExecuteCommand(oCmd, transaction);
        //        }
        //    }
        //}

        /// <summary>
		/// Resemble a value of a time series as it might be stored/retrieved from
		/// database, including keys, sub-keys, etc. etc..
		/// </summary>
		public struct TimeSeriesDbPoint: IComparable
		{
			private int stepType;
			private int timsKey;
			private int vlcode;
			private long pdtimeDatetime;
			private double value;
			private int status;
			private int? readKey;
			private int? reptKey;
			private int? reasKey;

			public int StepType { get { return this.stepType; } }
			public int TimsKey { get { return this.timsKey; } }
			public int Vlcode { get { return this.vlcode; } }
			public long PdtimeDatetime { get { return this.pdtimeDatetime; } }
			public double Value { get { return this.value; } }
			public int Status { get { return this.status; } }
			public int? ReadKey { get { return this.readKey; } }
			public int? ReptKey { get { return this.reptKey; } }
			public int? ReasKey { get { return this.reasKey; } }

			public TimeSeriesDbPoint(int stepType,
								int timsKey,
								int vlcode,
								long pdtimeDatetime,
								double value,
								int status,
								int? readKey,
								int? reptKey,
								int? reasKey)
			{
				this.stepType = stepType;
				this.timsKey = timsKey;
				this.vlcode = vlcode;
				this.pdtimeDatetime = pdtimeDatetime;
				this.value = value;
				this.status = status;
				this.readKey = readKey;
				this.reptKey = reptKey;
				this.reasKey = reasKey;
			}

			public int CompareTo(object obj)
			{
				if (obj is TimeSeriesDbPoint)
				{
					var timeSeries = (TimeSeriesDbPoint)obj;

					int stepComp = this.StepType.CompareTo(timeSeries.StepType);
					int timsComp = this.TimsKey.CompareTo(timeSeries.TimsKey);
					int codeComp = this.Vlcode.CompareTo(timeSeries.Vlcode);
					int timeComp = this.PdtimeDatetime.CompareTo(timeSeries.PdtimeDatetime);

					if (stepComp > 0)
						return 1;
					if (stepComp == 0 && timsComp > 0)
						return 1;
					if (stepComp == 0 && timsComp == 0 && codeComp > 0)
						return 1;
					if (stepComp == 0 && timsComp == 0 && codeComp == 0 && timeComp > 0)
						return 1;
					if (stepComp == 0 && timsComp == 0 && codeComp == 0 && timeComp == 0)
						return 0;

                    return -1;
				}

                throw new ArgumentException("object is not a TimeSeries");
			}
		}
        public static int StepTypeLookUp(TimeResolution resolution)
        {
            // Obsolete: returns steptypes rather than resolutions
            return resolution.Steptype;
        }
    }
}
