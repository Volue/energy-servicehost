using System;
using System.CodeDom;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using log4net.Util;
using MassTransit.Internals;
using Newtonsoft.Json;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Settings;
using Volue.MassTransit.Contracts.TimeSeriesMessage.ActivationRequestModels;
using Volue.MassTransit.Enums;
using Volue.MassTransit.Extensions;
using Volue.MassTransit.RabbitMq;
using Volue.MassTransit.RabbitMq.Configuration;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Modules
{
    public class MassTransitFileWatcherImportModule : ThreadBasedModule
    {
        // This class watches directories and file patterns for new message type instances, reads them and sends them to subscribers with MassTransit/RabbitMq.
        internal const string Modulename = "MassTransitFileWatcherImporter";
        public override string ModuleName => Modulename;
        private MassTransitFileWatcherParameters _parameters;
        private readonly FileSystemWatcher _paramFileWatcher = new FileSystemWatcher();  // For the parameters file.
        private List<FileSystemWatcher> _fileWatchers;
        private MassTransitFileWatcherModuleSettings _settings;
        private readonly MassTransitFileWatcherLogic _logic;
        protected TimeSpan SleepTime { get; private set; }

        public MassTransitFileWatcherImportModule(MassTransitFileWatcherModuleSettings settings, IServiceEventLogger serviceEventLogger, MassTransitFileWatcherLogic logic)
            : base(serviceEventLogger)
        {
            _settings = settings;
            _logic = logic;
            SleepTime = settings.ExportPollingInterval;
            var paramFile = settings.ImportParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(settings.ImportParametersFilePath);
            var paramFilter = settings.ImportParametersFileFilter;
            InitParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFilter);
        }

        protected override void ExecuteUntilStopped()
        {
            InitFileWatchers();
            do
            {
                // Keep thread alive.
                Thread.Sleep(SleepTime);
            }
            while (!IsStopRequested);
        }

        private void InitFileWatchers()
        {
            _fileWatchers = new List<FileSystemWatcher>();
            foreach (var fileWatcherParam in _parameters.FileWatcherParametersArray)
            {
                if (!Directory.Exists(fileWatcherParam.Directory))
                {
                    Log.Error($"Directory {fileWatcherParam.Directory} does not exist.");
                    continue;
                }
                var watcher = new FileSystemWatcher(fileWatcherParam.Directory,fileWatcherParam.FileNameTemplate);
                watcher.Created += OnFileCreated;
                watcher.EnableRaisingEvents = true;
                _fileWatchers.Add(watcher);
            }
            if (!_fileWatchers.Any())
                Log.Warn("No file watchers defined.");
        }
        
        private void OnFileCreated(object source, FileSystemEventArgs eArgs)
        {
            _logic.SubmitImport(eArgs.FullPath);
        }

        #region ParameterFile

        private void InitParametersFromJsonFile(string fullPathFilename)
        {
            //lock (_executionLock)
            //{
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg, $"Loading {fullPathFilename}");
                using (var strReader = new StreamReader(fullPathFilename))
                {
                    _parameters = JsonConvert.DeserializeObject<MassTransitFileWatcherParameters>(strReader.ReadToEnd());
                }

                _logic.FileWatcherParametersArray = _parameters.FileWatcherParametersArray;
                Log.DebugExt(() => $"{ModuleName}: New parameters loaded. FileWatcherParametersArray: {string.Join("\n", _parameters.FileWatcherParametersArray.Select(m => $"Directory: {m.Directory}, FileNameTemplate: {m.FileNameTemplate}, Host: {m.Host}, MessageType: {m.MessageType}"))}");
            //}
        }

        private void SetUpFileWatcherForParametersFile(string paramPath, string paramFilter)
        {
            _paramFileWatcher.NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            _paramFileWatcher.Path = paramPath;
            _paramFileWatcher.Filter = paramFilter;
            _paramFileWatcher.Changed += OnParametersFileChanged;
            _paramFileWatcher.EnableRaisingEvents = true;
        }

        private void OnParametersFileChanged(object source, FileSystemEventArgs eArgs)
        {
            InitParametersFromJsonFile(eArgs.FullPath);
        }

        #endregion
    }

    public class FilenameWildCardPattern : Regex
    {
        // Class to check if a filename matches a wildcard pattern.
        // Uses Regex, but the pattern is a filename pattern which can be used by FileWatcher with possible wildcards like * and/or ?
        public FilenameWildCardPattern(string wildCardPattern)
            : base(WildcardPatternToRegex(wildCardPattern))
        {
        }

        public FilenameWildCardPattern(string wildcardPattern, RegexOptions regexOptions)
            : base(WildcardPatternToRegex(wildcardPattern), regexOptions)
        {
        }

        private static string WildcardPatternToRegex(string wildcardPattern)
        {
            string patternWithWildcards = "^" + Regex.Escape(wildcardPattern).Replace("\\*", ".*");
            patternWithWildcards = patternWithWildcards.Replace("\\?", ".") + "$";
            return patternWithWildcards;
        }
    }
}
