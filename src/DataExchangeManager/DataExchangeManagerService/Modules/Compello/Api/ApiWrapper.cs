using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using EDI.Server.API.Client;
using log4net;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api
{
    /// <summary>
    /// this class is not disposable, instead it creates new api client each time any method is called and disposes it on her own
    /// </summary>
    public class ApiWrapper : IApiWrapper
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private readonly Settings _settings;

        public ApiWrapper(Settings settings)
        {
            _settings = settings;
        }

        public long SendMessage(Stream data, Dictionary<string, object> metaData)
        {
            if (Log.IsInfoEnabled) Log.Info("SendMessage");
            using (var apiClient = CreateApiClient())
            {
                return apiClient.SendMessage(data, metaData);
            }
        }

        public string GetTransactionData(long messageId,Encoding encoding)
        {
            using (var apiClient = CreateApiClient())
            {
                Stream transaction = apiClient.GetTransaction(messageId);

                using (var streamReader = new StreamReader(transaction,encoding))
                {
                    return streamReader.ReadToEnd();
                }
            }
        }

        private ApiClient CreateApiClient()
        {
            if (Log.IsDebugEnabled) Log.Debug($"CreateApiClient: HostAddress={_settings.HostAddress}, Port={_settings.Port}, ApiKey={_settings.ApiKey}");            
            return _settings.Port > 0 ? new ApiClient(_settings.HostAddress, _settings.Port, _settings.ApiKey) : new ApiClient(_settings.ApiKey,_settings.HostAddress);
        }
    }
}