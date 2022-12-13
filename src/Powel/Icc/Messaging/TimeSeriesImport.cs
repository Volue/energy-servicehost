using System;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Configuration;
using System.Text.RegularExpressions;
using System.Xml.Schema;
using System.Xml.Serialization;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Process;
using Powel.Icc.TimeSeries.TimeSeriesXML;
using Powel.Xml;
using Microsoft.Web.Services3;

namespace Powel.Icc.Messaging
{
	/// <summary>
	/// Summary description for MessageHandler.
	/// </summary>
	public class TimeSeriesImport : ServiceIterationBase
	{
		DateTime lastStatusMessageTime = DateTime.Now;
		//int statusMessageMinutes;
		TimeSeriesResponseWSPort responseService;
		string logpath;
		Validation validation;
		string systemID = "";
		
		public TimeSeriesImport(EventLog eventLog, EventLogModuleItem iccLog) : base(eventLog, iccLog)
		{
		}

		public override string ServiceIterationName
		{
			get { return "Time Series Import Service"; }
		}
		public override IccModule ServiceIterationModule
		{
			get { return IccModule.M_POWEL_ICC_TIME_SERIES_IMPORT_SERVICE; }
		}

		public override void Initialize()
		{
			LogToEventLog("Checking if database is ready.", EventLogEntryType.Information);

			bool retryLogon = true;

			string connectionString = Util.GetConfiguredConnectionString(false);
			
			while (retryLogon)
			{
				try
				{
					CheckDatabase();
					retryLogon = false;
				}
				catch (DatabaseNotReadyException ex)
				{
					LogToEventLog(
					              String.Format("The database ({0}) is not ready yet.",
					                            ex.ConnectionString), 
					              EventLogEntryType.Warning);
				}
			}

			responseService = new TimeSeriesResponseWSPort {Url = (String.IsNullOrEmpty(ConfigurationManager.AppSettings["TimeSeriesResponseUrl"]) 
																			? IccConfiguration.Messaging.TimeSeriesResponseUrl
																			: ConfigurationManager.AppSettings["TimeSeriesResponseUrl"])};													
			logpath = IccConfiguration.Messaging.LogPath;
			systemID = ConfigurationManager.AppSettings["SystemID"];

			string schemaPath = ConfigurationManager.AppSettings["SchemaPath"];
			validation = new Validation();
			// validation.ReadSchemas(ConfigurationManager.AppSettings["SchemaPath"]);
			// Only validate against specified schemas 
			validation.AddSchema(schemaPath + "\\Powel_TimeSeriesMessages_v.1.0.xsd", true);
			// This is included in TimeSeriesMessages_v.1.0.xsd
			// validation.AddSchema(schemaPath + "\\Powel_TimeSeriesData_v.1.0.xsd", false);
			// validation.AddSchema(schemaPath + "\\Powel_TimeSeriesEnums_v.1.0.xsd", false);
		}

		public override void RunIteration(out bool actualWorkDone)
		{
			DateTime start = DateTime.Now;
			actualWorkDone = false;

			using (IDbConnection connection = Util.OpenConnection())
			{
				using (IDbTransaction transaction = Util.OpenTransaction(connection))
				{
				    //Console.WriteLine("Try to claim input message");
					Message msg = MessageData.ClaimInputMessage(QueueMessageType.TIME_SERIES, connection, "claimed", systemID);
					
					bool validationOK = true;
					//duration = DateTime.Now - start;
					//Console.WriteLine("After DequeInputMessage {0} ms.", duration.TotalMilliseconds);15ms

					if (msg.message == null)
					{
						return;
					}
					transaction.Commit();
				    //Console.WriteLine("Row fetched");

                    using (IDbTransaction transaction2 = Util.OpenTransaction(connection))
                    {

                        #region validate

                        // Deactivated validation until we can validate entire SOAP message.
                        try
                        {
                            validation.Validate(msg.message);
                        }
                        catch (XmlSchemaException)
                        {
                            // Invalid messages should still be dequeued.
                            validationOK = false;
                            //transaction.Commit();
                            //throw;
                        }
                        catch (System.Xml.XmlException)
                        {
                            validationOK = false;
                            //transaction.Commit();
                        }

                        #endregion

                        SubmitTimeSeriesResponse response = null;
                        if (validationOK)
                        {
                            var parser = new SubmitTimeSeriesParser(msg.message);
                            response = parser.Import(logpath, iccLog);
                        }
                        else
                        {
                            // Here we need to trap all exceptions so we don't miss the transaction.commit
                            // down below, missing that will result in the message staying in VF_INPUT
                            // resulting in a endless stream of errors. maybe se should move this try to a
                            // higher level where we can place the transaction.commit in a finally
                            // clause.
                            try
                            {
                                int stpos = msg.message.IndexOf("soap:Body");
                                stpos = msg.message.IndexOf("submitTimeSeries", stpos);
                                const string regPattern = @"<(.*:)?MessageID>(.*)</(.*:)?MessageID>";
                                Regex reg = new Regex(regPattern, RegexOptions.IgnoreCase & RegexOptions.Singleline);
                                Match mat = reg.Match(msg.message.Substring(stpos));
                                string msgID = mat.Groups[2].Value;

                                StringCollection txt = new StringCollection {msgID + ": XML-ValidationError"};
                                response = new SubmitTimeSeriesResponse(msgID + "Response", msgID, StatusType.FAILED,
                                                                        txt, null);
                                iccLog.LogMessage(IccModule.M_XML_WEB_SERVICES, 5410,
                                                  new string[] {msgID, "XML-ValidationError"});
                            }
                            catch (System.Exception e)
                            {
                                StringCollection txt = new StringCollection();
                                txt.Add(e.ToString());
                                response = new SubmitTimeSeriesResponse(msg.message, "unkown", StatusType.FAILED, txt,
                                                                        null);
                                iccLog.LogMessage(IccModule.M_XML_WEB_SERVICES, 5410,
                                                  new string[] {"unknown", e.ToString()});
                            }
                        }
                        if (response != null)
                        {
                            XmlSerializer s = new XmlSerializer(typeof (SoapEnvelope));

                            MemoryStream mem = new MemoryStream();
                            SoapEnvelope se = new SoapEnvelope();
                            se.SetBodyObject(response, "http://www.powel.no/icc/ws/tsws/messages/1.0/");
                            s.Serialize(mem, se);
                            mem.Position = 0;
                            using (TextReader reader = new StreamReader(mem))
                            {
                                MessageData.EnqueueOutputMessage(QueueMessageType.TIME_SERIES_IMPORT_RESPONSE,
                                                                 reader.ReadToEnd(), null, systemID);
                            }
                        }
                        MessageData.DeleteInputMessage(msg.id, connection);
                        transaction2.Commit();
                    }
				}
			}
			
			TimeSpan duration = DateTime.Now - start;
			//Console.WriteLine("Processing took {0} ms.", duration.TotalMilliseconds);
			
			actualWorkDone = true;
		}

		void CheckDatabase()
		{
			try {
				IDbConnection connection = Util.OpenConnection();
				TimeSeriesData.Exists("test", connection);
			}
			catch (DatabaseNotReadyException)
			{
				throw;
			}
			catch (Exception ex)
			{
				string server = IccConfiguration.Data.Server;
				string user = IccConfiguration.Data.User;

				LogToEventLog(
				              String.Format("Logon to database server '{0}' with username '{1}' failed. The error was: {2}",
				                            server, user, ex.Message), 
				              EventLogEntryType.Error);

				throw;
			}

			LogToEventLog("Logon ok.", EventLogEntryType.Information);
		}
	}
}
