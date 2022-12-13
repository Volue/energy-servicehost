using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Timers;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class CompelloTimerTest
    {
        private Mock<ISettingsProvider> _settingsProvider;
        private Mock<IServiceEventLogger> _serviceEventLoggger;
        private TimerAdapter _timer;
        private Mock<IApiEventsListener> _listener;
        private Mock<IHeartbeatTimer> _heartbeatTimer;
        private CompelloTimer _compelloTimer;

        [SetUp]
        public void SetUp()
        {
            _settingsProvider = new Mock<ISettingsProvider>();
            _serviceEventLoggger = new Mock<IServiceEventLogger>();
            _heartbeatTimer = new Mock<IHeartbeatTimer>();
           
            _timer = new TimerAdapter();

            _settingsProvider.Setup(x => x.GetSettings()).Returns(new Settings(null, 0, null, 3000, 1000));

            _compelloTimer = new CompelloTimer(_settingsProvider.Object, _serviceEventLoggger.Object, _timer, _heartbeatTimer.Object);
        }

        [Test]
        public void Run_TimerStarted()
        {
            _listener = new Mock<IApiEventsListener>();
            _compelloTimer.Listener = _listener.Object;
            _compelloTimer.Run();
            System.Threading.Thread.Sleep(2000);
            _listener.VerifyGet(x => x.IsStarted, Times.AtLeastOnce());
        }

        [Test]
        public void Run_TimerNotStarted()
        {
            _listener = new Mock<IApiEventsListener>();
            _compelloTimer.Listener = _listener.Object;
            _compelloTimer.Run();
            System.Threading.Thread.Sleep(100);
            _listener.VerifyGet(x => x.IsStarted, Times.Never());
        }

        [Test]
        public void OnTimeEvent_ListenerAndHeardbeatStarted()
        {
            _listener = new Mock<IApiEventsListener>();
            _listener.SetupGet(x => x.IsStarted).Returns(false);         
            _compelloTimer.Listener = _listener.Object;
            _compelloTimer.Run();
            System.Threading.Thread.Sleep(2000);
            _listener.Verify(x => x.Start(), Times.AtLeastOnce());
            _heartbeatTimer.Verify(x=>x.Run(), Times.AtLeastOnce());            
        }
    }
}
