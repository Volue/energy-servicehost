using System;
using System.Collections;
using System.Collections.Generic;

//using Powel.Icc.Interop;
//using Powel.Icc.Common;

namespace Powel.Icc.Common
{
	/// <summary>
	/// Summary description for Exception.
	/// </summary>
	public class IccBaseException : ApplicationException
	{
		protected int id;
		protected string message;

		protected IccBaseException()
		{
		}

		public IccBaseException(int id, string message)
		{
			this.id = id;
			this.message = message;
		}

		public int Id => this.id;

		public override string Message => this.message;
	}

    public class DataExchangeConfigurationException : Exception
    {
        public DataExchangeConfigurationException()
        {
        }

        public DataExchangeConfigurationException(string message) : base(message)
        {
        }

        public DataExchangeConfigurationException(string message, Exception innerException) : base(message, innerException)
        {
        }
    }

    public class DataExchangeQueueOperationFailedException : Exception
    {
        public DataExchangeQueueOperationFailedException()
        {
        }

        public DataExchangeQueueOperationFailedException(string message) : base(message)
        {
        }

        public DataExchangeQueueOperationFailedException(string message, Exception innerException) : base(message, innerException)
        {
        }

        public override string ToString()
        {
            var ret = base.ToString();
            ret += "\nData:";
            foreach (DictionaryEntry kv in Data)
            {
                ret += "\n" + kv.Key.ToString() + ": " + kv.Value ?? String.Empty;
            }
            return ret;
        }
    }

    public class DataExchangeInvalidMetadataException : ArgumentException
    {
        public long MessageId { get; private set; }
        public DataExchangeInvalidMetadataException(long messageId,string argument) : base("Invalid metadata.",argument)
        {
            MessageId = messageId;
        }

        public DataExchangeInvalidMetadataException(long messageId, string message, string argument) : base(message,argument)
        {
            MessageId = messageId;
        }

        public DataExchangeInvalidMetadataException(long messageId, string message, string argument, Exception innerException) : base(message, argument, innerException)
        {
            MessageId = messageId;
        }
        public DataExchangeInvalidMetadataException(string message) : base(message)
        {
        }

    }

    public class DataExchangeInvalidMessageException : Exception
    {
        private readonly string _messageId;

        public DataExchangeInvalidMessageException(string internalMessageId)
        {
            _messageId = internalMessageId;
        }

        public DataExchangeInvalidMessageException(string internalMessageId, string message) : base(message)
        {
            _messageId = internalMessageId;
        }

        public DataExchangeInvalidMessageException(string internalMessageId, string message, Exception innerException) : base(message, innerException)
        {
            _messageId = internalMessageId;
        }

        public string MessageId
        {
            get { return _messageId; }
        }
    }

    public class DataExchangeImportRunnerFailedException : Exception
    {
        public DataExchangeImportRunnerFailedException()
        {
        }

        public DataExchangeImportRunnerFailedException(string message) : base(message)
        {
        }

        public DataExchangeImportRunnerFailedException(string message, Exception innerException) : base(message, innerException)
        {
        }
    }
}
