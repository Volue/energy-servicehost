using NUnit.Framework;

//If no tests, the class generates "Inconclusive" result


//namespace Powel.Icc.Interop.UnitTests
//{
//    [TestFixture]
//    public class TestMonitoring
//    {

//		[OneTimeSetUp]
//		public void Setup()
//		{
//		}

//		[OneTimeTearDown]
//		public void TearDown()
//		{
//		}
/*
        [Test]
        public void TestMonitorItem()
        {
            RegionalCalendar utc = RegionalCalendar.UtcCalendar;
            MonitorItem item = new MonitorItem(1, 2, ObjectTypes.Test, 
                new TimePeriod(utc.ToUtcTime(2000, 1, 1), utc.ToUtcTime(2000, 2, 1)));
            Assert.AreEqual(1, item.MonitorKey1);
            Assert.AreEqual(2, item.MonitorKey2);
            Assert.AreEqual(ObjectTypes.Test, (ObjectTypes) item.ObjectType);
            Assert.AreEqual(utc.ToUtcTime(2000, 1, 1), item.MonitorPeriod.From);
            Assert.AreEqual(utc.ToUtcTime(2000, 2, 1), item.MonitorPeriod.Until);
        }

		[Test]
		[Category("LongRunning")]
		public void TestTimeSeriesMonitoring()
        {
			TS.TimeSeriesMonitor m = new TS.TimeSeriesMonitor();

			TS.OracleTimeSeriesAdapter tsa = new Powel.Icc.TimeSeries.OracleTimeSeriesAdapter(IccProgram.P_IMPCOMMON);
            TS.TimeSeries ts1 = new TS.TimeSeries("TestTimeSeriesMonitoring1", TimeResolution.Hour);
			ts1.SetValue(UtcTime.Now, 5);
			tsa.Create(ts1);
			tsa.Fill(ts1, "TestTimeSeriesMonitoring1");
            TS.TimeSeries ts2 = new TS.TimeSeries("TestTimeSeriesMonitoring2", TimeResolution.Unconstrained);
			ts2.SetValue(UtcTime.Now, 6);
			tsa.Create(ts2);
			tsa.Fill(ts2, "TestTimeSeriesMonitoring2");
			TS.TimeSeries ts3 = new TS.TimeSeries("TestTimeSeriesMonitoring3", TimeResolution.Unconstrained);
			ts3.SetValue(UtcTime.Now, 7);
			tsa.Create(ts3);
			tsa.Fill(ts3, "TestTimeSeriesMonitoring3");
            
            m.Add(ts1);
			m.Add(new TS.TimeSeries[] { ts2, ts3 });
			m.Start();	
			
			tsa.Update(ts1);			
			
			TS.TimeSeries[] changedTimeSeries = m.GetChanges(); //monitor er av typen TimeSeriesMonitor slik at TimeSeries kan returneres?
			Assert.AreEqual(ts1.Name, changedTimeSeries[0].Name);

			m.End();
        }

		[Test]
		//[ExpectedException(typeof(ApplicationException))]
		public void TestTypeBasedMonitoring()
		{
			Monitor<ImportDefinition> m = new Monitor<ImportDefinition>(MonitoringType.TypeBased);
			bool exceptionCaught;
			m.Start();
			try
			{
				exceptionCaught = false;
				m.Add(new ImportDefinition()); // not allowed on typebased monitor
			}
			catch (ApplicationException)
			{
				// Console.WriteLine(ex.Message);
				exceptionCaught = true;
			}
			Assert.IsTrue(exceptionCaught);

			//TODO should check changes for type-based

			//ImportDefinitons[] changedImportDefinitions = mAll.getChanges();

			m.End();
		}
*/
//    }
//}
