using System;
using System.ComponentModel;
using log4net;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic
{
    public class ImportApplication : IImportApplication
    {
        public ILog Log { set; get; }
        public const int GeneralErrorMsg = 8000;
        public const int EmptyMessage = 8206;
        public const int CorruptOrInvalidMessage = 8209;
        public const int FailedToImportMessage = 8210;

        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly ImportApplicationRunnerResolver _importApplicationRunnerResolver;

        public ImportApplication(IServiceEventLogger serviceEventLogger, ImportApplicationRunnerResolver importApplicationRunnerResolver)
        {
            Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
            _serviceEventLogger = serviceEventLogger;
            _importApplicationRunnerResolver = importApplicationRunnerResolver;
        }

        public bool HandleMessageWithExceptionHandling(
            Func<bool> handlingCall,
            Action<DataExchangeInvalidMessageException> onInvalidMessage = null,
            Action<DataExchangeInvalidMetadataException> onInvalidMetadata = null,
            Action<DataExchangeImportRunnerFailedException> onImportRunnerFailed = null,
            Action<TimeoutException> onTimeout = null,
            Action onErrorCaught = null)
        {
            try
            {
                return handlingCall();
            }
            catch (DataExchangeInvalidMessageException exception)
            {
                LogException(exception);
                _serviceEventLogger.LogMessage(CorruptOrInvalidMessage);
                onInvalidMessage?.Invoke(exception);
            }
            catch (ClaimHandlerException exception)
            {
                LogException(exception);
                _serviceEventLogger.LogMessage(CorruptOrInvalidMessage);
                onInvalidMessage?.Invoke(new DataExchangeInvalidMessageException("", "", exception));
            }
            catch (DataExchangeInvalidMetadataException exception)
            {
                LogException(exception);
                onInvalidMetadata?.Invoke(exception);
            }
            catch (DataExchangeImportRunnerFailedException exception)
            {
                // These messages fail during import. This means they are present in the message log, and should be handled from there with possible re-import.
                _serviceEventLogger.LogMessage(GeneralErrorMsg, exception.Message);
                _serviceEventLogger.LogMessage(FailedToImportMessage);
                onImportRunnerFailed?.Invoke(exception);
                return true; //  Commit. No onErrorCaught, i.e. will not be put in error queue.
            }
            catch (TimeoutException exception)
            {
                // We don't set any timeout, so the default from machine.config is used (i.e. 10 min). But we have seen that XmlElImp hangs when Mesh is in some faulted state.
                // These messages fail during import. This means they are present in the message log, and should be handled from there with possible re-import.
                _serviceEventLogger.LogMessage(GeneralErrorMsg, exception.Message);
                Log.Error("Timeout exception:", exception);
                onTimeout?.Invoke(exception);
                return true; //  Commit. No onErrorCaught, i.e. will not be put in error queue.
            }
            catch (Win32Exception exception)
            {
                // Non-existing application, Operating system errors.
                LogException(exception);
            }
            onErrorCaught?.Invoke();    // Default implementation: Put on Error queue.
            return true;
        }

        /// <summary>
        /// onFinished: Delegate (action) to be called upon completion. Should be a void function that accepts one boolean as parameter.
        /// The boolean will be true when import has completed successfully or when a known exception has been caught and duly reported.
        /// </summary>
        /// <param name="message"></param>
        /// <param name="onFinished"></param>
        /// <param name="onErrorCaught">Delegate action to be called when an error that needs the message to be moved to the error queue is caught.</param>
        /// <returns></returns>
        public bool StartImportJobImmediately(DataExchangeImportMessage message, Action<bool> onFinished, 
            Action<DataExchangeImportRunnerFailedException> onImportRunnerFailed = null,
            Action<TimeoutException> onTimeout = null,
            Action onErrorCaught = null)
        {
            Log.Info($"ExternalReference: {message.ExternalReference}");
            var ret = HandleMessageWithExceptionHandling(() => HandleMessage(message), onImportRunnerFailed: onImportRunnerFailed, onTimeout: onTimeout,onErrorCaught: onErrorCaught);
            onFinished?.Invoke(ret);
            Log.Info($"returns: {ret}");
            return ret;
        }

        /// <summary>
        /// Caller must handle exceptions!
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public bool HandleMessage(DataExchangeImportMessage message)
        {
            bool messageIsHandled = false;
            if (message != null)
            {
                if (!message.IsEmpty)
                {
                    var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);
                    Log.Debug($"Runner: {importApplicationRunner.GetType()}");
                    importApplicationRunner.Run(message);
                    messageIsHandled = true;
                }
                else
                {
                    // Empty messages are discarded directly with a log message
                    _serviceEventLogger.LogMessage(EmptyMessage);
                    messageIsHandled = true;
                }
            }
            return messageIsHandled;
        }

        public void LogException(Exception exception)
        {
            _serviceEventLogger.LogCritical(exception, GeneralErrorMsg, false);
        }
    }
}
