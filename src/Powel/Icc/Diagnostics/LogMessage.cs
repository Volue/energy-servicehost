using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using Powel.Icc.Data;

namespace Powel.Icc.Diagnostics
{
    /// <summary>
    /// Summary description for LogMessage.
    /// </summary>
    public class LogMessage
    {
        int id;
        int applicationID;
        int messageDefinitionID;
        DateTime timeStamp;
        string text;
        string[] arguments;
        private int _osUserId;
        private int _dbUserId;

        public LogMessage()
        {
        }

        public LogMessage(string text)
            : this()
        {
            this.text = text;
        }

        public LogMessage(int id, DateTime timeStamp, string text)
            : this()
        {
            this.id = id;
            this.timeStamp = timeStamp;
            this.text = text;
        }

        public LogMessage(DataRow dr, DataTable argumentsDataTable)
            : this()
        {
            string errorMessage = string.Empty;

            id = Util.ConvertTo<int>(dr, "rcount", ref errorMessage);
            applicationID = Util.ConvertTo<int>(dr, "logged_application_key", ref errorMessage);
            messageDefinitionID = Util.ConvertTo<int>(dr, "message_key", ref errorMessage);
            timeStamp = Util.ConvertFromIccDateTime(dr, "message_datetim", ref errorMessage);
            text = Util.ConvertTo<string>(dr, "descript", ref errorMessage);
            _dbUserId = Util.ConvertTo<int>(dr, "user_key", ref errorMessage);
            _osUserId = Util.ConvertTo<int>(dr, "osuser_key", ref errorMessage);
            if (!string.IsNullOrEmpty(errorMessage))
            {
                text = Util.AppendText(text, errorMessage);
            }

            if (argumentsDataTable != null)
                ReadArguments(id, argumentsDataTable);
        }

        public LogMessage(int id, int applicationID, int messageDefinitionID, DateTime timeStamp, string text)
            : this()
        {
            this.id = id;
            this.applicationID = applicationID;
            this.messageDefinitionID = messageDefinitionID;
            this.timeStamp = timeStamp;
            this.text = text;
        }

        public int ID
        {
            get { return id; }
        }

        public int ApplicationID
        {
            get { return applicationID; }
            set { applicationID = value; }
        }

        public int OsUserId
        {
            get { return _osUserId; }
        }

        public int DbUserId
        {
            get { return _dbUserId; }
        }

        public int MessageDefinitionID
        {
            get { return messageDefinitionID; }
            set { messageDefinitionID = value; }
        }

        public DateTime TimeStamp
        {
            get { return timeStamp; }
            set { timeStamp = value; }
        }

        public string Text
        {
            get { return text; }
            set { text = value; }
        }

        public string[] Arguments
        {
            get { return arguments; }
            set { arguments = value; }
        }

        public static LogMessage Fetch(int id)
        {
            DataTable dt = LogMessageData.FetchLogMessage(id);
            DataTable dtArguments = LogMessageData.FetchLogMessageArguments(id);
            return new LogMessage(dt.Rows[0], dtArguments);
        }

        public void FetchArguments()
        {
            ReadArguments(id, LogMessageData.FetchLogMessageArguments(id));
        }

        void ReadArguments(int logMessageID, DataTable dt)
        {
            var list = new List<string>();  // new ArrayList(); See https://docs.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1?view=netframework-4.8#performance-considerations
            var rcountIdx = dt.Columns.IndexOf("rcount");
            var numberIdx = dt.Columns.IndexOf("arg_number");
            var valueIdx = dt.Columns.IndexOf("arg_value");
            //foreach (DataRow dr in dt.Rows)    Bad performance related to for()
            for (int i = 0;i < dt.Rows.Count;i++)
            {
                DataRow dr = dt.Rows[i];
                if (logMessageID != (int)dr[rcountIdx])
                    continue;

                int number = (int)dr[numberIdx] - 1;
                string value = (string)dr[valueIdx];

                while (list.Count <= number)
                    list.Add("");

                list[number] = value;
            }

            arguments = list.ToArray();
        }

        public static void Log(int messageID, bool requiresSigning, string message)
        {
            LogMessageData.Log(messageID, requiresSigning, message);
        }
    }
}
