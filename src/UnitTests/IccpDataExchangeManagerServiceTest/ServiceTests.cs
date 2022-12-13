using System;
using System.Collections.Generic;
using System.Linq;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Service = Powel.Icc.Messaging.IccpDataExchangeManager.IccpDataExchangeManagerService;

namespace IccpDataExchangeManagerServiceTest
{
    [TestFixture]
    public class ServiceTests
    {
        private Service.IccpDataExchangeManagerService _instance;
        private IEnumerable<IDataExchangeModule> _modules;
        [SetUp]
        public void SetUpTest()
        {
            var mockIServiceEventLogger = new Mock<IServiceEventLogger>();

            _modules = new List<IDataExchangeModule>
            {
                new DummyModule1(),
                new DummyModule2(),
                new DummyModule3()
            };
            var moduleFactory = new Func<IEnumerable<IDataExchangeModule>>(() => _modules);
            mockIServiceEventLogger.SetupAllProperties();
            _instance = new Service.IccpDataExchangeManagerService(mockIServiceEventLogger.Object, moduleFactory);
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
            Assert.AreEqual(3, _modules.Count());
            Assert.IsTrue(_modules.All(module => ((DummyModule)module).IsStartCalled));
        }

        [Test]
        public void RunIteration_StopModulesIsCalled_AllRegisteredModulesAreStopped()
        {
            // Assign

            // Act
            bool actualWorkDone;
            _instance.RunIteration(out actualWorkDone);

            // Assert
            Assert.AreEqual(3, _modules.Count());
            Assert.IsTrue(_modules.All(module => ((DummyModule)module).IsStopCalled));
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
            Assert.AreEqual(3, _modules.Count());
            Assert.IsTrue(_modules.All(module => ((DummyModule)module).IsAbortModuleThreadCalled));
        }

        #region TestClasses
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

        public class DummyModule1 : DummyModule
        {
        }

        public class DummyModule2 : DummyModule
        {
        }

        public class DummyModule3 : DummyModule
        {
        }
        #endregion
    }
}
