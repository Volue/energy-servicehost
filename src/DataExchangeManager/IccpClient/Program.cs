using System;
using System.Configuration;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using TASE2.Library.Client;
using TASE2.Library.Common;
using TASE2.Library.Server;

namespace IccpClient
{
    class Program
    {
        private static void LogHandler(LogLevel level, LogSource source, Endpoint ep, EndpointConnection epCon, string message)
        {
            if (ep != null)
            {
                if (epCon != null)
                    Console.WriteLine("{5}: LIBTASE2[{0}|{4}]: [{2}][{3}] {1}", level.ToString(), message, ep.Id, epCon.PeerAddress, source.ToString(), DateTime.Now.ToString("yyyy.MM.dd HH:mm:ss.fff"));
                else
                    Console.WriteLine("{4}: LIBTASE2[{0}|{3}]: [{2}] {1}", level.ToString(), message, ep.Id, source.ToString(), DateTime.Now.ToString("yyyy.MM.dd HH:mm:ss.fff"));
            }
            else
            {
                Console.WriteLine("{3}: LIBTASE2[{0}|{2}]: {1}", level.ToString(), message, source.ToString(), DateTime.Now.ToString("yyyy.MM.dd HH:mm:ss.fff"));
            }
        }

        static void Main(string[] args)
        {
            var logLevel = (LogLevel)Convert.ToInt16(ConfigurationManager.AppSettings["LogLevel"] ?? "5");
            Tase2Library.SetLogLevel(logLevel);
            Tase2Library.SetLogHandler(LogHandler);

            var useTls = Convert.ToBoolean(ConfigurationManager.AppSettings["UseTls"] ?? "false");
            /****************************************************************
		     * Setup TLS configuration
		     ***************************************************************/

            TLSConfiguration tlsConfig = null;
            if (useTls)
            {
                var chainValidation = Convert.ToBoolean(ConfigurationManager.AppSettings["ChainValidation"] ?? "true");
                var allowOnlyKnownCertificates = Convert.ToBoolean(ConfigurationManager.AppSettings["AllowOnlyKnownCertificates"] ?? "false");
                var ownKeyFile = ConfigurationManager.AppSettings["OwnKeyFile"];    // "client1-key.pem"
                var ownKeyPassword = ConfigurationManager.AppSettings["OwnKeyPassword"];
                var ownCertificateFile = ConfigurationManager.AppSettings["OwnCertificateFile"];  // "client1.cer"
                var CACertificateFiles = ConfigurationManager.AppSettings["CACertificateFiles"];    // "root.cer[,cert.cer]"
                var dir = ConfigurationManager.AppSettings["CertificatesDirectory"] ?? "certificates";
                if (string.IsNullOrEmpty(ownKeyFile) || string.IsNullOrEmpty(ownCertificateFile) || string.IsNullOrEmpty(CACertificateFiles))
                {
                    Console.WriteLine("Setup TLS parameters.");
                    return;
                }

                tlsConfig = new TLSConfiguration();

                try
                {
                    tlsConfig.ChainValidation = chainValidation;
                    tlsConfig.AllowOnlyKnownCertificates = allowOnlyKnownCertificates;

                    tlsConfig.SetOwnKey(Path.Combine(dir,ownKeyFile), ownKeyPassword);
                    tlsConfig.SetOwnCertificate(Path.Combine(dir,ownCertificateFile));
                    foreach (var file in CACertificateFiles.Split(','))
                    {
                        tlsConfig.AddCACertificate(Path.Combine(dir,file));
                    }
                }
                catch (CryptographicException)
                {
                    Console.WriteLine("TLS configuration failed");

                    tlsConfig = null;
                }
            }

            const string localApTitleDefault = "1.1.1.999.1";
            const string remoteApTitleDefault = "1.1.1.999";
            const string localAeQualifierDefaultString = "13";
            const string remoteAeQualifierDefaultString = "12";
            const string localPSelectorDefault = "0 0 0 1";
            const string localSSelectorDefault = "0 1";
            const string localTSelectorDefault = "0 1";

            string localApTitle = ConfigurationManager.AppSettings["LocalApTitle"] ?? localApTitleDefault;
            int localAeQualifier;
            if (!Int32.TryParse(ConfigurationManager.AppSettings["LocalAeQualifier"] ?? localAeQualifierDefaultString, out localAeQualifier))
                localAeQualifier = Convert.ToInt32(localAeQualifierDefaultString);
            string localPSelector = ConfigurationManager.AppSettings["LocalPSelector"] ?? localPSelectorDefault;
            string localSSelector = ConfigurationManager.AppSettings["LocalSSelector"] ?? localSSelectorDefault;
            string localTSelector = ConfigurationManager.AppSettings["LocalTSelector"] ?? localTSelectorDefault;

            Client client = null;
            int tries = 0, maxTries = 3;
            do 
            {
	            string hostname = ConfigurationManager.AppSettings["Hostname"];
	            if (string.IsNullOrEmpty(hostname))
	            {
	                Console.Write("Hostname [localhost] or exit:");
	                hostname = Console.ReadLine();
	                if (string.IsNullOrEmpty(hostname))
	                    hostname = "localhost";
                    else if (hostname == "exit")
                        return;
	            }
	
	            string apTitle = ConfigurationManager.AppSettings["ApTitle"];
	            if (string.IsNullOrEmpty(apTitle)) {
	                Console.Write($"ApTitle [{remoteApTitleDefault}]:");
	                apTitle = Console.ReadLine();
	                if (string.IsNullOrEmpty(apTitle))
	                    apTitle = remoteApTitleDefault;
	            };
	            int aeQualifier = 0;
                string aeQualifierString = ConfigurationManager.AppSettings["AeQualifier"];
	            if (string.IsNullOrEmpty(aeQualifierString) || !Int32.TryParse(aeQualifierString, out aeQualifier))
	            {
	                Console.Write($"AeQualifier [{remoteAeQualifierDefaultString}]:");
                    aeQualifierString = Console.ReadLine();
	                if (string.IsNullOrEmpty(aeQualifierString))
	                    aeQualifier = Convert.ToInt32(remoteAeQualifierDefaultString);
	                else if (!Int32.TryParse(aeQualifierString, out aeQualifier))
	                {
	                    Console.WriteLine($"Bad AeQualifier: {aeQualifierString}. Using default");
	                    aeQualifier = Convert.ToInt32(remoteAeQualifierDefaultString);
	                }
	            }
	
	            string pSelector = ConfigurationManager.AppSettings["PSelector"];
	            if (string.IsNullOrEmpty(pSelector))
	            {
	                Console.Write("PSelector [0 0 0 1]:");
	                pSelector = Console.ReadLine();
	                if (string.IsNullOrEmpty(pSelector))
	                    pSelector = "0 0 0 1";
	            };
	
	            string sSelector = ConfigurationManager.AppSettings["SSelector"];
	            if (string.IsNullOrEmpty(sSelector))
	            {
	                Console.Write("SSelector [0 1]:");
	                sSelector = Console.ReadLine();
	                if (string.IsNullOrEmpty(sSelector))
	                    sSelector = "0 1";
	            };
	
	            string tSelector = ConfigurationManager.AppSettings["TSelector"];
	            if (string.IsNullOrEmpty(tSelector))
	            {
	                Console.Write("TSelector [0 1]:");
	                tSelector = Console.ReadLine();
	                if (string.IsNullOrEmpty(tSelector))
	                    tSelector = "0 1";
	            };
	
	            try
	            {
		            client = new Client(tlsConfig);
		            client.SetRemoteApTitle(apTitle, aeQualifier);
		            client.SetLocalAddresses(localPSelector.Split(' ').Select(b => Convert.ToByte(b, 16)).ToArray(),
		                localSSelector.Split(' ').Select(b => Convert.ToByte(b, 16)).ToArray(),
		                localTSelector.Split(' ').Select(b => Convert.ToByte(b, 16)).ToArray());
                    client.SetRemoteAddresses(pSelector.Split(' ').Select(b => Convert.ToByte(b, 16)).ToArray(),
                        sSelector.Split(' ').Select(b => Convert.ToByte(b, 16)).ToArray(),
                        tSelector.Split(' ').Select(b => Convert.ToByte(b, 16)).ToArray());
                    client.Endpoint.SetMaxServOutstandingCalling(15);
                    client.Endpoint.SetMaxServOutstandingCalled(15);

                    client.Connect(hostname, localApTitle, localAeQualifier);
	            }
	            catch (ClientException ex)
	            {
	            	Console.WriteLine(ex.Message);
                    tries++;
                    if (tries > maxTries)
                        return;
                }
            } while (client != null && client.GetState() != ClientState.STATE_CONNECTED);

            bool goOn = true;
            while (goOn)
            {
                string domainName, datasetName;
                try
                {
                    Console.WriteLine("Functions:");
                    Console.WriteLine("0.\tExit.");
                    Console.WriteLine("1.\tGetVersionString");
                    Console.WriteLine("2.\tGetPeerIdentity");
                    Console.WriteLine("3.\tGetSupportedFeatures");
                    Console.WriteLine("10.\tGetVCCVariables.");
                    Console.WriteLine("11.\tGetVCCDataSets");
                    Console.WriteLine("12.\tGetBilateralTableId");
                    Console.WriteLine("20.\tGetDomainNames");
                    Console.WriteLine("21.\tGetDomainDataSets");
                    Console.WriteLine("22.\tGetDomainVariables");
                    Console.WriteLine("23.\tGetDataSetDirectory");
                    Console.WriteLine("24.\tGetDataSetDirectory as JSON");
                    Console.WriteLine("29.\tDeleteDataSet");
                    Console.WriteLine("30.\tReadPointValue");
                    Console.WriteLine("9.\tGetStatistics");
                    Console.Write("Function:");
                    var func = Console.ReadLine();
                    switch (func)
                    {
                        case "0":
                            goOn = false;
                            break;
                        case "1":
                            Console.WriteLine(Tase2Library.GetVersionString());
                            break;
                        case "2":
                        {
                            string vendor, model, revision;
                            client.GetPeerIdentity(out vendor, out model, out revision);
                            Console.WriteLine($"Peer identity: Vendor={vendor}, Model={model}, Revision={revision}");
                            break;
                        }
                        case "3":
                            Console.WriteLine(client.GetSupportedFeatures());
                            break;
                        case "10":
                            Console.WriteLine(string.Join(",", client.GetVCCVariables()));
                            break;
                        case "11":
                            Console.WriteLine(string.Join(",", client.GetVCCDataSets()));
                            break;
                        case "12":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                                Console.WriteLine(client.GetBilateralTableId(domainName));
                            break;
                        case "20":
                            Console.WriteLine(string.Join(",", client.GetDomainNames()));
                            break;
                        case "21":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                                Console.WriteLine(string.Join(",", client.GetDomainDataSets(domainName)));
                            break;
                        case "22":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                                Console.WriteLine(string.Join(",", client.GetDomainVariables(domainName)));
                            break;
                        case "23":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                            {
                                Console.Write("Dataset name:");
                                datasetName = Console.ReadLine();
                                if (!string.IsNullOrEmpty(datasetName))
                                {
                                    var dataSetDirectory = client.GetDataSetDirectory(domainName, datasetName);
                                    foreach (var entrySpec in dataSetDirectory)
                                    {
                                        Console.WriteLine($"{entrySpec.DomainId}/{entrySpec.ItemId}");
                                    }
                                }
                            }
                            break;
                        case "24":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                            {
                                Console.Write("Dataset name:");
                                datasetName = Console.ReadLine();
                                if (!string.IsNullOrEmpty(datasetName))
                                {
                                    var strBuilder = new StringBuilder();
                                    var dataSetDirectory = client.GetDataSetDirectory(domainName, datasetName);
                                    foreach (var entrySpec in dataSetDirectory)
                                    {
                                        if (strBuilder.Length > 0)
                                            strBuilder.Append("," + Environment.NewLine);
                                        strBuilder.Append(
                                            $"\t{{ \"Variable\": {{ \"Domain\": \"{entrySpec.DomainId}\", \"Dataset\": \"{datasetName}\", \"VariableCode\": \"{entrySpec.ItemId}\" }}, \"ExternalReference\": \"\", \"ValueUnit\": \"\" }}");

                                    }

                                    if (strBuilder.Length > 0)
                                    {
                                        Console.Write("Write to filename [Console]:");
                                        var filename = Console.ReadLine();
                                        if (string.IsNullOrEmpty(filename))
                                        {
                                            Console.WriteLine(strBuilder.ToString());
                                        }
                                        else
                                        {
                                            if (!filename.Contains("."))
                                                filename += ".json";
                                            File.WriteAllText(filename, strBuilder.ToString());
                                        }
                                    }
                                }
                            }
                            break;
                        case "29":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                            {
                                Console.Write("Dataset name:");
                                datasetName = Console.ReadLine();
                                if (!string.IsNullOrEmpty(datasetName))
                                {
                                    Console.WriteLine(client.DeleteDataSet(domainName,datasetName) ? "Deleted" : "Not deleted");
                                }
                            }
                            break;
                        case "30":
                            Console.Write("Domain name:");
                            domainName = Console.ReadLine();
                            if (!string.IsNullOrEmpty(domainName))
                            {
                                Console.Write("Point name:");
                                var pointName = Console.ReadLine();
                                if (!string.IsNullOrEmpty(pointName))
                                {
                                    var pointValue = client.ReadPointValue(domainName, pointName);
                                    Console.WriteLine($"{DateTimeOffset.FromUnixTimeMilliseconds((long)pointValue.TimeStamp).DateTime}: {ValueAsString(pointValue)}");
                                }
                            }
                            break;
                        case "9":
                        {
                            var stats = client.GetStatistics();
                            Console.WriteLine("Application messages sent: {0}", stats.messagesSent);
                            Console.WriteLine("Application messages received: {0}", stats.messagesRcvd);
                            Console.WriteLine("Application layer errors: {0}", stats.errors);
                            break;
                        }
                        default:
                            goOn = false;
                            break;
                    }
                }
                catch (ClientException ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
            client.Disconnect();
        }

        private static string ValueAsString(PointValue pointValue)
        {
            var ret = string.Empty;
            switch (pointValue.Type)
            {
                case PointValueType.DISCRETE:
                    ret = pointValue.ValueDiscrete.ToString();
                    break;
                case PointValueType.REAL:
                    ret = pointValue.ValueReal.ToString(CultureInfo.InvariantCulture);
                    break;
                default:
                    throw new NotImplementedException($"Not implemented type: {pointValue.Type.ToString()}");
            }

            return ret;
        }
    }
}
