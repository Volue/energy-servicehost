using System;
using System.Configuration;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class Gs2ImpRunner : BaseImportRunner
    {
        private const string SUBDIRECTORY = @"\GS2\new";

		public Gs2ImpRunner(IFileUtility importFileUtility, IImportEventLogger importEventLogger,
            IProcessRunner processRunner, Func<ImportSettings> settingsFactory)
            : base(importFileUtility, importEventLogger, processRunner, settingsFactory)
		{
		    Subdirectory = SUBDIRECTORY;
		    ReadsFromStandardInput = Boolean.Parse(ConfigurationManager.AppSettings["Gs2.ReadsFromStandardInput"] ?? "false");
		}

        protected override string RelativeExePath { get { return @"bin\gs2imp.exe"; } }
    }
}
