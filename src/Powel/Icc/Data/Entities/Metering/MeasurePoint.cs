using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for MeasurePoint.
	/// </summary>
	public class MeasurePoint
	{
		/// <summary>
		/// If a field is added, changed or removed, remember to update
		/// - Merge() method
		/// - System.Object overriden methods in the System.Object region below.
		/// - properties region, if applicable
		/// </summary>
		#region Fields

		protected BitVector32 fieldEditStatus = new BitVector32(0);
		private static int seqnoBit = BitVector32.CreateMask();
		private static int idBit = BitVector32.CreateMask(seqnoBit);
		private static int secondaryIdBit = BitVector32.CreateMask(idBit);
		private static int netAreaBit = BitVector32.CreateMask(secondaryIdBit);
		private static int validFromBit = BitVector32.CreateMask(netAreaBit);
		private static int validToBit = BitVector32.CreateMask(validFromBit);
		private static int spatialReferenceSystemBit = BitVector32.CreateMask(validToBit);
		private static int xCoordinateBit = BitVector32.CreateMask(spatialReferenceSystemBit);
		private static int yCoordinateBit = BitVector32.CreateMask(xCoordinateBit);
		private static int stationIdBit = BitVector32.CreateMask(yCoordinateBit);
		private static int nPhasesBit = BitVector32.CreateMask(stationIdBit);
		private static int supplyVoltageBit = BitVector32.CreateMask(nPhasesBit);
		private static int fuseSizeBit = BitVector32.CreateMask(supplyVoltageBit);
		private static int subscribedPowerBit = BitVector32.CreateMask(fuseSizeBit);
		private static int orgUnitBit = BitVector32.CreateMask(subscribedPowerBit);
		private static int gridOwnerBit = BitVector32.CreateMask(orgUnitBit);
		private static int loadProfileBit = BitVector32.CreateMask(gridOwnerBit);
		private static int metersBit = BitVector32.CreateMask(loadProfileBit);
		private static int transformersBit = BitVector32.CreateMask(metersBit);
		private static int descriptionBit = BitVector32.CreateMask(transformersBit);
		private static int yearlyVolumeBit = BitVector32.CreateMask(descriptionBit);
		private static int locationBit = BitVector32.CreateMask(yearlyVolumeBit);
		private static int supplierBit = BitVector32.CreateMask(locationBit);
		private static int terminalBit = BitVector32.CreateMask(supplierBit);
		private static int centralNetPointCodeBit = BitVector32.CreateMask(terminalBit);
		private static int debtorTypeBit = BitVector32.CreateMask(centralNetPointCodeBit);
	    private static int stateBit = BitVector32.CreateMask(debtorTypeBit);

		private int key;
		private int seqno;
		private int inslKey;
		private string id;
		private string secondaryId;
		private string description;
		private string netAreaCode;
		internal UtcTime validFromDate;
		internal UtcTime validToDate;
		private int spatialReferenceSystem;
		private Decimal xCoordinate;
		private Decimal yCoordinate;
		private string stationId;
		private int numberOfPhases;
		private string supplyVoltage;
		private string fuseSize;
		private int subscribedPower;
		private string orgUnit;
		private string gridOwner;
		private string loadProfile;
		private int yearlyVolume;
		private string location;
		private string supplier;
		private Agreement[] agreements;
		private Meter[] meters;
		private Transformer[] transformers;
		private Terminal terminal;
		private string centralNetPointCode;
		private DebtorTypes debtorType;
	    private StateTypes state;
		private int valueTypeKey ;
		private string valueTypeValfTime;
		private int timeSeriesKey;
		private int tsinKey;
		public enum DebtorTypes
		{
			Unspecified = 0,
			Hourly = 1,
			MeterReadings = 2
		}
        public enum StateTypes
        {
            Unspecified = -1,
            Closed = 0,
            Active = 1,
            NoCustomer = 2
        }

		#endregion

		#region Properties

		public int Key
		{
			get { return this.key; }
			set { key = value; }
		}

        /// Removed internal because tests are now external in Powel.Icc.Data.Test
        /*internal*/ public
        int Seqno
		{
			get { return this.seqno; }
			set
			{
				this.seqno = value;
				fieldEditStatus[seqnoBit] = true;
			}
		}

		internal bool SeqnoEdited
		{
			get { return fieldEditStatus[seqnoBit]; }
		}

		public int InstallationKey
		{
			get { return inslKey; }
			set { this.inslKey = value; }
		}

		public string Id
		{
			get { return this.id; }
			set
			{
				this.id = value;
				fieldEditStatus[idBit] = true;
			}
		}

		public bool IdEdited
		{
			get { return fieldEditStatus[idBit]; }
		}

		/// <summary>
		/// A valid netAreaCode must exist upon storage of this measure point field
		/// </summary>
		public string NetAreaCode
		{
			get { return this.netAreaCode; }
			set
			{
				this.netAreaCode = value;
				fieldEditStatus[netAreaBit] = true;
			}
		}

		public bool NetAreaCodeEdited
		{
			get { return fieldEditStatus[netAreaBit]; }
		}

		public string Description
		{
			get { return this.description; }
			set
			{
				this.description = value;
				fieldEditStatus[descriptionBit] = true;
			}
		}

		public bool DescriptionEdited
		{
			get { return fieldEditStatus[descriptionBit]; }
		}

		public UtcTime ValidFromDate
		{
			get { return this.validFromDate; }
			set
			{
				this.validFromDate = value;
				fieldEditStatus[validFromBit] = true;
			}
		}

		public bool ValidFromDateEdited
		{
			get { return fieldEditStatus[validFromBit]; }
		}

		public UtcTime ValidToDate
		{
			get { return this.validToDate; }
			set
			{
				this.validToDate = value;
				fieldEditStatus[validToBit] = true;
			}
		}

		public bool ValidToDateEdited
		{
			get { return fieldEditStatus[validToBit]; }
		}

		public string SecondaryId
		{
			get { return this.secondaryId; }
			set
			{
				this.secondaryId = value;
				fieldEditStatus[secondaryIdBit] = true;
			}
		}

		public bool SecondaryIdEdited
		{
			get { return fieldEditStatus[secondaryIdBit]; }
		}

		public int SpatialReferenceSystem
		{
			get { return this.spatialReferenceSystem; }
			set {
				this.spatialReferenceSystem = value;
				fieldEditStatus[spatialReferenceSystemBit] = true;
			}
		}

		public bool SpatialReferenceSystemEdited
		{
			get { return fieldEditStatus[spatialReferenceSystemBit]; }
		}

		public Decimal XCoordinate
		{
			get { return this.xCoordinate; }
			set {
				this.xCoordinate = value;
				fieldEditStatus[xCoordinateBit] = true;
			}
		}

		public bool XCoordinateEdited
		{
			get { return fieldEditStatus[xCoordinateBit]; }
		}
	
	  public Decimal YCoordinate
	  {
			get { return this.yCoordinate; }
			set
			{
				this.yCoordinate = value;
				fieldEditStatus[yCoordinateBit] = true;
			}
		}

		public bool YCoordinateEdited
		{
			get { return fieldEditStatus[yCoordinateBit]; }
		}

		public string StationId
		{
			get { return this.stationId; }
			set
			{
				this.stationId = value;
				fieldEditStatus[stationIdBit] = true;
			}
		}

		public bool StationIdEdited
		{
			get { return fieldEditStatus[stationIdBit]; }
		}

		public int NumberOfPhases
		{
			get { return this.numberOfPhases; }
			set
			{
				this.numberOfPhases = value;
				fieldEditStatus[nPhasesBit] = true;
			}
		}

		public bool NumberOfPhasesEdited
		{
			get { return fieldEditStatus[nPhasesBit]; }
		}

		public string SupplyVoltage
		{
			get { return this.supplyVoltage; }
			set
			{
				this.supplyVoltage = value;
				fieldEditStatus[supplyVoltageBit] = true;
			}
		}

		public bool SupplyVoltageEdited
		{
			get { return fieldEditStatus[supplyVoltageBit]; }
		}

		public string FuseSize
		{
			get { return this.fuseSize; }
			set
			{
				this.fuseSize = value;
				fieldEditStatus[fuseSizeBit] = true;
			}
		}

		public bool FuseSizeEdited
		{
			get { return fieldEditStatus[fuseSizeBit]; }
		}

		/// <summary>
		/// Not in use! SubScribedPower is always set to 0;
		/// </summary>
		public int SubscribedPower
		{
			get { return this.subscribedPower; }
			set
			{
				//TODO set to value and true, respectively, if field is to be used
				this.subscribedPower = 0; 
				fieldEditStatus[subscribedPowerBit] = false;
			}
		}

		public bool SubscribedPowerEdited
		{
			get { return fieldEditStatus[subscribedPowerBit]; }
		}

		public string OrgUnit
		{
			get { return this.orgUnit; }
			set
			{
				this.orgUnit = value;
				fieldEditStatus[orgUnitBit] = true;
			}
		}

		public bool OrgUnitEdited
		{
			get { return fieldEditStatus[orgUnitBit]; }
		}

		public string GridOwner
		{
			get { return this.gridOwner; }
			set
			{
				this.gridOwner = value;
				fieldEditStatus[gridOwnerBit] = true;
			}
		}

		public bool GridOwnerEdited
		{
			get { return fieldEditStatus[gridOwnerBit]; }
		}

		public string LoadProfile
		{
			get { return this.loadProfile; }
			set
			{
				this.loadProfile = value;
				fieldEditStatus[loadProfileBit] = true;
			}
		}

		public bool LoadProfileEdited
		{
			get { return fieldEditStatus[loadProfileBit]; }
		}

		public int YearlyVolume
		{
			get { return this.yearlyVolume; }
			set
			{
				this.yearlyVolume = value;
				fieldEditStatus[yearlyVolumeBit] = true;
			}
		}

		public bool YearlyVolumeEdited
		{
			get { return fieldEditStatus[yearlyVolumeBit]; }
		}

		public Agreement[] Agreements
		{
			get { return agreements; }
			set
			{
				agreements = value;
				//fieldEditStatus[] = true;
			}
		}

		public string Location
		{
			get { return this.location; }
			set
			{
				this.location = value;
				fieldEditStatus[locationBit] = true;
			}
		}

		public bool LocationEdited
		{
			get { return fieldEditStatus[locationBit]; }
		}

		public string Supplier
		{
			get { return this.supplier; }
			set
			{
				this.supplier = value;
				fieldEditStatus[supplierBit] = true;
			}
		}

		public bool SupplierEdited
		{
			get { return fieldEditStatus[supplierBit]; }
		}

		public bool AgreementsEdited
		{
			get { return false/*fieldEditStatus[17]*/; }
		}

		public Meter[] Meters
		{
			get { return meters; }
			set
			{
				meters = value;
				fieldEditStatus[metersBit] = true;
			}
		}

		public Terminal Terminal
		{
			get { return terminal; }
			set
			{
				terminal = value;
				fieldEditStatus[terminalBit] = true;
			}
		}

		public bool MetersEdited
		{
			get { return fieldEditStatus[metersBit]; }
		}

		public Transformer[] Transformers
		{
			get { return transformers; }
			set
			{
				transformers = value;
				fieldEditStatus[transformersBit] = true;
			}
		}

		public bool TransformersEdited
		{
			get { return fieldEditStatus[transformersBit]; }
		}

		public string CentralNetPointCode
		{
			get { return this.centralNetPointCode; }
			set
			{
				this.centralNetPointCode = value;
				fieldEditStatus[centralNetPointCodeBit] = true;
			}
		}

		public bool CentralNetPointCodeEdited
		{
			get { return fieldEditStatus[centralNetPointCodeBit]; }
		}

		public DebtorTypes DebtorType
		{
			get { return this.debtorType; }
			set
			{
				this.debtorType = value;
				fieldEditStatus[debtorTypeBit] = true;
			}
		}

		public bool DebtorTypeEdited
		{
			get { return fieldEditStatus[debtorTypeBit]; }
		}

	    public StateTypes State
	    {
	        get { return this.state; }
            set { 
                this.state = value;
                fieldEditStatus[stateBit] = true;
            }
	    }
	    public bool StateEdited
	    {
            get { return fieldEditStatus[stateBit]; }
	    }

		public int ValueTypeKey
		{
			get { return valueTypeKey; }
			set { valueTypeKey = value; }
		}

		public string ValueTypeValfTime
		{
			get { return valueTypeValfTime; }
			set { valueTypeValfTime = value; }
		}

		public int TimeSeriesKey
		{
			get { return timeSeriesKey; }
			set { timeSeriesKey = value; }
		}

		public int TsinKey
		{
			get { return tsinKey; }
			set { tsinKey = value; }
		}

		#endregion

		#region Constructors

		public MeasurePoint()
		{
		}

		internal MeasurePoint(MeasurePoint measurePoint)
		{
			this.key = measurePoint.Key; // read-only property
			this.seqno = measurePoint.Seqno;
			this.inslKey = measurePoint.inslKey;
			this.Id = measurePoint.Id;
			this.SecondaryId = measurePoint.SecondaryId;
			this.InstallationKey = measurePoint.InstallationKey;
			this.NetAreaCode = measurePoint.NetAreaCode;
			this.Description = measurePoint.Description;
			this.ValidFromDate = measurePoint.ValidFromDate;
			this.ValidToDate = measurePoint.ValidToDate;
			this.XCoordinate = measurePoint.XCoordinate;
			this.YCoordinate = measurePoint.YCoordinate;
			this.StationId = measurePoint.StationId;
			this.NumberOfPhases = measurePoint.NumberOfPhases;
			this.SupplyVoltage = measurePoint.SupplyVoltage;
			this.FuseSize = measurePoint.FuseSize;
			this.YearlyVolume = measurePoint.YearlyVolume;
			this.SubscribedPower = measurePoint.SubscribedPower;
			this.OrgUnit = measurePoint.OrgUnit;
			this.GridOwner = measurePoint.GridOwner;
			this.LoadProfile = measurePoint.LoadProfile;
			this.Location = measurePoint.Location;
			this.Supplier = measurePoint.Supplier;
			this.CentralNetPointCode = measurePoint.CentralNetPointCode;
			this.DebtorType = measurePoint.DebtorType;
		    this.State = measurePoint.State;

			int i;
			//copy Agreements
			if (measurePoint.Agreements != null)
			{
				Agreement[] agreements = new Agreement[measurePoint.Agreements.Length];
				i = 0;
				foreach (Agreement agreement in measurePoint.Agreements)
				{
					agreements[i] = new Agreement(agreement);
					i++;
				}
				this.Agreements = agreements;
			}

			//copy Meters
			if (measurePoint.Meters != null)
			{
				Meter[] meters = new Meter[measurePoint.Meters.Length];
				i = 0;
				foreach (Meter meter in measurePoint.Meters)
				{
					meters[i] = new Meter(meter);
					i++;
				}
				this.Meters = meters;
			}

			//copy Transformers
			if (measurePoint.Transformers != null)
			{
				Transformer[] transformers = new Transformer[measurePoint.Transformers.Length];
				i = 0;
				foreach (Transformer transformer in measurePoint.Transformers)
				{
					transformers[i] = new Transformer(transformer);
					i++;
				}
				this.Transformers = transformers;
			}
			ClearEdited();
		}

		internal MeasurePoint(
			int key,
			int seqno,
			int installationKey,
			string id,
			string secondaryId,
			string netAreaCode,
			string description,
			UtcTime validFromDate,
			UtcTime validToDate,
			Decimal xCoordinate,
			Decimal yCoordinate,
			string stationID,
			int numberOfPhases,
			string supplyVoltage,
			string fuseSize,
			int yearlyVolume,
			int subscribedPower,
			string orgUnit,
			string gridOwner,
			string loadProfile,
			string location,
			string supplier,
			string centraNetPointCode,
			int debtorType,
            int state)
			: this(id, secondaryId, netAreaCode, description, validFromDate, validToDate, xCoordinate, yCoordinate,
			stationID, numberOfPhases, supplyVoltage, fuseSize, yearlyVolume, subscribedPower, orgUnit, gridOwner, 
            loadProfile, location, supplier, centraNetPointCode, debtorType, state)
		{
			this.key = key;		// read-only property
			this.seqno = seqno; // internal property
			this.inslKey = installationKey; //internal property
			ClearEdited();
		}

		public MeasurePoint(
			string id,
			string secondaryId,
			string netAreaCode,
			string description,
			UtcTime validFromDate,
			UtcTime validToDate,
			Decimal xCoordinate,
			Decimal yCoordinate,
			string stationID,
			int numberOfPhases,
			string supplyVoltage,
			string fuseSize,
			int yearlyVolume,
			int subscribedPower,
			string orgUnit,
			string gridOwner,
			string loadProfile,
			string location,
			string supplier,
			string centraNetPointCode,
			int debtorType,
            int state)
		{
			this.Id = id;
			this.SecondaryId = secondaryId;
			this.NetAreaCode = netAreaCode;
			this.Description = description;
			this.ValidFromDate = validFromDate;
			this.ValidToDate = validToDate;
			this.XCoordinate = xCoordinate;
			this.YCoordinate = yCoordinate;
			this.StationId = stationID;
			this.NumberOfPhases = numberOfPhases;
			this.SupplyVoltage = supplyVoltage;
			this.FuseSize = fuseSize;
			this.YearlyVolume = yearlyVolume;
			this.SubscribedPower = subscribedPower;
			this.OrgUnit = orgUnit;
			this.GridOwner = gridOwner;
			this.LoadProfile = loadProfile;
			this.Location = location;
			this.Supplier = supplier;
			this.CentralNetPointCode = centraNetPointCode;

			switch (debtorType)
			{
				case Int32.MinValue:
					this.DebtorType = DebtorTypes.Unspecified;
					break;
				case 0:
					this.DebtorType = DebtorTypes.Hourly;
					break;
				case 1:
					this.DebtorType = DebtorTypes.MeterReadings;
					break;
			}
            switch (state)
            {
                case Int32.MinValue:
                case -1:
                    this.State = StateTypes.Unspecified;
                    break;
                case 0:
                    this.State = StateTypes.Closed;
                    break;
                case 1:
                    this.State = StateTypes.Active;
                    break;
                case 2:
                    this.State = StateTypes.NoCustomer;
                    break;
            }

			ClearEdited();

		}

		#endregion

		#region Methods

		public bool Merge(MeasurePoint measurePoint)
		{
			bool bEdited = true;
			if ((measurePoint.DescriptionEdited) && (this.Description != measurePoint.Description))
			{
				this.Description = measurePoint.Description;
				bEdited = true;
			}
			if ((measurePoint.SecondaryIdEdited) && (this.SecondaryId != measurePoint.SecondaryId))
			{
				this.SecondaryId = measurePoint.SecondaryId;
				bEdited = true;
			}
			if ((measurePoint.FuseSizeEdited) && (this.FuseSize != measurePoint.FuseSize))
			{
				this.FuseSize = measurePoint.FuseSize;
				bEdited = true;
			}
			if ((measurePoint.GridOwnerEdited) && (this.GridOwner != measurePoint.GridOwner))
			{
				this.GridOwner = measurePoint.GridOwner;
				bEdited = true;
			}
			if ((measurePoint.IdEdited) && (this.Id != measurePoint.Id))
			{
				this.Id = measurePoint.Id;
				bEdited = true;
			}
			if ((measurePoint.LoadProfileEdited) && (this.LoadProfile != measurePoint.LoadProfile))
			{
				this.LoadProfile = measurePoint.LoadProfile;
				bEdited = true;
			}
			if ((measurePoint.NetAreaCodeEdited) && (this.NetAreaCode != measurePoint.NetAreaCode))
			{
				this.NetAreaCode = measurePoint.NetAreaCode;
				bEdited = true;
			}
			if ((measurePoint.NumberOfPhasesEdited) && (this.NumberOfPhases != measurePoint.NumberOfPhases))
			{
				this.NumberOfPhases = measurePoint.NumberOfPhases;
				bEdited = true;
			}
			if ((measurePoint.OrgUnitEdited) && (this.OrgUnit != measurePoint.OrgUnit))
			{
				this.OrgUnit = measurePoint.OrgUnit;
				bEdited = true;
			}
			if ((measurePoint.StationIdEdited) && (this.StationId != measurePoint.StationId))
			{
				this.StationId = measurePoint.StationId;
				bEdited = true;
			}
			if ((measurePoint.YearlyVolumeEdited) && (this.YearlyVolume != measurePoint.YearlyVolume))
			{
				this.YearlyVolume = measurePoint.YearlyVolume;
				bEdited = true;
			}
			if ((measurePoint.SubscribedPowerEdited) && (this.SubscribedPower != measurePoint.SubscribedPower))
			{
				this.SubscribedPower = measurePoint.SubscribedPower;
				bEdited = true;
			}
			if ((measurePoint.SupplyVoltageEdited) && (this.SupplyVoltage != measurePoint.SupplyVoltage))
			{
				this.SupplyVoltage = measurePoint.SupplyVoltage;
				bEdited = true;
			}
			if ((measurePoint.TransformersEdited) && (this.Transformers != measurePoint.Transformers))
			{
				this.Transformers = measurePoint.Transformers;
				bEdited = true;
			}
			if ((measurePoint.ValidFromDateEdited) && (this.ValidFromDate != measurePoint.ValidFromDate))
			{
				this.ValidFromDate = measurePoint.ValidFromDate;
				bEdited = true;
			}
			if ((measurePoint.ValidToDateEdited) && (this.ValidToDate != measurePoint.ValidToDate))
			{
				this.ValidToDate = measurePoint.ValidToDate;
				bEdited = true;
			}
			if ((measurePoint.XCoordinateEdited) && (this.XCoordinate != measurePoint.XCoordinate))
			{
				this.XCoordinate = measurePoint.XCoordinate;
				bEdited = true;
			}
			if ((measurePoint.YCoordinateEdited) && (this.YCoordinate != measurePoint.YCoordinate))
			{
				this.YCoordinate = measurePoint.YCoordinate;
				bEdited = true;
			}
			if ((measurePoint.LocationEdited) && (this.Location != measurePoint.Location))
			{
				this.Location = measurePoint.Location;
				bEdited = true;
			}
			if ((measurePoint.SupplierEdited) && (this.Supplier != measurePoint.Supplier))
			{
				this.Supplier = measurePoint.Supplier;
				bEdited = true;
			}
			if ((measurePoint.CentralNetPointCodeEdited) && (this.CentralNetPointCode != measurePoint.CentralNetPointCode))
			{
				this.CentralNetPointCode = measurePoint.CentralNetPointCode;
				bEdited = true;
			}
			if ((measurePoint.DebtorTypeEdited) && (this.DebtorType != measurePoint.DebtorType))
			{
				this.DebtorType = measurePoint.DebtorType;
				bEdited = true;
			}
            if (measurePoint.StateEdited && measurePoint.State != StateTypes.Unspecified && this.State != measurePoint.State)
            {
                this.State = measurePoint.State;
                bEdited = true;
            }
            
			//if(measurePoint.AgreementsEdited) this.Agreements = measurePoint.Agreements;
			//if(measurePoint.MetersEdited) this.Meters = measurePoint.Meters;
			//if(measurePoint.TransformersEdited) this.Transformers = measurePoint.Transformers;
			return bEdited;
		}

		private void ClearEdited()
		{
			fieldEditStatus = new BitVector32(0);
		}

		#endregion

		#region System.Object

		/// <summary>
		/// Prints all public properties of a Measure Point, excluded any related agreements, components or terminals.
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			string separator = ";";
			return "Id: " + this.Id + separator +
			       "SecondaryId: " + this.SecondaryId + separator +
			       "Description: " + this.Description + separator +
			       "NetAreaCode: " + this.NetAreaCode + separator +
			       "ValidFromDate: " + this.ValidFromDate + separator +
			       "ValidToDate: " + this.ValidToDate + separator +
			       "X-coordinate: " + this.XCoordinate + separator +
			       "Y-coordinate: " + this.YCoordinate + separator +
			       "StationId: " + this.StationId + separator +
			       "NumberOfPhases: " + this.NumberOfPhases + separator +
			       "SupplyVoltage: " + this.SupplyVoltage + separator +
			       "FuseSize: " + this.FuseSize + separator +
			       "SubscribedPower: " + this.SubscribedPower + separator +
			       "OrganizationalUnit: " + this.OrgUnit + separator +
			       "GridOwner: " + this.GridOwner + separator +
			       "LoadProfile: " + this.LoadProfile + separator +
			       "YearlyVolume: " + this.YearlyVolume + separator +
			       "Loacation: " + this.Location + separator +
			       "Supplier: " + this.Supplier + separator +
			       "CentralNetPointCode: " + this.CentralNetPointCode + separator +
			       "DebtorType: " + this.DebtorType + separator;
		}

		/// <summary>
		/// Calculates a unique hash code for an installation object, excluded any related measure points. I.e. two installations may produce the same hash code if they have equal properties, but unequal related measure points.
		/// </summary>
		/// <returns></returns>
		public override int GetHashCode()
		{
			return (this.ToString() + key.ToString() + seqno.ToString() + inslKey.ToString()).GetHashCode();
		}

		/// <summary>
		/// Two installations are treated as equal if all properties (except the MeasurePoints property) are equal.
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public override bool Equals(object obj)
		{
			MeasurePoint m = obj as MeasurePoint;
			if (m == null)
				return false;
			if (this.Key == m.Key &&
				this.Seqno == m.Seqno &&
				this.InstallationKey == m.InstallationKey &&
				this.Id == m.Id &&
				this.SecondaryId == m.SecondaryId &&
				this.Description == m.Description &&
				this.NetAreaCode == m.NetAreaCode &&
				this.ValidFromDate == m.ValidFromDate &&
				this.ValidToDate == m.ValidToDate &&
				this.XCoordinate == m.XCoordinate &&
				this.YCoordinate == m.YCoordinate &&
				this.StationId == m.StationId &&
				this.NumberOfPhases == m.NumberOfPhases &&
				this.SupplyVoltage== m.SupplyVoltage &&
				this.FuseSize == m.FuseSize &&
				this.SubscribedPower == m.SubscribedPower &&
				this.OrgUnit == m.OrgUnit &&
				this.GridOwner == m.GridOwner &&
				this.LoadProfile == m.LoadProfile &&
				this.YearlyVolume == m.YearlyVolume &&
				this.Location == m.Location &&
				this.Supplier == m.Supplier	&&
				this.CentralNetPointCode == m.CentralNetPointCode &&
				this.DebtorType == m.DebtorType)
				return true;
			return false;
		}

		#endregion
	}
}
