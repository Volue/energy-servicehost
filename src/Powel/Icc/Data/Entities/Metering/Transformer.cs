using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;
using Powel.Icc.Common;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Transformer.
	/// </summary>
	public class Transformer : Component
	{
		#region Fields

		private static readonly int trafoPrimaryBit = BitVector32.CreateMask(stateBit);
		private static readonly int trafoSecondaryBit = BitVector32.CreateMask(trafoPrimaryBit);
		private static readonly int trafoTypeBit = BitVector32.CreateMask(trafoSecondaryBit);

		//private int trafoPrimary;
		//private int trafoSecondary;
		private TransformerType trafoType;

	    #endregion

		#region Properties

		public int TrafoPrimary
		{
			get { return GetAttributeInt("instr_primary"); }
			set
			{
				SetAttributeInt("instr_primary", value);
				fieldEditStatus[trafoPrimaryBit] = true;
			}
		}

		public bool TrafoPrimaryEdited
		{
			get { return fieldEditStatus[trafoPrimaryBit]; }
		}

		public int TrafoSecondary
		{
			get { return GetAttributeInt("instr_secondary"); }
			set
			{
				SetAttributeInt("instr_secondary", value);
				fieldEditStatus[trafoSecondaryBit] = true;
			}
		}

		public bool TrafoSecondaryEdited
		{
			get { return fieldEditStatus[trafoSecondaryBit]; }
		}

		public int Phase { get; set; }

		public TransformerType TrafoType
		{
			get { return this.trafoType; }
			set
			{
				this.trafoType = value;
				fieldEditStatus[trafoTypeBit] = true;
			}
		}

		public bool TrafoTypeEdited
		{
			get { return fieldEditStatus[trafoTypeBit]; }
		}


		public override ComponentType ComponentType
		{
			get
			{
				if (this.TrafoType == TransformerType.CURRENT)
					return ComponentType.TRANSFORMER_CURRENT;
				else if (this.TrafoType == TransformerType.VOLTAGE)
					return ComponentType.TRANSFORMER_VOLTAGE;
				else
					return ComponentType.UNKNOWN;
			}
		}

		#endregion

		#region Constructors

		public Transformer()
		{
		    Phase = 0;
		}

	    public Transformer(Transformer transformer)
			: this(transformer.Key, transformer.MasterName, transformer.MeasKey, transformer.Id,
				 transformer.ValidFromDate, transformer.ValidToDate, transformer.State, transformer.Location,
				 transformer.TrafoPrimary, transformer.TrafoSecondary, transformer.TrafoType)
		{
			base.ClearEdited();
		}

		internal Transformer(
			int key,
			string master_name,
			int meas_key,
			string id,
			UtcTime validFrom,
			UtcTime validTo,
			ComponentState state,
			string location,
			int trafoPrimary,
			int trafoSecondary,
			TransformerType trafoType)
			: base(key, master_name, meas_key, id, validFrom, validTo, state, location)
		{
		    Phase = 0;
		    this.TrafoPrimary = trafoPrimary;
			this.TrafoSecondary = trafoSecondary;
			this.TrafoType = trafoType;
			base.ClearEdited();
		}

		public Transformer(
			string id,
			UtcTime validFrom,
			UtcTime validTo,
			ComponentState state,
			string location,
			int trafoPrimary,
			int trafoSecondary,
			TransformerType trafoType)
			: base(id, validFrom, validTo, state, location)
		{
		    Phase = 0;
		    this.TrafoPrimary = trafoPrimary;
			this.TrafoSecondary = trafoSecondary;
			this.TrafoType = trafoType;
			base.ClearEdited();
		}

		#endregion

		public override bool Merge(Component transformer)
		{
			bool bEdited = base.Merge(transformer);
			if (transformer is Transformer)
			{
				Transformer t = transformer as Transformer;
				if (t.TrafoPrimaryEdited && this.TrafoPrimary != t.TrafoPrimary)
				{
					bEdited = true;
					this.TrafoPrimary = t.TrafoPrimary;
				}
				if (t.TrafoSecondaryEdited && this.TrafoSecondary != t.TrafoSecondary)
				{
					bEdited = true;
					this.TrafoSecondary = t.TrafoSecondary;
				}
				if (t.TrafoTypeEdited && this.TrafoType != t.TrafoType)
				{
					bEdited = true;
					this.TrafoType = t.TrafoType;
				}
			}
			return bEdited;
		}
	}

	public enum TransformerType
	{
		CURRENT,
		VOLTAGE
	}
}
