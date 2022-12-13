using System;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeCommon.Abstract
{
    public interface IImportApplication
    {
        bool HandleMessageWithExceptionHandling(
            Func<bool> handlingCall,
            Action<DataExchangeInvalidMessageException> onInvalidMessage = null,
            Action<DataExchangeInvalidMetadataException> onInvalidMetadata = null,
            Action<DataExchangeImportRunnerFailedException> onImportRunnerFailed = null,
            Action<TimeoutException> onTimeout = null,
            Action onErrorCaught = null);

        /// <summary>
        /// onFinished: Delegate (action) to be called upon completion. Should be a void function that accepts one boolean as parameter.
        /// The boolean will be true when import has completed successfully or when a known exception has been caught and duly reported.
        /// </summary>
        /// <param name="message"></param>
        /// <param name="onFinished"></param>
        /// <returns></returns>
        bool StartImportJobImmediately(DataExchangeImportMessage message, Action<bool> onFinished,
            Action<DataExchangeImportRunnerFailedException> onImportRunnerFailed = null,
            Action<TimeoutException> onTimeout = null,
            Action onErrorCaught = null);

        /// <summary>
        /// Caller must handle exceptions!
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        bool HandleMessage(DataExchangeImportMessage message);

        void LogException(Exception exception);
    }
}
