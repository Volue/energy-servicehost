using System;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class EdiImpRunner : BaseImportRunner
    {
        private const string SUBDIRECTORY = @"\EDI\new";
        public EdiImpRunner(IFileUtility importFileUtility, IImportEventLogger importEventLogger,
            IProcessRunner processRunner, Func<ImportSettings> settingsFactory)
            : base(importFileUtility, importEventLogger, processRunner, settingsFactory)
        {
            Subdirectory = SUBDIRECTORY;
        }

        protected override string RelativeExePath
        {
            get { return @"bin\ediimp.exe"; }
        }
    }
}