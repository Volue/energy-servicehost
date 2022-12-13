using System;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using TASE2.Library.Client;
using TASE2.Library.Common;
using TASE2.Library.Server;

namespace tests
{
    [TestFixture()]
    public class ClientTests
    {
        public const int TCP_PORT = 12347;

        private Endpoint endpoint = null;
        private Server server = null;

        IndicationPoint icc1Discrete1;
        IndicationPoint icc1Discrete2;
        IndicationPoint icc1Discrete3;
        IndicationPoint icc1Discrete4;

        IndicationPoint icc1State1;
        IndicationPoint icc1State2;

        Domain icc1;
        Domain icc2;

        private DataModel createDataModel1()
        {
            IndicationPoint vccVCCDiscreteQTimeTag1;
            IndicationPoint vccVCCDiscreteExtended;


            ControlPoint vccCommand1;
            ControlPoint vccCommand2;

            ControlPoint icc1Command1;
            ControlPoint icc1Command2;

            DataModel dataModel = new DataModel();

            /* get VCC pseudo domain to populate the VCC scope data model part */
            Domain vcc = dataModel.VCC;

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
            icc1 = dataModel.AddDomain("icc1");

            /* Add some data points and devices to domain "icc1" */
            icc1Discrete1 = icc1.AddIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc1Discrete2 = icc1.AddIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc1Discrete3 = icc1.AddIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
            icc1Discrete4 = icc1.AddIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);

