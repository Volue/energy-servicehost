using System;
using System.Collections.Generic;
using System.ServiceModel;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;

namespace DataExchangeModuleStandardWsClient
{
    public class StandardWsClient
    {
        private readonly ChannelFactory<IStandardWs> _channelFactory;

        #region client factory

        public static StandardWsClient GetClient(string endpointAddress)
        {
            ChannelFactory<IStandardWs> channelFactory = new ChannelFactory<IStandardWs>(GetBasicHttpBinding(), new EndpointAddress(endpointAddress));
            return new StandardWsClient(channelFactory);
        }

        private static System.ServiceModel.Channels.Binding GetBasicHttpBinding()
        {
            BasicHttpBinding binding = new BasicHttpBinding
            {
                Security = { Mode = BasicHttpSecurityMode.None },
                OpenTimeout = TimeSpan.FromMinutes(1),
                CloseTimeout = TimeSpan.FromMinutes(1),
                ReceiveTimeout = TimeSpan.FromMinutes(10),
                SendTimeout = TimeSpan.FromMinutes(10),
                TransferMode = TransferMode.Buffered,
                HostNameComparisonMode = HostNameComparisonMode.StrongWildcard,
                MaxBufferPoolSize = 524288,
                MaxBufferSize = 2147483647,
                MaxReceivedMessageSize = 2147483647,
                ReaderQuotas = { MaxDepth = 32, MaxStringContentLength = 4194304, MaxArrayLength = 16384, MaxBytesPerRead = 4096, MaxNameTableCharCount = 16384 }
            };
            return binding;
        }

        #endregion

        public StandardWsClient(ChannelFactory<IStandardWs> channelFactory)
        {
            _channelFactory = channelFactory;
        }

        public GetExportResponse GetExport(int timeoutInSeconds)
        {
            return _channelFactory.CreateChannel().GetExport(timeoutInSeconds);
        }

        public AcknowledgeExportResponse SendExportAck(string externalReference)
        {
            return _channelFactory.CreateChannel().SendExportAck(externalReference);
        }

        public SetMessageStatusResponse SetMessageStatus(string externalReference, MessageStatusTypeEnum status, string statusText)
        {
            return _channelFactory.CreateChannel().SetMessageStatus(externalReference, status, statusText);
        }

        public SubmitImportResponse SubmitImport(Dictionary<string, string> metadata, string importData, string reference)
        {
            return _channelFactory.CreateChannel().SubmitImport(metadata, importData, reference);
        }

        public SubmitEventResponse SubmitEvent(EventLevelTypeEnum level, string eventText)
        {
            return _channelFactory.CreateChannel().SubmitEvent(level, eventText);
        }
    }
}
