using System;
using System.Diagnostics;
using System.Threading;
using log4net;

namespace Powel.Icc.Diagnostics
{
	public class CriticalLogger : ICriticalLogger
	{
        private static readonly ILog _log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

	    private TimeSpan _errorWait;
	    private readonly TimeSpan _minimumErrorWait;
	    private readonly TimeSpan _maximumErrorWait;
	    private const int AnErrorOccured = 6704;

		public CriticalLogger()
		{
			_minimumErrorWait = new TimeSpan(0, 0, 5);
			_maximumErrorWait = new TimeSpan(1, 0, 0);
			_errorWait = _minimumErrorWait;
		}

        public void Log(Exception ex, IEventLogModuleItem iccLog, EventLog windowsLog, bool retrying)
        {
            Log(ex, AnErrorOccured, iccLog, windowsLog, retrying);
        }

		public void Log(Exception ex, int messageKey, IEventLogModuleItem iccLog, EventLog windowsLog, bool retrying)
		{
			// We will try to write to Log4Net, the Windows Event Log and the ICC Event Log
			// and hope that at least one of them works.
            // The message given by messageKey must take ex.Message as parameter.

            _log.Error(ex.ToString());

			DateTime wait = DateTime.Now + _errorWait;
			string message;
			if (retrying)
			{
				_errorWait = new TimeSpan((long) (_errorWait.Ticks*1.25));

				if (_errorWait > _maximumErrorWait)
					_errorWait = _maximumErrorWait;

				message = String.Format("An error occurred. Will wait until {0} before continuing. The error was:\n{1}",wait, ex);
			}
			else
				message = String.Format("An error occurred. The error was:\n{0}", ex);

#if DEBUG
            Console.WriteLine(message);
#endif

            var loggerLock = new Object();
			lock (loggerLock)
			{
			    var msg = string.Empty;
				try
				{
					if (windowsLog != null)
						windowsLog.WriteEntry(message, EventLogEntryType.Error);
				}
				catch (Exception e)
				{
					msg = e.Message;
				}

				try
				{
					if (iccLog != null)
					{
					    string text = msg;  // We include exception from windows event log also.
					    if (!string.IsNullOrEmpty(text))
					        text += "\n";
                        text += ex.Message;
						// We have to trim the exception text before sending it to the ICC log.
						if (text.Length > 1000)
							text = text.Substring(0, 1000);
						iccLog.LogMessage(messageKey, text);
					}
				}
				catch (Exception e)
				{
					_log.Error(e.Message);
				}
			}

			while (retrying && DateTime.Now < wait)
				Thread.Sleep(1);
		}
	}
}
