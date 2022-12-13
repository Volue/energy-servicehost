using System;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeCommon.Abstract
{
    public interface IWsLogicBase
    {
        void SubmitImport(DataExchangeImportMessage message, Action<bool> onCompletion);
        //void LogError(string error);
        //void LogError(int msgKey, params string[] errorArgs);
    }
}
