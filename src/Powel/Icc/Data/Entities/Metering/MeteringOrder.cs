using System;
using System.Collections;
using Powel.Icc.Services.Time;
using Powel.Icc.Data.Entities;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for MeteringOrder.
	/// </summary>
	public class MeteringOrder
	{
		private Register register;
		private MeasurePoint measurePoint;
		private ImportDefinition impDef;
		private SendingStatus sendStatus;
		private Hashtable registerImportDefinitions;

		public MeteringOrder()
		{
		}

		public MeteringOrder(Register register, MeasurePoint measurePoint, ImportDefinition importDefinition)
		{
			this.Register = register;
			this.MeasurePoint = measurePoint;
			this.ImportDefinition = importDefinition;
		}

		public MeteringOrder(Register register, MeasurePoint measurePoint, ImportDefinition importDefinition, SendingStatus sendingStatus)
			:this(register, measurePoint, importDefinition)
		{
			this.SendStatus = sendingStatus;
		}

		public Register Register
		{
			get{ return this.register;}
			set{ this.register = value;}
		}

		public MeasurePoint MeasurePoint
		{
			get{ return this.measurePoint;}
			set{ this.measurePoint = value;}
		}

		public ImportDefinition ImportDefinition
		{
			get{ return this.impDef;}
			set{ this.impDef = value;}
		}

		public Hashtable RegisterImportDefinitions
		{
			get { return this.registerImportDefinitions; }
			set { this.registerImportDefinitions = value; }
		}

		public SendingStatus SendStatus
		{
			get{return this.sendStatus;}
			set{this.sendStatus = value;}
		}

		public enum SendingStatus
		{
			OK,
			MISSING_IMPDEF,
			IMPORT_FREQUENCY_IS_NULL,
			INCORRECT_PROTOCOL,
			MISSING_MEASUREPOINT,
			MISSING_REGISTER,
			SPLITTED,					//only applies to MOOD
			SPLIT,						//only applies to MOOD
			SPLIT_MISSING_REGISTER,		//only applies to MOOD
			FAILED
		}

	}

	public class MeteringOrderOnDemand : MeteringOrder
	{
		internal int timsKey;
		internal int opunKey;
		internal int seqno;
		private MeteringOrderOnDemand original;
		private ExpStatus expStatus;

		internal MeteringOrderOnDemand(int timsKey, int opunKey, int seqno)
		{
			this.timsKey = timsKey;
			this.opunKey = opunKey;
			this.seqno = seqno;
			this.SendStatus = SendingStatus.OK;
		}

		internal MeteringOrderOnDemand(int timsKey, int opunKey, int seqno, Register register, MeasurePoint measurePoint, ImportDefinition impDef)
		{
			this.timsKey = timsKey;
			this.opunKey = opunKey;
			this.seqno = seqno;
			this.Register = register;
			this.MeasurePoint = measurePoint;
			this.ImportDefinition = ImportDefinition;
			this.SendStatus = SendingStatus.OK;
			this.ExportStatus = ExpStatus.NOT_EXPORTED;
		}
		
		public MeteringOrderOnDemand(MeasurePoint measurePoint, ImportDefinition importDefinition, Register register, MeteringOrderOnDemand original, SendingStatus sendStatus)
		{
			this.MeasurePoint = measurePoint;
			this.ImportDefinition = importDefinition;
			this.Register = register;
			this.Original = original;
			this.SendStatus = sendStatus;
			this.ExportStatus = ExpStatus.NOT_EXPORTED;
		}

		public int TimsKey
		{
			get{return this.timsKey;}
		}
		public int OpunKey
		{
			get{return this.opunKey;}
		}

		public MeteringOrderOnDemand Original
		{
			get{return this.original;}
			set{this.original = value;}
		}
		public ExpStatus ExportStatus
		{
			get{return this.expStatus;}
			set{this.expStatus = value;}
		}

		public enum ExpStatus
		{
			NOT_EXPORTED,
			EXPORTED_OK,
			EXPORT_FAILED
		}

		public enum Status
		{
			REGISTERED_MDMS = 0,
			FORWARDED_MSS = 1,
			FORWARD_MSS_FAILED = 2,
			EXPORTED_GS2 = 3,
			EXPORT_GS2_FAILED = 4
		}

	}

	public class MeteringOrderOnDemandCollection
	{
		private UtcTime dateOfTransfer;
		private UtcTime dataFromTime;
		private UtcTime dataToTime;
		private RegistrationReason reason;
		private ArrayList meteringOrdersOnDemand;

		public MeteringOrderOnDemandCollection(UtcTime dateOfTransfer, UtcTime dataFromTime, UtcTime dataToTime, RegistrationReason reason)
		{
			this.DateOfTransfer = dateOfTransfer;
			this.DataFromTime = dataFromTime;
			this.DataToTime = dataToTime;
			this.Reason = reason;
			this.meteringOrdersOnDemand = new ArrayList();
		}

		public MeteringOrderOnDemandCollection(UtcTime dateOfTransfer, UtcTime dataFromTime, UtcTime dataToTime, RegistrationReason reason, ArrayList alMeteringOrdersOnDemand)
		{
			this.DateOfTransfer = dateOfTransfer;
			this.DataFromTime = dataFromTime;
			this.DataToTime = dataToTime;
			this.Reason = reason;
			this.MeteringOrdersOnDemand = alMeteringOrdersOnDemand;
		}

		public UtcTime DateOfTransfer
		{
			get{return dateOfTransfer;}
			set{dateOfTransfer = value;}
		}
		public UtcTime DataFromTime
		{
			get{return dataFromTime;}
			set{dataFromTime = value;}
		}
		public UtcTime DataToTime
		{
			get{return dataToTime;}
			set{dataToTime = value;}
		}
		public RegistrationReason Reason
		{
			get{return reason;}
			set{reason = value;}
		}
		public ArrayList MeteringOrdersOnDemand
		{
			get{return meteringOrdersOnDemand;}
			set{meteringOrdersOnDemand = value;}
		}

		public void AddMeteringOrderOnDemand(MeteringOrderOnDemand meteringOrderOnDemand)
		{
			if( this.meteringOrdersOnDemand == null)
				this.meteringOrdersOnDemand = new ArrayList();
			this.meteringOrdersOnDemand.Add(meteringOrderOnDemand);
		}

		public void RemoveMeteringOrderOnDemand(MeteringOrderOnDemand meteringOrderOnDemand)
		{
			this.meteringOrdersOnDemand.Remove(meteringOrderOnDemand);
		}

		public MeteringOrderOnDemand Find(string impRef)
		{
			foreach(MeteringOrderOnDemand mood in this.meteringOrdersOnDemand)
				if(mood.ImportDefinition != null)
					if( mood.ImportDefinition.ExtRef == impRef)
						return mood;
			return null;
		}


	}

	
}
