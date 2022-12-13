using System;
using TASE2.Library.Common;
using TASE2.Library.Client;
using TASE2.Library.Server;
using System.Threading;
using System.Security.Cryptography;
using System.Text;

namespace endpoint1
{
    class Endpoint1
    {
        static Client client1 = null;
        static Client client2 = null;

        static bool client1IMEnabled = false;
        static bool client2IMEnabled = false;

        private static IndicationPoint icc1Discrete1;
        private static IndicationPoint icc1Discrete2;
        private static IndicationPoint icc1Discrete3;
        private static IndicationPoint icc1Discrete4;
        private static IndicationPoint icc1State1;

        private static IndicationPoint icc2Discrete1;
        private static IndicationPoint icc2Discrete2;
        private static IndicationPoint icc2Discrete3;
        private static IndicationPoint icc2Discrete4;
        private static IndicationPoint icc2State1;


        /* callback handler that is called when the server closes the connection */
        private static void connectionClosedHandler(object parameter, Client connection)
        {   
            if (connection == client1)
            {
                Console.WriteLine ("Endpoint2 closed connection!");
                client1IMEnabled = false;
            }

            if (connection == client2)
            {
                Console.WriteLine ("Endpoint3 closed connection!");
                client2IMEnabled = false;
            }
        }

        /* server callback handler - is called when the state of a data set is changed */
        private static void dstsStateChangedHandler(object parameter, DSTransferSet transferSet, bool enabled)
        {
            if (enabled)
                Console.WriteLine("DSTS {0} enabled", transferSet.Name);
            else
                Console.WriteLine("DSTS {0} disabled", transferSet.Name);
        }

        /* callback hander for information messages */
        private static void informationMessageHandler(object parameter, int infoRef, int localRef, int msgId, byte[] message)
        {
            Console.WriteLine ("IM message: info-ref {0} local-ref: {1} msg-id: {2} size: {3}", infoRef, localRef, msgId, message.Length);
            Console.WriteLine ("  content: " + Encoding.Default.GetString(message));
        }

        private static void endpointConnectionHandler(object parameter, Endpoint endpoint, EndpointConnection connection, bool connect)
        {
            if (connect)
            {
                Console.WriteLine("Peer {0} {1} connected from {2} (max PDU size: {3})", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress, connection.MaxPduSize);
            }
            else
            {
                Console.WriteLine("Peer {0} {1} disconnected from {2}", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress);
            }
        }

        public static void Main(string[] args)
        {
            bool running = true;

            /* run until Ctrl-C is pressed */
            Console.CancelKeyPress += delegate(object sender, ConsoleCancelEventArgs e) {
                e.Cancel = true;
                running = false;
            };

            Console.WriteLine ("Setting up TASE.2 server ...");

            /*********************************************************************
             * Define the data model
             *********************************************************************/

            DataModel dataModel = new DataModel ();

            /* get VCC pseudo domain to populate the VCC scope data model part */
            Domain vcc = dataModel.VCC;

            vcc.AddInformationMessageObject(64);
            vcc.AddInformationMessageObject(1024);

            /* Create a new domain "icc1" for communication with endpoint2 */
            Domain icc1 = dataModel.AddDomain("icc1");

            /* Add information buffer objects for IM */
            icc1.AddInformationMessageObject(64);
            icc1.AddInformationMessageObject(1024);

            /* Add some data points and devices to domain "icc1" */
            icc1Discrete1 = icc1.AddIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc1Discrete2 = icc1.AddIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc1Discrete3 = icc1.AddIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
            icc1Discrete4 = icc1.AddIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);
            icc1State1 = icc1.AddIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);

            /* add a data set transfer set to domain "icc1" */
            icc1.AddDSTransferSet("DSTrans1");

            /* add a new data set to domain "icc1" */
            DataSet ds1 = icc1.AddDataSet("ds1");

            ds1.AddEntry(icc1, "Transfer_Set_Name");
            ds1.AddEntry(icc1, "DSConditions_Detected");
            ds1.AddEntry(icc1, "Event_Code_Detected");
            ds1.AddEntry(icc1, "Transfer_Set_Time_Stamp");
            ds1.AddEntry(icc1, "Discrete1");
            ds1.AddEntry(icc1, "Discrete2");
            ds1.AddEntry(icc1, "Discrete3");
            ds1.AddEntry(icc1, "DiscreteQTimeTag4");
            ds1.AddEntry(icc1, "State1");

            /* Create a new domain "icc2" for communication with endpoint3 */
            Domain icc2 = dataModel.AddDomain("icc2");

            /* Add information buffer objects for IM */
            icc2.AddInformationMessageObject(64);
            icc2.AddInformationMessageObject(1024);

