using System;
using TASE2.Library.Common;
using TASE2.Library.Client;
using System.Security.Cryptography;
using System.Collections.Generic;

namespace tase2_client
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			Console.WriteLine ("Setting up TASE.2 client ...");

			/****************************************************************
		     * Setup TLS configuration
		     ***************************************************************/

			TLSConfiguration tlsConfig = new TLSConfiguration ();

			try {
				tlsConfig.ChainValidation = true;
				tlsConfig.AllowOnlyKnownCertificates = false;

				tlsConfig.SetOwnKey ("client1-key.pem", null);
				tlsConfig.SetOwnCertificate ("client1.cer");
				tlsConfig.AddCACertificate ("root.cer");
			}
			catch (CryptographicException) {
				Console.WriteLine ("TLS configuration failed");

				tlsConfig = null;
			}

			string hostname = "localhost";
			string apTitle = "1.1.1.998";
			int aeQualifier = 12;

			if (args.Length > 0)
				hostname = args [0];
			if (args.Length > 1)
				apTitle = args [1];
			if (args.Length > 2) {
				Int32.TryParse (args[2], out aeQualifier);
			}

			Client client = new Client (tlsConfig);

            client.SetRemoteApTitle("1.1.1.999", 12);

            client.SetLocalAddresses("0 0 0 2", "0 2", "0 2");

            client.SetRemoteAddresses("0 0 0 1", "0 1", "0 1");

            client.Connect(hostname, apTitle, aeQualifier);

            List<string> vccVariables = client.GetVCCVariables ();

			Console.WriteLine ("VCC variables:");

			foreach (string vccVariable in vccVariables)
				Console.WriteLine ("  " + vccVariable);

			Console.WriteLine ();

			List<string> domainNames = client.GetDomainNames ();

			Console.WriteLine ("Domain names:");

			foreach (string domainName in domainNames)
				Console.WriteLine ("  " + domainName);

			Console.WriteLine ();

			foreach (string domainName in domainNames) {

				Console.WriteLine ("Variables in domain {0}:", domainName);
			
				List<string> domainVariables = client.GetDomainVariables (domainName);

				foreach (string domainVariable in domainVariables) {
					Console.WriteLine ("  " + domainVariable);
				}
			
				Console.WriteLine ("Data set in domain {0}:", domainName);

				List<string> domainDataSets = client.GetDomainDataSets (domainName);

				foreach (string dataset in domainDataSets) {
					Console.WriteLine ("  " + dataset);

					List<DataSetEntrySpec> dataSetDirectory = client.GetDataSetDirectory (domainName, dataset);

					foreach (DataSetEntrySpec entry in dataSetDirectory) {
						Console.WriteLine ("    {0}/{1}", entry.DomainId, entry.ItemId);
					}
				}

				Console.WriteLine ();
			}

			try {
				client.SendCommand (null, "Command2", CommandValue.TRIP);
			}
			catch (ClientException e) {
				Console.WriteLine ("{0} - reason: {1}", e.Message, e.GetError ().ToString ());
			}

			Statistics stats = client.GetStatistics ();

			Console.WriteLine ("Application messages send: {0}", stats.messagesSent);
			Console.WriteLine ("Application messages rcvd: {0}", stats.messagesRcvd);
			Console.WriteLine ("Application layer errors: {0}", stats.errors);

			client.Disconnect ();
		}
	}
}
