using System;
using System.Diagnostics;
using System.Text;
using Microsoft.Practices.Unity;
using NDesk.Options;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using log4net;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardMsmq;
using System.Configuration;
using System.Management;
using System.Runtime.InteropServices;
using Volue.Secrets.Storage;

namespace Powel.Icc.Messaging.DataExchangeManager.ExpressExportConnector
{
    class Program
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private static readonly IEventLogModuleItem EventLogModuleItem = new EventLogModuleItem(IccModule.M_EXPRESS_EXPORT_CONNECTOR);
        private static int GeneralInfoMessage = 1010;

        static int Main(string[] args)
        {
            var retVal = -1;

            try
            {
                Log.Debug("Enter");

                Log.Debug("Raw arguments: " + string.Join(" ", args));
                EventLogModuleItem.LogMessage(GeneralInfoMessage,
                    string.Format("Arguments: {0}", string.Join(" ", args)));

                var unityContainer = RegisterDependencies();
                var dataExchangeApi = unityContainer.Resolve<IDataExchangeApi>();

                var exportMessage = new DataExchangeExportMessage();
                exportMessage.Version = DataExchangeMessageBase.LatestVersion;

                bool help = false;

                var p = new OptionSet()
                {
                    {
                        "MessageLogId=", "The internal message log id, this is the msg_id column in message_header.",
                        (long v) => exportMessage.MessageLogId = v
                    },
                    { "Protocol=", "Is derived from ProtocolId.", v => exportMessage.Protocol = v },
                    {
                        "ProtocolId=",
                        "Only relevant for STANDARDMSMQ. Which application to use when importing this message to an SmG database, see system documentation for valid values.",
                        (int v) => exportMessage.ProtocolId = v
                    },
                    {
                        "MessageReference=",
                        "Unique Id of the message. Should be extracted from the message. Used for message log updating.",
                        v => exportMessage.MessageReference = v
                    },
                    {
                        "SenderId=",
                        "Communication address of the sender. It is recommended to skip this and use SenderKey.",
                        v => exportMessage.SenderId = v
                    },
                    {
                        "ReceiverId=",
                        "Communication address of the receiver. It is recommended to skip this and use ReceiverKey.",
                        v => exportMessage.ReceiverId = v
                    },
                    {
                        "SenderName=", "Name of the sender. It is recommended to skip this and use SenderKey.",
                        v => exportMessage.SenderName = v
                    },
                    {
                        "ReceiverName=", "Name of the receiver. It is recommended to skip this and use ReceiverKey.",
                        v => exportMessage.ReceiverName = v
                    },
                    {
                        "SenderKey=", "Numeric internal key of the sender in DB.",
                        (int v) => exportMessage.SenderKey = v
                    },
                    {
                        "ReceiverKey=", "Numeric internal key of the receiver in DB.",
                        (int v) => exportMessage.ReceiverKey = v
                    },
                    {
                        "Format=", "Can be used for PiP parameter setting in Azure module.",
                        v => exportMessage.Format = v
                    },
                    { "Country=", "Derived from ReceiverKey", v => exportMessage.Country = v },
                    {
                        "RoutingAddress=",
                        "STANDARDMSMQ: module:machine:port, MADES/ECPAMQP: module:receiverAddress. Default: ICC_DEFAULT_ROUTING_ADDRESS",
                        v => exportMessage.RoutingAddress = v
                    },
                    {
                        "Priority=", "Priority of the message: HIGH, NORMAL, LOW. Default: NORMAL",
                        v => exportMessage.Priority = v
                    },
                    {
                        "SubAddress=",
                        "Applikasjons-ID/Routing/MADES/ECPAMQP: BusinessType, Azure: Rules matching for setting message.properties.",
                        v => exportMessage.SubAddress = v
                    },
                    { "h|?|help", "", v => help = v != null }
                };

                try
                {
                    p.Parse(args);
                    if (exportMessage.RoutingAddress != null && exportMessage.RoutingAddress.ToUpper()
                            .Contains(StandardMsmqExportModule.MODULE_NAME))
                        exportMessage.Version = 0; // No ClaimHandler for internal MSMQ messages.
                }
                catch (OptionException)
                {
                    help = true;
                }

                if (Log.IsDebugEnabled)
                {
                    Log.Debug("Parsed arguments:");
                    Log.DebugFormat("\tMessageLogId={0}", exportMessage.MessageLogId);
                    Log.DebugFormat("\tProtocol={0}", exportMessage.Protocol);
                    Log.DebugFormat("\tProtocolId={0}", exportMessage.ProtocolId);
                    Log.DebugFormat("\tMessageReference={0}", exportMessage.MessageReference);
                    Log.DebugFormat("\tSenderId={0}", exportMessage.SenderId);
                    Log.DebugFormat("\tReceiverId={0}", exportMessage.ReceiverId);
                    Log.DebugFormat("\tSenderName={0}", exportMessage.SenderName);
                    Log.DebugFormat("\tReceiverName={0}", exportMessage.ReceiverName);
                    Log.DebugFormat("\tSenderKey={0}", exportMessage.SenderKey);
                    Log.DebugFormat("\tReceiverKey={0}", exportMessage.ReceiverKey);
                    Log.DebugFormat("\tFormat={0}", exportMessage.Format);
                    Log.DebugFormat("\tCountry={0}", exportMessage.Country);
                    Log.DebugFormat("\tRoutingAddress={0}", exportMessage.RoutingAddress);
                    Log.DebugFormat("\tPriority={0}", exportMessage.Priority);
                    Log.DebugFormat("\tSubAddress={0}", exportMessage.SubAddress);
                }

                if (help)
                {
                    Console.WriteLine(
                        "Program that puts a message read from standard input on the given MSMQ queue with the given meta data.");
#if DEBUG
                    Console.WriteLine("For metadata definition See:");
                    Console.WriteLine(
                        @"http://powelplanet.powelasa.powel.com/projects/newcomm/_layouts/WordViewer.aspx?id=/projects/newcomm/Shared%20Documents/commproj_IP.docx&Source=http%3A%2F%2Fpowelplanet%2Epowelasa%2Epowel%2Ecom%2Fprojects%2Fnewcomm%2FSitePages%2FHome%2Easpx%3F%26%26p%5FSortBehavior%3D0%26p%5FFileLeafRef%3DHandelsobjekt%2520og%2520tidsserier%252epptx%26%26PageFirstRow%3D1%26%26View%3D%7B7E1451E2%2D87D5%2D4CFA%2DB116%2DAFCBDD1D50BC%7D%26InitialTabId%3DRibbon%252EDocument%26VisibilityContext%3DWSSTabPersistence&DefaultItemOpen=1");
#endif
                    Console.WriteLine("");
                    p.WriteOptionDescriptions(Console.Out);
                    Console.WriteLine("\nMODULES: STANDARDMSMQ, STANDARDWS, COMPELLO, MADES, ECPAMQP, AZURE");
                    Console.WriteLine(
                        "         Given by RoutingAddress. Default if omitted: ICC_DEFAULT_ROUTING_ADDESSS.");
                    Console.WriteLine("Parameter relevance (Mandatory, Optional):");
                    Console.WriteLine("Parameter    | STANDARDMSMQ | STANDARDWS | COMPELLO | MADES | ECPAMQP | AZURE");
                    Console.WriteLine("SubAddress   |              |            |          |   M   |    M    |   O");
                    Console.WriteLine("ReceiverKey  |      M       |     M      |    M     |       |         | ");
                    Console.WriteLine("SenderKey    |      M       |     M      |    M     |       |         | ");
                    Console.WriteLine("ReceiverId   |      O       |     O      |    O     |       |         | ");
                    Console.WriteLine("SenderId     |      O       |     O      |    O     |       |         | ");
                    Console.WriteLine("Format       |              |            |          |       |         |   O");
                    Console.WriteLine("Country      |              |            |          |       |         |   O");
                    Console.WriteLine("ProtocolId   |              |            |          |       |         |   O");
                    Console.WriteLine("ReceiverName |              |            |          |       |         |   O");
                    Console.WriteLine("SenderName   |              |            |          |       |         |   O");
                    Console.WriteLine(
                        "\nFor SenderId, ReceiverId, SenderName, ReceiverName and Country use SenderKey and ReceiverKey as keys for DB lookup.");
                    Console.WriteLine("\nEXAMPLES:");
                    Console.WriteLine(
                        @"   ExpressExportConnector.exe -RoutingAddress ECPAMQP:50V-POWELBRP-ATJ -SubAddress testamqp -MessageReference ERRP-20200903092622213-0-b2 < C:\temp\ECP\ECP-melding-BRM-A40.txt");
                    return -1;
                }

                var msg = ReadStdIn();
                var msmqLimit = (4000 * 1024) - 2000; // MSMQ limit of 4Mb - Meta data.
                if (exportMessage.Version == 0 && msg.Length > msmqLimit)
                {
                    EventLogModuleItem.LogMessage( /*TooLargeMessage*/GeneralInfoMessage,
                        new string[] { $"Too Large message {msg.Length} > {msmqLimit}" });
                    EventLogModuleItem.Close();
                    return GeneralInfoMessage;
                }

                exportMessage.SetMessageData(msg, null);
                Log.Debug("Std in: " + msg);
                var enqueued = false;
                /* B-31268:
                 * Alternatives considered:
                 * - Using loadbalancer ip: This is only possible on non-transactional queues. We use transactional queues.
                 * - ICC_MSMQ_MACHINE_EXPORT with list of machines: MSMQ is built for messaging with reliable message transfer. So if the machine is down, the MSMQ manager keeps the message waiting for the machine to come up. It gives up after a long time (hours not seconds), and then the message is put on a Dead-letter-queue.
                 *   - Loops through this list but introduces separate call for checking that the remote machine is alive and ServiceHost is running. controlled by config variables.
                 *   - Machine names must be with domain (e.g. tdtrhsmgtrunka1.voluead.volue.com)
                 * Requirements:
                 * - Execution controlled by 2 config variables:
                 *   - CheckMachineLogon: Test connection to machine.
                 *   - CheckServiceStatus: Check that service has state Running.
                 * - Username and Password to logon to all machines has to be the same.
                 *   - User has to have privileges to logon and optionally request for service state.
                 *   - User has format: domain\user
                 * - ServiceName has to be the same for all machines.
                 */
                var machineName = dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName;
                bool checkServiceStatus = Convert.ToBoolean(ConfigurationManager.AppSettings["CheckServiceStatus"] ?? "false"),
                    checkMachineLogon = Convert.ToBoolean(ConfigurationManager.AppSettings["CheckMachineLogon"] ?? "false");
                ConnectionOptions connOpt = null;
                string serviceName = string.Empty;
                if (checkMachineLogon)
                {
                    connOpt = new ConnectionOptions();
                    connOpt.Username = new SecretsManager(IccConfiguration.Data.OracleConnectionString).Decrypt(ConfigurationManager.AppSettings["CheckMachineUsername"]); // @"domain\user"
                    connOpt.Password = new SecretsManager(IccConfiguration.Data.OracleConnectionString).Decrypt(ConfigurationManager.AppSettings["CheckMachinePassword"]);
                    serviceName = ConfigurationManager.AppSettings["CheckServiceName"];
                }

                while (!enqueued)
                {
                    try
                    {
                        /* B-31268: MSMQ was designed to work when the destination is temporarily unreachable.
                         * The message resides in the outgoing queue/queue manager for a while before MSMQ gives up and moves it to the dead letter queue.
                         * I forgot what the default timeout is but it is hours, not seconds.
                         * This is not good enough for us.
                         * Uniper has a load balancer and switches between computers.
                         * - load balancer IP works only on non-transactional queues. We use transactional.
                         * So we introduce a check for service running.
                         */
                        if (checkMachineLogon)
                        {
                            Log.Debug(
                                $"Checking machine: {dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName}");
                            var scope = new ManagementScope(
                                $@"\\{dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName}\root\cimv2",
                                connOpt); // computer.domain
                            scope.Connect();    // Throws COMException if unknown/down.
                            bool isRunning = true;
                            if (checkServiceStatus)
                            {
                                Log.Debug($"Checking {serviceName} state.");
                                isRunning = false;
                                var query = new ObjectQuery(
                                    $"select state from Win32_Service where name = '{serviceName}'");
                                var searcher = new ManagementObjectSearcher(scope, query);
                                var queryColl = searcher.Get();
                                foreach (var queryObj in queryColl)
                                {
                                    if (queryObj["state"].ToString() == "Running")
                                    {
                                        isRunning = true;
                                        break;
                                    }
                                }
                            }

                            if (!isRunning)
                            {
                                dataExchangeApi.IncrementExportMachineIndex();
                                if (machineName == dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName)
                                {
                                    // All machines tried. No accessible machine.
                                    retVal = 1;
                                    enqueued = true;
                                    Log.Error("All machines tried. Exiting.");
                                    break;
                                }

                                continue;
                            }
                        }

                        using (var transaction =
                               dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
                        {
                            transaction.Begin();
                            dataExchangeApi.EnqueueExportMessage(exportMessage, transaction);
                            transaction.Commit();
                            retVal = 0;
                            EventLogModuleItem.LogMessage(8200);
                        }

                        enqueued = true;
                    }
                    catch (COMException Ex) // The RPC server is unavailable
                    {
                        dataExchangeApi.IncrementExportMachineIndex();
                        Log.Info(Ex);
                        if (machineName == dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName)
                        {
                            // All machines tried. No accessible machine.
                            retVal = (int)Ex.HResult; // -2147023174:
                            enqueued = true;
                            Log.Error("All machines tried. Exiting.");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                EventLogModuleItem.LogMessage(8202, ex.Message);
                retVal = 8202;
                Console.Error.WriteLine(ex.ToString());
            }
            finally
            {
                EventLogModuleItem.Close();
            }
            Log.Debug($"Exit. retVal={retVal}");

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
