using System;
using System.Collections.Generic;
using System.ServiceModel;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS;

namespace DataExchangeTestClient
{
    public class StandardWsClient
    {
        private readonly ChannelFactory<IStandardWS> _channelFactory;

        #region client factory

        public static StandardWsClient GetClient(string endpointAddress)
        {
            ChannelFactory<IStandardWS> channelFactory = new ChannelFactory<IStandardWS>(GetBasicHttpBinding(), new EndpointAddress(endpointAddress));
            return new StandardWsClient(channelFactory);
        }

        private static System.ServiceModel.Channels.Binding GetBasicHttpBinding()
        {
            BasicHttpBinding binding = new BasicHttpBinding();
            binding.Security.Mode = BasicHttpSecurityMode.None;
            binding.OpenTimeout = TimeSpan.FromMinutes(1);
            binding.CloseTimeout = TimeSpan.FromMinutes(1);
            binding.ReceiveTimeout = TimeSpan.FromMinutes(10);
            binding.SendTimeout = TimeSpan.FromMinutes(1);
            //binding.TransactionFlow = false;
            binding.TransferMode = TransferMode.Buffered;
            //binding.TransactionProtocol = TransactionProtocol.OleTransactions;
            binding.HostNameComparisonMode = HostNameComparisonMode.StrongWildcard;
            binding.MaxBufferPoolSize = 524288;
            binding.MaxBufferSize = 2147483647;
            //binding.MaxConnections = 10;
            binding.MaxReceivedMessageSize = 2147483647;
            binding.ReaderQuotas.MaxDepth = 32;
            binding.ReaderQuotas.MaxStringContentLength = 8192;
            binding.ReaderQuotas.MaxArrayLength = 16384;
            binding.ReaderQuotas.MaxBytesPerRead = 4096;
            binding.ReaderQuotas.MaxNameTableCharCount = 16384;
            //binding.Security.Transport.ProtectionLevel = System.Net.Security.ProtectionLevel.EncryptAndSign;

            return binding;
        }

        #endregion

        public StandardWsClient(ChannelFactory<IStandardWS> channelFactory)
        {
            _channelFactory = channelFactory;
        }

        public SubmitImportResponse SubmitImport(Dictionary<string, string> metaData, string importData)
        {
            return _channelFactory.CreateChannel().SubmitImport(metaData, importData);
        }

        public GetExportsResponse GetExports(int maxNumberOfExportsInResponse, int timeoutInSeconds)
        {
            return _channelFactory.CreateChannel().GetExports(maxNumberOfExportsInResponse, timeoutInSeconds);
        }

        public AcknowledgeExportsResponse SendExportsAck(long responseId)
        {
            return _channelFactory.CreateChannel().SendExportsAck(responseId);
        }

        public AcknowledgeExportsResponse SendExportsNak(long responseId, string reasonText)
        {
            return _channelFactory.CreateChannel().SendExportsNak(responseId, reasonText);
        }

        public AcknowledgeExportsResponse SendExportsAckAndSetTransactionIds(long responseId, Dictionary<string, string> transactionIdMapping)
        {
            return _channelFactory.CreateChannel().SendExportsAckAndSetTransactionIds(responseId, transactionIdMapping);
        }

        public SetMessageStatusResponse SetMessageStatus(string transactionId, MessageStatusTypeEnum status, string statusText)
        {
            return _channelFactory.CreateChannel().SetMessageStatus(transactionId, status, statusText);
        }

        public SubmitImportResponse SubmitImportAndSetTransactonId(Dictionary<string, string> metadata, string importData, string transactionId)
        {
            return _channelFactory.CreateChannel().SubmitImportAndSetTransactonId(metadata, importData, transactionId);
        }

        public SubmitEventResponse SubmitEvent(EventLevelTypeEnum level, string eventText)
        {
            return _channelFactory.CreateChannel().SubmitEvent(level, eventText);
        }
    }
}