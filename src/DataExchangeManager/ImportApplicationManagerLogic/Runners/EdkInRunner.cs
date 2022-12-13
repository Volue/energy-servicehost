using System;
using System.IO;
using System.Configuration;
using System.Text;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Extensions;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class EdkInRunner : BaseImportRunner
    {
        public EdkInRunner(IFileUtility importFileUtility, IImportEventLogger importEventLogger,
            IProcessRunner processRunner, Func<ImportSettings> settingsFactory)
            : base(importFileUtility, importEventLogger, processRunner, settingsFactory)
        {
        }

        protected override string ImportDirectoryPath
        {
            get { return Path.Combine(Settings.EdkFilesDirectory, "new"); }
        }

        protected override string RelativeExePath
        {
            get { return ConfigurationManager.AppSettings["EdkInRunner.RelativeExePath"] ?? @"bin\edkin.exe"; }
        }

        protected override string WorkingDirectory
        {
            get { return Settings.EdkFilesDirectory; }
        }

        protected override string GetProcessArguments(DataExchangeImportMessage message, string importFileName) 
        {
            var arguments = new StringBuilder(base.GetProcessArguments(message, importFileName));
            arguments.AppendIfNotNull('o', message.ReceiverId);

            return arguments.ToString();
        }
    }
}