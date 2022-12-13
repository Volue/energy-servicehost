using System;
using System.Configuration;
using System.Text;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Extensions;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class XmlElImpRunner : BaseImportRunner
    {
        private const string SUBDIRECTORY = @"\XMLEL\new";

        public XmlElImpRunner(IFileUtility importFileUtility, IImportEventLogger importEventLogger,
            IProcessRunner processRunner, Func<ImportSettings> settingsFactory)
            : base(importFileUtility, importEventLogger, processRunner, settingsFactory)
        {
            Subdirectory = SUBDIRECTORY;
            FileEncoding = null;
            ReadsFromStandardInput = Boolean.Parse(ConfigurationManager.AppSettings["XmlElImp.ReadsFromStandardInput"] ?? "false");
        }

        protected override string RelativeExePath => @"bin\XMLELImpExp\XmlElImp.exe";

        protected override string GetProcessArguments(DataExchangeImportMessage message, string importFileName)
        {
            var arguments = new StringBuilder(30);

            arguments.AppendIfNotNull('s', message.SubAddress);

            return base.GetProcessArguments(message, importFileName) + " " + arguments;
        }
    }
}
