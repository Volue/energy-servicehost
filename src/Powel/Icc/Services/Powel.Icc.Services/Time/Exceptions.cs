using System;
using Powel.Icc.Common;
#if !TSS_Availability
using Powel.Icc.Interop;
#endif

namespace Powel.Icc.Data
{
	public class DatabaseConnectionException : ApplicationException
	{
		public DatabaseConnectionException(string message) : base(message)
		{
		}

		public DatabaseConnectionException(string message, 
		                                   Exception innerException) : base(message, innerException)
		{
		}
	}

	public class TransactionSerializationException : ApplicationException
	{
		public TransactionSerializationException(string message) 
			: base(message)
		{
		}

		public TransactionSerializationException(string message, 
		                                         Exception innerException) : base(message, innerException)
		{
		}
	}

	public class BrokenConnectionException : ApplicationException
	{
		string connectionString;

		public BrokenConnectionException() : base() 
		{
		}

		public BrokenConnectionException(string message) : base(message)
		{
		}
		
		public BrokenConnectionException(string message, string connectionString) 
			: base(message)
		{ 
			this.connectionString = connectionString;
		}
	}

#if !TSS_Availability
	public class IccException : IccBaseException
	{
		private string[] args;

		public IccException(int logMessageId, params string[] args)
		{
			this.id = logMessageId;
			this.args = args;
			try
			{
				message = String.Format(IccApplication.PrepareFormatString(LogMessageData.FetchMessageDefinition(this.id), LogFormat.Positional), args);
            }
			catch
			{
				message = "Missing description for this message ID";
			}
		}

		public IccException(int logMessageId, LogFormat logFormat, params string[] args)
		{
			this.id = logMessageId;
			this.args = args;
			try
			{
				this.message = String.Format(IccApplication.PrepareFormatString(LogMessageData.FetchMessageDefinition(this.id), logFormat), args);
			}
			catch
			{
				this.message = "Missing description for this message ID";
			}
		}

		public IccException(string message, int id, params string[] args)
			:base(id, message)
		{
			this.args = args;
		}

		public string[] Args => args;
	}
#endif	    
}
