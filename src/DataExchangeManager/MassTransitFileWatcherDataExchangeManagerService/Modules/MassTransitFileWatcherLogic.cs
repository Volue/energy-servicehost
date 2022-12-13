using System;
using System.IO;
using System.Linq;
using log4net;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Settings;
using Volue.MassTransit.Contracts.TimeSeriesMessage.ActivationRequestModels;
using Volue.MassTransit.RabbitMq;
using Volue.MassTransit.RabbitMq.Configuration;
using Volue.MassTransit.Extensions;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Modules
{
    public class MassTransitFileWatcherLogic /*: WsLogicBase*/
    {
        /* Stores message payload to a file with name = FileNameTemplate. Template can include {%mt} and {%t}
         * {%mt}: MessageType
         * {%t}: DateTime.Now
         */
        protected static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        public MassTransitFileWatcherParameters.MessageFileParameters[] MessageFileParametersArray { get; set; }
        public MassTransitFileWatcherParameters.FileWatcherParameters[] FileWatcherParametersArray { get; set; }

        #region ExportLogic

        private bool SaveToFile(string messageType, string message)
        {
            var msgTpParams = MessageFileParametersArray.FirstOrDefault(mtp => mtp.MessageType == messageType );
            if (msgTpParams == null)
            {
                Log.Error($"Cannot find messageTypeParameters for {messageType}");
                return false;
            }

            if (!Directory.Exists(msgTpParams.Directory))
            {
                Directory.CreateDirectory(msgTpParams.Directory);
            }

            string importFileName = msgTpParams.FileNameTemplate;
            importFileName = importFileName.Replace("{%mt}", messageType);
            importFileName = importFileName.Replace("{%t}", DateTime.Now.ToString("O"));

            using (var fStream = new FileStream(Path.Combine(msgTpParams.Directory, importFileName), FileMode.Create, FileAccess.Write))
            {
                using (var sWriter = msgTpParams.Encoding == null ? new StreamWriter(fStream) : new StreamWriter(fStream, Encoding.GetEncoding(msgTpParams.Encoding)))
                {
                    sWriter.Write(message);
                }
            }

            Log.Debug($"Message written to {importFileName}");
            return true;
        }

        public Task SubmitExport(TimeSeriesMessage message)
        {
            Log.Debug($"[{DateTime.Now}: Received message {message.TimeSeriesHeader.DocumentIdentification}");
            if (!SaveToFile(typeof(TimeSeriesMessage).Name, JsonConvert.SerializeObject(message)))
                Log.Error("SaveToFile failed.");
            return Task.CompletedTask;
        }

        #endregion

        #region ImportLogic

        private void SendMassTransit(MassTransitFileWatcherParameters.FileWatcherParameters fileWatcherParameters, string fullPathFileName)
        {
            var configuration = MassTransitRabbitMqSetup.BusConfiguration
                .StartNew()
                .ConfigureConnection(new RabbitMqConnectionConfiguration
                {
                    Host = new Uri(fileWatcherParameters.Host)
                })
                .Create();

            var busControl = MassTransitRabbitMqSetup.CreateBusControl(configuration);
            busControl.StartAsync().GetAwaiter();
            var messageProducerFactory = busControl.CreateMessageProducerFactory();
            switch (fileWatcherParameters.MessageType)
            {
                case "AcknowledgeMessage":
                {
                    //var messageProducer = messageProducerFactory.CreateFor<AcknowledgeMessage>(MassTransitRabbitMqSetup.EndpointAddress
                    //    .StartNew()
                    //    .SetName(typeof(AcknowledgeMessage).FullName)
                    //    .SetSchema(EndpointSchema.Queue).Create());
                    //using (var strReader = new StreamReader(fullPathFileName))
                    //{
                    //    var message = JsonConvert.DeserializeObject<AcknowledgeMessage>(strReader.ReadToEnd());
                    //    messageProducer.SendMessage(message);
                    //}
                }
                    break;
                default:
                    Log.Error($"Unknown message type {fileWatcherParameters.MessageType}");
                    break;
            }
        }

        public void SubmitImport(string fullPathFileName)
        {
            var fwPar = FileWatcherParametersArray.FirstOrDefault(p =>
            {
                var re = new FilenameWildCardPattern(Path.Combine(p.Directory, p.FileNameTemplate));
                return re.IsMatch(fullPathFileName);
            });
            if (fwPar == null)
            {
                Log.Error($"Did not find a match on {fullPathFileName}");
                return;
            }
            SendMassTransit(fwPar, fullPathFileName);
        }

        #endregion
    }
}
