using System.Globalization;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Exceptions;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators
{
    public class DataExchangeExportMessageValidator : IDataExchangeExportMessageValidator
    {
        public void Validate(DataExchangeExportMessage exportMessage)
        {
            string moduleName = null;

            try
            {
                moduleName = RoutingAddressParser.ParseModuleName(exportMessage.RoutingAddress);
            }
            catch (DataExchangeConfigurationException)
            {
                // swallow the exception on purpose
            }

            if (IsModuleNameInvalid(moduleName))
            {
                throw new DataExchangeInvalidRoutingAddressException(
                    exportMessage.MessageLogId.ToString(CultureInfo.InvariantCulture),
                    exportMessage.RoutingAddress);
            }
        }

        private static bool IsModuleNameInvalid(string moduleName)
        {
            return moduleName != CompelloExportModule.MODULE_NAME;
        }
    }
}