using System.Collections;
using System.Collections.Specialized;
//using System.Runtime.CompilerServices;
using Powel.Icc.Services.Time;
using Powel.Icc.Interop;
//using Powel.Icc.Interop.Monitoring;

/*[assembly: InternalsVisibleToAttribute("Powel.Icc.Data.Test,PublicKey=0024000004800000940000000602000000240000525341310004000001000100992d71dcb54c55" +
    "ef9c6b49c0502b6fb8bf140bd0f0d234ced22b1e662c22cdab1c6c843ab0ebc2b10170921a37c9" +
    "e4ed5ebeb521b13e27b5b9ed1515cb5f67b30d16312719bb3750917a0a13656e77f0b69fc5f5e6" +
    "620298efd0bc6ffe49a546cd9050c5dd3910f10208de193e9cc2e58abb91030541c6490ee4814b" +
    "ee4fcfc9")]   // To make internal methods available to Test project.
*/
namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Component.
	/// </summary>
	public class Component //: IMonitorable
	{
		/// <summary>
		/// If a field is added, changed or removed, remember to update
		/// - Merge() method
		/// - System.Object overriden methods in the System.Object region below.
		/// - properties region, if applicable
		/// </summary>
		#region Fields

		protected BitVector32 fieldEditStatus = new BitVector32(0);

		protected static readonly int idBit = BitVector32.CreateMask();
		protected static readonly int locationBit = BitVector32.CreateMask(idBit);
		protected static readonly int masterBit = BitVector32.CreateMask(locationBit);
		protected static readonly int validFromDateBit = BitVector32.CreateMask(masterBit);
		protected static readonly int validToDateBit = BitVector32.CreateMask(validFromDateBit);
		protected static readonly int stateBit = BitVector32.CreateMask(validToDateBit); // must be last in list!

		protected int m_key;
		protected int m_key2;
		protected string m_master_name;
		protected int m_object_type;
		protected int m_meas_key;
		protected string m_id;       //instr_no
		protected string m_location;
		protected internal UtcTime m_validFromDate;
		protected internal UtcTime m_validToDate;
		protected ComponentState m_state;
		protected Hashtable m_attributesInt;
		protected Hashtable m_attributesFloat;
		protected Hashtable m_attributesString;

		protected TimePeriod m_monitorPeriod;

		#endregion

		#region Properties

		public int Key
		{
			get { return m_key; }
			set { m_key = value; }
		}

		public int MeasKey
		{
			get { return m_meas_key; }
			set { m_meas_key = value; }

		}

		public string Id
		{
			get { return m_id; }
			set
			{
				m_id = value;
				fieldEditStatus[idBit] = true;
			}
		}

		public bool IdEdited
		{
			get { return fieldEditStatus[idBit]; }
		}

		public string MasterName
		{
			get { return m_master_name; }
			set 
			{
				m_master_name = value;
				fieldEditStatus[masterBit] = true;
			}
		}

		public bool MasterNameEdited
		{
			get { return fieldEditStatus[masterBit]; }
		}

		/// <summary>
		/// ValidToDate are not used during storage, but are set when reading from database, hence internal set 
		/// </summary>
		/// Removed internal because tests are now external in Powel.Icc.Data.Test
		public UtcTime ValidFromDate
		{
			get { return m_validFromDate; }
			/*internal*/ set
			{
				m_validFromDate = value;
				fieldEditStatus[validFromDateBit] = true;
			}
		}

		public bool ValidFromDateEdited
		{
			get { return fieldEditStatus[validFromDateBit]; }
		}

		/// <summary>
		/// ValidToDate are not used during storage, but are set when reading from database, hence internal set 
		/// </summary>
		public UtcTime ValidToDate
		{
			get { return m_validToDate; }
			internal set
			{
				m_validToDate = value;
				fieldEditStatus[validToDateBit] = true;
			}
		}

		public bool ValidToDateEdited
		{
			get { return fieldEditStatus[validToDateBit]; }
		}

		public ComponentState State
		{
			get { return m_state; }
			set
			{
				m_state = value;
				fieldEditStatus[stateBit] = true;
			}
		}

		public bool StateEdited
		{
			get { return fieldEditStatus[stateBit]; }
		}

		public string Location
		{
			get { return m_location; }
			set
			{
				m_location = value;
				fieldEditStatus[locationBit] = true;
			}
		}

		public bool LocationEdited
		{
			get { return fieldEditStatus[locationBit]; }
		}

		public virtual ComponentType ComponentType
		{
			get { return ComponentType.METER; }
		}

		public Hashtable AttributesInt
		{
			get { return m_attributesInt; }
		}

		public Hashtable AttributesFloat
		{
			get { return m_attributesFloat; }
		}

		public Hashtable AttributesString
		{
			get { return m_attributesString; }
		}

		public const int FES_FIRST_AVAILABLE = 5; // first unused bit in fieldEditStatus

		#region IMonitorable

		public int MonitorKey1
		{
			get { return m_key; }
			set { m_key = value; }
		}

		public int MonitorKey2
		{
			get { return m_key2; }
			set { m_key2 = value; }
		}

		public int ObjectType
		{
			get
			{
				return (int)ObjectTypes.Component;
			}
		}

		public TimePeriod MonitorPeriod
		{
			get { return m_monitorPeriod; }
			set { m_monitorPeriod = value; }
		}

		#endregion

		#endregion

		#region Constructors

		public Component()
		{
			this.m_attributesInt = new Hashtable();
			this.m_attributesFloat = new Hashtable();
			this.m_attributesString = new Hashtable();
		}

		public Component(string id, UtcTime validFromDate, UtcTime validToDate, ComponentState state, string location)
			: this()
		{
			this.Id = id;
			this.ValidFromDate = validFromDate;
			this.ValidToDate = validToDate;
			this.State = state;
			this.Location = location;
			ClearEdited();
		}

		internal Component(int key, int meas_key, string id, UtcTime validFromDate, UtcTime validToDate, ComponentState state, string location)
			: this(id, validFromDate, validToDate, state, location)
		{
			this.Key = key;
			this.MeasKey = meas_key;
			ClearEdited();
		}

		internal Component(int key, string master_name, int meas_key, string id, UtcTime validFromDate, UtcTime validToDate, ComponentState state, string location)
			: this(key, meas_key, id, validFromDate, validToDate, state, location)
		{
			this.MasterName = master_name;
			ClearEdited();
		}

		#endregion

		#region Methods

		/// <summary>
		/// Merges all non-key attributes of a component
		/// </summary>
		/// <param name="component"></param>
		public virtual bool Merge(Component component)
		{
			bool bEdited = false;
			if (component.IdEdited && this.Id != component.Id)
			{
				bEdited = true;
				this.Id = component.Id;
			}
			if (component.LocationEdited && this.Location != component.Location)
			{
				bEdited = true;
				this.Location = component.Location;
			}
			if (component.StateEdited && this.State != component.State)
			{
				bEdited = true;
				this.State = component.State;
			}
			if (component.MasterNameEdited && this.MasterName != component.MasterName)
			{
				bEdited = true;
				this.MasterName = component.MasterName;
			}
			if (component.ValidFromDateEdited && this.ValidFromDate != component.ValidFromDate)
			{
				bEdited = true;
				this.ValidFromDate = component.ValidFromDate;
			}
			if (component.ValidToDateEdited && this.ValidToDate != component.ValidToDate)
			{
				bEdited = true;
				this.ValidToDate = component.ValidToDate;
			}

			foreach(DictionaryEntry compattr in component.AttributesString)
			{
				if (this.AttributesString.ContainsKey(compattr.Key))
				{
					if (this.AttributesString[compattr.Key].ToString() != compattr.Value.ToString())
						bEdited = true;
						this.AttributesString[compattr.Key] = compattr.Value;
				}
				else
				{
					bEdited = true;
					this.m_attributesString.Add(compattr.Key, compattr.Value);
				}
			}

			foreach (DictionaryEntry compattr in component.AttributesInt)
			{
				if (this.AttributesInt.ContainsKey(compattr.Key))
				{
					if (this.AttributesInt[compattr.Key].ToString() != compattr.Value.ToString())
						bEdited = true;
						this.AttributesInt[compattr.Key] = compattr.Value;
				}
				else
				{
					bEdited = true;
					this.AttributesInt.Add(compattr.Key, compattr.Value);
				}
			}
		   
			return bEdited;
		}

		protected void ClearEdited()
		{
			fieldEditStatus = new BitVector32(0);
		}


		public int GetAttributeInt(string attribute)
		{
			if (m_attributesInt.ContainsKey(attribute))
				return (int)m_attributesInt[attribute];
			return 0;
		}

		public void SetAttributeInt(string attribute, int valueInt)
		{
			if (m_attributesInt.ContainsKey(attribute))
				m_attributesInt[attribute] = valueInt;
			else
				m_attributesInt.Add(attribute, valueInt);
		}

		public float GetAttributeFloat(string attribute)
		{
			if (m_attributesFloat.ContainsKey(attribute))
				return (float)m_attributesFloat[attribute];
			return 0F;
		}

		internal void SetAttributeFloat(string attribute, float valueFloat)
		{
			if (m_attributesFloat.ContainsKey(attribute))
				m_attributesFloat[attribute] = valueFloat;
			else
				m_attributesFloat.Add(attribute, valueFloat);
		}

		public string GetAttributeString(string attribute)
		{
			if (m_attributesString.ContainsKey(attribute))
				return m_attributesString[attribute].ToString();
			return null;
		}

		internal void SetAttributeString(string attribute, string valueString)
		{
			if (m_attributesString.ContainsKey(attribute))
				m_attributesString[attribute] = valueString;
			else
				m_attributesString.Add(attribute, valueString);
		}

		#endregion

		#region System.Object

		/// <summary>
		/// Prints all public properties of a Component, excluded any attributes.
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			string separator = ";";
			return "Id: " + this.Id + separator +
				   "Key: " + this.Key + separator +
				   "State: " + this.State + separator +
				   "Master: " + this.MasterName + separator +
				   "ValidFromDate: " + this.ValidFromDate + separator +
				   "ValidToDate: " + this.ValidToDate + separator +
				   "Location: " + this.Location;
		}

		/// <summary>
		/// Calculates a unique hash code for a component object, excluded any attributes. I.e. two components may produce the same hash code if they have equal properties, but unequal attributes.
		/// </summary>
		/// <returns></returns>
		public override int GetHashCode()
		{
			return (this.ToString() + m_meas_key.ToString()).GetHashCode();
		}

		/// <summary>
		/// Two components are treated as equal if all properties (except the attributes) are equal.
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public override bool Equals(object obj)
		{
			Component c = obj as Component;
			if (c == null)
				return false;
			if (this.Key == c.Key &&
				this.Id == c.Id &&
				this.State == c.State &&
				this.MasterName == c.MasterName &&
				this.ValidFromDate == c.ValidFromDate &&
				this.ValidToDate == c.ValidToDate &&
				this.Location == c.Location &&
				this.MeasKey == c.MeasKey)
				return true;
			return false;
		}


		#endregion
	}

	public enum ComponentState
	{
		INSTR_STATE_UNKNOWN = -1,
		INSTR_STATE_ACTIVE = 1,
		INSTR_STATE_INSTOCK = 2,
		INSTR_STATE_CONTROL = 3,
		INSTR_STATE_REVISION = 4,
		INSTR_STATE_DESTROYED = 5,
		INSTR_STATE_OUTOFSTOCK = 6,
		INSTR_STATE_TAKENDOWN = 7,
		INSTR_STATE_SPOT_CHECK = 8
	}

	public enum ComponentType
	{
		UNKNOWN,
		METER,
		TERMINAL,
		TRANSFORMER_VOLTAGE,
		TRANSFORMER_CURRENT
	}
}
