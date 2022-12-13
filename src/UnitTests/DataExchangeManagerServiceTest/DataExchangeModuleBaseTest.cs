using System;
using System.Threading;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest
{
    [TestFixture]
    public class DataExchangeModuleBaseTest
    {
        private class DummyModuleException : Exception
        {
        }

        private class DummyModule : TaskBasedModule
        {
            public bool IsFailing { get; set; }
            public bool IsRunThreadCalled { get; set; }

            public DummyModule(IServiceEventLogger serviceEventLogger) 
                : base(serviceEventLogger)
            {
            }

            public override string ModuleName
            {
                get { throw new NotImplementedException(); }
            }

            protected override TimeSpan SleepTime
            {
                get { return TimeSpan.FromMilliseconds(1);  }
            }

            protected override bool TryExecuteSingleTask()
            {
                IsRunThreadCalled = true;

                int nRuns = 0;
                while (!IsStopRequested && nRuns < 10)
                {
                    Thread.Sleep(1);
                    nRuns++;
                }
                if (IsFailing)
                {
                    throw new DummyModuleException();
                }
                return false;
            }
        }

        private DummyModule _dummyModule;

        [SetUp]
        public void SetUp()
        {
            //var dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            //dataExchangeMessageLogMock.SetupAllProperties();

            var dataExchangeApiMock = new Mock<IDataExchangeApi>();

            var serviceEventLoggerMock = new Mock<IServiceEventLogger>();

            _dummyModule = new DummyModule(serviceEventLoggerMock.Object);
        }

        [Test]
        public void Start_RunThreadIsCalled_ThreadRunsUntilStopIsCalled()
        {
            // Assign

            _dummyModule.IsFailing = false;
            _dummyModule.IsRunThreadCalled = false;

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.IsTrue(_dummyModule.IsRunThreadCalled);
        }

        [Test]
        public void Start_RunThreadIsCalledAndThreadThrowsException_ExceptionIsSet()
        {
            // Assign

            _dummyModule.IsFailing = true;
            _dummyModule.IsRunThreadCalled = false;

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.AreEqual(typeof(DummyModuleException), _dummyModule.FailureReason.GetType());
        }

        [Test]
        public void Stop_IsRunning_IsFalse()
        {
            // Assign

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.IsFalse(_dummyModule.IsRunning);
        }

        [Test]
        public void Stop_IsCalledTwice_IsStopped()
        {
            // Assign

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.IsFalse(_dummyModule.IsRunning);
        }
    }
}
