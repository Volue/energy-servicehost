using System;
using System.ComponentModel;
using System.IO;
using System.Text;
using log4net;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Extensions;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract
{
    public abstract class BaseImportRunner : IImportApplicationRunner
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private const int EXIT_CODE_SUCCESS = 0;
        private const int WINDOWS1252_ENCODING_CODE_PAGE = 1252;    // See: https://msdn.microsoft.com/en-us/library/system.text.encoding(v=vs.110).aspx

        private readonly IFileUtility _importFileUtility;
        private readonly IProcessRunner _processRunner;
        protected readonly ImportSettings Settings;
        private readonly IImportEventLogger _importEventLogger;
        private string _subdirectory = "";
        protected string Subdirectory { get { return _subdirectory; } set { _subdirectory = value; } }

        protected BaseImportRunner(IFileUtility importFileUtility, IImportEventLogger importEventLogger,
            IProcessRunner processRunner, Func<ImportSettings> settingsFactory)
        {
            _importFileUtility = importFileUtility;
            _importEventLogger = importEventLogger;
            _processRunner = processRunner;
            Settings = settingsFactory();
            FileEncoding = Encoding.GetEncoding(WINDOWS1252_ENCODING_CODE_PAGE);    // Default for all EDIFACT/NP-/PD-/GS2-format
        }

        protected virtual string ImportDirectoryPath => Settings.EdiImportDirectory + Subdirectory;

        protected virtual string WorkingDirectory => Settings.EdiImportDirectory + Subdirectory;

        protected abstract string RelativeExePath { get; }  // Abstract to assure this is given.
        protected string ExePath => Path.Combine(IccConfiguration.IccHome, RelativeExePath);

        public Encoding FileEncoding { get; set; }

        protected virtual string ImportFileExtension => "imp";
        public bool ReadsFromStandardInput { get; protected set; }

        public void Run(DataExchangeImportMessage message)
        {
            if (ReadsFromStandardInput)
            {
                RunImportApplication(message);
                return;
            }

            // Save the import data to file and let the import application use the file afterwards.
            _importFileUtility.ImportFileExtension = ImportFileExtension;
            Log.Debug($"Starting save to {ImportDirectoryPath}");
            string importFileName = _importFileUtility.SaveImportToFile(message, ImportDirectoryPath,FileEncoding);

            try
            {
                RunImportApplication(message, importFileName);
            }
            catch (Win32Exception e)
            {
                // Non-existing application.
                Log.Error(e);
                _importEventLogger.LogFailedImport(message);
                e.Data.Add("ExePath",ExePath);
                e.Data.Add("Filename",importFileName);
                // if import fails, we should try to delete the file that we created. (When the import succeeds, the import itself will delete this file)
                string importFilePath = Path.Combine(ImportDirectoryPath, importFileName);
                _importFileUtility.DeleteFile(importFilePath);

                //throw new DataExchangeImportRunnerFailedException($"Runner '{ExePath}' failed for file '{importFileName}'.", e);
                throw;
            }
        }

        private void RunImportApplication(DataExchangeImportMessage message, string importFileName)
        {
            string arguments = GetProcessArguments(message, importFileName);

            string fileName = ExePath;
            string directory = WorkingDirectory;
            Log.Debug($"Starting {fileName} with arguments {arguments}");
            if (string.IsNullOrEmpty(Environment.GetEnvironmentVariable("ICC_NO_LOGONUIS")))    // Test for not unusual error situation which may be troublesome to find.
                Log.Error("ICC_NO_LOGONUIS is not set.");
            int exitCode = _processRunner.Run(fileName, directory, arguments);

            if (exitCode == EXIT_CODE_SUCCESS)
            {
                _importEventLogger.LogSuccessfulImport(message);
            }
            else
            {
                Log.Error($"Import failed with exitCode={exitCode}");
                _importEventLogger.LogFailedImport(message);
                throw new DataExchangeImportRunnerFailedException($"Failed to import the message using the command '{fileName} {arguments}'.");
            }
            Log.Debug("Exit");
        }

        private void RunImportApplication(DataExchangeImportMessage message)
        {
            Log.Info("Enter");

            string arguments = GetProcessArguments(message, null);

            string fileName = ExePath;
            string directory = WorkingDirectory;
            Log.Debug($"Streaming message to {fileName} with arguments {arguments}");
            if (string.IsNullOrEmpty(Environment.GetEnvironmentVariable("ICC_NO_LOGONUIS")))    // Test for not unusual error situation which may be troublesome to find.
                Log.Error("ICC_NO_LOGONUIS is not set.");
            int exitCode = _processRunner.Run(fileName, directory, arguments,message.GetMessageData());

            if (exitCode == EXIT_CODE_SUCCESS)
            {
                _importEventLogger.LogSuccessfulImport(message);
            }
            else
            {
                Log.Error($"Import failed with exitCode={exitCode}");
                _importEventLogger.LogFailedImport(message);
                throw new DataExchangeImportRunnerFailedException(
                    $"Failed to import the message using the command '{fileName} {arguments}'.");
            }
            Log.Info("Exit");
        }

        protected virtual string GetProcessArguments(DataExchangeImportMessage message, string importFileName)
        {
            var arguments = new StringBuilder(30);

            arguments.AppendIfNotNull('f', importFileName);
            arguments.AppendIfNotNull('R', message.RoutingAddress);
            arguments.AppendIfNotNull('Q', message.ExternalReference);

            return arguments.ToString();
        }
    }
}