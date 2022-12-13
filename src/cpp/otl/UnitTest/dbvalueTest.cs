using OTLNET;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Powel.Time;
namespace CsTestOtlNet
{
    
    
    /// <summary>
    ///This is a test class for dbvalueTest and is intended
    ///to contain all dbvalueTest Unit Tests
    ///</summary>
    [TestClass()]
    public class dbvalueTest
    {


        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        // 
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion


        /// <summary>
        ///A test for set_null
        ///</summary>
        public void set_nullTest1Helper<T>(T initV)
        {
            dbvalue<T> target = new dbvalue<T>(initV); // TODO: Initialize to an appropriate value
            Assert.AreEqual(target.is_null(), false);
            Assert.AreEqual(target.v, initV);
            bool x = false; 
            target.set_null(x);
            Assert.AreEqual(target.is_null(), false);
            target.set_null(true);
            Assert.AreEqual(target.is_null(), true);
        }

        [TestMethod()]
        public void set_nullTest1()
        {
            set_nullTest1Helper<double>(3.4);
            set_nullTest1Helper<int>(2);
            set_nullTest1Helper<long>(2);
            set_nullTest1Helper<string>("Hello");
            set_nullTest1Helper<UtcTime>(UtcTime.Now);

        }


        /// <summary>
        ///A test for dbvalue`1 Constructor
        ///</summary>
        public void dbvalueConstructorTestHelper<T>( T x)
        {
            dbvalue<T> target = new dbvalue<T>();
            Assert.AreEqual(target.is_null(), true);
            target = new dbvalue<T>(x);
            Assert.AreEqual(target.v, x);
            Assert.AreEqual(target.is_null(), false);
        }

        [TestMethod()]
        public void DbValueConstructorTest()
        {
            dbvalueConstructorTestHelper<double>(4.7);
            dbvalueConstructorTestHelper<int>(2);
            dbvalueConstructorTestHelper<long>(2);
            dbvalueConstructorTestHelper<string>("Hello");
            dbvalueConstructorTestHelper<UtcTime>(UtcTime.Now);

        }
    }
}
