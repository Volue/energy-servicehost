using System;
using System.Collections.Generic;

namespace Powel.Icc.Data
{
    /// <summary>
    /// Parameter container used to keep track of filtering parameters
    /// </summary>
    public class MessageLogQueryParameters
    {
        private string _persistedState = "";

        public void SetDefaults()
        {
            FilterId = -1;
            FilterName = "";
            FilterDescription = "";
            Import = true;
            Export = true;
            IncludeAck = false;
            IncludeHandled = false;
			EndTime = DateTime.Today; //DateTime.MinValue;
			StartTime = EndTime.AddDays(-7); //DateTime.MinValue;
            CreatedDate = true;
            TransactionPeriod = false;
            TransferDate = false;
            MeasurePointName = "";
            MessageReference = "";
            TransactionReference = "";
            SenderName = "";
            ReceiverName = "";
            ComponentCode = "";
            ProductCode = "";
            ExchangeMethodName = "";
            transactionStatus.Clear();
            protocolType.Clear();
            messageStatus.Clear();
            ActorType = -1;
            ActorName = "";
            RoutingAddress = "";
            ExternalReference = "";
            FileName = "";
            GlobalReference = "";
        }

        public Int64 FilterId { get; set; }
        public string FilterName { get; set; }
        public string FilterDescription { get; set; }
        public bool Import { get; set; }
        public bool Export { get; set; }
        public DateTime StartTime { get; set; }
        public DateTime EndTime { get; set; }
        public bool CreatedDate { get; set; }
        public bool TransactionPeriod { get; set; }
		public bool TransferDate { get; set; }
        public string MeasurePointName { get; set; }
        public string MessageReference { get; set; }
        public string TransactionReference { get; set; }
        public string SenderName { get; set; }
        public string ReceiverName { get; set; }
        public string ComponentCode { get; set; }
        public string ProductCode { get; set; }
        public string ExchangeMethodName { get; set; }
        public int ActorType { get; set; }
        public string ActorName { get; set; }
        public bool IncludeAck { get; set; }
        public bool IncludeHandled { get; set; }
        public string RoutingAddress { get; set; }
        public string ExternalReference { get; set; }
        public string FileName { get; set; }
        public string GlobalReference { get; set; }

        List<int> transactionStatus = new List<int>();
        List<int> protocolType = new List<int>();
        List<int> messageStatus = new List<int>();

        public bool IsOneWeekInterval
        {
            get { return (EndTime - StartTime).TotalDays == 7.0; }
        }

        public bool Is14DaysInterval 
        { 
            get  { return (EndTime - StartTime).TotalDays == 14.0; } 
        }

        public bool IsMonthInterval 
        { 
            get {
                // February is quite kinky since 30.03 - 1 month = 28.02 and 28.02 + 1 month = 28.03
                if (StartTime.Month == 2 || EndTime.Month == 2 &&
                    (EndTime - StartTime).Days == 30)
                {
                    return true;
                }
                return (StartTime.AddMonths(1) == EndTime); 
            } 
        }

        public bool IsQuarterInterval 
        { 
            get { return (StartTime.AddMonths(3) == EndTime); } 
        }

        public bool IsYearInterval 
        { 
            get { return (StartTime.AddYears(1) == EndTime); } 
        }

        public bool IsOtherInterval
        {
            get
            {
				return !IsOneWeekInterval && !Is14DaysInterval && !IsMonthInterval && !IsQuarterInterval && !IsYearInterval && StartTime != DateTime.MinValue;
            }
        }

        public bool AddTransactionStatus(int newstatus)
        {            
            if (!transactionStatus.Contains(newstatus))
            {
                transactionStatus.Add(newstatus);
                return true;
            }
            return false;
        }

        public bool RemoveTransactionStatus(int delstatus)
        {
            if (transactionStatus.Contains(delstatus))
            {
                transactionStatus.Remove(delstatus);
                return true;
            }
            return false;
        }

        public int[] TransactionStatus
        {
            get { return transactionStatus.ToArray(); }
        }

        public bool AddProtocolType(int newproto)
        {
            if (!protocolType.Contains(newproto))
            {
                protocolType.Add(newproto);
                return true;
            }
            return false;
        }

        public bool RemoveProtocolType(int delproto)
        {
            if (protocolType.Contains(delproto))
            {
                protocolType.Remove(delproto);
                return true;
            }
            return false;
        }

        public int[] ProtocolType
        {
            get { return protocolType.ToArray(); }
        }

        public bool AddMessageStatus(int newms)
        {
            if (!messageStatus.Contains(newms))
            {
                messageStatus.Add(newms);
                return true;
            }
            return false;
        }

        public bool RemoveMessageStatus(int delms)
        {
            if (messageStatus.Contains(delms))
            {
                messageStatus.Remove(delms);
                return true;
            }
            return false;
        }

        public int[] MessageStatus
        {
            get { return messageStatus.ToArray(); }
        }

        public bool ContainsProtocol(int prot_key)
        {
            return protocolType.Contains(prot_key);
        }

        private static string IntArrToString(int[] array)
        {
            string str = "";

            if (array.GetLength(0) > 0)
            {
                foreach (int i in array)
                {
                    str += Convert.ToString(i) + ".";
                }

                // Remove last separator character
                str = str.Substring(0, str.Length - 1);
            }

            return str;
        }

        public static string AsSerializedString(MessageLogQueryParameters parameters)
        {
            string str = parameters.FilterId.ToString();
            str += "," + parameters.FilterName;
            str += "," + parameters.FilterDescription;
            str += "," + parameters.Import.ToString();
            str += "," + parameters.Export.ToString();
            str += "," + parameters.StartTime.ToString("yyyyMMddHHmmss");
            str += "," + parameters.EndTime.ToString("yyyyMMddHHmmss");
            str += "," + parameters.CreatedDate.ToString();
            str += "," + parameters.TransactionPeriod.ToString();
            str += "," + parameters.TransferDate.ToString();
            str += "," + parameters.MeasurePointName;
            str += "," + parameters.MessageReference;
            str += "," + parameters.TransactionReference;
            str += "," + parameters.SenderName;
            str += "," + parameters.ReceiverName;
            str += "," + parameters.ComponentCode;
            str += "," + parameters.ActorType.ToString();
            str += "," + parameters.ActorName;
            str += "," + parameters.ProductCode;
            str += "," + parameters.ExchangeMethodName;
            str += "," + parameters.RoutingAddress;
            str += "," + parameters.ExternalReference;
            str += "," + parameters.IncludeAck.ToString();
            str += "," + parameters.IncludeHandled.ToString();
            str += "," + IntArrToString(parameters.TransactionStatus);
            str += "," + IntArrToString(parameters.ProtocolType);
            str += "," + IntArrToString(parameters.MessageStatus);
            str += "," + parameters.FileName;
            str += "," + parameters.GlobalReference;

            return str;
        }

        public void SetPersistedState()
        {
            _persistedState = MessageLogQueryParameters.AsSerializedString(this);
        }

        public bool IsChanged()
        {
            return (!string.IsNullOrEmpty(_persistedState) && (_persistedState != MessageLogQueryParameters.AsSerializedString(this)));
        }
    }
}
