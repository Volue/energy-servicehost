using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;
using Powel.Icc.Common;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Meter.
	/// </summary>
	public class Meter : Component
	{
		#region Fields

		protected static readonly int terminalComponentIdBit = BitVector32.CreateMask(stateBit);
		protected static readonly int registersBit = BitVector32.CreateMask(terminalComponentIdBit);

		private string terminalComponentId;
		private Register[] registers;

		#endregion

		#region Properties


		public string TerminalComponentId
		{
			get { return terminalComponentId; }
			set
			{
				terminalComponentId = value;
				fieldEditStatus[terminalComponentIdBit] = true;
			}
		}

		public bool TerminalComponentIdEdited
		{
			get { return fieldEditStatus[terminalComponentIdBit]; }
		}

		public Register[] Registers
		{
			get { return registers; }
			set
			{
				registers = value;
				fieldEditStatus[registersBit] = true;
			}
		}

		public bool RegistersEdited
		{
			get { return fieldEditStatus[registersBit]; }
		}

		public override ComponentType ComponentType
		{
			get { return ComponentType.METER; }
		}

		#endregion

		#region Constructors

		public Meter()
		{
		}

		public Meter(Meter meter)
			: this(meter.Key, meter.MasterName, meter.MeasKey, meter.Id, meter.ValidFromDate, meter.ValidToDate, meter.State,
				 meter.Location, meter.TerminalComponentId, meter.Registers)
		{
			base.ClearEdited();
		}

		internal Meter(
			int key,
			string master_name,
			int meas_key,
			string id,
			UtcTime validFromDate,
			UtcTime validToDate,
			ComponentState state,
			string location,
			string terminalComponentId,
			Register[] registers)
			: base(key, master_name, meas_key, id, validFromDate, validToDate, state, location)
		{
			this.terminalComponentId = terminalComponentId;

			//copy Registers
			if (registers != null)
			{
				Register[] registersCopy = new Register[registers.Length];
				int i = 0;
				foreach (Register register in registers)
				{
					registersCopy[i] = new Register(register);
					i++;
				}
				this.Registers = registersCopy;
			}
			base.ClearEdited();
		}

		public Meter(
			string id,
			UtcTime validFromDate,
			UtcTime validToDate,
			ComponentState state,
			string location,
			string terminalComponentId)
			: base(id, validFromDate, validToDate, state, location)
		{
			this.TerminalComponentId = terminalComponentId;
			base.ClearEdited();
		}

		#endregion

		public override bool Merge(Component meter)
		{
			bool bEdited = base.Merge(meter);
			if (meter is Meter)
			{
				Meter m = meter as Meter;
				if (m.TerminalComponentIdEdited && this.TerminalComponentId != m.TerminalComponentId)
				{
					bEdited = true;
					this.TerminalComponentId = m.TerminalComponentId;
				}
				if (m.RegistersEdited && this.Registers != m.Registers)
				{
					bEdited = true;
					this.Registers = m.Registers; //TODO Merge for register?
				}
			}
			return bEdited;
		}
	}
}
