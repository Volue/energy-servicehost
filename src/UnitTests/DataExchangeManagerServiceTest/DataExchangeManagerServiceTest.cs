using System;
using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest
{
    [TestFixture]
    public class DataExchangeManagerServiceTest
    {
        public class DummyModule : IDataExchangeModule
        {
            public bool IsStartCalled { get; set; }
            public bool IsStopCalled { get; set; }
            public bool IsAbortModuleThreadCalled { get; set; }
            public bool IsRunThreadCalled { get; set; }
            public bool IsHanging { get; set; }

            private bool _isRunning;

            public void Start()
            {
                IsStartCalled = true;
                _isRunning = true;
            }

            public void RequestStop()
            {
                IsStopCalled = true;
            }

            public void Stop(TimeSpan timeout)
            {
                IsStopCalled = true;

                if(!IsHanging)
                {
                    _isRunning = false;
                }
            }

            public void Abort()
            {
                IsAbortModuleThreadCalled = true;
                _isRunning = false;
            }

            public string ModuleName { get; private set; }
            public Exception FailureReason { get; private set; }
            public bool IsExecutingJobRightNow { get { return false; } }

            public bool IsRunning
            {
                get { return _isRunning; }
            }

            protected void RunThread()
            {
                IsRunThreadCalled = true;
            }
        }

        public class DummyModule1 : DummyModule
        {
        }

        public class DummyModule2 : DummyModule
        {
        }

        public class DummyModule3 : DummyModule
        {
        }

        private DataExchangeManagerService.DataExchangeManagerService _dataExchangeManagerService;
        private IList<IDataExchangeModule> _modules;
        private Mock<IServiceEventLogger> _serviceEventLogger;

        [SetUp]
        public void SetUp()
        {
            _modules = new List<IDataExchangeModule>
                {
                    new DummyModule1(),
                    new DummyModule2(),
                    new DummyModule3()
                };

            var dataExchangeModuleFactory = new Func<IEnumerable<IDataExchangeModule>>(() => _modules);

            _serviceEventLogger = new Mock<IServiceEventLogger>();
            _serviceEventLogger.SetupAllProperties();

            _dataExchangeManagerService = new DataExchangeManagerService.DataExchangeManagerService(_serviceEventLogger.Object, dataExchangeModuleFactory);
            _dataExchangeManagerService.Initialize();
            _dataExchangeManagerService.RequestStop(); // This makes sure the RunIteration method only executes once
        }

        [Test]
        public void RunIteration_StartModulesIsCalled_AllRegisteredModulesAreStarted()
        {
            // Assign

            // Act
            
            bool actualWorkDone;
            _dataExchangeManagerService.RunIteration(out actualWorkDone);
            
            // Assert

            foreach (IDataExchangeModule module in _modules)
            {
                Assert.IsTrue(((DummyModule)module).IsStartCalled);
            }
        }

        [Test]
        public void RunIteration_StopModulesIsCalled_AllRegisteredModulesAreStopped()
        {
            // Assign

            // Act

            bool actualWorkDone;
            _dataExchangeManagerService.RunIteration(out actualWorkDone);

            // Assert

            foreach (IDataExchangeModule module in _modules)
            {
                Assert.IsTrue(((DummyModule)module).IsStopCalled);
            }
        }

        [Test]
        public void RunIteration_TerminateRunningModulesIsCalled_AllRegisteredModulesAreTerminated()
        {
            // Assign

            _dataExchangeManagerService.TimeoutInSecondsBeforeTerminatingModules = 0;

            foreach (IDataExchangeModule module in _modules)
            {
                ((DummyModule) module).IsHanging = true;
            }

            // Act

            bool actualWorkDone;
            _dataExchangeManagerService.RunIteration(out actualWorkDone);

            // Assert

            foreach (IDataExchangeModule module in _modules)
            {
                Assert.IsTrue(((DummyModule)module).IsAbortModuleThreadCalled);
            }
        }
    }
}
