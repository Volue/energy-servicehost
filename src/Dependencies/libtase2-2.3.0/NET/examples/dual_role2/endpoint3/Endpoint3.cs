using System;
using TASE2.Library.Common;
using TASE2.Library.Client;
using TASE2.Library.Server;
using System.Threading;
using System.Security.Cryptography;
using System.Text;

namespace endpoint3
{
    class Endpoint3
    {
        static bool peerIMEnabled = false;

        /* callback handler that is called when the server closes the connection */
        private static void connectionClosedHandler(object paramter, Client conneciton)
        {
            Console.WriteLine ("Server closed connection!");
            peerIMEnabled = false;
        }

        private static void endpointConnectionHandler(object parameter, Endpoint endpoint, EndpointConnection connection, bool connect)
        {
            if (connect)
            {
                Console.WriteLine("Peer {0} {1} connected from {2}", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress);
            }
            else
            {
                Console.WriteLine("Peer {0} {1} disconnected from {2}", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress);
            }
        }

        /* callback hander for information messages */
        private static void informationMessageHandler(object parameter, int infoRef, int localRef, int msgId, byte[] message)
        {
            Console.WriteLine ("IM message: info-ref {0} local-ref: {1} msg-id: {2} size: {3}", infoRef, localRef, msgId, message.Length);
            Console.WriteLine ("  content: " + Encoding.Default.GetString(message));
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

            endpoint.SetIdentity("MZ Automation", "endpoint3", "1");
            endpoint.SetLocalApTitle("1.1.1.999.3", 12);

            endpoint.SetConnectionHandler(endpointConnectionHandler, null);

            /* add a TASE.2 server to the endpoint */
            Server server = new Server(dataModel, endpoint);

            /* add a TASE.2 client to the endpoint */
            Client client = new Client(endpoint);

            /* remote address is required for the client to associate with the correct incoming TCP client connection */
            client.SetRemoteApTitle("1.1.1.999.1", 12);

            client.SetConnectionClosedHandler (connectionClosedHandler, null);

            client.SetInformationMessageHandler (informationMessageHandler, null);

            endpoint.Connect();

            if (endpoint.WaitForState(EndpointState.CONNECTED, 2000))
            {
                Console.WriteLine("endpoint is listening for incoming connections");

                bool firstConnected = true;
                int msgId = 0;

                while (running)
                {

                    if (client.GetState() == ClientState.STATE_CONNECTED)
                    {
                        try
                        {
                            if (firstConnected)
                            {
                                string vendor;
                                string model;
                                string revision;

                                client.GetPeerIdentity(out vendor, out model, out revision);

                                Console.WriteLine("Peer identity:");
                                Console.WriteLine("  vendor: " + vendor);
                                Console.WriteLine("  model: " + model);
                                Console.WriteLine("  revision: " + revision);

                                firstConnected = false;
                            }

                            if (peerIMEnabled == false)
                            {
                                peerIMEnabled = true;

                                client.IMTransferSetEnable();

                                Console.WriteLine("Enabled IM transfer set");
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

                    server.SendInformationMessage(null, 1, 1, msgId, "test info message (from endpoint3)");
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
            client.Dispose();
        }
    }
}
