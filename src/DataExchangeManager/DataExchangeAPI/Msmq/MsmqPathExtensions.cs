using System;
using System.Collections.Generic;
using System.Linq;
using System.Messaging;
using System.Threading;
using log4net;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public static class MsmqPathExtensions
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private class AsynchronousPeekHandle
        {
            public MessageQueue MessageQueue;
            public bool MessageFound;
            public AutoResetEvent AutoResetEvent;
        }

        private static readonly MessagePropertyFilter MessageQueueFilter = new MessagePropertyFilter
                    {
                        AdministrationQueue = false,
                        ArrivedTime = false,
                        CorrelationId = false,
                        Priority = false,
                        ResponseQueue = false,
                        SentTime = false,
                        Body = false,
                        Label = false,
                        Id = false
                    };

        public static int CountMessages(this Dictionary<DataExchangeQueuePriority, MsmqPath> msmqPaths)
        {
            int count = 0;

            foreach (var key in msmqPaths.Keys)
            {
                using(var messageQueue = new MessageQueue(msmqPaths[key].FullPath))
                {
                    messageQueue.MessageReadPropertyFilter = new MessagePropertyFilter
                        {
                            AdministrationQueue = false,
                            ArrivedTime = false,
                            CorrelationId = false,
                            Priority = false,
                            ResponseQueue = false,
                            SentTime = false,
                            Body = false,
                            Label = false,
                            Id = false
                        };

                    count += messageQueue.GetAllMessages().Length;
                }
            }
            
            return count;
        }

        /// <summary>
        /// Waits until any of the queues has received a message.
        /// </summary>
        /// <param name="msmqPaths">Queues that will be polled.</param>
        /// <param name="timeout">The maximum amount of time to wait.</param>
        /// <returns></returns>
        public static int WaitAny(this Dictionary<DataExchangeQueuePriority, MsmqPath> msmqPaths, TimeSpan timeout)
        {
            var handles = new List<AsynchronousPeekHandle>();
            foreach (var key in msmqPaths.Keys)
            {
                try
                {
                    var handle = new AsynchronousPeekHandle
                    {
                        AutoResetEvent = new AutoResetEvent(false),
                        MessageQueue =
                            new MessageQueue(msmqPaths[key].FullPath) {MessageReadPropertyFilter = MessageQueueFilter},
                        MessageFound = false
                    };

                    handle.MessageQueue.PeekCompleted += (sender, args) => OnPeekCompleted(handle, args.AsyncResult);

                    handles.Add(handle);
                }
                catch (MessageQueueException Ex)
                {
                    throw new Exception(msmqPaths[key].FullPath,Ex);
                }
            }

            var handlesNotCompleted = new List<AsynchronousPeekHandle>(handles);

            try
            {
                foreach (var handle in handles)
                {
                    handle.MessageQueue.BeginPeek(timeout);
                }

                // Wait until all peeks found no messages, or until any peek found a message.
                while(!handles.Any(x => x.MessageFound) && handlesNotCompleted.Count > 0)
                {
                    int handleIndex = WaitHandle.WaitAny((from x in handlesNotCompleted select (WaitHandle)x.AutoResetEvent).ToArray());

                    if(handleIndex >= 0)
                    {
                        handlesNotCompleted.RemoveAt(handleIndex);
                    }
                }
            }
            finally
            {
                foreach (var handle in handles)
                {
                    ((IDisposable)handle.MessageQueue).Dispose();
                }
            }

            // Wait for all asynchronous peeks to be completed, just to be sure there are no pending events, this should finish immediately.
            if(handlesNotCompleted.Count > 0)
            {
                Log.Debug("Executing WaitAll() on PeekHandles");
                WaitHandle.WaitAll((from x in handlesNotCompleted select (WaitHandle)x.AutoResetEvent).ToArray());
            }

            // Find and return the index of the message queue that contained a message.
            var messageQueueIndex = -1;
            for (int i = 0; i < handles.Count; i++)
            {
                if (handles[i].MessageFound)
                {
                    messageQueueIndex = i;
                    break;
                }
            }
            return messageQueueIndex;
        }

        private static void OnPeekCompleted(AsynchronousPeekHandle handle, IAsyncResult asyncResult)
        {
            try
            {
                handle.MessageQueue.EndPeek(asyncResult);
                handle.MessageFound = true;
            }
            catch (MessageQueueException ex)
            {
                // This error code is thrown when the queue is closed while trying to peek, which is normal operation here.
                // I was unable to find the enum value for this error code, this line of code solves this problem.
                var messageQueueClosedErrorCode = (MessageQueueErrorCode) Enum.ToObject(typeof(MessageQueueErrorCode), -1073741536);

                if (ex.MessageQueueErrorCode != MessageQueueErrorCode.IOTimeout && ex.MessageQueueErrorCode != messageQueueClosedErrorCode)
                {
                    throw;
                }
            }
            finally
            {
                handle.AutoResetEvent.Set();
            }
        }
    }
}
