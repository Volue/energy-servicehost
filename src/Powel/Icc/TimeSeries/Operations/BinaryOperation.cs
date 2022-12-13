using System;

namespace Powel.Icc.TimeSeries.Operations
{
	/// <summary>
	/// Summary description for BinaryOperation.
	/// </summary>
	public class BinaryOperation
	{
		public BinaryOperation()
		{
		}

		public TimeSeries Run(TimeSeries ts, IBinaryOperator op)
		{
			TimeSeries result = ts.Clone(false);

			Tvq tvqRef;
			foreach (Tvq tvq in ts)
			{
				tvqRef = op.Calculate(tvq);
				result.SetValue(tvqRef, tvqRef.Quality.IsModified);
			}
		

			return result;
		}
	}


	public interface IBinaryOperator
	{
		Tvq Calculate(Tvq tvq);
	}

	public class TruncateOperator : IBinaryOperator
	{
		int digits;
		int decimals;

		public TruncateOperator(int digits, int decimals)
		{
			this.digits = digits;
			this.decimals = decimals;
		}

		public Tvq Calculate(Tvq tvq)
		{
			if( digits > 0)
			{
				double limit = Math.Pow(10.0, digits);
				if( tvq.Value >= limit)
				{
					int removePart = (int)tvq.Value / (int) limit;
					int remove = removePart * (int) limit;
					tvq.Value = tvq.Value - remove;
				}
			}
			if( decimals > 0)
			{
				double newValue = Math.Round(tvq.Value, decimals);
				if( newValue != tvq.Value)
					tvq.Value = newValue;
			}
			return tvq;
		}
	}
}