            icc1State1 = icc1.AddIndicationPoint("State1", IndicationPointType.STATE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc1State2 = icc1.AddIndicationPoint("State2", IndicationPointType.STATE, QualityClass.NO_QUALITY, TimeStampClass.TIMESTAMP_EXTENDED, false, true);

            /* add a data set transfer set to domain "icc1" */
            icc1.AddDSTransferSet("DSTrans1");

            /* add a new data set to domain "icc1" */
            DataSet ds1 = icc1.AddDataSet("ds1");

            ds1.AddEntry(icc1, "Transfer_Set_Name");
            ds1.AddEntry(icc1, "DSConditions_Detected");
            ds1.AddEntry(icc1, "Event_Code_Detected");
            ds1.AddEntry(icc1, "Transfer_Set_Time_Stamp");
            ds1.AddEntry(vcc, "VCCDiscrete1");
            ds1.AddEntry(icc1, "Discrete1");
            ds1.AddEntry(icc1, "Discrete2");
            ds1.AddEntry(icc1, "DiscreteQTimeTag4");
            ds1.AddEntry(icc1, "State1");
            ds1.AddEntry(icc1, "State2");

            /* Create a new domain "icc2" */
            icc2 = dataModel.AddDomain("icc2");

            /* Add some data points to domain "icc2" */
            icc2.AddIndicationPoint("Discrete1", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc2.AddIndicationPoint("Discrete2", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc2.AddIndicationPoint("Discrete3", IndicationPointType.DISCRETE, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, false);
            icc2.AddIndicationPoint("DiscreteQTimeTag4", IndicationPointType.DISCRETE, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, true, false);

            icc2.AddIndicationPoint("Real1", IndicationPointType.REAL, QualityClass.NO_QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc2.AddIndicationPoint("RealQ2", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.NO_TIMESTAMP, false, true);
            icc2.AddIndicationPoint("RealQTimeTag3", IndicationPointType.REAL, QualityClass.QUALITY, TimeStampClass.TIMESTAMP, false, true);

            return dataModel;
        }


        public void setupServer()
        {
            endpoint = new Endpoint(true);

            DataModel dataModel = createDataModel1();

            server = new Server(dataModel, endpoint);
        }

        public void startServer()
        {
            server.TcpPort = TCP_PORT;
            server.Start();
        }

        [Test()]
        public void CreateMultipleBLTs()
        {
            endpoint = new Endpoint(true);

            DataModel dataModel = createDataModel1();

            server = new Server(dataModel, endpoint);

            server.AddBilateralTable(new BilateralTable("BLT01", icc1, "1.1.999.1", 12));

            dataModel = null;

            server.Dispose();
            endpoint.Dispose();

            server = null;
            endpoint = null;

            GC.Collect();

            endpoint = new Endpoint(true);

            dataModel = createDataModel1();

            server = new Server(dataModel, endpoint);

            server.AddBilateralTable(new BilateralTable("BLT01", icc1, "1.1.999.1", 12));

            server.Dispose();
            endpoint.Dispose();
        }

        [Test()]
        public void TestReadMultipleDataPoints()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            List<string> pointNames = new List<string>();

            pointNames.Add("Discrete1");
            pointNames.Add("Discrete2");
            pointNames.Add("Discrete3");

            List<PointValue> pointValues = client.ReadMultiplePointValues("icc1", pointNames);

            Assert.AreEqual(3, pointValues.Count);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestReadMultipleDataPointsOneNotExisting()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            List<string> pointNames = new List<string>();

            pointNames.Add("Discrete1");
            pointNames.Add("Discrete2");
            pointNames.Add("Discrete3NOTEXISTS");

            List<PointValue> pointValues = client.ReadMultiplePointValues("icc1", pointNames);

            Assert.AreEqual(3, pointValues.Count);
           
            Assert.AreEqual(pointValues[0].Type, PointValueType.DISCRETE);
            Assert.AreEqual(pointValues[1].Type, PointValueType.DISCRETE);
            Assert.AreEqual(pointValues[2].Type, PointValueType.ERROR);

            Assert.AreEqual(pointValues[0].Error, ClientError.OK);
            Assert.AreEqual(pointValues[1].Error, ClientError.OK);
            Assert.AreEqual(pointValues[2].Error, ClientError.OBJECT_NOT_EXISTING);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestCreateVCCDataSet()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            List<string> dataSetEntries = new List<string>();

            dataSetEntries.Add("VCCDiscreteQTimeTag1");
            dataSetEntries.Add("icc1/Discrete1");
            dataSetEntries.Add("icc1/Discrete2");

            ClientException e = null;

            try
            {
                bool result = client.CreateDataSet(null, "TestDS1", dataSetEntries);
            }
            catch(ClientException ex)
            {
                e = ex;
            }


            Assert.NotNull(e);

            List<string> vccDataSets = client.GetVCCDataSets();

            Assert.NotNull(vccDataSets);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestCreateDomainDataSet()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            List<string> dataSetEntries = new List<string>();

            dataSetEntries.Add("icc1/Transfer_Set_Name"); ;
            dataSetEntries.Add("VCCDiscreteQTimeTag1");
            dataSetEntries.Add("icc1/Discrete1");
            dataSetEntries.Add("icc1/Discrete2");

            bool result = client.CreateDataSet("icc1", "TestDS1", dataSetEntries);

            Assert.AreEqual(true, result);

            List<string> domainDataSets = client.GetDomainDataSets("icc1");

            Assert.NotNull(domainDataSets);

            Assert.AreEqual(2, domainDataSets.Count);

            Assert.AreEqual("TestDS1", domainDataSets[1]);

            result = client.DeleteDataSet("icc1", "TestDS1");

            Assert.AreEqual(true, result);

            domainDataSets = client.GetDomainDataSets("icc1");

            Assert.NotNull(domainDataSets);

            Assert.AreEqual(1, domainDataSets.Count);

            client.Disconnect();

            server.Stop();
        }

        static int reportCount = 0;
        static ReportReason lastReportReason = 0;

        /* callback handler that is called twice for each received transfer set report */
        private static void dsTransferSetReportHandler(object parameter, bool finished, UInt32 seqNo, ClientDSTransferSet transferSet)
        {
            if (finished)
            {
                Console.WriteLine("--> ({0}) report processing finished", seqNo);
                reportCount++;

                if (transferSet != null)
                    lastReportReason = transferSet.GetDSConditionsDetected();
                else
                    lastReportReason = 0;
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

        [Test()]
        //[Ignore]
        public void TestEnableTransferSet()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            reportCount = 0;

            client.SetDSTransferSetValueHandler(dsTransferSetValueHandler, null);
            client.SetDSTransferSetReportHandler(dsTransferSetReportHandler, reportCount);

            ClientDataSet dataSet = client.GetDataSet("icc1", "ds1");

            ClientDSTransferSet transferSet = client.GetNextDSTransferSet("icc1");

            transferSet.WriteDataSetName("icc1", "ds1");

            transferSet.SetDataSet(dataSet);

            transferSet.ReadValues();

            Console.WriteLine(" data-set: {0}:{1}", transferSet.GetDataSetDomain(), transferSet.GetDataSetName());

            transferSet.WriteInterval(1);
            transferSet.WriteRBE(true);
            transferSet.WriteCritical(true);
            transferSet.WriteDSConditionsRequested(ReportReason.INTERVAL_TIMEOUT);
            transferSet.WriteStatus(true);

            GC.Collect();
            GC.WaitForPendingFinalizers();
            GC.Collect();

            Thread.Sleep(10000);

            Assert.IsTrue(reportCount > 0);
            Assert.AreEqual(ReportReason.INTERVAL_TIMEOUT, lastReportReason);

            client.Disconnect();

            server.Stop();
        }


        [Test()]
        public void TestGetNextTransferSetMultipleTimes()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            reportCount = 0;

            client.SetDSTransferSetValueHandler(dsTransferSetValueHandler, null);
            client.SetDSTransferSetReportHandler(dsTransferSetReportHandler, reportCount);

            ClientDSTransferSet transferSet = client.GetNextDSTransferSet("icc1");

            transferSet.Dispose();

            Assert.Throws<ClientException>(() => client.GetNextDSTransferSet("icc1"));

            client.Disconnect();

            server.Stop();
        }


        [Test()]
        public void TestEnableTransferSetMultipleTimes()
        {
            setupServer();
            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            reportCount = 0;

            client.SetDSTransferSetValueHandler(dsTransferSetValueHandler, null);
            client.SetDSTransferSetReportHandler(dsTransferSetReportHandler, reportCount);

            ClientDataSet dataSet = client.GetDataSet("icc1", "ds1");

            ClientDSTransferSet transferSet = client.GetNextDSTransferSet("icc1");

            transferSet.WriteDataSetName("icc1", "ds1");

            transferSet.SetDataSet(dataSet);

            transferSet.ReadValues();

            Console.WriteLine(" data-set: {0}:{1}", transferSet.GetDataSetDomain(), transferSet.GetDataSetName());

            transferSet.WriteInterval(1);
            transferSet.WriteRBE(true);
            transferSet.WriteCritical(true);
            transferSet.WriteDSConditionsRequested(ReportReason.OPERATOR_REQUESTED);

            transferSet.WriteStatus(true);

            Thread.Sleep(500);

            transferSet.WriteStatus(false);

            transferSet.ReadValues();

            Assert.AreEqual(false, transferSet.GetStatus());

            transferSet.Dispose();

            Thread.Sleep(500);

            transferSet = client.GetNextDSTransferSet("icc1");

            Assert.NotNull(transferSet);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestStateIndicationPoint()
        {
            setupServer();

            icc1State1.SetState(DataState.HI);
            icc1State1.SetQuality(DataFlags.VALIDITY_HI);
            server.UpdateOnlineValue(icc1State1);

            icc1State2.SetState(DataState.HI);
            icc1State2.SetQuality(DataFlags.VALIDITY_HI);
            server.UpdateOnlineValue(icc1State2);

            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            PointValue value = client.ReadPointValue("icc1", "State1");

            Assert.AreEqual(DataState.HI | DataState.VALIDITY_HI, value.ValueState);

            value = client.ReadPointValue("icc1", "State2");

            Assert.AreEqual(DataState.HI | DataState.VALIDITY_HI, value.ValueState);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestClientDataSet()
        {
            setupServer();

            icc1State1.SetState(DataState.HI);
            icc1State1.SetQuality(DataFlags.VALIDITY_HI);
            server.UpdateOnlineValue(icc1State1);

            icc1State2.SetState(DataState.HI | DataState.VALIDITY_HI);
            //icc1State2.SetQuality(DataFlags.VALIDITY_HI);
            server.UpdateOnlineValue(icc1State2);

            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999.1";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetRemoteApTitle("1.1.1.999", 12);

            client.SetTcpPort(TCP_PORT);

            client.Connect(hostname, apTitle, aeQualifier);

            ClientDataSet dataSet = client.GetDataSet("icc1", "ds1");
            dataSet.Read();

            Assert.AreEqual(10, dataSet.GetSize());

            for (int i = 0; i < dataSet.GetSize(); i++)
            {
                PointValue pval = dataSet.GetPointValue(i);
                Assert.NotNull(pval);
            }

            PointValue value = dataSet.GetPointValue(8);

            Assert.AreEqual(DataState.HI | DataState.VALIDITY_HI, value.ValueState);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestSetLocalIpAddress()
        {
            setupServer();

            endpoint.SetLocalIpAddress("0.0.0.0");

            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            client.Disconnect();

            server.Stop();
        }

        [Test()]
        public void TestRemoveAddServer()
        {
            setupServer();

            endpoint.SetLocalIpAddress("0.0.0.0");

            startServer();

            string hostname = "localhost";
            string apTitle = "1.1.1.999";
            int aeQualifier = 12;

            Client client = new Client();

            client.SetTcpPort(TCP_PORT);
            client.SetRemoteApTitle("1.1.1.999", 12);

            client.Connect(hostname, apTitle, aeQualifier);

            client.Disconnect();

            Assert.NotNull(endpoint.RemoveServer(server));

            Exception ex = null;

            try
            {
                client = new Client();

                client.SetTcpPort(TCP_PORT);
                client.SetRemoteApTitle("1.1.1.999", 12);

                client.Connect(hostname, apTitle, aeQualifier);
            }
            catch (ClientException cex)
            {
                ex = cex;
            }

            Assert.NotNull(ex);

            endpoint.Disconnect();
        }

        [Test()]
        public void TestEndpointSetAddresses()
        {
            setupServer();

            endpoint.SetLocalIpAddress("0.0.0.0");
            endpoint.SetLocalApTitle("1.1.1.998");
            endpoint.SetLocalAddresses("00 00 00 02", "00 00 00 02", "00 01");
            endpoint.SetRemoteApTitle("1.1.1.999");
            endpoint.SetRemoteAddresses("00 00 00 03", "00 00 00 03", "00 00 00 03");

            try
            {
                endpoint.SetRemoteAddresses("00 00 00 03", "00 00 00 03", "00 00 00 03 04 01 02 03 04 05 06 07 08 09 0a 0b 0c");
                Assert.IsFalse(true);
            }
            catch (Exception ex) {
                Assert.IsTrue(ex is ArgumentOutOfRangeException);
            }

            server.Stop();
        }
    }
}

