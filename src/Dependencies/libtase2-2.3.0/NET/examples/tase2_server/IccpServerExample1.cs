using System;
using TASE2.Library.Common;
using TASE2.Library.Server;
using System.Threading;
using System.Security.Cryptography;

namespace tase2_server2
{
	class MainClass
	{
		private static IndicationPoint vccVCCDiscreteQTimeTag1;
		private static IndicationPoint vccVCCDiscreteExtended;

		private static IndicationPoint icc1Discrete1;
		private static IndicationPoint icc1Discrete2;
		private static IndicationPoint icc1Discrete3;
		private static IndicationPoint icc1Discrete4;

		private static IndicationPoint icc1State1;

		private static ControlPoint vccCommand1;
		private static ControlPoint vccCommand2;

		private static ControlPoint icc1Command1;
		private static ControlPoint icc1Command2;

		/* callback function for client connect/disconnect events */
		private static void clientConnectionHandler(object parameter, string clientAddress, BilateralTable blt, bool connect)
		{
			if (connect)
				Console.WriteLine ("Client from " + clientAddress + " connected");
			else
				Console.WriteLine ("Client from " + clientAddress + " disconnected");

			if (blt != null) {
				Console.WriteLine ("  with BLT: " + blt.ID);
				Console.WriteLine ("  ap-title: " + blt.ApTitle);
				Console.WriteLine ("  ae-qualifier: " + blt.AeQualifier);
			}
		}

		/* callback function to handle device select requests form clients (only required for SBO control) */
		private static HandlerResult selectHandler (object parameter, ControlPoint controlPoint)
		{
			if (controlPoint == vccCommand1) 
			{
				Console.WriteLine("Received select for VCC/Command1");

				/* 
				 * Library will take care that no other client can select/operate the device.
				 * Optional: some internal action to select the device 
				 */

				return HandlerResult.SUCCESS;
			}

			return HandlerResult.OBJECT_ACCESS_DENIED;
		}

		/* callback function to handle device operate requests from clients */
		private static HandlerResult operateHandler(object parameter, ControlPoint controlPoint, OperateValue value)
		{
			if (controlPoint == vccCommand1) 
			{
				Console.Write("Received operate for VCC/Command1 --> ");

				if (value.commandValue == CommandValue.TRIP)
					Console.WriteLine ("trip");
				else if (value.commandValue == CommandValue.CLOSE)
					Console.WriteLine ("close");
				else {
					Console.WriteLine ("invalid command value");
					return HandlerResult.OBJECT_VALUE_INVALID;
				}

				return HandlerResult.SUCCESS;
			}

			if (controlPoint == vccCommand2) 
			{
				Console.Write("Received operate for VCC/Command2 --> ");

				if (value.commandValue == CommandValue.TRIP)
					Console.WriteLine ("trip");
				else if (value.commandValue == CommandValue.CLOSE)
					Console.WriteLine ("close");
				else {
					Console.WriteLine ("invalid command value");
					return HandlerResult.OBJECT_VALUE_INVALID;
				}

				return HandlerResult.SUCCESS;
			}
				
			return HandlerResult.OBJECT_ACCESS_DENIED;
		}

		/* callback function to handle set tag requests from clients */
		private static HandlerResult setTagHandler (object parameter, ControlPoint controlPoint, TagValue value, string reason)
		{
			if (controlPoint == vccCommand2) 
			{
				Console.WriteLine("Received operate for VCC/Command2 --> ");
				Console.WriteLine ("  tag-value: " + value.ToString ());
				Console.WriteLine ("  reason: " + reason);

				return HandlerResult.SUCCESS;
			}
				
			return HandlerResult.OBJECT_ACCESS_DENIED;
		}

		/* callback function to handle write requests from clients */
		private static HandlerResult writeDataHandler(object parameter, DataPoint dataPoint, PointValue value)
		{
			Console.WriteLine("Write to " + dataPoint.Name);

			return HandlerResult.SUCCESS;
		}

