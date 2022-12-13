using OTLNET;
using Powel.Time;
using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CsTestOtlNet.Properties;
namespace CsTestOtlNet
{
    
    
    /// <summary>
    ///This is a test class for dbstreamTest and is intended
    ///to contain all dbstreamTest Unit Tests
    ///</summary>
    [TestClass()]
    public class dbstreamTest
    {


        private TestContext testContextInstance;
        private String dbServer {
            get {
                    return Settings.Default.dbserver;
            }
        }
        private String dbUser {
            get { return Settings.Default.dbuser; }
        }
        private String dbPasswd {
            get { return Settings.Default.dbpasswd; }
        }
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
        
        public void testSimpleVectorDouble(dbconnect db, int n) {
         using (   dbstream sql = new dbstream(1,
                    "begin " +
                    "  pkg_test.prc_test(:t1<double,inout[10]>); " +
                   "end;"
                    , db)  ){ // TODO: Initialize to an appropriate value

                try {
                    dbvector<double> dbv = new dbvector<double>(n, false); // TODO: Initialize to an appropriate value
                    int  x = 0;
                    for (int i = 0; i < dbv.len(); i++) {
                        dbv[i] = (double)x;
                        x++;
                    }
                    sql.put(dbv);
                    dbvector<double> dbr;
                    sql.get(out dbr);
                    x = 0;
                    for (int i = 0; i < dbv.len(); i++) {
                        Assert.AreEqual(x + 100, dbr[i]);
                        x++;
                    }
                } catch( dbexception e) {
                    Assert.Fail("testSimpleVector failed." + e.ToString());
                } 
            }
        }
        public void testSimpleVectorInt(dbconnect db, int n) {
            using (dbstream sql = new dbstream(1,
                    "begin " +
                    "  pkg_test.prc_test(:t1<int,inout[10]>); " +
                   "end;"
                    , db)) // TODO: Initialize to an appropriate value
                    {
                try {
                    dbvector<int> dbv = new dbvector<int>(n, false); // TODO: Initialize to an appropriate value
                    int x = 0;
                    for (int i = 0; i < dbv.len(); i++)
                    {
                        dbv[i] = (int)x;
                        x++;
                    }
                    sql.put(dbv);
                    dbvector<int> dbr;
                    sql.get(out dbr);
                    x = 0;
                    for (int i = 0; i < dbv.len(); i++)
                    {
                        Assert.AreEqual(x + 100, dbr[i]);
                        x++;
                    }
                }
                catch (dbexception e)
                {
                    Assert.Fail("testSimpleVector failed." + e.ToString());
                }
            }

        }
        public void testSimpleVectorString(dbconnect db, int n) {
            using( dbstream sql = new dbstream(1,
                    "begin " +
                    "  pkg_test.String_test(:t1<char[31],inout[10]>); " +
                   "end;"
                    , db)) { // TODO: Initialize to an appropriate value
                try
                {
                    dbvector<string> dbv = new dbvector<string>(n, false); // TODO: Initialize to an appropriate value
                    int x = 0;
                    for (int i = 0; i < dbv.len(); i++)
                    {
                        dbv[i] = "The number is"+ i.ToString();
                        x++;
                    }
                    sql.put(dbv);
                    dbvector<string> dbr;
                    sql.get(out dbr);
                    x = 0;
                    for (int i = 0; i < dbv.len(); i++)
                    {
                        Assert.AreEqual(dbv[i]+".Ok", dbr[i]);
                        x++;
                    }
                }
                catch (dbexception e)
                {
                    Assert.Fail("testSimpleVector failed." + e.ToString());
                }
            }

        }

