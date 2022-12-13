using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Timers;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class HeartbeatTimerTests
    {
        private Mock<ISettingsProvider> _settingsProvider;
        private Mock<IServiceEventLogger> _serviceEventLoggger;
        private TimerAdapter _timer;
        private Mock<IApiEventsListener> _listener;
        private HeartbeatTimer _heartbeatTimer;

        [SetUp]
        public void SetUp()
        {
            _settingsProvider = new Mock<ISettingsProvider>();
            _serviceEventLoggger = new Mock<IServiceEventLogger>();
            _timer = new TimerAdapter();
            
            _settingsProvider.Setup(x => x.GetSettings()).Returns(new Settings(null,0,null,1000,30000));

            _heartbeatTimer = new HeartbeatTimer(_settingsProvider.Object, _serviceEventLoggger.Object, _timer);
        }

        [Test]
        public void Run_TimerStarted()
        {
            _listener = new Mock<IApiEventsListener>();
            _heartbeatTimer.Listener = _listener.Object;
            _heartbeatTimer.Run();
            System.Threading.Thread.Sleep(2000);
            _listener.Verify(x => x.InvokeHeartbeat(), Times.AtLeastOnce());
        }
    }
}
