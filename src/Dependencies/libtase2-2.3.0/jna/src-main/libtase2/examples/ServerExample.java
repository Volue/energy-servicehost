package libtase2.examples;

import java.io.IOException;
import java.util.Collection;

import com.libtase2.common.AuthenticationMethod;
import com.libtase2.common.ConnectionAcceptHandler;
import com.libtase2.common.Endpoint;
import com.libtase2.common.EndpointConnection;
import com.libtase2.common.EndpointConnectionHandler;
import com.libtase2.common.LibTase2;
import com.libtase2.common.LogHandlerEx;
import com.libtase2.common.LogLevel;
import com.libtase2.common.LogSource;
import com.libtase2.common.ReportReason;
import com.libtase2.common.Tag;
import com.libtase2.common.TimeStampClass;
import com.libtase2.server.BilateralTable;
import com.libtase2.server.ClientConnectionHandler;
import com.libtase2.server.ControlPoint;
import com.libtase2.server.ControlPointType;
import com.libtase2.server.DSTransferSet;
import com.libtase2.server.DSTransferSetReportSentHandler;
import com.libtase2.server.DSTransferSetUpdateHandler;
import com.libtase2.server.DataModel;
import com.libtase2.server.DataPoint;
import com.libtase2.server.DataSet;
import com.libtase2.server.DataSetEventHandler;
import com.libtase2.server.DeviceClass;
import com.libtase2.server.DeviceOperationHandler;
import com.libtase2.server.Domain;
import com.libtase2.server.Edition;
import com.libtase2.server.HandlerResult;
import com.libtase2.server.IMTransferSetEventHandler;
import com.libtase2.server.IndicationPoint;
import com.libtase2.server.IndicationPointType;
import com.libtase2.server.OperateValue;
import com.libtase2.server.QualityClass;
import com.libtase2.server.SentPointValue;
import com.libtase2.server.Server;

public class ServerExample {

    private static String byteArrayToHexString(byte[] byteArray) {
        StringBuilder hexString = new StringBuilder(byteArray.length * 2);

        for (byte b : byteArray)
            hexString.append(String.format("%02x", b));

        return hexString.toString();
    }

