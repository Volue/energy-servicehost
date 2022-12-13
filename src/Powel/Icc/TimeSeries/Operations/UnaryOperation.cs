using System;

namespace Powel.Icc.TimeSeries.Operations
{
	/// <summary>
	/// Summary description for UnaryOperation.
	/// </summary>
	public class UnaryOperation
	{
		public UnaryOperation()
		{
		}

		public TimeSeries Run(TimeSeries ts, IUnaryOperator op)
		{
			TimeSeries result = ts.Clone(false);

			foreach (Tvq tvq in ts)
				result.SetValue(op.Calculate(tvq));

			return result;
		}
	}

	public interface IUnaryOperator
	{
		Tvq Calculate(Tvq tvq);
	}

	public class ScaleOperator : IUnaryOperator
	{
		double factor;

		public ScaleOperator(double factor)
		{
			this.factor = factor;
		}

		public Tvq Calculate(Tvq tvq)
		{
			tvq.Value *= factor;
			return tvq;
		}
	}

	public class TranslateOperator : IUnaryOperator
	{
		double offset;

		public TranslateOperator(double offset)
		{
			this.offset = offset;
		}

		public Tvq Calculate(Tvq tvq)
		{
			tvq.Value += offset;
			return tvq;
		}
	}
}
