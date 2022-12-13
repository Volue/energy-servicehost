package libtase2.examples;

import java.io.UnsupportedEncodingException;
import java.util.Collection;
import java.util.LinkedList;

import com.libtase2.client.Client;
import com.libtase2.client.ClientDSTransferSet;
import com.libtase2.client.ClientDataSet;
import com.libtase2.client.ClientError;
import com.libtase2.client.ClientException;
import com.libtase2.client.ClientState;
import com.libtase2.client.ConnectionClosedHandler;
import com.libtase2.client.DSTransferSetHandler;
import com.libtase2.client.InformationMessageHandler;
import com.libtase2.client.StateChangedHandler;
import com.libtase2.client.SupportedFeatures;
import com.libtase2.common.Command;
import com.libtase2.common.DSConditions;
import com.libtase2.common.Endpoint;
import com.libtase2.common.EndpointConnection;
import com.libtase2.common.LibTase2;
import com.libtase2.common.LogHandler;
import com.libtase2.common.LogHandlerEx;
import com.libtase2.common.LogLevel;
import com.libtase2.common.LogSource;
import com.libtase2.common.PointValue;
import com.libtase2.common.PointValueException;
import com.libtase2.common.TagValue;

public class ClientExample {

    public static void main(String[] args) throws ClientException {
        LibTase2.setLogHandler(new LogHandlerEx() {

            @Override
            public void logEvent(LogLevel logLevel, LogSource source, Endpoint endpoint, EndpointConnection peer,
                    String message) {

                if (endpoint != null) {

                    if (peer != null) {
                        System.out.println("LOG(" + logLevel.toString() + ") [" + endpoint.getId() + "]["
                                + peer.getPeerIpAddress() + "] : " + message);
                    } else {
                        System.out.println("LOG(" + logLevel.toString() + ") [" + endpoint.getId() + "]: " + message);
                    }
                } else
                    System.out.println("LOG(" + logLevel.toString() + "): " + message);

            }
        });

        String remoteAddress = "localhost";

        if (args.length > 0)
            remoteAddress = args[0];

        LibTase2.setLogLevel(LogLevel.DEBUG);

        System.out.println("Start client...");

        Client client = new Client();

        client.setStateChangedHandler(new StateChangedHandler() {

            @Override
            public void stateChanged(Object parameter, Client client, ClientState newState) {
                System.out.println("Connection state: " + newState.toString());
            }
        }, null);

        byte[] tsel = { 0, 1 };
        byte[] ssel = { 0, 1 };
        byte[] psel = { 0, 0, 0, 3 };

        client.setLocalAddresses(psel, ssel, tsel);

        client.setRemoteApTitle("1.1.1.998", 12);
        client.connect(remoteAddress, "1.1.1.999", 12);

        client.setIMTransferSetHandler(new InformationMessageHandler() {

            @Override
            public void newMessage(Object parameter, int infoRef, int localRef, int msgId, byte[] messageBuffer) {
                System.out.printf("Information message: info-ref: %d local-ref: %d msg-id: %d\n  message: ", infoRef,
                        localRef, msgId);
                try {
                    System.out.println(new String(messageBuffer, "UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }

            }
        }, null);

        client.setDSTransferSetHandler(new DSTransferSetHandler() {

            @Override
            public void valueReceived(ClientDSTransferSet transferSet, String domainName, String pointName,
                    PointValue value) {
                System.out.printf("  DS value: %s/%s: %s\n", domainName, pointName, value.toString());
            }

            @Override
            public void startReport(ClientDSTransferSet transferSet, long seq, Object parameter) {
                if (transferSet != null)
                    System.out.println("New DS report received: " + transferSet.getName() + " seq: " + seq);
                else
                    System.out.println("New DS report received - seq: " + seq);
            }

            @Override
            public void finishedReport(ClientDSTransferSet transferSet, long seq, Object parameter) {
                System.out.println("New DS report finished: " + seq);
            }
        }, null);

        ClientState state = client.getState();

        System.out.println("State: " + state.toString());

        SupportedFeatures sf = client.getSupportedFeatures();

        System.out.println("supported features: " + sf.toString());

        String bltId = client.getBilateralTableId("icc1");

        System.out.println("BLT: " + bltId);

        String[] peerIdentity = client.readPeerIdentity();

        System.out.println("Identity:");
        System.out.println("  vendor: " + peerIdentity[0]);
        System.out.println("  model: " + peerIdentity[1]);
        System.out.println("  revision: " + peerIdentity[2]);

        Collection<String> vccVariables = client.getVCCVariables();

        for (String variableName : vccVariables) {
            System.out.println("* " + variableName);
        }

        client.enableIMTransferSet();

//        if (client.setTag(null, "Command1", TagValue.CLOSE_ONLY_INHIBIT, "maintenance work in progress"))
//        {
//            System.out.println("Set tag success");
//        }

        try {
            int checkback = client.selectDevice(null, "Command1");

            System.out.println("Command1 selected - checkback = " + checkback);
        } catch (ClientException ex) {
            System.out.println(ex.getMessage() + ": " + ex.getError().toString());
        }

        try {
            if (client.sendCommand(null, "Command1", Command.CLOSE)) {
                System.out.println("Send command success");
            }
        } catch (ClientException ex) {
            System.out.println(ex.getMessage() + ": " + ex.getError().toString());
        }

        /* read a single data point */

        PointValue pointValue = client.readPointValue(null, "VCCDiscreteQTimeTag1");

        if (pointValue != null) {
            System.out.println("  value = " + pointValue.getValueDiscrete());

            try {
                System.out.println("  timestamp = " + pointValue.getTimeStamp());
            } catch (PointValueException e) {
                System.err.println("  " + e.getMessage());
            }

            try {
                System.out.println("  quality flags = " + pointValue.getFlags());
            } catch (PointValueException e) {
                System.err.println("  " + e.getMessage());
            }

            try {
                System.out.println("  COV: " + pointValue.getCOV());
            } catch (PointValueException e) {
                System.err.println("  " + e.getMessage());
            }

        }

        /* read multiple data points */

        Collection<String> pointNames = new LinkedList<String>();

        pointNames.add("Discrete1");
        pointNames.add("Discrete2");
        pointNames.add("Discrete3");
        pointNames.add("Real1");
        pointNames.add("RealQ2");
        pointNames.add("RealQ2UNKNOWN");

        Collection<PointValue> pointValues = client.readMultiplePointValues("icc1", pointNames);

        for (PointValue pv : pointValues) {
            System.out.println("  value = " + pv.toString());
        }

        /* Delete data set when already exists */
        try {
            client.deleteDataSet("icc1", "DSR_AUTO_0000");
            System.out.println("Deleted DS DSR_AUTO_0000");
        } catch (ClientException e1) {
            System.out.println(e1.getMessage() + ": " + e1.getError().toString());
        }

        /* Create a new dynamic data set */

        Collection<String> newDataSet = new LinkedList<String>();

        newDataSet.add("icc1/Transfer_Set_Name");
        newDataSet.add("icc1/Transfer_Set_Time_Stamp");
        newDataSet.add("icc1/DSConditions_Detected");
        newDataSet.add("icc1/Real1");
        newDataSet.add("icc1/RealQ2");
        newDataSet.add("icc1/RealQTimeTag3");
        newDataSet.add("icc1/State1");

        try {
            client.createDataSet("icc1", "DSR_AUTO_0000", newDataSet);
            System.out.println("Created DS DSR_AUTO_0000");
        } catch (ClientException e1) {
            System.out.println(e1.getMessage() + ": " + e1.getError().toString());
        }

        try {

            ClientDataSet ds = client.getDataSet("icc1", "DSR_AUTO_0000");

            ClientDSTransferSet dsts = client.getNextDSTransferSet("icc1");

            dsts.setDataSet(ds);

            System.out.println(" --> transfer set: " + dsts.getName());

            dsts.readValues(client);

            dsts.setDataSetName("icc1", "DSR_AUTO_0000");
            dsts.setInterval(5);
            dsts.setTLE(60);
            dsts.setBufferTime(2);
            dsts.setIntegrityCheck(5);
            dsts.setRBE(true);
            dsts.setCritical(false);
            dsts.setDSConditionsRequested(DSConditions.INTEGRITY | DSConditions.CHANGE);
            dsts.setStatus(true);

            dsts.writeValues(client);

            try {
                Thread.sleep(10000);
            } catch (InterruptedException e) {
            }

            /* disable DS transfer set */

            // dsts.setStatus(false);
            // dsts.writeValues(client);
        } catch (ClientException e1) {
            System.out.println(e1.getMessage() + ": " + e1.getError().toString());
        }

//        Collection<String> newDataSet2 = new LinkedList<String>();
//        
//        newDataSet2.add("icc1/Transfer_Set_Name");
//        newDataSet2.add("icc1/Transfer_Set_Time_Stamp");
//        newDataSet2.add("icc1/DSConditions_Detected");
//        newDataSet2.add("VCCDiscrete1");
//        newDataSet2.add("VCCDiscreteQTimeTag1");
//        newDataSet2.add("icc1/Discrete1");
//        newDataSet2.add("icc1/Discrete2");
//        newDataSet2.add("icc1/Discrete3");
//        
//        try {
//            client.createDataSet("icc1", "DSR_AUTO_0001", newDataSet2);
//            System.out.println("Created DS DSR_AUTO_0001");
//        } catch (ClientException e1) {
//            System.out.println(e1.getMessage() + ": " + e1.getError().toString());
//        }
//        
//        try {
//            
//            ClientDataSet ds = client.getDataSet("icc1", "DSR_AUTO_0001");
//            
//            ClientDSTransferSet dsts = client.getNextDSTransferSet("icc1");
//            
//            dsts.setDataSet(ds);
//            
//            System.out.println(" --> transfer set: " + dsts.getName());
//            
//            dsts.readValues(client);
//            
//            dsts.setDataSetName("icc1", "DSR_AUTO_0001");
//            dsts.setInterval(5);
//            dsts.setTLE(60);
//            dsts.setBufferTime(2);
//            dsts.setIntegrityCheck(30);
//            dsts.setRBE(true);
//            dsts.setCritical(false);
//            dsts.setDSConditionsRequested(DSConditions.INTERVAL | DSConditions.CHANGE);
//            dsts.setStatus(true);
//            
//            dsts.writeValues(client);
//        }
//        catch (ClientException e1) {
//            System.out.println(e1.getMessage() + ": " + e1.getError().toString());
//        }

        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
        }

        client.disconnect();
        System.out.println("Disconnected");

        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        client.dispose();
    }
}
