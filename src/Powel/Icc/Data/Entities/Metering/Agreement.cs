using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Agreement.
	/// </summary>
	public class Agreement
	{
		#region Fields

		private BitVector32 fieldEditStatus = new BitVector32(0);

		private static readonly int validFromDateBit = BitVector32.CreateMask();
		private static readonly int validToDateBit = BitVector32.CreateMask(validFromDateBit);
		private static readonly int measurePointIdBit = BitVector32.CreateMask(validToDateBit);
		private static readonly int tariffCodeBit = BitVector32.CreateMask(measurePointIdBit);
		private static readonly int supplierIdBit = BitVector32.CreateMask(tariffCodeBit);
		private static readonly int exportFormatBit = BitVector32.CreateMask(supplierIdBit);
		private static readonly int pulseImportBit = BitVector32.CreateMask(exportFormatBit);
		private static readonly int trafoImportBit = BitVector32.CreateMask(pulseImportBit);
		private static readonly int pulseExportBit = BitVector32.CreateMask(trafoImportBit);
		private static readonly int trafoExportBit = BitVector32.CreateMask(pulseExportBit);
		private static readonly int senderIdBit = BitVector32.CreateMask(trafoExportBit);
		private static readonly int receiverIdBit = BitVector32.CreateMask(senderIdBit);
		private static readonly int commoditiesBit = BitVector32.CreateMask(receiverIdBit);

		private readonly int key;
		private readonly int senderKey;
		private readonly int receiverKey;
		private UtcTime validFromDate;
		private UtcTime validToDate;
		private string measurePointId;
		private string tariffCode;
		private string supplierId;
		private ExportFormatType exportFormat;
		private bool pulseImport;
		private bool trafoImport;
		private bool pulseExport;
		private bool trafoExport;
	    private Commodity[] commodities;

		#endregion

		#region Properties

		public int Key
		{
			get{ return key;}
		}

		public int SenderKey
		{
			get{ return senderKey;}
		}

		public int ReceiverKey
		{
			get{ return receiverKey;}
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

		public string MeasurePointId
		{
			get{ return measurePointId; }
			set
			{
				measurePointId = value;
				fieldEditStatus[measurePointIdBit] = true;
			}
		}

		public bool MeasurePointIdEdited
		{
			get{ return fieldEditStatus[measurePointIdBit]; }
		}

		public string TariffCode
		{
			get{ return tariffCode; }
			set
			{
				tariffCode = value;
				fieldEditStatus[tariffCodeBit] = true;
			}
		}

		public bool TariffCodeEdited
		{
			get{ return fieldEditStatus[tariffCodeBit]; }
		}
        
		public string SupplierId
		{
			get{ return supplierId; }
			set
			{
				supplierId = value;
				fieldEditStatus[supplierIdBit] = true;
			}
		}

		public bool SupplierIdEdited
		{
			get{ return fieldEditStatus[supplierIdBit]; }
		}
        
		public ExportFormatType ExportFormat
		{
			get{ return exportFormat; }
			set
			{
				exportFormat = value;
				fieldEditStatus[exportFormatBit] = true;
			}
		}

		public bool ExportFormatEdited
		{
			get{ return fieldEditStatus[exportFormatBit]; }
		}

		public bool PulseImport
		{
			get{ return pulseImport; }
			set
			{
				pulseImport = value;
				fieldEditStatus[pulseImportBit] = true;
			}
		}

		public bool PulseImportEdited
		{
			get{ return fieldEditStatus[pulseImportBit]; }
		}

		public bool PulseExport
		{
			get{ return pulseExport; }
			set
			{
				pulseExport = value;
				fieldEditStatus[pulseExportBit] = true;
			}
		}

		public bool PulseExportEdited
		{
			get{ return fieldEditStatus[pulseExportBit]; }
		}

		public bool TrafoImport
		{
			get{ return trafoImport; }
			set
			{
				trafoImport = value;
				fieldEditStatus[trafoImportBit] = true;
			}
		}

		public bool TrafoImportEdited
		{
			get{ return fieldEditStatus[trafoImportBit]; }
		}

		public bool TrafoExport
		{
			get{ return trafoExport; }
			set
			{
				trafoExport = value;
				fieldEditStatus[trafoExportBit] = true;
			}
		}

		public bool TrafoExportEdited
		{
			get{ return fieldEditStatus[trafoExportBit]; }
		}

	    public string SenderId { get; set; }

	    public string ReceiverId { get; set; }

	    public bool ExportScheduleDateFixed { get; set; }

	    public Commodity[] Commodities
		{
			get { return commodities; }
			set
			{
				commodities = value;
				fieldEditStatus[commoditiesBit] = true;
			}
		}

		public bool CommoditiesEdited
		{
			get { return fieldEditStatus[commoditiesBit]; }
		}

		#endregion

		#region Constructors

		public Agreement()
		{
		}

		public Agreement(Agreement agreement)
		{
			key = agreement.Key; // read-only property
			ValidFromDate = agreement.ValidFromDate;
			ValidToDate = agreement.ValidToDate;
			MeasurePointId = agreement.MeasurePointId;
			TariffCode = agreement.TariffCode;
			SupplierId = agreement.SupplierId;
			ExportFormat = agreement.ExportFormat;
			PulseImport = agreement.PulseImport;
			TrafoImport = agreement.TrafoImport;
			PulseExport = agreement.PulseExport;
			TrafoExport = agreement.TrafoExport;
			SenderId = agreement.SenderId;
			senderKey = agreement.SenderKey; //R-O
			receiverKey = agreement.ReceiverKey; //R-O
			ReceiverId = agreement.ReceiverId;
			Commodities = agreement.Commodities;
		}

		public Agreement(
			int key,
			UtcTime validFromDate,
			UtcTime validToDate,
			string measurePointId,
			string tariffCode,
			string supplierId,
			ExportFormatType exportFormat,
			bool pulseImport,
			bool trafoImport,
			bool pulseExport,
			bool trafoExport,
			string senderId,
			int senderKey,
			string receiverId,
			int receiverKey,
			Commodity[] commodities)
			: this(
			validFromDate,
			validToDate,
			measurePointId,
			tariffCode,
			supplierId,
			exportFormat,
			pulseImport,
			trafoImport,
			pulseExport,
			trafoExport,
			senderId,
			receiverId,
			commodities)
		{
			this.key = key;
			this.senderKey = senderKey;
			this.receiverKey = receiverKey;
		}

		public Agreement(
			UtcTime validFromDate,
			UtcTime validToDate,
			string measurePointId,
			string tariffCode,
			string supplierId,
			ExportFormatType exportFormat,
			bool pulseImport,
			bool trafoImport,
			bool pulseExport,
			bool trafoExport,
			string senderId,
			string receiverId,
			Commodity[] commodities)
		{
			ValidFromDate = validFromDate;
			ValidToDate = validToDate;
			MeasurePointId = measurePointId;
			TariffCode = tariffCode;
			SupplierId = supplierId;
			ExportFormat = exportFormat;
			PulseImport = pulseImport;
			TrafoImport = trafoImport;
			PulseExport = pulseExport;
			TrafoExport = trafoExport;
			SenderId = senderId;
			ReceiverId = receiverId;
			Commodities = commodities;
		}

		#endregion

		#region Methods

		public bool Merge(Agreement agreement)
		{
			throw new NotImplementedException("Agreement.Merge has not been implemented yet");
		}

		/// <summary>
		/// Prints all public scalar properties of Agreement.
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			string separator = ";";
			return 
				"key: " + key + separator +
				"MeasurePointId: " + MeasurePointId + separator +
				"ValidFromDate: " + ValidFromDate + separator +
				"ValidToDate: " + ValidToDate + separator +
				"TariffCode: " + TariffCode + separator +
				"SupplierId: " + SupplierId + separator +
				"ExportFormat: " + ExportFormat + separator +
				"PulseImport: " + PulseImport + separator +
				"TrafoImport: " + TrafoImport + separator +
				"PulseExport: " + PulseExport + separator +
				"TrafoExport: " + TrafoExport + separator +
				"SenderId: " + SenderId + separator +
				"senderKey: " + SenderKey + separator +
				"receiverKey: " + ReceiverKey + separator +
				"ReceiverId: " + ReceiverId + separator +
				"Commodities.Length: " + (Commodities == null ? "null" : Commodities.Length.ToString()) + separator +
				""
				;
	}

		#endregion

		public enum DebitingType
		{	
			S,
			T,
			B,
			M
		}
		public enum ExportFormatType
		{
			GS2
		}
		public enum Frequency
		{
			NEVER = -1,
			HOURLY = 3,
			DAILY = 4,
			WEEKLY = 5,
			MONTHLY = 6,
			BIMONTHLY = 7,
			QUARTERLY = 8,
			SEMIYEARLY = 9,
			YEARLY = 99,	
		}
	}
}