        public void testSimpleVectorTimeStamp(dbconnect db, int n) {
            RegionalCalendar Zdb = new RegionalCalendar();
            Zdb.UtcOffset = new TimeSpan(1, 0, 0);
            Zdb.Name = "Zdb";
            using (dbstream sqlX = new dbstream(1,
                    "begin " +
                    "  pkg_test.Timestamp_test(:t1<timestamp,inout["+ n.ToString()+"]>); " +
                   "end;"
                    , db,Zdb))
            {

                try
                {
                    dbvector<UtcTime> dbv = new dbvector<UtcTime>(n, false); // TODO: Initialize to an appropriate value
                    int x = 0;
                    for (int i = 0; i < dbv.len(); i++)
                    {
                        dbv[i] =Zdb.trim(UtcTime.Now, new TimeSpan(0, 0, 1)) + new TimeSpan(1, 2, i);
                        x++;
                    }
                    sqlX.put(dbv);
                    dbvector<UtcTime> dbr;
                    sqlX.get(out dbr,Zdb);
                    x = 0;
                    for (int i = 0; i < dbv.len(); i++)
                    {
                        Assert.AreEqual(dbv[i] + new TimeSpan(1, 0, 0, 0, 0), dbr[i]);
                        x++;
                    }
                }
                catch (dbexception e)
                {
                    Assert.Fail("testSimpleVector failed." + e.ToString());
                }
                finally {
                    sqlX.Dispose();
                }
            }

        }
        [TestMethod()]
        public void TestCalendarException() {
            using (dbconnect db = new dbconnect()) {
                try {

                    db.server_attach(dbServer);
                    db.session_begin(dbUser, dbPasswd, false);
                    UtcTime dbTimeNow;
                    using (dbstream sql = new dbstream(1, "select sysdate from dual", db)) {
                        bool gotCalendarException = false;
                        try {
                            sql.get(out dbTimeNow);
                        } catch( dbexception e) {
                            gotCalendarException = e.category == ExceptionCategory.CALENDARMISSING_EXCEPTION ? true : false;
                        }
                        Assert.IsTrue(gotCalendarException, "Failed to throw (correct type of) exception");
                        sql.get(out dbTimeNow,RegionalCalendar.UtcCalendar);
                    }
                    db.session_end();
                    db.server_detach();
                } catch (dbexception e) {
                    Assert.Fail("TestCalendarException failed." + e.ToString());
                }
            }
        }
        [TestMethod()]
        public void TestBadSqlException() {
            using (dbconnect db = new dbconnect()) {
                try {

                    db.server_attach(dbServer);
                    db.session_begin(dbUser, dbPasswd, false);
                    UtcTime dbTimeNow;
                    bool gotOtlException = false;
                    try {                    
                        using (dbstream sql = new dbstream(1, "selct gurk sysdate from dual", db)) {
                            sql.get(out dbTimeNow);
                        };
                    } catch (dbexception e) {
                        gotOtlException = e.category == ExceptionCategory.OTL_EXCEPTION ? true : false;
                    }
                    Assert.IsTrue(gotOtlException, "Failed to throw (correct type of) exception");
                    db.session_end();
                    db.server_detach();
                } catch (dbexception e) {
                    Assert.Fail("TestBadSqlException failed." + e.ToString());
                }
            }
        }
        /// <summary>
        ///A test for put/get
        ///</summary>
        [TestMethod()]
        public void PutGetTest() {
            using (dbconnect db = new dbconnect()) {
                try {
                    
                    db.server_attach(dbServer);
                    db.session_begin(dbUser, dbPasswd, false);
                    db.direct_exec(
                        "CREATE OR REPLACE PACKAGE pkg_test IS\n" +
                        " TYPE my_numeric_table IS TABLE OF NUMBER INDEX BY BINARY_INTEGER;\n" +
                        " TYPE my_varchar_table IS TABLE OF VARCHAR2(31) INDEX BY BINARY_INTEGER;\n" +
                        " TYPE my_timestamp_table IS TABLE OF DATE INDEX BY BINARY_INTEGER;\n" +
                        "  PROCEDURE prc_test(v1 IN OUT my_numeric_table); \n" +
                        "  PROCEDURE String_test(v1 IN OUT my_varchar_table);\n" +
                        "  PROCEDURE Timestamp_test(v1 IN OUT my_timestamp_table);\n" +
                        "  PROCEDURE PackageCallCount( currentCallCount out number);\n" +
                        "END; "
                    );
                    db.direct_exec(
                        "CREATE OR REPLACE PACKAGE BODY pkg_test IS \n" +
                        " callCount number;\n" +
                        "  PROCEDURE prc_test(v1 IN OUT my_numeric_table) \n" +
                        "  IS \n" +
                        "  BEGIN \n" +
                        "    callCount := callCount +1;\n" +
                        "    FOR i IN 1..v1.last LOOP \n" +
                        "      v1(i) := v1(i)+100; \n" +
                        "    END LOOP; \n" +
                        "  END; \n" +
                        "   \n" +
                        " \n" +
                        "  PROCEDURE String_test(v1 IN OUT my_varchar_table) \n" +
                        "  IS \n" +
                        "  BEGIN \n" +
                        "    callCount := callCount +1;\n" +
                        "    FOR i IN 1..v1.last LOOP \n" +
                        "      v1(i) := v1(i) || '.Ok'; \n" +
                        "    END LOOP; \n" +
                        "  END; \n" +
                        "   \n" +
                        "  PROCEDURE Timestamp_test(v1 IN OUT my_timestamp_table) \n" +
                        "  IS \n" +
                        "  BEGIN \n" +
                        "    callCount := callCount +1;\n" +
                        "    FOR i IN 1..v1.last LOOP \n" +
                        "      v1(i) := v1(i) +1.0;\n" +
                        "    END LOOP; \n" +
                        "  END; \n" +
                        "  PROCEDURE PackageCallCount( currentCallCount out number) \n" +
                        "  IS \n" +
                        "  BEGIN \n" +
                        "    callCount := callCount +1;\n" +
                        "    currentCallCount := callCount;\n" +
                        "  END; \n" +

                        "BEGIN \n" +
                        " callCount := 0;\n" +
                        "END; \n"
                    );


                    try
                    {
                        testSimpleVectorInt(db, 10);
                        testSimpleVectorTimeStamp(db, 10);
                        testSimpleVectorDouble(db, 10);
                        testSimpleVectorString(db, 10);
                        testSimpleVectorTimeStamp(db, 10);
                    }
                    catch (dbexception x)
                    {
                        Assert.Fail("dbvector<int> exception:" + x.ToString());
                    }
                    finally
                    {
                        db.rollback();
                    }

                }
                finally
                {
                    db.session_end();
                    db.server_detach();
                }
            }
        }
    }
}

