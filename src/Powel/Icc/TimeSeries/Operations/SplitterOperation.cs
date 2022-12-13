using System;
using System.Collections;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;

namespace Powel.Icc.TimeSeries.Operations
{
	/// <summary>
	/// Summary description for SplitterOperation.
	/// </summary>
	public class SplitterOperation
	{
		public SplitterOperation()
		{
		}

		public TimeSeries[] Run(TimeSeries source, ISplitterOperator op)
		{
			ArrayList destinations = new ArrayList();

			foreach (Tvq tvq in source)
			{
				string destinationName = op.FindDestinationName(tvq);	
				TimeSeries destination = FindTimeSeries(source, destinations, destinationName);
				destination.SetValue(tvq, false);
			}

			return (TimeSeries[])destinations.ToArray(typeof(TimeSeries));
		}

		TimeSeries FindTimeSeries(TimeSeries source, ArrayList timeSeriesList, string name)
		{
			foreach (TimeSeries ts in timeSeriesList)
				if (ts.Name == name)
					return ts;

			// If we reach this point, the time series is not yet in the list
			TimeSeries newTimeSeries = source.Clone(false);
			newTimeSeries.Name = name;
			timeSeriesList.Add(newTimeSeries);
			return newTimeSeries;
		}
	}

	public interface ISplitterOperator
	{
		string FindDestinationName(Tvq tvq);
	}

	public class ExternalReferenceSplitterOperator : ISplitterOperator
	{
		ExternalReference externalReference;

		public ExternalReferenceSplitterOperator(ExternalReference externalReference)
		{
			this.externalReference = externalReference;
		}

		public string FindDestinationName(Tvq tvq)
		{
			if (externalReference.UseMapping(tvq.Time))
				return externalReference.InternalName;
			else
				return null;
		}
	}

	public class NewerDataSplitterOperator : ISplitterOperator
	{
		TimeSeries ts;
		TimeSeriesAge age;
		UtcTime timeStamp;

		public NewerDataSplitterOperator(TimeSeries ts, TimeSeriesAge age, UtcTime timeStamp)
		{
			this.ts = ts;
			this.age = age;
			this.timeStamp = timeStamp;
		}

		public string FindDestinationName(Tvq tvq)
		{
            return age.IsOlder(tvq.Time, timeStamp) ? null : ts.Name;
		}
	}
}
