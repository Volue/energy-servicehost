using System.Collections.Specialized;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Installation is an entity object, describing the identification and properties of a physical installation, as well as the owner.
	/// </summary>
	public class Installation
	{
		/// <summary>
		/// If a field is added, changed or removed, remember to update
		/// - Merge() method
		/// - System.Object overriden methods in the System.Object region below.
		/// - properties region, if applicable
		/// </summary>
		#region Fields

		protected BitVector32 fieldEditStatus = new BitVector32(0);
		private static readonly int idBit = BitVector32.CreateMask();
		private static int seqnoBit = BitVector32.CreateMask();
		private static readonly int secondaryIdBit = BitVector32.CreateMask(idBit);
		private static readonly int descriptionBit = BitVector32.CreateMask(secondaryIdBit);
		private static readonly int codeBit = BitVector32.CreateMask(descriptionBit);
		private static readonly int validFromDateBit = BitVector32.CreateMask(codeBit);
		private static readonly int validToDateBit = BitVector32.CreateMask(validFromDateBit);
		private static readonly int customerNameBit = BitVector32.CreateMask(validToDateBit);
		private static readonly int customerIdBit = BitVector32.CreateMask(customerNameBit);
		private static readonly int apartmentNoBit = BitVector32.CreateMask(customerIdBit);
        private static readonly int houseNoBit = BitVector32.CreateMask(apartmentNoBit);
		private static readonly int mainAddressBit = BitVector32.CreateMask(apartmentNoBit);
		private static readonly int mainCoInfoBit = BitVector32.CreateMask(mainAddressBit);
		private static readonly int mainPostCodeBit = BitVector32.CreateMask(mainCoInfoBit);
		private static readonly int mainCityBit = BitVector32.CreateMask(mainPostCodeBit);
		private static readonly int contactAddressBit = BitVector32.CreateMask(mainCityBit);
		private static readonly int contactCoInfoBit = BitVector32.CreateMask(contactAddressBit);
		private static readonly int contactPostCodeBit = BitVector32.CreateMask(contactCoInfoBit);
		private static readonly int contactCityBit = BitVector32.CreateMask(contactPostCodeBit);
		private static readonly int phoneHomeBit = BitVector32.CreateMask(contactCityBit);
		private static readonly int phoneWorkBit = BitVector32.CreateMask(phoneHomeBit);
		private static readonly int phoneMobileBit = BitVector32.CreateMask(phoneWorkBit);
		private static readonly int measurePointsBit = BitVector32.CreateMask(phoneMobileBit);
		
		private int key;
		private string id;
		private int seqno;
		private string secondaryId;
		private string description;
		private string code;
		internal UtcTime validFromDate = UtcTime.Null;
		internal UtcTime validToDate = UtcTime.Null;
		private string customerName;
		private string customerId;
		private string apartmentNo;
        private string houseNo;
		private string mainAddress;
		private string mainCoInfo;
		private string mainPostCode;
		private string mainCity;
		private string contactAddress;
		private string contactCoInfo;
		private string contactPostCode;
		private string contactCity;
		private string phoneHome;
		private string phoneWork;
		private string phoneMobile;
		private MeasurePoint[] measurePoints;

		#endregion
		
		#region Properties

		public int Key
		{
			get{ return key; }
			set{ key = value;}
		}
		

		public string Id
		{
			get{ return id; }
			set
			{
				id = value;
				fieldEditStatus[idBit] = true;
			}
		}

		public bool IdEdited
		{
			get { return fieldEditStatus[idBit]; }
		}

		public int Seqno
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

		public string SecondaryId
		{
			get { return secondaryId; }
			set
			{
				secondaryId = value;
				fieldEditStatus[secondaryIdBit] = true;
			}
		}


		public bool SecondaryIdEdited
		{
			get { return fieldEditStatus[secondaryIdBit]; }
		}

		public string Description
		{
			get{ return description; }
			set
			{
				description = value;
				fieldEditStatus[descriptionBit] = true;
			}
		}

		public bool DescriptionEdited
		{
			get{ return fieldEditStatus[descriptionBit]; }
		}

		public string Code
		{
			get{ return code; }
			set
			{
				code = value;
				fieldEditStatus[codeBit] = true;
			}
		}

		public bool CodeEdited
		{
			get{ return fieldEditStatus[codeBit]; }
		}

		public UtcTime ValidFromDate
		{
			get{ return validFromDate; }
			set
			{
				validFromDate = value;
				fieldEditStatus[validFromDateBit] = true;
			}
		}

		public bool ValidFromDateEdited
		{
			get{ return fieldEditStatus[validFromDateBit]; }
		}

		public UtcTime ValidToDate
		{
			get{ return validToDate; }
			set
			{
				validToDate = value;
				fieldEditStatus[validToDateBit] = true;
			}
		}

		public bool ValidToDateEdited
		{
			get{ return fieldEditStatus[validToDateBit]; }
		}

		public string CustomerName
		{
			get{ return customerName; }
			set
			{
				customerName = value;
				fieldEditStatus[customerNameBit] = true;
			}
		}

		public bool CustomerNameEdited
		{
			get{ return fieldEditStatus[customerNameBit]; }
		}

		public string CustomerId
		{
			get{ return customerId; }
			set
			{
				customerId = value;
				fieldEditStatus[customerIdBit] = true;
			}
		}

		public bool CustomerIdEdited
		{
			get{ return fieldEditStatus[customerIdBit]; }
		}

		public string ApartmentNo
		{
			get{ return apartmentNo; }
			set
			{
				apartmentNo = value;
				fieldEditStatus[apartmentNoBit] = true;
			}
		}

		public bool ApartmentNoEdited
		{
			get{ return fieldEditStatus[apartmentNoBit]; }
		}

        public string HouseNo
        {
            get { return houseNo; }
            set
            {
                houseNo = value;
                fieldEditStatus[houseNoBit] = true;
            }
        }

        public bool HouseNoEdited
        {
            get { return fieldEditStatus[houseNoBit]; }
        }

		public string AddressMain
		{
			get{ return mainAddress; }
			set
			{
				mainAddress = value;
				fieldEditStatus[mainAddressBit] = true;
			}
		}

		public bool AddressMainEdited
		{
			get{ return fieldEditStatus[mainAddressBit]; }
		}

		public string CoInfoMain
		{
			get{ return mainCoInfo; }
			set
			{
				mainCoInfo = value;
				fieldEditStatus[mainCoInfoBit] = true;
			} 
		}

		public bool CoInfoMainEdited
		{
			get{ return fieldEditStatus[mainCoInfoBit]; }
		}

		public string PostCodeMain
		{
			get{ return mainPostCode; }
			set
			{
				mainPostCode = value;
				fieldEditStatus[mainPostCodeBit] = true;
			}
		}

		public bool PostCodeMainEdited
		{
			get{ return fieldEditStatus[mainPostCodeBit]; }
		}

		public string CityMain
		{
			get{ return mainCity; }
			set
			{
				mainCity = value;
				fieldEditStatus[mainCityBit] = true;
			}
		}

		public bool CityMainEdited
		{
			get{ return fieldEditStatus[mainCityBit]; }
		}

		public string AddressContact
		{
			get{ return contactAddress; }
			set
			{
				contactAddress = value;
				fieldEditStatus[contactAddressBit] = true;
			}
		}

		public bool AddressContactEdited
		{
			get{ return fieldEditStatus[contactAddressBit]; }
		}

		public string CoInfoContact
		{
			get{ return contactCoInfo; }
			set
			{
				contactCoInfo = value;
				fieldEditStatus[contactCoInfoBit] = true;
			}
		}

		public bool CoInfoContactEdited
		{
			get{ return fieldEditStatus[contactCoInfoBit]; }
		}

		public string PostCodeContact
		{
			get{ return contactPostCode; }
			set
			{
				contactPostCode = value;
				fieldEditStatus[contactPostCodeBit] = true;
			}
		}

		public bool PostCodeContactEdited
		{
			get{ return fieldEditStatus[contactPostCodeBit]; }
		}

		public string CityContact
		{
			get{ return contactCity; }
			set
			{
				contactCity = value;
				fieldEditStatus[contactCityBit] = true;
			}
		}

		public bool CityContactEdited
		{
			get{ return fieldEditStatus[contactCityBit]; }
		}

		public string PhoneHome
		{
			get{ return phoneHome; }
			set
			{
				phoneHome = value;
				fieldEditStatus[phoneHomeBit] = true;
			}
		}

		public bool PhoneHomeEdited
		{
			get{ return fieldEditStatus[phoneHomeBit]; }
		}

		public string PhoneWork
		{
			get{ return phoneWork; }
			set
			{
				phoneWork = value;
				fieldEditStatus[phoneWorkBit] = true;
			}
		}

		public bool PhoneWorkEdited
		{
			get{ return fieldEditStatus[phoneWorkBit]; }
		}

		public string PhoneMobile
		{
			get{ return phoneMobile; }
			set
			{
				phoneMobile = value;
				fieldEditStatus[phoneMobileBit] = true;
			}
		}

		public bool PhoneMobileEdited
		{
			get{ return fieldEditStatus[phoneMobileBit]; }
		}

		public MeasurePoint[] MeasurePoints
		{
			get{ return measurePoints; }
			set
			{
				measurePoints = value;
				fieldEditStatus[measurePointsBit] = true;
			}
		}

		public bool MeasurePointsEdited
		{
			get{ return fieldEditStatus[measurePointsBit]; }
		}

		#endregion

		#region Constructors

		/// <summary>
		/// Empty constructor.
		/// </summary>
		public Installation()
		{
		}

		/// <summary>
		/// Constructor that makes a copy of an existing Installation.
		/// </summary>
		/// <param name="installation"></param>
		public Installation(Installation installation)
		{
			this.key = installation.Key; // read-only property
			this.Seqno = installation.Seqno;
			this.Id = installation.Id;
			this.SecondaryId = installation.SecondaryId;
			this.Description = installation.Description;
			this.Code = installation.Code;
			this.ValidFromDate = installation.ValidFromDate;
			this.ValidToDate = installation.ValidToDate;
			this.CustomerName = installation.CustomerName;
			this.CustomerId = installation.CustomerId;
			this.ApartmentNo = installation.ApartmentNo;
            this.HouseNo = installation.HouseNo;
			this.AddressMain = installation.AddressMain;
			this.CoInfoMain = installation.CoInfoMain;
			this.PostCodeMain = installation.PostCodeMain;
			this.CityMain = installation.CityMain;
			this.AddressContact = installation.AddressContact;
			this.CoInfoContact = installation.CoInfoContact;
			this.PostCodeContact = installation.PostCodeContact;
			this.CityContact = installation.CityContact;
			this.PhoneHome = installation.PhoneHome;
			this.PhoneWork = installation.PhoneWork;
			this.PhoneMobile = installation.PhoneMobile;

			MeasurePoint[] measurePoints = new MeasurePoint[installation.MeasurePoints.Length];
			int i = 0;
			foreach(MeasurePoint measurePoint in installation.MeasurePoints)
			{
				measurePoints[i] = new MeasurePoint(measurePoint);
				i++;
			}
			this.MeasurePoints = measurePoints;
			ClearEdited();
		}

		internal Installation(
			int key,
			int seqno,
			string id,
			string secondaryId,
			string description,
			string code,
			UtcTime validFromDate,
			UtcTime validToDate,
			string customerName,
			string customerId,
			string apartmentNo,
            string houseNo,
			string addressMain,
			string coInfoMain,
			string postcodeMain,
			string cityMain,
			string addressContact,
			string coInfoContact,
			string postCodeContact,
			string cityContact,
			string phoneHome,
			string phoneWork,
			string phoneMobile)
			: this(
			seqno,
			id,
			secondaryId,
			description,
			code,
			validFromDate,
			validToDate,
			customerName,
			customerId,
			apartmentNo,
            houseNo,
			addressMain,
			coInfoMain,
			postcodeMain,
			cityMain,
			addressContact,
			coInfoContact,
			postCodeContact,
			cityContact,
			phoneHome,
			phoneWork,
			phoneMobile)
		{
			this.key = key; // read-only property
			ClearEdited();
		}

		public Installation(
			int seqno,
			string id,
			string secondaryId,
			string description,
			string code,
			UtcTime validFromDate,
			UtcTime validToDate,
			string customerName,
			string customerId,
			string apartmentNo,
            string houseNo,
			string addressMain,
			string coInfoMain,
			string postcodeMain,
			string cityMain,
			string addressContact,
			string coInfoContact,
			string postCodeContact,
			string cityContact,
			string phoneHome,
			string phoneWork,
			string phoneMobile)
		{
			this.Seqno = seqno;
			this.Id = id;
			this.SecondaryId = secondaryId;
			this.Description = description;
			this.Code = code;
			this.ValidFromDate = validFromDate;
			this.ValidToDate = validToDate;
			this.CustomerName = customerName;
			this.CustomerId = customerId;
			this.ApartmentNo = apartmentNo;
            this.HouseNo = houseNo;
			this.AddressMain = addressMain;
			this.CoInfoMain = coInfoMain;
			this.PostCodeMain = postcodeMain;
			this.CityMain = cityMain;
			this.AddressContact = addressContact;
			this.CoInfoContact = coInfoContact;
			this.PostCodeContact = postCodeContact;
			this.CityContact = cityContact;
			this.PhoneHome = phoneHome;
			this.PhoneWork = phoneWork;
			this.PhoneMobile = phoneMobile;
			ClearEdited();
		}

		#endregion

		#region Methods

		public bool Merge(Installation installation)
		{
			bool bEdited = false;
			if(installation.AddressContactEdited && this.AddressContact != installation.AddressContact)
			{
				this.AddressContact = installation.AddressContact;
				bEdited = true;
			}
			if(installation.AddressMainEdited && this.AddressMain != installation.AddressMain)
			{
				this.AddressMain = installation.AddressMain;
				bEdited = true;
			}
			if(installation.ApartmentNoEdited && this.ApartmentNo != installation.ApartmentNo)
			{
				this.ApartmentNo = installation.ApartmentNo;
				bEdited = true;
			}
            if (installation.HouseNoEdited && this.houseNo != installation.houseNo)
            {
                this.houseNo = installation.houseNo;
                bEdited = true;
            }
			if(installation.CityContactEdited && this.CityContact != installation.CityContact)
			{
				this.CityContact = installation.CityContact;
				bEdited = true;
			}
			if(installation.CityMainEdited && this.CityMain != installation.CityMain)
			{
				this.CityMain = installation.CityMain;
				bEdited = true;
			}
			if(installation.CodeEdited && this.Code != installation.Code)
			{
				this.Code = installation.Code;
				bEdited = true;
			}
			if(installation.CoInfoContactEdited && this.CoInfoContact != installation.CoInfoContact)
			{
				this.CoInfoContact = installation.CoInfoContact;
				bEdited = true;
			}
			if(installation.CoInfoMainEdited && this.CoInfoMain != installation.CoInfoMain)
			{
				this.CoInfoMain = installation.CoInfoMain;
				bEdited = true;
			}
			if(installation.CustomerIdEdited && this.CustomerId != installation.CustomerId)
			{
				this.CustomerId = installation.CustomerId;
				bEdited = true;
			}
			if(installation.CustomerNameEdited && this.customerName != installation.CustomerName)
			{
				this.CustomerName = installation.CustomerName;
				bEdited = true;
			}
			if(installation.DescriptionEdited && this.Description != installation.Description)
			{
				this.Description = installation.Description;
				bEdited = true;
			}
			if(installation.IdEdited && this.Id != installation.Id)
			{
				this.Id = installation.Id;
				bEdited = true;
			}
			if(installation.PhoneHomeEdited && this.PhoneHome != installation.PhoneHome)
			{
				this.PhoneHome = installation.PhoneHome;
				bEdited = true;
			}
			if(installation.PhoneMobileEdited && this.PhoneMobile != installation.PhoneMobile)
			{
				this.PhoneMobile = installation.PhoneMobile;
				bEdited = true;
			}
			if(installation.PhoneWorkEdited && this.PhoneWork != installation.PhoneWork)
			{
				this.PhoneWork = installation.PhoneWork;
				bEdited = true;
			}
			if(installation.PostCodeContactEdited && this.PostCodeContact != installation.PostCodeContact)
			{
				this.PostCodeContact = installation.PostCodeContact;
				bEdited = true;
			}
			if(installation.PostCodeMainEdited && this.PostCodeMain != installation.PostCodeMain)
			{
				this.PostCodeMain = installation.PostCodeMain;
				bEdited = true;
			}
			if(installation.SecondaryIdEdited && this.SecondaryId != installation.SecondaryId)
			{
				this.SecondaryId = installation.SecondaryId;
				bEdited = true;
			}
			if(installation.ValidFromDateEdited && this.ValidFromDate != installation.ValidFromDate)
			{
				this.ValidFromDate = installation.ValidFromDate;
				bEdited = true;
			}
			if(installation.ValidToDateEdited && this.ValidToDate != installation.ValidToDate)
			{
				this.ValidToDate = installation.ValidToDate;
				bEdited = true;
			}

			return bEdited;
		}

		private void ClearEdited()
		{
			fieldEditStatus = new BitVector32(0);
		}

		#endregion

		#region System.Object

		/// <summary>
		/// Prints all public properties of an Installation, excluded any related measure points.
		/// </summary>
		/// <returns></returns>
		public override string  ToString()
		{
			string separator = ";";
			return "SeqNo: " + this.Seqno + separator +
				   "Id: " + this.Id + separator +
			       "SecondaryId: " + this.SecondaryId + separator +
			       "Description: " + this.Description + separator +
			       "Code: " + this.Code + separator +
			       "ValidFromDate: " + this.ValidFromDate.ToString() + separator +
			       "ValidToDate: " + this.ValidToDate.ToString() + separator +
			       "CustomerName: " + this.CustomerName + separator +
			       "CustomerId: " + this.CustomerId + separator +
			       "ApartmentNo: " + this.ApartmentNo + separator +
                   "HouseNo: " + this.HouseNo + separator +
			       "MainAddress: " + this.AddressMain + separator +
			       "MainCoInfo: " + this.CoInfoMain + separator +
			       "MainPostcode: " + this.PostCodeMain + separator +
			       "MainCity: " + this.CityMain + separator +
			       "ContactAddress: " + this.AddressContact + separator +
			       "ContactCoInfo: " + this.CoInfoContact + separator +
			       "ContactPostCode: " + this.PostCodeContact + separator +
			       "ContactCity: " + this.CityContact + separator +
			       "PhoneHome: " + this.PhoneHome + separator +
			       "PhoneWork: " + this.PhoneWork + separator +
			       "PhoneMobile: " + this.PhoneMobile;
		}

		/// <summary>
		/// Calculates a unique hash code for an installation object, excluded any related measure points. I.e. two installations may produce the same hash code if they have equal properties, but unequal related measure points.
		/// </summary>
		/// <returns></returns>
		public override int GetHashCode()
		{
			return (this.ToString() + key.ToString()).GetHashCode();
		}

		/// <summary>
		/// Two installations are treated as equal if all properties (except the MeasurePoints property) are equal.
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public override bool Equals(object obj)
		{
			Installation i = obj as Installation;
			if (i == null)
				return false;
			if (this.Key == i.Key &&
				this.Seqno == i.Seqno &&
				this.Id == i.Id &&
				this.SecondaryId == i.SecondaryId &&
				this.Description == i.Description &&
				this.Code == i.Code &&
				this.ValidFromDate == i.ValidFromDate &&
				this.ValidToDate == i.ValidToDate &&
				this.CustomerName == i.CustomerName &&
				this.CustomerId == i.CustomerId &&
				this.ApartmentNo == i.ApartmentNo &&
                this.HouseNo == i.HouseNo &&
				this.AddressMain == i.AddressMain &&
				this.CoInfoMain == i.CoInfoMain &&
				this.PostCodeMain == i.PostCodeMain &&
				this.CityMain == i.CityMain &&
				this.AddressContact == i.AddressContact &&
				this.CoInfoContact == i.CoInfoContact &&
				this.PostCodeContact == i.PostCodeContact &&
				this.CityContact == i.CityContact &&
				this.PhoneHome == i.PhoneHome &&
				this.PhoneWork == i.PhoneWork &&
				this.PhoneMobile == i.PhoneMobile)
				return true;
			return false;
		}

		#endregion

	}
}
