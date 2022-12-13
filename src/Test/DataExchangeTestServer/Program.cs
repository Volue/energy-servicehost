using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Windows.Forms;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeTestServer
{
    public static class Program
    {
        static CmdArgs GetCmdArgs()
        {
            CmdArgs cmdArgs = new CmdArgs();
            string[] args = Environment.GetCommandLineArgs();
            string s = "";

            foreach (string arg in args)
            {
                s = arg.ToLower().Replace(" ", "");

                // Disable GUI
                if ((s == "-n") || (s == "-nogui"))
                {
                    cmdArgs.disableGUI = true;
                } 
                // writeToFile
                else if (((s == "-w") || (s == "-writeToFile")))
                {
                    cmdArgs.writeToFile = true;
                }
                // rawData
                else if (((s == "-r") || (s == "-rawData")))
                {
                    cmdArgs.rawData = true;
                }
                // filePath
                else if (((s.StartsWith("-f=")) || (s.StartsWith("-filePath="))))
                {
                    int i = s.IndexOf("=", 0);

                    if (i > -1)
                    {
                        cmdArgs.filePath = s.Substring(i + 1, s.Length - (i + 1));
                    }
                }
            }

            return cmdArgs;
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        public static void Main()
        {
            Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs = GetCmdArgs();

            // Start service host
            using (ServiceHost host = new ServiceHost(typeof (Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer)))
            {
                // Open the ServiceHost to start listening for messages. Since no endpoints are explicitly configured, 
                // the runtime will create one endpoint per base address for each service contract implemented by the service.
                host.Open();

                List<string> endpoints = new List<string>();

                foreach (ServiceEndpoint endpoint in host.Description.Endpoints)
                {
                    endpoints.Add(endpoint.Address.ToString());
                }

                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);

                if (Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.disableGUI)
                {
                    Application.Run();
                }
                else
                {
                    Application.Run(new DataExchangeTestServerForm(endpoints));
                }

                // Close the ServiceHost.
                host.Close();
            }
        }
    }
}