            /* Add some data points and devices to domain "icc2" */
            icc2Discrete1 = icc2.AddIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc2Discrete2 = icc2.AddIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc2Discrete3 = icc2.AddIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
            icc2Discrete4 = icc2.AddIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);
            icc2State1 = icc2.AddIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);

            /* add a data set transfer set to domain "icc2" */
            icc2.AddDSTransferSet("DSTrans1");

            /* add a new data set to domain "icc1" */
            ds1 = icc2.AddDataSet("ds1");

            ds1.AddEntry(icc2, "Transfer_Set_Name");
            ds1.AddEntry(icc2, "DSConditions_Detected");
            ds1.AddEntry(icc2, "Event_Code_Detected");
            ds1.AddEntry(icc2, "Transfer_Set_Time_Stamp");
            ds1.AddEntry(icc2, "Discrete1");
            ds1.AddEntry(icc2, "Discrete2");
            ds1.AddEntry(icc2, "Discrete3");
            ds1.AddEntry(icc2, "DiscreteQTimeTag4");
            ds1.AddEntry(icc2, "State1");

            /* create bilateral table with ID "BLT_ENDPOINT2" */
            BilateralTable blt1 = new BilateralTable("BLT_ENDPOINT2", icc1, "1.1.1.999.2", 12);

            /* add some access rules to the bilateral table */
            blt1.AddDataPoint(icc1Discrete1, true, false);
            blt1.AddDataPoint(icc1Discrete2, true, false);
            blt1.AddDataPoint(icc1Discrete3, true, false);
            blt1.AddDataPoint(icc1Discrete4, true, false);
            blt1.AddDataPoint(icc1State1, true, false);

            /* Add information message objects to bilateral table */
            blt1.AddInformationMessage(1, 1, true);

            /* create bilateral table with ID "BLT_ENDPOINT3" */
            BilateralTable blt2 = new BilateralTable("BLT_ENDPOINT3", icc2, "1.1.1.999.3", 12);

            /* add some access rules to the bilateral table */
            blt2.AddDataPoint(icc1Discrete1, true, false);
            blt2.AddDataPoint(icc1Discrete2, true, false);
            blt2.AddDataPoint(icc1Discrete3, true, false);
            blt2.AddDataPoint(icc1Discrete4, true, false);
            blt2.AddDataPoint(icc1State1, true, false);

            /* Add information message objects to bilateral table */
            blt2.AddInformationMessage(1, 1, true);
            blt2.AddInformationMessage(2, 2, true);

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

            /* create a passive endpoint (TCP server) */
            Endpoint endpoint = new Endpoint(true, tlsConfig);

            endpoint.SetIdentity("MZ Automation", "endpoint1", "1");
            endpoint.SetLocalApTitle("1.1.1.999.1", 12);

            endpoint.SetConnectionHandler(endpointConnectionHandler, null);

            /* add a TASE.2 server to the endpoint */
            Server server = new Server(dataModel, endpoint);

            server.SetDSTransferSetStateChangedHandler(dstsStateChangedHandler, null);

            /* add a TASE.2 client to the endpoint */
            client1 = new Client(endpoint);

            /* remote address is required for the client to associate with the correct incoming TCP client connection */
            client1.SetRemoteApTitle("1.1.1.999.2", 12);
            client1.SetConnectionClosedHandler (connectionClosedHandler, null);
            client1.SetInformationMessageHandler (informationMessageHandler, null);

            client2 = new Client(endpoint);
            client2.SetRemoteApTitle("1.1.1.999.3", 12);
            client2.SetConnectionClosedHandler (connectionClosedHandler, null);
            client2.SetInformationMessageHandler (informationMessageHandler, null);

            endpoint.Connect();

            if (endpoint.WaitForState(EndpointState.LISTENING, 1000))
            {
                Console.WriteLine("endpoint is listening for incoming connections");

                bool client1firstConnected = true;
                bool client2firstConnected = true;
                int msgId = 0;

                while (running)
                {

                    if (client1.GetState() == ClientState.STATE_CONNECTED)
                    {
                        try
                        {
                            if (client1firstConnected)
                            {
                                string vendor;
                                string model;
                                string revision;

                                client1.GetPeerIdentity(out vendor, out model, out revision);

                                Console.WriteLine("Peer identity:");
                                Console.WriteLine("  vendor: " + vendor);
                                Console.WriteLine("  model: " + model);
                                Console.WriteLine("  revision: " + revision);

                                client1firstConnected = false;
                            }

                            if (client1IMEnabled == false)
                            {
                                client1IMEnabled = true;

                                client1.IMTransferSetEnable();

                                Console.WriteLine("Enabled IM transfer set");
                            }
                        }
                        catch (ClientException ex)
                        {
                            Console.WriteLine("client1 error: " + ex.GetError().ToString());
                        }

                    }
                    else
                    {
                        Console.WriteLine("client (endpoint2) not connected!");
                    }

                    if (client2.GetState() == ClientState.STATE_CONNECTED)
                    {
                        try
                        {
                            if (client2firstConnected)
                            {
                                string vendor;
                                string model;
                                string revision;

                                client2.GetPeerIdentity(out vendor, out model, out revision);

                                Console.WriteLine("Peer identity:");
                                Console.WriteLine("  vendor: " + vendor);
                                Console.WriteLine("  model: " + model);
                                Console.WriteLine("  revision: " + revision);

                                client2firstConnected = false;
                            }

                            if (client2IMEnabled == false)
                            {
                                client2IMEnabled = true;

                                client2.IMTransferSetEnable();

                                Console.WriteLine("Enabled IM transfer set");
                            }
                        }
                        catch (ClientException ex)
                        {
                            Console.WriteLine("client2 error: " + ex.GetError().ToString());
                        }

                    }
                    else
                    {
                        Console.WriteLine("client (endpoint3) not connected!");
                    }

                    server.SendInformationMessage(null, 1, 1, msgId, "test info message (from endpoint1)");
                    msgId++;

                    Thread.Sleep(1000);

                } /* while (running) */

                endpoint.Disconnect();
            }
            else
            {
                Console.WriteLine("Failed to connect to peer");
            }

            endpoint.Dispose();
            server.Dispose();
            client1.Dispose();
            client2.Dispose();
        }
    }
}
