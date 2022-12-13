using System;
using System.Collections;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Text;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Services.Time;

using Process = System.Diagnostics.Process;

namespace Powel.Icc.Messaging
{
	/// <summary>
	/// Summary description for GS2Export.
	/// </summary>
	public class GS2Export
	{
		private const string ArgProtocol = "-P";
		private const string ArgReceiver = "-R";
		private const string ArgCustomer = "-C";
		private const string ArgStartTime = "-s";
		private const string ArgEndTime = "-e";
		private const string ArgFileName = "-f";
		private const string ArgLog = "-l";
		private const string ArgValueType = "-t";
		private const string ArgTimsKey = "-T";
		private const string ArgTestIndicator = "-i";
		private const string ArgMailTo = "-m";
		private const string ArgMethod = "-M"; //only applies to automatic export
		private const string ArgNN = "-S"; //only applies to automatic export

		private ArrayList timsKeys;
		private TsProtocol protocol;
		private UtcTime startTime;
		private UtcTime endTime;
		private ValueTypeType valueType;
		private int receiver;
		private string iccHome;
		private RegionalCalendar cal;


		public GS2Export()
		{
			iccHome = IccConfiguration.IccHome;
			cal = IccConfiguration.Time.DatabaseCalendar;
			timsKeys = new ArrayList();
		}
		public GS2Export(TsProtocol protocol, int receiver, UtcTime startTime, UtcTime endTime, ValueTypeType valueType)
			:this()
		{
			this.Protocol = protocol;
			this.Receiver = receiver;
			this.StartTime = startTime;
			this.EndTime = endTime;
			this.ValueType = valueType;
		}

		public GS2Export(TsProtocol protocol, int receiver, UtcTime startTime, UtcTime endTime, ValueTypeType valueType, ArrayList timsKeys)
			:this(protocol, receiver, startTime, endTime, valueType)
		{
			foreach(int i in timsKeys)
				this.AddTimsKey(i);
		}

		public UtcTime StartTime
		{
			get{ return this.startTime;}
			set{ this.startTime = value;}
		}
		public UtcTime EndTime
		{
			get{return this.endTime;}
			set{this.endTime = value;}
		}
		public TsProtocol Protocol
		{
			get{return this.protocol;}
			set{this.protocol = value;}
		}
		public ValueTypeType ValueType
		{
			get{return this.valueType;}
			set{this.valueType = value;}
		}
		public int Receiver
		{
			get{ return this.receiver;}
			set{this.receiver = value;}
		}
		public string Arguments
		{
			get
			{
				var s = new StringBuilder(
					 ArgProtocol + " " + (int) Protocol + " " +
					 ArgStartTime + " " + cal.ToDatabaseString(StartTime) + " " +
					 ArgEndTime + " " + cal.ToDatabaseString(EndTime) + " " +
					 ArgReceiver + " " + Receiver + " " +
					 ArgCustomer + " " + Receiver + " " +
					 ArgValueType + " " + (int) ValueType + " ");
				foreach( int i in this.timsKeys)
					s.Append(ArgTimsKey + " " + i + " ");
				return s.ToString();
			}
		}

		public void AddTimsKey(int timsKey)
		{
			if(!this.timsKeys.Contains(timsKey))
				this.timsKeys.Add(timsKey);
		}

		public bool RunExport()
		{
			var procGs2Export = new System.Diagnostics.Process();
			procGs2Export.StartInfo.FileName = iccHome + @"\bin\gs2exp.exe";
			procGs2Export.StartInfo.Arguments = this.Arguments;
			procGs2Export.StartInfo.UseShellExecute = false;
			procGs2Export.StartInfo.EnvironmentVariables.Add("ICC_NO_LOGONUIS", "TRUE");
			try
			{
				procGs2Export.Start();
				return true;
			}
			catch(Exception e)
			{
				throw e;
			}
		}

		public enum GS2ExpArgument
		{
			Customer,
			Receiver,
			StartTime,
			EndTime,
			Protocol,
			FileName,
			Log,
			ValueType,
			TimsKey,
			Test,
			Method,
			MailTo,
			NN
		}

		public enum ValueTypeType
		{
			ALL_NEW_VALUES = 1, 
			OK_VALUES = 2,
			ALL_VALUES_IF_OK = 3,
			ALL_VALUES = 4
		}


	}
}