    public static void main(String[] args) {
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

        LibTase2.setLogLevel(LogLevel.ERROR);

        DataModel dataModel = new DataModel();

        // Set TASE.2/ICCP edition
        // dataModel.setEdition(Edition.EDITION_1996_08);

        Domain vcc = dataModel.getVCC();

        vcc.addInformationMessageObject(64);
        vcc.addInformationMessageObject(1024);

        IndicationPoint vccDiscrete1 = vcc.addIndicationPoint("VCCDiscrete1", IndicationPointType.DISCRETE,
                QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
        IndicationPoint vccVCCDiscreteQTimeTag1 = vcc.addIndicationPoint("VCCDiscreteQTimeTag1",
                IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, false, true);

        vcc.addIndicationPoint("VCCDiscreteQExtendedTimeTag1", IndicationPointType.DISCRETE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP_EXTENDED, true, true);
        IndicationPoint vccVCCDiscreteExtended = vcc.addIndicationPoint("VCCDiscreteExtended",
                IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, true);

        ControlPoint vccCommand1 = vcc.addControlPoint("Command1", ControlPointType.COMMAND, DeviceClass.SBO, true,
                (short) 1365);
        ControlPoint vccCommand2 = vcc.addControlPoint("Command2", ControlPointType.COMMAND, DeviceClass.DIRECT, true,
                (short) 1366);
        ControlPoint vccCommand3 = vcc.addControlPoint("Command3", ControlPointType.COMMAND, DeviceClass.DIRECT, true,
                (short) 1367);

        IndicationPoint vccState1 = vcc.addIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        IndicationPoint vccState2 = vcc.addIndicationPoint("State2", IndicationPointType.STATE, QualityClass.QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);
        IndicationPoint vccState3 = vcc.addIndicationPoint("State3", IndicationPointType.STATE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP, false, true);
        IndicationPoint vccState4 = vcc.addIndicationPoint("State4", IndicationPointType.STATE, QualityClass.QUALITY,
                TimeStampClass.TIMESTAMP_EXTENDED, false, true);

        Domain icc1 = dataModel.addDomain("icc1");

        /* Add information buffer objects for IM */
        icc1.addInformationMessageObject(64);
        icc1.addInformationMessageObject(1024);

        /* Add some data points and devices to domain "icc1" */
        IndicationPoint icc1Discrete1 = icc1.addIndicationPoint("Discrete1", IndicationPointType.DISCRETE,
                QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
        IndicationPoint icc1Discrete2 = icc1.addIndicationPoint("Discrete2", IndicationPointType.DISCRETE,
                QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
        IndicationPoint icc1Discrete3 = icc1.addIndicationPoint("Discrete3", IndicationPointType.DISCRETE,
                QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
        IndicationPoint icc1Discrete4 = icc1.addIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE,
                QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);

        icc1.addIndicationPoint("Real1", IndicationPointType.REAL, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP,
                false, true);
        icc1.addIndicationPoint("RealQ2", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.NO_TIMESTAMP,
                false, true);
        icc1.addIndicationPoint("RealQTimeTag3", IndicationPointType.REAL, QualityClass.QUALITY,
                TimeStampClass.NO_TIMESTAMP, false, true);

        IndicationPoint icc1State1 = icc1.addIndicationPoint("State1", IndicationPointType.STATE,
                QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);

        icc1.addProtectionEquipment("Protect1", false);
        icc1.addProtectionEquipment("ProtectPacked1", true);

        icc1.addControlPoint("Command1", ControlPointType.COMMAND, DeviceClass.SBO, true, (short) 1365);

        DataSet ds1 = icc1.addDataSet("ds1");

        ds1.addEntry(icc1, "Transfer_Set_Name");
        ds1.addEntry(icc1, "DSConditions_Detected");
        ds1.addEntry(icc1, "Event_Code_Detected");
        ds1.addEntry(icc1, "Transfer_Set_Time_Stamp");
        ds1.addEntry(vcc, "VCCDiscrete1");
        ds1.addEntry(icc1, "Discrete1");
        ds1.addEntry(icc1, "Discrete2");
        ds1.addEntry(icc1, "DiscreteQTimeTag4");

        icc1.addDSTransferSet("transfer_set1");
        icc1.addDSTransferSet("transfer_set2");

        BilateralTable blt1 = new BilateralTable("BLT_MZA_001_V1", icc1, "1.1.1.999", 12);

        /* add some access rules to the bilateral table */
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

        Server server = new Server(dataModel);

        server.addBilateralTable(blt1);

        Endpoint endpoint = server.getEndpoint();

        /* install handler to track connection events */
        endpoint.setConnectionHandler(new EndpointConnectionHandler() {

            @Override
            public void disconnected(Endpoint endpoint, Object parameter, EndpointConnection connection) {
                System.out.println(
                        "endpoint (IP " + connection.getPeerIpAddress() + " AP-title: " + connection.getPeerApTitle()
                                + " AE-qualifier: " + connection.getPeerAeQualifier() + ") disconnected");

                if (connection.getPSelector() != null)
                    System.out.println("  P-sel: " + byteArrayToHexString(connection.getPSelector()));

                if (connection.getSSelector() != null)
                    System.out.println("  S-sel: " + byteArrayToHexString(connection.getSSelector()));

                if (connection.getTSelector() != null)
                    System.out.println("  T-sel: " + byteArrayToHexString(connection.getTSelector()));
            }

            @Override
            public void connected(Endpoint endpoint, Object parameter, EndpointConnection connection) {
                System.out.println(
                        "endpoint (IP " + connection.getPeerIpAddress() + " AP-title: " + connection.getPeerApTitle()
                                + " AE-qualifier: " + connection.getPeerAeQualifier() + ") connected");

                if (connection.getPSelector() != null)
                    System.out.println("  P-sel: " + byteArrayToHexString(connection.getPSelector()));

                if (connection.getSSelector() != null)
                    System.out.println("  S-sel: " + byteArrayToHexString(connection.getSSelector()));

                if (connection.getTSelector() != null)
                    System.out.println("  T-sel: " + byteArrayToHexString(connection.getTSelector()));
            }
        }, null);

        endpoint.setConnectionAcceptHandler(new ConnectionAcceptHandler() {

            @Override
            public boolean connectionRequest(Endpoint endpoint, Object parameter, String peerAddress,
                    BilateralTable blt, AuthenticationMethod authMethod, byte[] authValue) {
                System.out.println("Connection accepted for peer " + peerAddress);
                if (blt != null) {
                    System.out.println("  with BLT: " + blt.getID());
                    System.out.println("  ap-title: " + blt.getApTitle());
                    System.out.println("  ae-qualifier: " + blt.getAeQualifier());
                }

                return true;
            }
        }, null);

        server.setDeviceOperationHandler(new DeviceOperationHandler() {

            @Override
            public HandlerResult setTag(Object parameter, ControlPoint controlPoint, Tag tagValue) {

                System.out.println("Set tag for " + controlPoint.getName());

                return HandlerResult.SUCCESS;
            }

            @Override
            public HandlerResult select(Object parameter, ControlPoint controlPoint) {
                System.out.println("Select");
                return HandlerResult.SUCCESS;
            }

            @Override
            public HandlerResult operate(Object parameter, ControlPoint controlPoint, OperateValue value) {

                if (controlPoint == null)
                    System.out.println("controlPoint is null!!!!");

                if (controlPoint.getType() == ControlPointType.SETPOINT_REAL) {
                    System.out.println("Operate for " + controlPoint.getName() + " : " + value.getRealSetPoint());
                } else {
                    System.out.println("Operate for " + controlPoint.getName() + " : " + value.getDiscreteSetPoint());
                }

                return HandlerResult.SUCCESS;
            }
        }, null);

        /* install handler to track DS transfer set state changes */
        server.setDSTransferSetUpdateHandler(new DSTransferSetUpdateHandler() {

            @Override
            public void transferSetUpdated(Object parameter, EndpointConnection peer, BilateralTable clientBlt,
                    DSTransferSet transferSet, boolean isEnabled) {
                Server serverInstance = (Server) parameter;

                System.out.println(
                        "DS transfer set : " + transferSet.getName() + " " + (isEnabled ? "enabled" : "disabled"));

            }
        }, server);

        /* install handler to track creation and deletion of dynamic data sets */
        server.setDataSetEventHandler(new DataSetEventHandler() {

            @Override
            public void dataSetEvent(Object parameter, boolean create, EndpointConnection peer,
                    BilateralTable clientBlt, Domain domain, String dataSetName, Collection<DataPoint> dataPoints) {
                if (create) {
                    System.out.println("Dataset " + domain.getName() + ":" + dataSetName + " created:");

                    for (DataPoint dp : dataPoints) {
                        System.out.println(" DP: " + dp.getDomain().getName() + ":" + dp.getName());
                    }
                } else {
                    System.out.println("Dataset " + domain.getName() + ":" + dataSetName + " deleted.");
                }
            }
        }, null);

        /* install handler to track sent transfer set reports */
        server.setDSTransferSetReportSentHandler(new DSTransferSetReportSentHandler() {

            @Override
            public void sentReport(Object parameter, EndpointConnection peer, BilateralTable clientBlt,
                    DSTransferSet ts, Collection<SentPointValue> sentData, ReportReason reason) {

                System.out.println(
                        "Sent report from DS transfer set : " + ts.getName() + " reason: " + reason.toString());

                for (SentPointValue sentDp : sentData) {
                    System.out.println("  DP:" + sentDp.getDataPoint().getDomain().getName() + ":"
                            + sentDp.getDataPoint().getName());
                }

            }
        }, null);

        server.setIMTransferSetEventHandler(new IMTransferSetEventHandler() {

            @Override
            public void event(Object parameter, EndpointConnection peer, BilateralTable clientBlt, boolean enabled) {
                System.out.println("peer: " + peer);

                if (enabled) {
                    System.out.println("Client with IP " + peer.getPeerIpAddress() + " enabled IM transfer set");
                } else {
                    System.out.println("IM disabled " + clientBlt);
                    // System.out.println("Client with IP " + peer.getPeerIpAddress() + " disabled
                    // IM transfer set");
                }
            }
        }, null);

        server.setLocalApTitle("1.1.1.998", 12);

        System.out.println("Starting server ...");

        server.start();

        if (server.isRunning()) {

            System.out.println("Server started ...");

            /* Create a thread to update process values */
            Thread updateThread = new Thread() {

                @Override
                public void run() {
                    int value = 0;

                    while (!Thread.currentThread().isInterrupted()) {

                        icc1Discrete1.setDiscrete(value++);
                        server.updateOnlineValue(icc1State1);

                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            break;
                        }
                    }

                    server.stop();
                }
            };

            updateThread.start();

            System.out.println("press any key to stop");

            try {
                System.in.read();

                updateThread.interrupt();
                updateThread.join();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        } else {
            System.out.println("Failed to start server!");
        }

        server.dispose();
    }

}
