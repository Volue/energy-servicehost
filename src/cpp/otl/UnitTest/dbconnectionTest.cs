using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CsTestOtlNet.Properties;
using OTLNET;
using Powel.Time;


namespace CsTestOtlNet
{
    /// <summary>
    /// Summary description for dbconnectionTest
    /// </summary>
    [TestClass]
    public class dbconnectionTest
    {
        private RegionalCalendar Zdb;
        public dbconnectionTest()
        {
            //
            // TODO: Add constructor logic here
            //
            Zdb = new RegionalCalendar();
            Zdb.Name = "Zdb"; Zdb.UtcOffset = new TimeSpan(1, 0, 0);
        }

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
        private String dbServer
        {
            get
            {
                return Settings.Default.dbserver;
            }
        }
        private String dbUser
        {
            get { return Settings.Default.dbuser; }
        }
        private String dbPasswd
        {
            get { return Settings.Default.dbpasswd; }
        }

        #region Additional test attributes
        //
        // You can use the following additional attributes as you write your tests:
        //
        // Use ClassInitialize to run code before running the first test in the class
        // [ClassInitialize()]
        // public static void MyClassInitialize(TestContext testContext) { }
        //
        // Use ClassCleanup to run code after all tests in a class have run
        // [ClassCleanup()]
        // public static void MyClassCleanup() { }
        //
        // Use TestInitialize to run code before running each test 
        // [TestInitialize()]
        // public void MyTestInitialize() { }
        //
        // Use TestCleanup to run code after each test has run
        // [TestCleanup()]
        // public void MyTestCleanup() { }
        //
        #endregion

