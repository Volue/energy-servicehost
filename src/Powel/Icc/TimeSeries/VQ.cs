#if !TSS_Availability
using Powel.Icc.Services.DataContracts;
#else
using Powel.Status;
#endif

namespace Powel.Icc.TimeSeries
{
	/// <summary>
	/// Summary description for VQ.
	/// </summary>
	internal struct VQ
	{
		double value;
		Quality quality;

		public VQ(double value)
		{
			this.value = value;
			this.quality = new Quality();
		}

		public VQ(double value, Quality quality)
		{
			this.value = value;
			this.quality = quality;
		}

		public double Value
		{
			get { return value; }
		}

		public Quality Quality
		{
			get { return quality; }
			set { quality = value; }
		}

		public override int GetHashCode()
		{
			return value.GetHashCode();
		}

		public override bool Equals(object o) 
		{
			VQ vq2 = (VQ)o;
			return value.Equals(vq2.value);
		}
	}
}
