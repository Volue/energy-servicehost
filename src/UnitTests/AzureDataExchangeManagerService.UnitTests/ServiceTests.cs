using System;
using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Service=Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService;
namespace AzureDataExchangeManagerService.UnitTests
{
    [TestFixture, Category("Unit tests")]
    public class ServiceTests
    {
        private Service.AzureBusDataExchangeManagerService _instance;
        private IEnumerable<IDataExchangeModule> _modules;
        [SetUp]
        public void SetUpTest()
        {
            var mockIServiceEventLogger = new Mock<IServiceEventLogger>();

            _modules = new[] { new DummyModule() };
            var moduleFactory = new Func<IEnumerable<IDataExchangeModule>>(() => _modules);
            mockIServiceEventLogger.SetupAllProperties();
            _instance = new Service.AzureBusDataExchangeManagerService(mockIServiceEventLogger.Object, moduleFactory);
            _instance.Initialize();
            _instance.RequestStop(); // This makes sure the RunIteration method only executes once

        }

       
        [Test]
        public void RunIteration_StartModulesIsCalled_AllRegisteredModulesAreStarted()
        {
            // Assign

            // Act

            bool actualWorkDone;
            _instance.RunIteration(out actualWorkDone);

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
            _instance.RunIteration(out actualWorkDone);

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

            _instance.TimeoutInSecondsBeforeTerminatingModules = 0;

            foreach (IDataExchangeModule module in _modules)
            {
                ((DummyModule)module).IsHanging = true;
            }

            // Act

            bool actualWorkDone;
            _instance.RunIteration(out actualWorkDone);

            // Assert

            foreach (IDataExchangeModule module in _modules)
            {
                Assert.IsTrue(((DummyModule)module).IsAbortModuleThreadCalled);
            }
        }


        #region testclass
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

                if (!IsHanging)
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
        #endregion
    }

}