        [TestMethod]
        public void TestMultipleConnections()
        {
            dbconnect db = new dbconnect();
            try
            {
                //-- need this first, due to assembly load time etc..
                Assert.IsTrue(db.server_attach(dbServer));
                db.session_begin(dbUser, dbPasswd, false);
                db.direct_exec(
                    "CREATE OR REPLACE PACKAGE pkg_test2 IS\n" +
                    "  PROCEDURE PackageCallCount( currentCallCount out number);\n" +
                    "END; "
                );
                db.direct_exec(
                    "CREATE OR REPLACE PACKAGE BODY pkg_test2 IS \n" +
                    " callCount number;\n" +
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
                Assert.IsTrue(db.session_end());
                Assert.IsTrue(db.server_detach());
                DateTime T1 = DateTime.UtcNow;
                Assert.IsTrue(db.server_attach(Settings.Default.dbserver));
                DateTime T2 = DateTime.UtcNow;
                for (int i = 0; i < 100; i++)
                {
                    db.session_begin(dbUser, dbPasswd, false);
                    using (dbstream sql = new dbstream(1,
                    "begin " +
                    "  pkg_test2.PackageCallCount(:t1<double,out>); " +
                   "end;"
                    , db))
                    { // If we got a package from local sql pool, 
                        // the package state, callCount could be >1.
                        // so we do a quick test here:
                        try
                        {
                            double callCount;
                            sql.get(out callCount);
                            int countX = (int)callCount;
                            Assert.AreEqual(countX, 1);
                        }
                        catch (dbexception e)
                        {
                            Assert.Fail("Test connections stateless failed." + e.ToString());
                        }
                    }
                    db.session_end();
                }
                DateTime T3 = DateTime.UtcNow;
                double attachTime = (T2 - T1).TotalMilliseconds;
                double sessionTime = (T3 - T2).TotalMilliseconds / 100.0;
                Assert.IsTrue(attachTime < 1000.0);
                Assert.IsTrue(sessionTime < 190.0);
            }
            catch (SystemException e)
            {
                Assert.Fail("System Exception"+ e.ToString());
            }
            finally
            {
                db.server_detach();
            }
        }
        [TestMethod]
        public void TestTransactionIsolation() {
            //-- create/replace a test table
            //-- create two db.connections
            //-- insert into table from the one
            //-- verify zero on the second
            //-- commit on the first
            //-- verify nonzero on the second.
            //
            dbconnect db = new dbconnect();
            dbconnect db2 = new dbconnect();
            try {
                //-- need this first, due to assembly load time etc..
                Assert.IsTrue(db.server_attach(dbServer));
                Assert.IsTrue(db2.server_attach(dbServer));
                db.session_begin(dbUser, dbPasswd, false);
                db2.session_begin(dbUser, dbPasswd, false);
                db.direct_exec("drop table OTL_TEST_TX_ISOLATION", 0);
                db.direct_exec("create table OTL_TEST_TX_ISOLATION ( f1 number, f2 varchar2(30) ) ");
                using (dbstream sql = new dbstream(1, "insert into OTL_TEST_TX_ISOLATION values(:f1<double>,:f2<char[31]>)", db)) {
                    try {
                        sql.put(1.23).put("JattaJatta");
                    } catch (dbexception e) {
                        Assert.Fail("Test connections stateless failed." + e.ToString());
                    }
                }
                using (dbstream sql = new dbstream(1, "select * from OTL_TEST_TX_ISOLATION", db2)) {
                    try {
                        if (!sql.eof()) {
                            double f1;
                            string f2;
                            sql.get(out f1).get(out f2);
                            Assert.Fail("Sorry, first statement got autocommit");
                        }
                    } catch (dbexception e) {
                        Assert.Fail("Test transaction isolation." + e.ToString());
                    }
                }
                // select from same session should work ok:
                using (dbstream sql = new dbstream(1, "select * from OTL_TEST_TX_ISOLATION", db)) {
                    try {
                        if (!sql.eof()) {
                            double f1;
                            string f2;
                            sql.get(out f1).get(out f2);

                        } else
                            Assert.Fail("Something wrong with the test");
                    } catch (dbexception e) {
                        Assert.Fail("Test transaction isolation." + e.ToString());
                    }
                }
                db.commit();// now commit changes
                using (dbstream sql = new dbstream(1, "select * from OTL_TEST_TX_ISOLATION", db2)) {
                    try {
                        if (!sql.eof()) {
                            double f1;
                            string f2;
                            sql.get(out f1).get(out f2);

                        } else
                            Assert.Fail("Sorry, commit did not work");
                    } catch (dbexception e) {
                        Assert.Fail("Test transaction isolation." + e.ToString());
                    }
                }
                // insert- rollback-verify.
                db.direct_exec("truncate table OTL_TEST_TX_ISOLATION", 0);
                using (dbstream sql = new dbstream(1, "insert into OTL_TEST_TX_ISOLATION values(:f1<double>,:f2<char[31]>)", db)) {
                    try {
                        sql.put(1.23).put("JattaJatta");
                    } catch (dbexception e) {
                        Assert.Fail("Test connections stateless failed." + e.ToString());
                    }
                }
                db.rollback();
                using (dbstream sql = new dbstream(1, "select * from OTL_TEST_TX_ISOLATION", db)) {
                    try {
                        if (!sql.eof()) {
                            double f1;
                            string f2;
                            sql.get(out f1).get(out f2);
                            Assert.Fail("Rollback did not work");
                        }
                    } catch (dbexception e) {
                        Assert.Fail("Test transaction isolation." + e.ToString());
                    }
                }

                db.session_end();
                db2.session_end();
            } catch (dbexception e) {
                Assert.Fail("Test transaction isolation." + e.ToString());
            } catch (SystemException e) {
                Assert.Fail("System Exception" + e.ToString());
            } finally {
                db.server_detach();
                db2.server_detach();
            }
        }
         [TestMethod]
        public void TestMassiveAttach() {
            try {
                //int n = 10; //If this parameter is set to 100, the tests starts failing.
                            // Sigbjørn claims that this happends due to a bug in Oracle.
                int n = 100; // When Oracle parameter was increased from 150 to 600, the tests starts to work even with n = 100.
                System.Collections.Generic.List<dbconnect> dbPool = new System.Collections.Generic.List<dbconnect>(n);
                for(int i=0;i<n;i++) {
                    dbconnect db = new dbconnect();
                    try {
                        db.server_attach(dbServer);
                        dbPool.Add(db);
                    } catch (dbexception ) {
                        db.Dispose();
                    }
                    //dbPool[i] = db;
                }

                foreach (dbconnect dbc in dbPool) {
                    dbc.session_begin(dbUser, dbPasswd);
                }
                foreach (dbconnect dbc in dbPool) {
                    dbc.session_end();
                }

                foreach (dbconnect dbc in dbPool) {
                    dbc.server_detach();
                    dbc.Dispose();
                }
                if (dbPool.Count < 1 + n / 2)
                       Assert.Fail("To few connections {1} available for test n={2}", dbPool.Count, n);
            } catch (dbexception e) {
                Assert.Fail("TestMassiveAttach." + e.ToString());
            }
         }
         [TestMethod]
         public void TestDbPoolBasic()  {
             dbpool pool = new dbpool(1, 1, dbServer);
             try {
                 for (int i = 0; i < 10; i++) {
                     using (dbconnect dbx = new dbconnect(pool, 100)) {
                         try {
                             dbx.session_begin(dbUser, dbPasswd);
                             // do some db.access. here
                             UtcTime dbTimeNow;
                             using (dbstream sql = new dbstream(1, "select sysdate from dual", dbx, Zdb)) {
                                 sql.get(out dbTimeNow);
                             }
                             dbx.session_end();
                             // while we still have a connection, require one more, expect exception.
                             bool gotException = false;
                             try {
                                 using (dbconnect dbz = new dbconnect(pool, 10)) {
                                     dbz.session_begin(dbUser, dbPasswd);
                                     // do some db.access. here, so that it does not get optimized away.
                                     UtcTime dbTimeNowX;
                                     using (dbstream sql = new dbstream(1, "select sysdate from dual", dbz, Zdb)) {
                                         sql.get(out dbTimeNowX);
                                     }
                                     dbz.session_end();
                                 }

                             } catch (dbexception e) {
                                 if(e.category== ExceptionCategory.CONNECTFAIL_EXCEPTION)
                                    gotException = true;
                             }
                             Assert.AreEqual(true, gotException);
                         } catch (dbexception e) {
                             Assert.Fail("TestDbPool." + e.ToString());
                         };
                     }
                 }
             } catch (dbexception e) {
                 Assert.Fail("TestDbPoolBasic." + e.ToString());
             } finally {
                 pool.Dispose();
             }
         }
 [TestMethod]
 public void TestDbPoolVolumeRequest() {
     dbpool pool = new dbpool(10, 20, dbServer);
     try {
         List<dbconnect> dbxv = new List<dbconnect>(20);
         for (int testN = 0; testN < 50; testN++) {
             int countExcessiveRequest = 0;
             for (int i = 0; i < 22; i++) {
                 dbconnect dbx;
                 if (pool.get(out dbx, 1)) {
                     try {
                         dbx.session_begin(dbUser, dbPasswd);
                         // do some db.access. here
                         UtcTime dbTimeNow;
                         using (dbstream sql = new dbstream(1, "select sysdate from dual", dbx, Zdb)) {
                             sql.get(out dbTimeNow);
                             dbxv.Add(dbx);
                         }
                         dbx.session_end();
                     } catch (dbexception e) {
                         Assert.Fail("TestDbPool." + e.ToString());
                     } finally {
                         
                         //pool.put(dbx);
                     }
                 } else {
                     countExcessiveRequest++;
                 }
             }
             foreach (dbconnect dbz in dbxv) {
                 dbz.Dispose();
             }
             Assert.AreEqual(countExcessiveRequest, 2);
         }
     } catch (dbexception e) {
         Assert.Fail("TestDbPool." + e.ToString());
     } finally {
         pool.Dispose();
     }
 }
        [TestMethod]
        public void TestMixNetNativeDbAccess() {
            dbconnect db = new dbconnect();
            try {
                //-- need this first, due to assembly load time etc..
                Assert.IsTrue(db.server_attach(dbServer));
                db.session_begin(dbUser, dbPasswd, false);
                db.direct_exec("drop table TEST_NET_MIX_NATIVE",0 );
                db.direct_exec("create table TEST_NET_MIX_NATIVE ( customername varchar2(300) ) ");

                for(int i=0;i<100;i++) {
                    NetNativeOtlDbAcess.Wrapper dbNative = new NetNativeOtlDbAcess.Wrapper();
                    String customerName= "Customer-";
                    customerName += i.ToString();
                    Assert.IsTrue(dbNative.StoreCustomer(db,customerName));

                    using (dbstream sql = new dbstream(1, "select customerName from TEST_NET_MIX_NATIVE where customerName= :cName<char[300]>", db)) {
                        try {
                            sql.put(customerName);
                            if (!sql.eof()) {
                                string cnameRead;
                                sql.get(out cnameRead);
                                Assert.AreEqual(cnameRead, customerName);
                            } else 
                                Assert.Fail("Sorry, Mix .NET native did not work");
                        } catch (dbexception e) {
                            Assert.Fail("TestMixNetNativeDbAccess." + e.ToString());
                        }
                    }
                }
                // select from same session should work ok:
                db.rollback();
                db.direct_exec("drop table TEST_NET_MIX_NATIVE",0 );                
                db.session_end();
            } catch (dbexception e) {
                Assert.Fail("TestMixNetNativeDbAccess." + e.ToString());
            } catch (SystemException e) {
                Assert.Fail("TestMixNetNativeDbAccess System Exception"+ e.ToString());
            } finally {
                db.server_detach();
            }
        }
    }
}
