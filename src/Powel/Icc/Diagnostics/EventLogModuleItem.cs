using System;
using System.Data;
using Powel.Icc.Data;
using Powel.Icc.Interop;

namespace Powel.Icc.Diagnostics
{
	/// <summary>
	/// Class that represents a single module log in the ICC event log.
	/// Usage: EventLogModuleItem moduleLog = new EventLogModuleItem();
	///		   moduleLog.LogMessage(1, 1001, null);
	///		   moduleLog.Dispose()
	/// </summary>
	public class EventLogModuleItem : IEventLogModuleItem
	{
		string connectionString;
		IDbConnection connection;
		bool bLogOpen;
		bool disposed;
		IccModule moduleKey;
		int appKeyOwner;
		bool useLogKey;
		string name;
		string appendix;
		DateTime birthDT;
		
		public EventLogModuleItem()
		{
			this.connectionString = Util.GetConfiguredConnectionString(true);
			Resurrect();
		}

		public EventLogModuleItem(IccModule moduleKey) : this() {
			this.moduleKey = moduleKey;
			Open(moduleKey);
		}

		public EventLogModuleItem(IccModule moduleKey, string connectionString)
		{
			this.connectionString = connectionString;
			this.moduleKey = moduleKey;
			Resurrect();
			Open(moduleKey);
		}

		public IDbConnection Connection
		{
			get
			{
				return connection;
			}
		}

		public int AppKey
		{
			get
			{
				try
				{
					return LogMessageData.GetAppKey(connection);
				}
				catch (Exception ex)
				{
					// Try to reopen the log and retry.

					Console.WriteLine(ex.Message);
					Resurrect();
					Open(moduleKey, appKeyOwner, useLogKey, name, appendix);
					return AppKey;
				}
			}
		}

		public void Dispose()
		{
			if( !disposed )
			{
				if( bLogOpen)
				{
					LogMessageData.Exit(connection);
					bLogOpen = false;
				}

				if( connection != null)
				{
					connection.Dispose();
					connection = null;
					disposed=true;
				}
			}
		}

	    public void Resurrect()
		{
			connection = Util.OpenConnection(connectionString);
			disposed = false;
		}

		public void Open(IccModule moduleKey )
		{
			this.Open(moduleKey, 0, false, null, null);
		}

		public void Open(IccModule moduleKey, int appKeyOwner, bool useLogKey, string name, string appendix)
		{
			if(disposed)
				Resurrect();

			this.moduleKey = moduleKey;
			this.appKeyOwner = appKeyOwner;
			this.useLogKey = useLogKey;
			this.name = name;
			this.appendix = appendix;
			this.birthDT = DateTime.Now;

			LogMessageData.InitLog(moduleKey, appKeyOwner, useLogKey, name, appendix, connection);
			bLogOpen = true;
		}

		public void LogMessage(IccModule moduleKey, int messageKey, params string[] args)
		{
			if( !bLogOpen)
				this.Open(moduleKey);

			try
			{
				if (!LogMessageData.AppKeyExists(AppKey, connection))
					throw new InvalidOperationException(String.Format(
					                                                  "App key {0} does not exist in the database.", AppKey));

				LogMessageData.LogMessage(moduleKey, messageKey, args, connection);
			}
			catch (Exception ex)
			{
				// Try to reopen the log and retry.

				Console.WriteLine(ex.Message);
				Resurrect();
				Open(moduleKey, appKeyOwner, useLogKey, name, appendix);
				LogMessage(moduleKey, messageKey, args);
			}
		}

		public string GetLogMessage(IccModule moduleKey, int messageKey, params string[] args) {
			if (!bLogOpen)
				this.Open(moduleKey);

			try {
				if (!LogMessageData.AppKeyExists(AppKey, connection))
					throw new InvalidOperationException(String.Format(
					                                                  "App key {0} does not exist in the database.", AppKey));

				return LogMessageData.GetLogMessage(moduleKey, messageKey, args, connection);
			} catch (Exception ex) {
				// Try to reopen the log and retry.

				Console.WriteLine(ex.Message);
				Resurrect();
				Open(moduleKey, appKeyOwner, useLogKey, name, appendix);
				GetLogMessage(moduleKey, messageKey, args);
			}
			return "";
		}
		public void LogMessage(int messageKey)
		{
			this.LogMessage(messageKey, null);
		}

		public void LogMessage(int messageKey, params string[] args)
		{
			this.LogMessage(this.moduleKey, messageKey, args);
		}

		public void LogMessage(IccModule moduleKey, int messageKey)
		{
			this.LogMessage(moduleKey, messageKey, null);
		}

		public string GetLogMessage(int messageKey) {
			return this.GetLogMessage(messageKey, null);
		}

		public string GetLogMessage(int messageKey, params string[] args) {
			return this.GetLogMessage(this.moduleKey, messageKey, args);
		}

		public string GetLogMessage(IccModule moduleKey, int messageKey) {
			return this.GetLogMessage(moduleKey, messageKey, null);
		}
		public void LogDebugMessage(string message)
		{
			if( !bLogOpen)
				throw new InvalidOperationException("You must open the event log module before you can write to it.");

			try
			{
				LogMessageData.LogDebugMessage(message, connection);
			}
			catch (Exception ex)
			{
				// Try to reopen the log and retry.

				Console.WriteLine(ex.Message);
				Resurrect();
				Open(moduleKey, appKeyOwner, useLogKey, name, appendix);
				LogDebugMessage(message);
			}
		}

		//Recycles the log item based on recycle interval
		public void Recycle(int recycleInterval)
		{
			if(recycleInterval>0)
			{
				TimeSpan life = DateTime.Now - birthDT;
				if(life.Minutes>=recycleInterval)
				{
					this.Dispose();
					Open(moduleKey, appKeyOwner, useLogKey, name, appendix);
				}
			}
		}

		public void Close()
		{
			this.Dispose();
		}
	}
}
