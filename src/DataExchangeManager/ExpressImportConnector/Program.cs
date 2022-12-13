using System;
using System.Diagnostics;
using System.Text;
using log4net;
using Microsoft.Practices.Unity;
using NDesk.Options;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.ExpressImportConnector
{
    class Program
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private static readonly IEventLogModuleItem EventLogModuleItem = new EventLogModuleItem(IccModule.M_EXPRESS_IMPORT_CONNECTOR);
        private static int GeneralInfoMessage = 1010;

        static int Main(string[] args)
        {
            var retVal = -1;

            try
            {
                Log.Debug("Enter");

                Log.Debug("Raw arguments: " + string.Join(" ", args));
                EventLogModuleItem.LogMessage(GeneralInfoMessage, string.Format("Arguments: {0}", string.Join(" ", args)));

                var importMessage = new DataExchangeImportMessage();

                var unityContainer = RegisterDependencies();
                var dataExchangeApi = unityContainer.Resolve<IDataExchangeApi>();

                bool help = false;

                var p = new OptionSet()
                    {                        
                        {"Protocol=", "Which application to use when importing this message to an SmG database, see system documentation for valid values.", v => importMessage.Protocol = v},
                        {"ProtocolId=", "", (int v) => importMessage.ProtocolId = v},                        
                        {"SenderId=", "", v => importMessage.SenderId = v},
                        {"ReceiverId=", "", v => importMessage.ReceiverId = v},
                        {"SenderName=", "", v => importMessage.SenderName = v},
                        {"ReceiverName=", "", v => importMessage.ReceiverName = v},                                                
                        {"Format=", "", v => importMessage.Format = v},
                        {"Country=", "", v => importMessage.Country = v},
                        {"RoutingAddress=", "", v => importMessage.RoutingAddress = v},
                        {"Priority=", "", v => importMessage.Priority = v},
                        {"SubAddress=","Applikasjons-ID/Routing/BusinessType", v => importMessage.SubAddress = v},
                        {"h|?|help", "", v => help = v != null }
                    };

                try
                {
                    p.Parse(args);
                }
                catch (OptionException)
                {
                    help = true;
                }

                if (Log.IsDebugEnabled)
                {
                    Log.Debug("Parsed arguments:");                    
                    Log.DebugFormat("\tProtocol={0}", importMessage.Protocol);
                    Log.DebugFormat("\tProtocolId={0}", importMessage.ProtocolId);                    
                    Log.DebugFormat("\tSenderId={0}", importMessage.SenderId);
                    Log.DebugFormat("\tReceiverId={0}", importMessage.ReceiverId);
                    Log.DebugFormat("\tSenderName={0}", importMessage.SenderName);
                    Log.DebugFormat("\tReceiverName={0}", importMessage.ReceiverName);                                        
                    Log.DebugFormat("\tFormat={0}", importMessage.Format);
                    Log.DebugFormat("\tCountry={0}", importMessage.Country);
                    Log.DebugFormat("\tRoutingAddress={0}", importMessage.RoutingAddress);
                    Log.DebugFormat("\tPriority={0}", importMessage.Priority);
                }

                if (help)
                {
                    p.WriteOptionDescriptions(Console.Out);
                    return -1;
                }

                try
                {
                    var msgDta = ReadStdIn();
                    importMessage.SetMessageData(msgDta,null);

                    Log.Debug("Std in: " + msgDta);

                    using (var transaction = dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
                    {
                        transaction.Begin();

                        dataExchangeApi.EnqueueImportMessage(importMessage, transaction);

                        transaction.Commit();

                        retVal = 0;

                        EventLogModuleItem.LogMessage(8200);
                    }

                }
                catch (Exception ex)
                {
                    EventLogModuleItem.LogMessage(8202, ex.Message);
                    retVal = 8202;
                    throw;
                }

                Log.Debug("Exit");

                EventLogModuleItem.Close();
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                Console.Error.WriteLine(ex.ToString());
                EventLogModuleItem.Close();
            }

            return retVal;
        }

        private static string ReadStdIn()
        {
            Log.Debug("Enter");

            var sb = new StringBuilder();
            using (var stdin = Console.OpenStandardInput())
            {
                var buffer = new byte[2048];
                int bytes;
                while ((bytes = stdin.Read(buffer, 0, buffer.Length)) > 0)
                {
                    sb.Append(Encoding.Default.GetString(buffer, 0, bytes));
                }
            }
            return sb.ToString();
        }

        private static UnityContainer RegisterDependencies()
        {
            var unityContainer = new UnityContainer();
            unityContainer.RegisterInstance<IEventLogModuleItem>(EventLogModuleItem);
            unityContainer.RegisterInstance<EventLog>(new EventLog());
            unityContainer.RegisterType<ICriticalLogger, CriticalLogger>();
            unityContainer.RegisterType<IServiceEventLogger, ServiceEventLogger>();
            unityContainer.RegisterModule<DataExchangeApiModule>();
            return unityContainer;
        }
    }
}
