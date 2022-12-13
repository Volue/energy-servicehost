using System;
using System.Collections.Concurrent;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    public static class ClaimCheckFactory
    {
        private static ConcurrentDictionary<string, IClaimHandler> _handlers = new ConcurrentDictionary<string, IClaimHandler>();
        public static IClaimHandler Create(string HandlerName)
        {
            string handlerName = string.IsNullOrEmpty(HandlerName)
                ? IccConfiguration.ImportExport.IccDataExchangeManagerClaimHandler
                : HandlerName;
            IClaimHandler handler = null;
            if (_handlers.TryGetValue(handlerName, out handler))
                return handler;

            if (handlerName == LocalFileshareClaimHandler.HandlerId)
                handler = new LocalFileshareClaimHandler();
            else if (handlerName == CloudBlobClaimHandler.HandlerId)
                handler = new CloudBlobClaimHandler();
            else if (handlerName == DatabaseClaimHandler.HandlerId)
                handler = new DatabaseClaimHandler();
            if (handler == null)
                throw new ArgumentOutOfRangeException("ClaimCheckFactory.Create(HandlerName)",handlerName,"Not implemented");

            _handlers[handler.Name] = handler;
            return handler;
        }
    }
}
