using System;
using System.Security.Cryptography;
using TASE2.Library.Common;
using TASE2.Library.Client;
using System.Threading;
using System.Text;

namespace tase2_client3
{
	class MainClass
	{
		/* callback handler that is called twice for each received transfer set report */
		private static void dsTransferSetReportHandler(object parameter, bool finished, UInt32 seqNo, ClientDSTransferSet transferSet)
		{
			if (finished) {
				Console.WriteLine ("--> ({0}) report processing finished", seqNo);
			} else {
				Console.WriteLine ("New report received with seq no: {0} transfer set: {1}", seqNo,
					transferSet != null ? transferSet.GetName() : "");
			}
		}

		/* callback handler that is called for each data point of a received transfer set report */
		private static void dsTransferSetValueHandler(object parameter, ClientDSTransferSet transferSet, string domainName, string pointName, PointValue pointValue)
		{
			Console.WriteLine ("  Received report value for: {0}:{1} type: {2}",  domainName != null ? domainName : "-", pointName, pointValue.Type);

			if (pointValue.Type == PointValueType.STATE) {
				Console.WriteLine ("    value: {0}", pointValue.ValueState.GetStateValue ().ToString ());
			} else if (pointValue.Type == PointValueType.DISCRETE) {
				Console.WriteLine ("    value: {0}", pointValue.ValueDiscrete.ToString ());
			} else if (pointValue.Type == PointValueType.REAL) {
				Console.WriteLine ("    value: {0}", pointValue.ValueReal.ToString ());
			}
		}

		/* callback handler that is called when the server closes the connection */
		private static void connectionClosedHandler(object paramter, Client conneciton)
		{
			Console.WriteLine ("Server closed connection!");
		}

		/* callback hander for information messages */
		private static void informationMessageHandler(object parameter, int infoRef, int localRef, int msgId, byte[] message)
		{
			Console.WriteLine ("IM message: info-ref {0} local-ref: {1} msg-id: {2} size: {3}", infoRef, localRef, msgId, message.Length);
			Console.WriteLine ("  content: " + Encoding.Default.GetString(message));
		}


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
			string apTitle = "1.1.1.999";
			int aeQualifier = 12;

			if (args.Length > 0)
				hostname = args [0];
			if (args.Length > 1)
				apTitle = args [1];
			if (args.Length > 2) {
				Int32.TryParse (args[2], out aeQualifier);
			}

			Client client = new Client (tlsConfig);

            client.SetIdentity("MZA", "TASE.2", "123");

			client.SetConnectionClosedHandler (connectionClosedHandler, null);

			client.SetInformationMessageHandler (informationMessageHandler, null);

			/* uncomment and adjust when specific lower layer addresses are required */
			//client.SetRemoteAddresses (1, new byte[]{ 0x01, 0x02, 0x03 }, new byte[]{ 0x04, 0x05, 0x06 });
			//client.SetLocalAddresses(2, new byte[]{ 0x01, 0x02, 0x03 }, new byte[]{ 0x04, 0x05, 0x06 });

			client.Connect (hostname, apTitle, aeQualifier);

            string vendor;
            string model;
            string revision;

            client.GetPeerIdentity(out vendor, out model, out revision);

            Console.WriteLine("Peer identity:");
            Console.WriteLine("  vendor: " + vendor);
            Console.WriteLine("  model: " + model);
            Console.WriteLine("  revision: " + revision);

			client.IMTransferSetEnable ();

			try {

				/* Set a tag on a VCC scope device (domain parameter = null) */
				client.SetTag (null, "Command2", TagValue.OPEN_AND_CLOSE_INHIBIT, "maintenance work in progress");

				/* Send a command to a VCC scope device (domain parameter = null) */
				client.SendCommand (null, "Command2", CommandValue.TRIP);

				Console.WriteLine ("Send command success");
			}
			catch (ClientException e) {
				Console.WriteLine (e.Message);
			}

			/* Create a local data set representation (reads data set information from the server) */
			ClientDataSet dataSet = client.GetDataSet ("icc1", "ds1");

			Console.WriteLine ("Data set has {0} elements", dataSet.GetSize ());

			/* Read the data set values */
			dataSet.Read ();

			for (int i = 0; i < dataSet.GetSize (); i++) {
				Console.WriteLine("  domain: {0} point-name: {1} type: {2}", dataSet.GetPointDomainName(i),
					dataSet.GetPointVariableName(i), dataSet.GetPointValue(i).Type.ToString());
			}

			/* set callback handlers for transfer set reports */
			client.SetDSTransferSetValueHandler (dsTransferSetValueHandler, null);
			client.SetDSTransferSetReportHandler (dsTransferSetReportHandler, null);

			/* get next available transfer set from domain "icc1" */
			ClientDSTransferSet transferSet = client.GetNextDSTransferSet ("icc1");

			/* connect data set with transfer set (received report values will be stored in the ClientDataSet instance) */
			transferSet.SetDataSet (dataSet);

			/* read transfer set paramters from server */
			transferSet.ReadValues ();

			Console.WriteLine (" data-set: {0}:{1}", transferSet.GetDataSetDomain (), transferSet.GetDataSetName ());

			/* Set some transfer set parameters */
			transferSet.SetDataSetName (dataSet.GetDomainName (), dataSet.GetDataSetName ());
			transferSet.SetInterval (1);
			transferSet.SetRBE (true);
			transferSet.SetCritical (false);
			transferSet.SetDSConditionsRequested(ReportReason.INTERVAL_TIMEOUT | ReportReason.OBJECT_CHANGE);
			transferSet.SetStatus (true);

			transferSet.WriteValues ();

			Thread.Sleep (10000);

			client.Disconnect ();

			client.Dispose ();
		}
	}
}
