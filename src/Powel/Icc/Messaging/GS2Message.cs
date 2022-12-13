using System;
using System.Collections;
using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;
using TS = Powel.Icc.TimeSeries;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging
{
	/// <summary>
	/// Summary description for GS2Message.
	/// </summary>
	public class GS2Message
	{
		const string dateFormat = "yyyy-MM-dd.HH:mm:ss";

		RegionalCalendar calendar = RegionalCalendar.UtcCalendar;

		string messageType;
		internal string to;
		internal string from;
		internal string gmtReference;
		internal string id;
		internal UtcTime time;
		internal ArrayList objects = new ArrayList();

		public GS2Message()
		{
			this.id = Guid.NewGuid().ToString();
			this.time = UtcTime.Now;
		}

		public GS2Message(string gs2Text)
		{
			int start = -1;
			int end = -1;
			int pos;

			do 
			{
				pos = gs2Text.IndexOf("##", end + 1);

				if (start == -1)
				{
					start = pos;
					end = pos;
				}
				else
				{
					start = end;
					end = pos;

					if (end == -1)
						end = gs2Text.Length;
				}

				if (end > start)
					AddObject(gs2Text.Substring(start, end - start));
			} 
			while (pos != -1);
		}

		public GS2Message(string messageType, string to, string from, string gmtReference)
		{
			this.messageType = messageType;
			this.to = to;
			this.from = from;
			this.gmtReference = gmtReference;
			this.id = Guid.NewGuid().ToString();
			this.time = UtcTime.Now;

			AddStartMessage();
			AddEndMessage();
		}

		public string ID
		{
			get { return id; }
		}

		public string MessageType
		{
			get { return messageType; }
		}

		public bool IsEmpty
		{
			get { return objects == null || objects.Count == 0; }
		}

		public void AddStartMessage()
		{
			string startMessage = String.Format(
				@"##Start-message
#Id={0}
#Message-type={1}
#Version=1.2
#Time={2}
#To={3}
#From={4}
#GMT-reference={5}" + "\r\n\r\n",
				id, messageType, 
				calendar.ToDateTime(time).ToString(dateFormat),
				to, from, gmtReference);

			objects.Add(new GS2MessageObject("Start-message", startMessage));
		}

		void AddEndMessage()
		{
			string endMessage = String.Format(
				@"##End-message
#Id={0}" + "\r\n",
				id);

			objects.Add(new GS2MessageObject("End-message", endMessage));
		}

		/// <summary>
		/// Adds a time series object to the GS2 message. This method
		/// assumes that the GS2 message already contains at least 
		/// Start-message and End-message.
		/// </summary>
		public void AddTimeSeries(TS.TimeSeries ts)
		{
			// GS2 specifies dot as the decimal separator.
			NumberFormatInfo provider = CultureInfo.InvariantCulture.NumberFormat;

			foreach (TS.Tvq tvq in ts)
			{
				string messageObject = String.Format(provider,
					@"##Meter-reading
#Reference={0}
#Time={1}
#Unit={2}
#Value={3}" + "\r\n\r\n",
					ts.Name, 
					calendar.ToDateTime(tvq.Time).ToString(dateFormat), 
					ts.Unit, tvq.Value);

				objects.Insert(objects.Count-1, 
					new GS2MessageObject("Meter-reading", messageObject));
			}
		}

		void AddObject(string messageObject)
		{
			string objectType = RegexMatch(messageObject, @"##(.*)");

			objects.Add(new GS2MessageObject(objectType, messageObject));

			if (objectType == "Start-message")
			{
				id = RegexMatch(messageObject, @"#Id=(.*)");
				messageType = RegexMatch(messageObject, @"#Message-type=(.*)");
				to = RegexMatch(messageObject, @"#To=(.*)");
				from = RegexMatch(messageObject, @"#From=(.*)");
				gmtReference = RegexMatch(messageObject, @"#GMT-reference=(.*)");
			}
		}

		string RegexMatch(string input, string pattern)
		{
			return new Regex(pattern).Match(input).Groups[1].Value.Trim();
		}

		public void AppendGS2Message(GS2Message message)
		{
			foreach (GS2MessageObject messageObject in message.objects)
			{
				bool isEmpty = (objects.Count == 0);

				if (isEmpty || (messageObject.Type != "Start-message" && messageObject.Type != "End-message"))
				{
					if (isEmpty)
					{
						// messageObject is Start-message

						this.messageType = message.messageType;
						this.to = message.to;
						this.from = message.from;
						this.gmtReference = message.gmtReference;

						AddStartMessage();
						AddEndMessage();
					}
					else
						objects.Insert(objects.Count - 1, messageObject);
				}
			}
		}

		public override string ToString()
		{
			var message = new StringBuilder();

			foreach (GS2MessageObject messageObject in objects)
				message.Append(messageObject.Text);

			return message.ToString();
		}

		public class GS2MessageObject
		{
			string type;
			string text;

			public GS2MessageObject(string type, string text)
			{
				this.type = type;
				this.text = text;
			}

			public string Type
			{
				get { return type; }
			}

			public string Text
			{
				get { return text; }
			}
		}
	}
}
