package dual_role2.endpoint1;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.concurrent.atomic.AtomicInteger;

import com.libtase2.client.Client;
import com.libtase2.client.ClientDSTransferSet;
import com.libtase2.client.ClientDataSet;
import com.libtase2.client.ClientException;
import com.libtase2.client.ClientState;
import com.libtase2.client.DSTransferSetHandler;
import com.libtase2.client.InformationMessageHandler;
import com.libtase2.client.StateChangedHandler;
import com.libtase2.common.DSConditions;
import com.libtase2.common.Endpoint;
import com.libtase2.common.EndpointConnection;
import com.libtase2.common.EndpointConnectionHandler;
import com.libtase2.common.EndpointState;
import com.libtase2.common.LibTase2;
import com.libtase2.common.LogHandler;
import com.libtase2.common.LogLevel;
import com.libtase2.common.PointValue;
import com.libtase2.common.TLSConfiguration;
import com.libtase2.common.TimeStampClass;
import com.libtase2.server.BilateralTable;
import com.libtase2.server.ControlPoint;
import com.libtase2.server.ControlPointType;
import com.libtase2.server.DataModel;
import com.libtase2.server.DataSet;
import com.libtase2.server.DeviceClass;
import com.libtase2.server.Domain;
import com.libtase2.server.IndicationPoint;
import com.libtase2.server.IndicationPointType;
import com.libtase2.server.QualityClass;
import com.libtase2.server.Server;

public class Endpoint1 {

    private static IndicationPoint vccDiscrete1;
    private static IndicationPoint vccVCCDiscreteQTimeTag1;
    private static IndicationPoint vccDiscreteQExtendedTimeTag1;
    private static IndicationPoint vccVCCDiscreteExtended;

    private static ControlPoint vccCommand1;
    private static ControlPoint vccCommand2;
    private static ControlPoint vccCommand3;

    private static IndicationPoint vccState1;
    private static IndicationPoint vccState2;
    private static IndicationPoint vccState3;
    private static IndicationPoint vccState4;

    private static IndicationPoint icc1Discrete1;
    private static IndicationPoint icc1Discrete2;
    private static IndicationPoint icc1Discrete3;
    private static IndicationPoint icc1Discrete4;

    private static IndicationPoint icc1State1;

    public static DSTransferSetHandler tsHandler = null;

    private static String byteArrayToHexString(byte[] byteArray) {
        StringBuilder hexString = new StringBuilder(byteArray.length * 2);

        for (byte b : byteArray)
            hexString.append(String.format("%02x", b));

        return hexString.toString();
    }

    public static void main(String[] args) {
        System.out.println("Using libtase2 version " + LibTase2.getVersion());

        /****************************************************************
         * Setup logging
         ***************************************************************/

        LibTase2.setLogLevel(LogLevel.DEBUG);

        LibTase2.setLogHandler(new LogHandler() {

            @Override
            public void logEvent(LogLevel logLevel, String message) {
                System.out.println("LOG(" + logLevel.toString() + "): " + message);

            }
        });

        /****************************************************************
         * Setup TLS configuration
         ***************************************************************/

        boolean useTls = true;

        TLSConfiguration tlsConfig = new TLSConfiguration();

        tlsConfig.setChainValidation(true);
        tlsConfig.setAllowOnlyKnownCertificates(false);

        if (tlsConfig.setOwnKey("server-key.pem", null) == false)
            useTls = false;

        if (tlsConfig.setOwnCertificate("server.cer") == false)
            useTls = false;

        if (tlsConfig.addCACertificate("root.cer") == false)
            useTls = false;

        if (useTls)
            System.out.println("TLS configured");
        else
            System.out.println("TLS not configured - missing key material");

        /****************************************************************
         * TASE.2 data model setup (data points, data sets, transfer sets)
         ***************************************************************/

        DataModel dataModel = new DataModel();

        /* get VCC pseudo domain to populate the VCC scope data model part */
        Domain vcc = dataModel.getVCC();

        /* Add information buffer objects for IM */
        vcc.addInformationMessageObject(64);
        vcc.addInformationMessageObject(128);
        vcc.addInformationMessageObject(1024);

        /*
         * Add some data points and devices to VCC scope (control center independent
         * information)
         */
        vccDiscrete1 = vcc.addIndicationPoint("VCCDiscrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        vccVCCDiscreteQTimeTag1 = vcc.addIndicationPoint("VCCDiscreteQTimeTag1", IndicationPointType.DISCRETE,
                QualityClass.QUALITY, TimeStampClass.TIMESTAMP, false, true);
        vccDiscreteQExtendedTimeTag1 = vcc.addIndicationPoint("VCCDiscreteQExtendedTimeTag1",
                IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP_EXTENDED, true, true);
        vccVCCDiscreteExtended = vcc.addIndicationPoint("VCCDiscreteExtended", IndicationPointType.DISCRETE,
                QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, true);

        vccCommand1 = vcc.addControlPoint("Command1", ControlPointType.COMMAND, DeviceClass.SBO, true, (short) 1365);
        vccCommand2 = vcc.addControlPoint("Command2", ControlPointType.COMMAND, DeviceClass.DIRECT, true, (short) 1366);
        vccCommand3 = vcc.addControlPoint("Command3", ControlPointType.COMMAND, DeviceClass.DIRECT, true, (short) 1367);

        vccState1 = vcc.addIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        vccState2 = vcc.addIndicationPoint("State2", IndicationPointType.STATE, QualityClass.QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        vccState3 = vcc.addIndicationPoint("State3", IndicationPointType.STATE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP, false, true);
        vccState4 = vcc.addIndicationPoint("State4", IndicationPointType.STATE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP_EXTENDED, false, true);

        /*
         * Create a new domain "icc1" (for information to exchange with specific control
         * center "icc1")
         */
        Domain icc1 = dataModel.addDomain("icc1");

        /* Add information buffer objects for IM */
        icc1.addInformationMessageObject(64);
        icc1.addInformationMessageObject(1024);

        /* Add some data points and devices to domain "icc1" */
        icc1Discrete1 = icc1.addIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        icc1Discrete2 = icc1.addIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        icc1Discrete3 = icc1.addIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, false);
        icc1Discrete4 = icc1.addIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP, true, false);

