using System;
using System.Configuration;


namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class CompelloSettings : ICompelloSettings
    {
        public string Address { get; private set; }
        public int Port { get; private set; }
        public string ApiKey { get; private set; }

        public CompelloSettings()
        {
            Address = ConfigurationManager.AppSettings["CompelloAddress"];
            Port = GetCompelloPortNumber();
            ApiKey = ConfigurationManager.AppSettings["CompelloApiKey"];
        }

        private int GetCompelloPortNumber()
        {
            int port;
            bool parseResult = Int32.TryParse(ConfigurationManager.AppSettings["CompelloPort"], out port);

            if (parseResult)
            {
                return port;
            }
            throw new ArgumentException(@"CompelloPort has to be intager value.", "CompelloPort");
        }
    }
}
