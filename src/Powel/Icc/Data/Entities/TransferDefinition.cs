using System;
using System.ComponentModel;
using Powel.Icc.Services.Time;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Interop;
//using Powel.Icc.Interop.Monitoring;

namespace Powel.Icc.Data.Entities
{
	/// <summary>
	/// Summary description for TransferDefinition.
	/// </summary>
	public abstract class ImpExpDefinition
	{
		internal int opunKey;
	    internal string opunName;
		internal int timsKey;
		internal int seqno;
		internal int tstrKey;
		private string tscode;
		private string extRef;
		private TimePeriod validPeriod;
		private double offset;
		private double scale;
		private ReadOff readoff;
		private int digits;
		private int decimals;
		private TsProtocol? protocol;

		
		public int OpunKey
		{
			get{ return opunKey;}
		}
        public string OpunName
        {
            get { return opunName; }
        }
        public int TimsKey
		{
			get{ return timsKey;}
		}
		internal int Seqno
		{
			get{ return seqno;}
		}
		internal int TstrKey
		{
			get{ return tstrKey;}
		}
		public string TsCode
		{
			get{return tscode;}
			set{tscode = value;}
		}
		public string ExtRef
		{
			get{return extRef;}
			set{extRef = value;}
		}
		public TimePeriod ValidPeriod
		{
			get{return validPeriod;}
			set{validPeriod = value;}
		}
		public double Offset
		{
			get{return this.offset;}
			set{ this.offset = value;}
		}
		public double Scale 
		{
			get{ return this.scale;}
			set{ this.scale = value;}
		}
		public ReadOff ReadOff 
		{
			get{ return this.readoff;}
			set{ this.readoff = value;}
		}
		public int Digits
		{
			get{ return this.digits;}
			set{ this.digits = value;}
		}
		public int Decimals
		{
			get{ return this.decimals;}
			set{ this.decimals = value;}
		}

		public TsProtocol? Protocol
		{
			get{ return this.protocol;}
			set{ this.protocol = value;}
		}

	}

	public class ImportDefinition : ImpExpDefinition//, IMonitorable
	{
		private Agreement.Frequency importFrequency;

		public ImportDefinition() { }

		public ImportDefinition(string extRef)
		{
			this.ExtRef = extRef;
		}

		public ImportDefinition(int timsKey, UtcTime fromTime)
		{
			this.timsKey = timsKey;
			this.ValidPeriod = new TimePeriod(fromTime);
		}

		public Agreement.Frequency ImportFrequency
		{
			get{return importFrequency;}
			set{importFrequency = value;}
		}

		internal ImportDefinition(int opunKey, string opunName, int timsKey, int seqno, string tscode, string extRef,
			TimePeriod validPeriod, int tstrKey, double offset, double scale, ReadOff readOff, int digits, int decimals, Agreement.Frequency importFrequency, TsProtocol? protocol)
		{
			this.opunKey = opunKey;
		    this.opunName = opunName;
			this.timsKey = timsKey;
			this.seqno = seqno;
			this.tstrKey = tstrKey;
			this.TsCode = tscode;
			this.ExtRef = extRef;
			this.ValidPeriod = validPeriod;
			this.Offset = offset;
			this.Scale = scale;
			this.ReadOff = readOff;
			this.Digits = digits;
			this.Decimals = decimals;
			this.ImportFrequency = importFrequency;
			this.Protocol = protocol;
		}

		public int MonitorKey1
		{
			get { return this.timsKey; }
			set { this.timsKey = value; }
		}

		public int MonitorKey2
		{
			get { return -1; }
			set { }
		}

		public int ObjectType
		{
			get { return (int) ObjectTypes.ImportDefinition; }
		}

		public TimePeriod MonitorPeriod
		{
			get { return this.ValidPeriod; }
			set { this.ValidPeriod = new TimePeriod(value.From, value.Until); }
		}

	}

    public class ExportDefinition : ImpExpDefinition
    {
        private int valtran;

        public ExportDefinition() { }

        public ExportDefinition(int opunKey,TsProtocol? protocol,int valtran,int timsKey)
        {
            this.opunKey = opunKey;
            this.Protocol = protocol;
            this.valtran = valtran;
            this.timsKey = timsKey;
        }

        internal ExportDefinition(int opunKey, string opunName, int timsKey, int seqno, string tscode, string extRef,
            TimePeriod validPeriod, int tstrKey, double offset, double scale, ReadOff readOff, int digits, int decimals, TsProtocol? protocol, int valtran)
        {
            this.opunKey = opunKey;
            this.opunName = opunName;
            this.timsKey = timsKey;
            this.seqno = seqno;
            this.tstrKey = tstrKey;
            this.TsCode = tscode;
            this.ExtRef = extRef;
            this.ValidPeriod = validPeriod;
            this.Offset = offset;
            this.Scale = scale;
            this.ReadOff = readOff;
            this.Digits = digits;
            this.Decimals = decimals;
            this.Protocol = protocol;
            this.Valtran = valtran;
        }

        public int Valtran
        {
            get { return valtran; }
            set { valtran = value; }
        }
    }

	public enum ReadOff
	{
		Start = 0,
		End = 1
	}
}
