using System;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Events;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Common
{
    [TestFixture]
    public class EventHandlersManagerTests
    {
        private EventRaiser _eventRaiser;
        private EventHandlerManager<SourceEventArgs, TargetEventArgs> _eventHandlerManager;

        private int _subscribedToSource;
        private int _unsubscribedFromSource;

        [SetUp]
        public void SetUp()
        {
            _subscribedToSource = 0;
            _unsubscribedFromSource = 0;

            _eventRaiser = new EventRaiser();

            _eventHandlerManager = new EventHandlerManager<SourceEventArgs, TargetEventArgs>
            (
                handler =>
                {
                    _eventRaiser.EventHandler += handler; 
                    ++_subscribedToSource;
                },
                handler => 
                { 
                    _eventRaiser.EventHandler -= handler;
                    ++_unsubscribedFromSource;
                },
                eventArgs => new TargetEventArgs() { Number = int.Parse(eventArgs.Number) }
            );
        }

        [Test]
        public void HandlerAdd_SubscribesEvent_GetExecuted()
        {
            TargetEventArgs received = null;
            _eventHandlerManager.Handler += (object sender, TargetEventArgs e) => received = e;

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(123, received.Number);
            Assert.AreEqual(1, _subscribedToSource);
            Assert.AreEqual(0, _unsubscribedFromSource);
        }

        [Test]
        [TestCase(7)]
        public void HandlerAdd_SubscribesUniqueEventNxTimes_GetExecutedNxTimes(int count)
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>(); 
            EventHandler<TargetEventArgs> method = (object sender, TargetEventArgs e) => received.Add(e);

            for (int i = 0; i < count; i++)
            {
                _eventHandlerManager.Handler += method;    
            }

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(count, received.Count);
            Assert.IsTrue(received.All(e => e.Number == 123));
            Assert.AreEqual(1, _subscribedToSource);
            Assert.AreEqual(0, _unsubscribedFromSource);
        }

        [Test]
        public void HandlerRemove_IsNotSubscribed_DoesNotExecute()
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, 0, 1);

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(0, received.Count);
            Assert.AreEqual(0, _subscribedToSource);
            Assert.AreEqual(0, _unsubscribedFromSource);
        }

        [Test]
        [TestCase(10, 1)]
        [TestCase(10, 5)]
        public void HandlerRemove_SubscribesMoreOftenThanUnsubscribes_SubscribesToSourceOnlyOnceAndDoesNotUnsubscribe(int subscribe, int unsubscribe)
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, subscribe, unsubscribe);

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(subscribe - unsubscribe, received.Count);
            Assert.AreEqual(1, _subscribedToSource);
            Assert.AreEqual(0, _unsubscribedFromSource);
        }

        [Test]
        [TestCase(10, 10)]
        [TestCase(10, 15)]
        public void HandlerRemove_UnsubscribesMoreOftenOrEqualThanSubscribes_SubscribesToSourceOnlyOnceAndDoesUnsubscribe(int subscribe, int unsubscribe)
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, subscribe, unsubscribe);

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(0, received.Count);
            Assert.AreEqual(1, _subscribedToSource);
            Assert.AreEqual(1, _unsubscribedFromSource);
        }

        [Test]
        [TestCase(7)]
        public void HandlerRemove_SubscribesNxTimes_UnsubscribesNxTimes(int count)
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, count, count);

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(0, received.Count);
        }

        [Test]
        [TestCase(10, 10, 3)]
        public void HandlerAdd_SubscribeAfterUnsubscribe_SubscribesToSourceCorrectly(int subscribeFirst, int unsubscribeFist, int subscribeSecond)
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, subscribeFirst, unsubscribeFist);
            AddAndRemoveSubscriptions(received, subscribeSecond, 0);

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(subscribeFirst - unsubscribeFist + subscribeSecond, received.Count);
            Assert.AreEqual(2, _subscribedToSource);
            Assert.AreEqual(1, _unsubscribedFromSource);
        }

        [Test]
        public void HandlerRepeatAddRemove_SameTimes_SubscribesAndUnsubscribesToSourceCorrectly()
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, 3, 3);
            AddAndRemoveSubscriptions(received, 3, 3);
            AddAndRemoveSubscriptions(received, 3, 3);

            _eventRaiser.InvokeEvent("123");

            Assert.AreEqual(0, received.Count);
            Assert.AreEqual(3, _subscribedToSource);
            Assert.AreEqual(3, _unsubscribedFromSource);
        }

        [Test]
        public void Dispose_CalledFewTimes_DoesNotThrow()
        {
            _eventHandlerManager.Dispose();
            _eventHandlerManager.Dispose();
            _eventHandlerManager.Dispose();
        }

        [Test]
        public void Dispose_AfterSubscription_DoesUnsubscribeFromSource()
        {
            List<TargetEventArgs> received = new List<TargetEventArgs>();
            AddAndRemoveSubscriptions(received, 3, 0);

            _eventHandlerManager.Dispose();

            Assert.AreEqual(1, _unsubscribedFromSource);
        }

        [Test]
        public void HandlerAdd_AfterDispose_ThrowsException()
        {
            EventHandler<TargetEventArgs> dummyMethod = (object sender, TargetEventArgs e) => { };
            _eventHandlerManager.Handler += dummyMethod;

            _eventHandlerManager.Dispose();

            Assert.Catch<ObjectDisposedException>(() => _eventHandlerManager.Handler += dummyMethod);
        }

        [Test]
        public void HandlerRemove_AfterDispose_ThrowsException()
        {
            EventHandler<TargetEventArgs> dummyMethod = (object sender, TargetEventArgs e) => { };
            _eventHandlerManager.Handler -= dummyMethod;

            _eventHandlerManager.Dispose();

            Assert.Catch<ObjectDisposedException>(() => _eventHandlerManager.Handler -= dummyMethod);
        }

        [Test]
        public void ExceptionPropagation_SubscribesThrowingMethodToHandler_InvokerCatchesException()
        {
            EventHandler<TargetEventArgs> throwingMethod = (object sender, TargetEventArgs e) => { throw new Exception("test");};
            _eventHandlerManager.Handler += throwingMethod;

            var ex = Assert.Catch<Exception>(() => _eventRaiser.InvokeEvent("123"));
            Assert.AreEqual("test", ex.Message);
        }

        private void AddAndRemoveSubscriptions(List<TargetEventArgs> received, int addCount, int removeCount)
        {
            EventHandler<TargetEventArgs> method = (object sender, TargetEventArgs e) => received.Add(e);

            for (int i = 0; i < addCount; i++)
            {
                _eventHandlerManager.Handler += method;
            }

            for (int i = 0; i < removeCount; i++)
            {
                _eventHandlerManager.Handler -= method;
            }
        }
    }

    public class EventRaiser
    {
        public event EventHandler<SourceEventArgs> EventHandler;

        public void InvokeEvent(string number)
        {
            if (EventHandler != null)
            {
                EventHandler.Invoke(this, new SourceEventArgs() { Number = number });
            }
        }
    }

    public class SourceEventArgs : EventArgs
    {
        public string Number { get; set; }
    }

    public class TargetEventArgs : EventArgs
    {
        public int Number { get; set; }
    }
}