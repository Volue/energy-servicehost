using System;
using NUnit.Framework;
using Powel.Icc.Interop;

namespace Powel.Icc.Interop.UnitTests
{
	/// <summary>
	/// Summary description for TestLogon.
	/// </summary>
	[TestFixture]
	public class TestConfiguration
	{
		[Test]
		public void Test()
		{
			IccConfigurationReader configuration = new IccConfigurationReader();
			Assert.IsTrue(configuration.Keys.Length > 0);
			Assert.IsTrue(configuration.Keys[0].Length > 0);
			Assert.IsTrue(configuration.ReadString(configuration.Keys[0]).Length > 0);
			Assert.IsTrue(configuration.ReadString("ICC_DBUSER").Length > 0);

			// Test again to see whether things disappear on cleanup
			configuration = new IccConfigurationReader();
			Assert.IsTrue(configuration.Keys.Length > 0);
		}

		[Test]
		public void TestUnknownParameter()
		{
			IccConfigurationReader configuration = new IccConfigurationReader();
			Assert.Throws<ConfigurationException>(() => configuration.ReadString("this parameter does not exist"));
		}

		[Test]
		[Category("LongRunning")]
		public void TestStability()
		{
			for (int i=0; i<10000; i++)
			{
				IccConfigurationReader configuration = new IccConfigurationReader();
				string s = configuration.ReadString("ICC_DBUSER");
			}

			Assert.IsTrue(true);
		}
	}
}
