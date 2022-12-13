using System;
using System.Threading.Tasks;
using NServiceBus;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract
{
    public interface INServiceBusEndpoint : IDisposable
    {
        Task<bool> Start();
        void Stop();
        Task Send(ICommand message);
    }
}