        icc1.addIndicationPoint("Real1", IndicationPointType.REAL, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP,
                false, true);
        icc1.addIndicationPoint("RealQ2", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.NO_TIMESTAMP,
                false, true);
        icc1.addIndicationPoint("RealQTimeTag3", IndicationPointType.REAL, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP, false, true);

        icc1State1 = icc1.addIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);

        icc1.addProtectionEquipment("Protect1", false);
        icc1.addProtectionEquipment("ProtectPacked1", true);

        icc1.addControlPoint("Command1", ControlPointType.COMMAND, DeviceClass.SBO, true, (short) 1365);

        /* add a data set transfer set to domain "icc1" */
        icc1.addDSTransferSet("DSTrans1");

        /* add a new data set to domain "icc1" */
        DataSet ds1 = icc1.addDataSet("ds1");

        ds1.addEntry(icc1, "Transfer_Set_Name");
        ds1.addEntry(icc1, "DSConditions_Detected");
        ds1.addEntry(icc1, "Event_Code_Detected");
        ds1.addEntry(icc1, "Transfer_Set_Time_Stamp");
        ds1.addEntry(vcc, "VCCDiscrete1");
        ds1.addEntry(icc1, "Discrete1");
        ds1.addEntry(icc1, "Discrete2");
        ds1.addEntry(icc1, "DiscreteQTimeTag4");

        /* Create a new domain "icc2" */
        Domain icc2 = dataModel.addDomain("icc2");

        /* Add some data points to domain "icc2" */
        icc2.addIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        icc2.addIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        icc2.addIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, false);
        icc2.addIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP, true, false);

        icc2.addIndicationPoint("Real1", IndicationPointType.REAL, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP,
                false, true);
        icc2.addIndicationPoint("RealQ2", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.NO_TIMESTAMP,
                false, true);
        icc2.addIndicationPoint("RealQTimeTag3", IndicationPointType.REAL, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP, false, true);

        /******************************************************************
         * Define the bilateral tables (BLT)
         *
         * - if no BLT is available all data can be accessed - only data/control points
         * that are part of the BLT can be accessed by the client - the client is
         * identified by its application reference - all data/control point that are
         * part of the client's ICC can be accessed
         *
         ******************************************************************/

        BilateralTable blt1 = new BilateralTable("BLT_MZA_001_V1", icc1, "1.1.999.2", 12);

        blt1.addDataPoint(vccDiscrete1, true, false);
        blt1.addDataPoint(vccVCCDiscreteQTimeTag1, true, false);

        blt1.addDataPoint(vccState1, true, false);
        blt1.addDataPoint(vccState2, true, false);
        blt1.addDataPoint(vccState3, true, false);
        blt1.addDataPoint(vccState4, true, false);

        blt1.addControlPoint(vccCommand1, (short) 1234, true, true, true, true);
        blt1.addControlPoint(vccCommand2, (short) 1236, true, true, true, true);
        blt1.addControlPoint(vccCommand3, (short) 1235, true, true, true, true);

        /* Add information message objects to bilateral table */
        blt1.addInformationMessage(1, 1, true);

        /* create bilateral table with ID "BLT_MZA_002_V1" */
        BilateralTable blt2 = new BilateralTable("BLT_MZA_002_V1", icc2, "1.1.999.3", 12);

        /* add some access rules to the bilateral table */
        blt2.addDataPoint(vccDiscrete1, true, false);
        blt2.addControlPoint(vccCommand2, (short) 1236, true, true, true, true);

        blt2.addDataPoint(vccState1, true, false);
        blt2.addDataPoint(vccState2, true, false);
        blt2.addDataPoint(vccState3, true, false);
        blt2.addDataPoint(vccState4, true, false);

        /* Add information message objects to bilateral table */
        blt2.addInformationMessage(1, 1, true);
        blt2.addInformationMessage(2, 2, false);

        /****************************************************************
         * TASE.2 endpoint setup
         ***************************************************************/

        Endpoint endpoint = new Endpoint(true, null);

        endpoint.setLocalApTitle("1.1.999.1", 12);
        endpoint.setLocalIpAddress("0.0.0.0");

        endpoint.setConnectionHandler(new EndpointConnectionHandler() {

            @Override
            public void disconnected(Endpoint endpoint, Object parameter, EndpointConnection connection) {
                System.out.println(
                        "endpoint (IP " + connection.getPeerIpAddress() + " AP-title: " + connection.getPeerApTitle()
                                + " AE-qualifier: " + connection.getPeerAeQualifier() + ") connected");

                if (connection.getPSelector() != null)
                    System.out.println("  P-sel: " + byteArrayToHexString(connection.getPSelector()));

                if (connection.getSSelector() != null)
                    System.out.println("  S-sel: " + byteArrayToHexString(connection.getSSelector()));

                if (connection.getPSelector() != null)
                    System.out.println("  T-sel: " + byteArrayToHexString(connection.getTSelector()));
            }

            @Override
            public void connected(Endpoint endpoint, Object parameter, EndpointConnection connection) {
                System.out.println(
                        "endpoint (IP " + connection.getPeerIpAddress() + " AP-title: " + connection.getPeerApTitle()
                                + " AE-qualifier: " + connection.getPeerAeQualifier() + ") disconnected");
            }
        }, null);

        Server server = new Server(dataModel, endpoint);

        server.addBilateralTable(blt1);
        server.addBilateralTable(blt2);

        Client client = new Client(endpoint);

        client.setRemoteApTitle("1.1.999.2", 12);

        client.setStateChangedHandler(new StateChangedHandler() {

            @Override
            public void stateChanged(Object parameter, Client client, ClientState newState) {
                System.out.println("Client state changed: " + newState.toString());
            }
        }, null);

        client.setIMTransferSetHandler(new InformationMessageHandler() {

            @Override
            public void newMessage(Object parameter, int infoRef, int localRef, int msgId, byte[] messageBuffer) {
                System.out.println("Received information message");
                System.out.println("  info-ref: " + infoRef);
                System.out.println("  local-ref: " + localRef);
                System.out.println("  msg-id: " + msgId);
                try {
                    System.out.println("  message: " + new String(messageBuffer, "UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }
        }, null);

        tsHandler = new DSTransferSetHandler() {

            @Override
            public void valueReceived(ClientDSTransferSet transferSet, String domainName, String pointName,
                    PointValue value) {
                System.out.println("  reported value (" + domainName + "/" + pointName + "): " + value.toString());
            }

            @Override
            public void startReport(ClientDSTransferSet transferSet, long seq, Object parameter) {
                // TODO Auto-generated method stub

            }

            @Override
            public void finishedReport(ClientDSTransferSet transferSet, long seq, Object parameter) {
                // TODO Auto-generated method stub

            }

            @Override
            public void finalize() {
                System.err.println("Finalizer called");
            }
        };

        client.setDSTransferSetHandler(tsHandler, null);

        endpoint.connect();

        if (endpoint.waitForState(EndpointState.LISTENING, 1000)) {
            System.out.println("Endpoint is listening for incoming connections...");

            AtomicInteger running = new AtomicInteger(1);

            Runnable runnable = () -> {

                boolean firstConnected = true;

                while (running.get() == 1) {

                    if (client.getState() == ClientState.CONNECTED) {

                        if (firstConnected) {
                            try {
                                /* read peer identity */

                                String[] identity = client.readPeerIdentity();

                                System.out.println("Peer identity:");
                                System.out.println("  vendor: " + identity[0]);
                                System.out.println("  model: " + identity[1]);
                                System.out.println("  revision: " + identity[2]);

                                /* Enable IM (information message) transfer sets */
                                client.enableIMTransferSet();

                                /* Enable DSTS */
                                ClientDataSet dataSet = client.getDataSet("icc1", "ds1");

                                ClientDSTransferSet ts = client.getNextDSTransferSet("icc1");

                                if (ts != null) {
                                    ts.setDataSet(dataSet);
                                    ts.writeDataSetName(client, "icc1", dataSet.getDataSetName());
                                    ts.writeInterval(client, 1);
                                    ts.writeIntegrityCheck(client, 1);
                                    ts.writeRBE(client, true);
                                    ts.writeCritical(client, true);
                                    ts.writeBufferTime(client, 0);
                                    ts.writeDSConditionsRequested(client,
                                            DSConditions.INTEGRITY + DSConditions.INTERVAL + DSConditions.CHANGE);
                                    ts.writeStatus(client, true);
                                }

                            } catch (ClientException e) {
                                System.err.println(
                                        "ClientException - " + e.getMessage() + ": " + e.getError().toString());
                            }

                            firstConnected = false;
                        }

                    } else {
                        firstConnected = true;
                    }

                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            };

            Thread thread = new Thread(runnable);
            thread.start();

            System.out.println("press any key to stop");

            try {
                System.in.read();
            } catch (IOException e) {
                e.printStackTrace();
            }

            running.set(0);
            try {
                thread.join(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        } else {
            System.err.println("Failed to setup passive endpoint");
        }

        System.out.println("Endpoint stopped.Exit");
    }
}
