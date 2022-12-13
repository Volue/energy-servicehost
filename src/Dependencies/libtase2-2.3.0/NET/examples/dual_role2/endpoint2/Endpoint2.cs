using System;
using TASE2.Library.Common;
using TASE2.Library.Client;
using TASE2.Library.Server;
using System.Threading;
using System.Security.Cryptography;
using System.Text;

namespace endpoint2
{
    class Endpoint2
    {
        static bool peerIMEnabled = false;
        static bool peerDSTSEnabled = false;

        /* callback handler that is called when the server closes the connection */
        private static void connectionClosedHandler(object paramter, Client conneciton)
        {
            Console.WriteLine ("Server closed connection!");
        }

        private static void endpointConnectionHandler(object parameter, Endpoint endpoint, EndpointConnection connection, bool connect)
        {
            if (connect)
            {
                Console.WriteLine("Peer {0} {1} connected from {2} (max. PDU size: {3})", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress, connection.MaxPduSize);
            }
            else
            {
                Console.WriteLine("Peer {0} {1} disconnected from {2}", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress);
                peerIMEnabled = false;
                peerDSTSEnabled = false;
            }
        }

        /* callback hander for information messages */
        private static void informationMessageHandler(object parameter, int infoRef, int localRef, int msgId, byte[] message)
        {
            Console.WriteLine ("IM message: info-ref {0} local-ref: {1} msg-id: {2} size: {3}", infoRef, localRef, msgId, message.Length);
            Console.WriteLine ("  content: " + Encoding.Default.GetString(message));
        }

        /* callback handler that is called twice for each received transfer set report */
        private static void dsTransferSetReportHandler(object parameter, bool finished, UInt32 seqNo, ClientDSTransferSet transferSet)
        {
            if (finished)
            {
                Console.WriteLine("--> ({0}) report processing finished", seqNo);
            }
            else
            {
                Console.WriteLine("New report received with seq no: {0} transfer set: {1}", seqNo,
                    transferSet != null ? transferSet.GetName() : "");
            }
        }

        /* callback handler that is called for each data point of a received transfer set report */
        private static void dsTransferSetValueHandler(object parameter, ClientDSTransferSet transferSet, string domainName, string pointName, PointValue pointValue)
        {
            Console.WriteLine("  Received report value for: {0}:{1} type: {2}", domainName != null ? domainName : "-", pointName, pointValue.Type);

            if (pointValue.Type == PointValueType.STATE)
            {
                Console.WriteLine("    value: {0}", pointValue.ValueState.GetStateValue().ToString());
            }
            else if (pointValue.Type == PointValueType.DISCRETE)
            {
                Console.WriteLine("    value: {0}", pointValue.ValueDiscrete.ToString());
            }
            else if (pointValue.Type == PointValueType.REAL)
            {
                Console.WriteLine("    value: {0}", pointValue.ValueReal.ToString());
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

            /* create an active endpoint (TCP server) */
            Endpoint endpoint = new Endpoint(false, tlsConfig);

            endpoint.SetIdentity("MZ Automation", "endpoint2", "1");
            endpoint.SetLocalApTitle("1.1.999.2", 12);
           // endpoint.SetRemoteIpAddress("192.168.2.101");

            endpoint.SetConnectionHandler(endpointConnectionHandler, null);

            /* add a TASE.2 server to the endpoint */
            Server server = new Server(dataModel, endpoint);

            /* add a TASE.2 client to the endpoint */
            Client client = new Client(endpoint);

            /* remote address is required for the client to associate with the correct incoming TCP client connection */
            client.SetRemoteApTitle("1.1.1.999.1", 12);

            client.SetConnectionClosedHandler (connectionClosedHandler, null);

            client.SetInformationMessageHandler (informationMessageHandler, null);

            /* set callback handlers for transfer set reports */
            client.SetDSTransferSetValueHandler(dsTransferSetValueHandler, null);
            client.SetDSTransferSetReportHandler(dsTransferSetReportHandler, null);

            endpoint.SetConnectTimeout(4000);
            endpoint.SetRequestTimeout(2000);

            while (running)
            {
                Console.WriteLine("Connect ...");
                endpoint.Connect();

                if (endpoint.WaitForState(EndpointState.CONNECTED, 5000))
                {
                    Console.WriteLine("Endpoint connected");

                    int msgId = 0;

                    while (running)
                    {

                        if (endpoint.State == EndpointState.CONNECTED)
                        {
                            if (client.GetState() == ClientState.STATE_CONNECTED)
                            {
                                try
                                {

                                    try
                                    {
                                        string vendor;
                                        string model;
                                        string revision;

                                        client.GetPeerIdentity(out vendor, out model, out revision);

                                        Console.WriteLine("Peer identity:");
                                        Console.WriteLine("  vendor: " + vendor);
                                        Console.WriteLine("  model: " + model);
                                        Console.WriteLine("  revision: " + revision);
                                    }
                                    catch (ClientException ex)
                                    {
                                        Console.WriteLine("Get peer identity failed: " + ex.Message);
                                        endpoint.Disconnect();
                                    }


                                    if (peerIMEnabled == false)
                                    {
                                        Console.WriteLine("Enable IM transfer set...");

                                        client.IMTransferSetEnable();

                                        Console.WriteLine("Enabled IM transfer set");

                                        /* Enable information message transfer set */
                                        peerIMEnabled = true;
                                    }

                                    if (peerDSTSEnabled == false)
                                    {
                                        Console.WriteLine("Enable DSTS");



                                        /* get next available transfer set from domain "icc1" */
                                        ClientDSTransferSet transferSet = client.GetNextDSTransferSet("icc1");

                                        ClientDataSet dataSet = client.GetDataSet("icc1", "ds1");

                                        /* connect data set with transfer set (received report values will be stored in the ClientDataSet instance) */
                                        transferSet.SetDataSet(dataSet);

                                        /* configure data set transfer set */
                                        transferSet.WriteDataSetName(dataSet.GetDomainName(), dataSet.GetDataSetName());

                                        /* read transfer set paramters from server */
                                        transferSet.ReadValues();

                                        Console.WriteLine(" data-set: {0}:{1}", transferSet.GetDataSetDomain(), transferSet.GetDataSetName());

                                        /* Set some transfer set parameters */
                                        transferSet.WriteInterval(1);
                                        transferSet.WriteRBE(true);
                                        transferSet.WriteCritical(true);
                                        transferSet.WriteDSConditionsRequested(ReportReason.INTERVAL_TIMEOUT | ReportReason.OBJECT_CHANGE);

                                        /* Enable transfer set reporting */
                                        transferSet.WriteStatus(true);
                                        Console.WriteLine("DSTS enabled");

                                        /* Enable data set transfer set */
                                        peerDSTSEnabled = true;
                                    }

                                }
                                catch (ClientException ex)
                                {
                                    Console.WriteLine("client error: " + ex.GetError().ToString());
                                }

                            }
                            else
                            {
                                Console.WriteLine("client not connected!");
                            }

                            server.SendInformationMessage(null, 1, 1, msgId, "test info message (from endpoint2)");
                            msgId++;

                            Thread.Sleep(1000);
                        }
                        else
                        {
                            Console.WriteLine("endpoint not connected");
                            break;
                        }

                    } /* while (running) */

                    endpoint.Disconnect();
                }
                else
                {
                    endpoint.Disconnect();
                    Console.WriteLine("Failed to connect to peer");
                }

                Thread.Sleep(3000);
            }

            endpoint.Dispose();
            server.Dispose();
            client.Dispose();
        }
    }
}
