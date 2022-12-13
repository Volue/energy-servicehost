using System;
using System.IO;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class NppoLoadRunner : BaseImportRunner
    {
        public NppoLoadRunner(IFileUtility importFileUtility, IImportEventLogger importEventLogger,
            IProcessRunner processRunner, Func<ImportSettings> settingsFactory)
            : base(importFileUtility, importEventLogger, processRunner, settingsFactory)
        {
        }

        protected override string ImportDirectoryPath
        {
            get { return Path.Combine(Settings.EdkFilesDirectory, "NPPO"); }
        }

        protected override string RelativeExePath
        {
            get { return @"bin/nppoload.exe"; }
        }

        protected override string WorkingDirectory
        {
            get { return ImportDirectoryPath; }
        }

        protected override string ImportFileExtension => "sdv";

        protected override string GetProcessArguments(DataExchangeImportMessage messagem, string importFileName)
        {
            return string.Format("{0}", importFileName);
        }
    }
}
