using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Terminal.
	/// </summary>
	public class Terminal : Component
	{
		public override ComponentType ComponentType
		{
			get { return ComponentType.TERMINAL; }
		}

		public Terminal()
			: base()
		{
		}

		public Terminal(string id, UtcTime validFrom, UtcTime validTo, ComponentState state, string location)
			: base(id, validFrom, validTo, state, location)
		{
		}

		internal Terminal(int key, string master_name, int meas_key, string id, UtcTime validFromTime, UtcTime validToTime, ComponentState state, string location)
			: base(key, master_name, meas_key, id, validFromTime, validToTime, state, location)
		{
			base.ClearEdited();
		}

		public override bool Merge(Component terminal)
		{
			bool bEdited = base.Merge(terminal);
			if (terminal is Terminal)
			{
				//no special terminal attributes to merge
			}
			return bEdited;
		}
	}
}