        public static void Main (string[] args)
		{
			bool running = true;

			/* run until Ctrl-C is pressed */
			Console.CancelKeyPress += delegate(object sender, ConsoleCancelEventArgs e) {
				e.Cancel = true;
				running = false;
			};
				
			Console.WriteLine ("Setting up TASE.2 server ...");

			/*********************************************************************
		     * Defining the data model
		     * - indication/control points
		     * - protection equipment
		     * - domains
		     * - transfer sets
		     * - data sets
		     *
		     *********************************************************************/

			DataModel dataModel = new DataModel ();

			/* get VCC pseudo domain to populate the VCC scope data model part */
			Domain vcc = dataModel.VCC;

            vcc.AddInformationMessageObject(64);
            vcc.AddInformationMessageObject(1024);

			IndicationPoint vccDiscrete1 = vcc.AddIndicationPoint("VCCDiscrete1", IndicationPointType.DISCRETE, 
				QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);

			/* create some indication points - references can be stored for later reuse (e.g. to update the values) */

			vccVCCDiscreteQTimeTag1 = vcc.AddIndicationPoint("VCCDiscreteQTimeTag1", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			vcc.AddIndicationPoint("VCCDiscreteQExtendedTimeTag1", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP_EXTENDED, true, true);
			vccVCCDiscreteExtended = vcc.AddIndicationPoint("VCCDiscreteExtended", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, true);

			/* create some control points (devices) - references can be stored for later reuse (e.g. in callbacks) */

			vccCommand1 = vcc.AddControlPoint("Command1", ControlPointType.COMMAND, DeviceClass.SBO, true, 1365);
			vccCommand2 = vcc.AddControlPoint("Command2", ControlPointType.COMMAND, DeviceClass.DIRECT, true, 1366);
			ControlPoint vccCommand3 = vcc.AddControlPoint("Command3", ControlPointType.COMMAND, DeviceClass.DIRECT, true, 1366);

			/* Create a new domain "icc1" */
			Domain icc1 = dataModel.AddDomain ("icc1");

			/* Add some data points and devices to domain "icc1" */
			icc1Discrete1 = icc1.AddIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			icc1Discrete2 = icc1.AddIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			icc1Discrete3 = icc1.AddIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
			icc1Discrete4 = icc1.AddIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);

			icc1State1 = icc1.AddIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);


			/* add a data set transfer set to domain "icc1" */
			icc1.AddDSTransferSet ("DSTrans1");

			/* add a new data set to domain "icc1" */
			DataSet ds1 = icc1.AddDataSet ("ds1");

			ds1.AddEntry(icc1, "Transfer_Set_Name");
			ds1.AddEntry(icc1, "DSConditions_Detected");
			ds1.AddEntry(icc1, "Event_Code_Detected");
			ds1.AddEntry(icc1, "Transfer_Set_Time_Stamp");
			ds1.AddEntry(vcc, "VCCDiscrete1");
			ds1.AddEntry(icc1, "Discrete1");
			ds1.AddEntry(icc1, "Discrete2");
			ds1.AddEntry(icc1, "DiscreteQTimeTag4");

			/* Create a new domain "icc2" */
			Domain icc2 = dataModel.AddDomain("icc2");

			/* Add some data points to domain "icc2" */
			icc2.AddIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			icc2.AddIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			icc2.AddIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
			icc2.AddIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);

