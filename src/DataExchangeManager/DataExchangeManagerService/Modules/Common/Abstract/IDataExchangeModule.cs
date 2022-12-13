using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public interface IDataExchangeModule
    {
        string ModuleName { get; }

        bool IsRunning { get; }

        bool IsExecutingJobRightNow { get; }

        /// <summary>
        /// the exception that made the Module to STOP working
        /// </summary>
        Exception FailureReason { get; }

        void Start();

        void RequestStop();

        /// <summary>
        /// wait for Thread to Join (finish current Task)
        /// </summary>
        void Stop(TimeSpan timeout);

        /// <summary>
        /// finish immediately
        /// </summary>
        void Abort();
    }
}