			icc2.AddIndicationPoint("Real1", IndicationPointType.REAL, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			icc2.AddIndicationPoint("RealQ2", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
			icc2.AddIndicationPoint("RealQTimeTag3", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, false, true);


			/******************************************************************
		     * Defining the bilateral tables (BLT)
		     *
		     * - if no BLT is available all data can be accessed
		     * - only data/control points that are part of the BLT can be accessed by the client
		     * - the client is identified by its application reference
		     * - all data/control point that are part of the client's  ICC can be accessed
		     *
		     ******************************************************************/

			/* create bilateral table with ID "BLT_MZA_001_V1" */
			BilateralTable blt1 = new BilateralTable ("BLT_MZA_001_V1", icc1, "1.1.1.999.1", 12);

			/* add some access rules to the bilateral table */
			blt1.AddDataPoint (vccDiscrete1, true, false);
			blt1.AddDataPoint (vccVCCDiscreteQTimeTag1, true, false);
			blt1.AddControlPoint (vccCommand1, 1234, true, true, true, true);
			blt1.AddControlPoint (vccCommand2, 1236, true, true, true, true);
			blt1.AddControlPoint (vccCommand3, 1235, true, true, true, true);

			/* create bilateral table with ID "BLT_MZA_002_V1" */
			BilateralTable blt2 = new BilateralTable ("BLT_MZA_002_V1", icc2, "1.1.999.1", 12);

			/* add some access rules to the bilateral table */
			blt2.AddDataPoint (vccDiscrete1, true, false);
			blt2.AddControlPoint (vccCommand2, 1236, true, true, true, true);


			TLSConfiguration tlsConfig = new TLSConfiguration ();

			try {
				tlsConfig.ChainValidation = true;
				tlsConfig.AllowOnlyKnownCertificates = false;

				tlsConfig.SetOwnKey ("server-key.pem", null);
				tlsConfig.SetOwnCertificate ("server.cer");
				tlsConfig.AddCACertificate ("root.cer");
			}
			catch (CryptographicException) {
				Console.WriteLine ("TLS configuration failed");

				tlsConfig = null;
			}

			tlsConfig = null;

			/****************************************************************
			 * TASE.2 server setup
			 ***************************************************************/

			Server server = new Server (dataModel, tlsConfig);

			vccDiscrete1.SetDiscrete (123);
			//vccDiscrete1.SetDiscreteQ(2, DataFlags.NORMAL_VALUE);
			server.UpdateOnlineValue (vccDiscrete1);

			server.AddBilateralTable (blt1);
			server.AddBilateralTable (blt2);

			server.SetIdentity("MZA", "TASE.2", "1.1.0");

			/* set callback handlers */
			server.SetOperateHandler (operateHandler, null);
			server.SetSelectHandler (selectHandler, null);
			server.SetSetTagHandler (setTagHandler, null);
			server.SetClientConnectionHandler (clientConnectionHandler, null);
			server.SetWriteDataHandler(writeDataHandler, null);
			server.SetDSTransferSetUpdateHandler(delegate(object parameter, EndpointConnection peer, BilateralTable clientBlt, DSTransferSet transferSet, bool isEnabled)
			{
				Console.WriteLine("Transfer set {0} updated by endpoint {1}", transferSet.Name, peer.PeerAddress);
				Console.WriteLine("    data set: " + transferSet.DataSetName);
			}, null);

			vccDiscrete1.SetDiscrete (123);
			//vccDiscrete1.SetDiscreteQ(2, DataFlags.NORMAL_VALUE);
			server.UpdateOnlineValue (vccDiscrete1);

			server.Start ();

			if (server.IsRunning ()) {

				Console.WriteLine ("Server started.");

				DateTime nextSendTime = DateTime.Now.AddMilliseconds (1000);

				int intVal = 0;
				bool stateVal = false;
				ushort cov = 0;

				while (running) {
					Thread.Sleep (100);

					DateTime now = DateTime.Now;

					if (nextSendTime < now) {
				
						/* update variable values */

						icc1Discrete1.SetDiscrete (intVal++);
						server.UpdateOnlineValue (icc1Discrete1);

						icc1Discrete2.SetDiscrete (intVal);
						server.UpdateOnlineValue (icc1Discrete2);

						icc1Discrete4.SetDiscrete (intVal);
						icc1Discrete4.SetQuality (DataFlags.NORMAL_VALUE | DataFlags.TIME_STAMP_QUALITY);
						icc1Discrete4.SetTimeStamp (now);
						icc1Discrete4.SetCOV (cov++);
						server.UpdateOnlineValue (icc1Discrete4);

						icc1State1.SetState (stateVal ? DataState.HI : DataState.LO);
						server.UpdateOnlineValue (icc1State1);

						stateVal = !stateVal;

						nextSendTime = now.AddMilliseconds (1000);
					}

				}

				server.Stop ();

				Console.WriteLine ("Server stopped.");
			} else {
				Console.WriteLine ("ERROR: Failed to start server!");
			}

			/* release all native resources. will also be called when the server instance is finalized */
			server.Dispose ();
		}
	}
}
