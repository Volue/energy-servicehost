#include <log4cxx/logger.h> // Include early to avoid C4251.
#include "PDLog4Cxx.h"
#ifndef EXCLUDE_ActivityLog
#include "ActivityLog.h"
#endif
#include "otl.h"
#include <stdio.h>

static log4cxx::LoggerPtr _l4n(log4cxx::Logger::getLogger("Powel.Icc.LibDbUtil.OtlConnection"));

using namespace std;

//ofstream OtlTrace;
unsigned int OtlTraceLevel=0;
/*
0x1 |  1st level of tracing
0x2 |  2nd level of tracing
0x4 |  3rd level of tracing
0x8;   4th level of tracing
0x10;  5th level of tracing. Variables.
 each level of tracing is represented by its own bit, 
 so levels of tracing can be combined in an arbitrary order.
*/
void SetOtlTraceLevel(unsigned int level) {
	OtlTraceLevel=level;
}
unsigned int GetOtlTraceLevel() {
	return OtlTraceLevel;
}
void BeginOtlTrace() {
	//if(OtlTraceLevel) {
	//	char *traceFileName=_tempnam("c:\\tmp","OTLTrace");
	//	if(traceFileName) {
	//		OtlTrace.open(traceFileName);
	//		free(traceFileName);
	//	}
	//}
}
void EndOtlTrace() {
	//if(OtlTrace.is_open())
	//	OtlTrace.close();
}
long otlInitCount=0; // refCounting common library
void OtlModuleInit() {
	otl_connect::otl_initialize(1);// multithreaded Api
	if(++otlInitCount == 1) {
		//otl_connect::otl_initialize(1);// multithreaded Api
		BeginOtlTrace();
	}
}
void OtlModuleExit() {
	if(--otlInitCount ==0) {
		//otl_connect::otl_terminate();
		//OtlTrace.close();
		//EndOtlTrace();
	}
}
/*
otl_datetime
asOtlDatetime(pdtime Tid,const PDZone& Z)
{
	otl_datetime t;
	int wd;
	Z.asYMDhms_wd(Tid,t.year,t.month,t.day,t.hour,t.minute,t.second,wd);
#ifdef OTL_ORA_TIMESTAMP
	t.tz_hour = (short)Z.diffGMT(Tid)/3600;	// Betydningen av fortegnet varierer litt. Usikker på dette.
	t.tz_minute = (short)Z.diffGMT(Tid) % 3600;
#endif
	return t;
}
*/
//---- OtlConnection ----------------------------------------------------------------------------------------------

OtlConnection::OtlConnection() 
{
	envhp_ = nullptr;
	svchp_ = nullptr;
	_transLevel = 0;
	connectMethod_ = NoConnect;
	otl_connect::otl_initialize(1);// multithreaded Api
}

OtlConnection::~OtlConnection() //noexcept(false)
{
	LOG4CXX_INFO_ENTRY(_l4n, 
		"\n\tdb.connected = " << db.connected <<
		"\n\tconnectMethod = " << connectMethod_ <<
		"");

	if (connectMethod_ == OracleAttachDetach) 
	{
		try
		{
			session_end();
			db.server_detach();
			LOG4CXX_INFO_EXIT(_l4n, "\nSESSION IS ENDED AND SERVER CONNECTION IS DETACHED");
            return;
		}
		catch (otl_exception &e) 
		{
			string stMsg = ReportException(__FUNCTION__, "attach to DB", __FILE__, __LINE__, e);
			connectMethod_ = NoConnect;
			LOG4CXX_INFO_EXIT(_l4n, "After otl_exception");
            return;
            //throw exception(stMsg.c_str());
		}
        catch (...)
        {
            connectMethod_ = NoConnect;
            LOG4CXX_ERROR(_l4n, "Unknown exception after session_end(), erver_detach()");
            return;
        }
    }

    LOG4CXX_INFO_EXIT(_l4n, "connectMethod_ != OracleAttachDetach. Nothing to do");
}

// Real connection loss results in
// ORA-03113: filslutt (EOF) på kommunikasjonskanalen
// SimulateConnectionLoss() just calls session_end(), but restores otl_connect::connected to 1.
// It gives ORA exception 
// ORA-01012: ikke logget på.
// We accept the latter as logical equivalent to the former:
void OtlConnection::SimulateConnectionLoss()
{
	LOG4CXX_ERROR(_l4n, __FUNCTION__
		"\n\tTHIS FUNCTION SHOULD BE USED ONLY FOR TESTING" <<
		"\n\tdb.connected = " << db.connected <<
		"\n\tconnectMethod = " << connectMethod_);

	try
	{
		// Exception thrown by commit() depends on type of method used to simulate connection loss.
		// server_detach() results in empty message and code = 0xFFFFFFFFCCCCCCCC or 0x1D779570, 
		// while session_end() leads to ORA-01012.
		// session_end() calls OCISessionEnd. Oracle documentation states that "The transaction specified by the service context is implicitly committed". 
		// We either should run rollback() before call to session_end() or use server_detach() which calls OCIServerDetach() and does not commit.
		// Explicit rollback here means much less reliable test result 
		// (problem with misterious implicit COMMIT after ALTER SESSION KILL only for METER_VL and only on SKDB2)
		// Hence we use server_detach():
		// db.rollback();
		// db.session_end();
        if (connectMethod_ == OracleAttachDetach)
        {
            db.server_detach();
        }
        else
        {
            db.logoff();
        }
        //We need to keep db.connected = 1 becuase many of Db access methods check it and do not attempt
        //to do something if it is 0:
        db.connected = 1;
        LOG4CXX_FATAL(_l4n, "\n" << __FUNCTION__ << ":\nSESSION IS ENDED AND SERVER CONNECTION IS DETACHED, BUT db.connected REMAINS 1");
	}
	catch (otl_exception &e) 
	{
		string stMsg = ReportException(__FUNCTION__, "simulate connection loss", __FILE__, __LINE__, e);
		connectMethod_ = NoConnect;
		LOG4CXX_FATAL_EXIT(_l4n,	"");
		throw exception(stMsg.c_str());
	}
}

void OtlConnection::server_attach_session_init(
    const string userName, const string password, const string tnsName, const int maxAttemptCount)
{
	LOG4CXX_INFO_ENTRY(_l4n, "\n\tstUserName = " << userName << 
        ", stTNSName = " << tnsName << ", maxAttemptCount = " << maxAttemptCount);

    if (connectMethod_ != NoConnect)
    {
        try
        {
            session_end();
            if (connectMethod_ == OracleAttachDetach)
                db.server_detach();
        }
        catch (otl_exception &e)
        {
            //Ignore the exception, just log it:
            string stMsg = ReportException(__FUNCTION__, "end session and detach", __FILE__, __LINE__, e);
        }
        connectMethod_ = NoConnect;
    }

    auto attemptCount = 0;

    while (true)
    {
        ++attemptCount;
        stringstream step;
        try
        {
            step << "\n\t\tserver_attach(" << tnsName << ", ..)";
            db.server_attach(tnsName.c_str(), nullptr, nullptr, true);
            //We begin new session here and end it immediately to use session_reopen afterwards:
            step << "\n\t\tsession_begin(" << userName << ", " << password << ", 0)";
            db.session_begin(userName.c_str(), password.c_str(), 0);
            step << "\n\t\tsession_end()";
            db.session_end();
            step << "\n\t\tauto_commit_off()";
            db.auto_commit_off();
            LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nSERVER CONNECTION IS ATTACHED, SESSION IS OPENED AND THEN ENDED");

            _userName = userName;
            _password = password;
            _tnsName = tnsName;
            
            connectMethod_ = OracleAttachDetach;
            
            break;
        }
        catch (otl_exception &e)
        {
            auto msg = ReportException(__FUNCTION__, "open DB connection", __FILE__, __LINE__, e);
            if (attemptCount >= maxAttemptCount)
            {
                connectMethod_ = NoConnect;
                LOG4CXX_ERROR(_l4n, 
                    "\n\tstUserName = " << userName <<
                    ", stTNSName = " << tnsName <<
                    "\n\tattemptCount = " << attemptCount <<
                    " >= maxAttemptCount = " << maxAttemptCount << 
                    "\n\tSteps:" << step.str() <<
                    "\n\tThrowing exception");
                throw exception(msg.c_str());
            }
            //SIV. this_thread::sleep_for() does not compile in Clr configuration (OTL.NET).
            //We use Windows API Sleep() instead.
            //See https://stackoverflow.com/questions/49071285/sleep-vs-sleep-for
            //std::this_thread::sleep_for(2s);
            Sleep(2000);
        }
    }

	LOG4CXX_INFO_EXIT(_l4n, "");
}

void OtlConnection::server_attach_session_init()
{
	LOG4CXX_INFO_ENTRY(_l4n, "\n\t_stUserName = " << _userName << ", _stTNSName = " << _tnsName);
	
	if (_userName.empty())
	{
		char * szUserName = getenv("ICC_DBUSER");
		if (szUserName != nullptr)
			_userName = szUserName;
	}
	if (_password.empty())
	{
		char * szPassword = getenv("ICC_DBPASSWD");
		if (szPassword != nullptr)
			_password = szPassword;
	}
	if (_tnsName.empty())
	{
		char * szTnsName = getenv("LOCAL");
		if (szTnsName == nullptr)
			szTnsName = getenv("TWO_TASK");
		if (szTnsName != nullptr)
			_tnsName = szTnsName;
	}

	server_attach_session_init(_userName, _password, _tnsName);

	LOG4CXX_INFO_EXIT(_l4n, "");
}

bool OtlConnection::setOCIEnvAndService(OCIEnv *envhpA, OCISvcCtx *svchpA) 
{
	LOG4CXX_DEBUG_ENTRY(_l4n,
		"\n\tconnectMethod = " << connectMethod_ <<
		""
		);
	envhp_ = envhpA;
	svchp_ = svchpA;
	connectMethod_ = OracleProC;

	LOG4CXX_DEBUG_EXIT(_l4n, "\nOK");

	return true;
}

void  OtlConnection::session_begin() 
{
	LOG4CXX_INFO_ENTRY(_l4n, 
		"\n\tdb.connected = " << db.connected <<
		"\n\tconnectMethod = " << connectMethod_ <<
		"\n\t_stUserName = " << _userName <<
		"\n\t_stTNSName = " << _tnsName <<
		""
		);
	
	try
	{
		if (db.connected == 0)
		{
			switch(connectMethod_) 
			{
				case OracleAttachDetach: 
					db.session_begin(_userName.c_str(), _password.c_str(), 0);
					LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nSESSION IS BEGUN");
					break;
				case OracleProC: 
					db.rlogon(envhp_, svchp_);
					LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nLOGON OK");
					break;
				default: 
					// error
					break;
			}
		}
		db.auto_commit_off();
	}
	catch (otl_exception &e) 
	{
		string stMsg = ReportException(__FUNCTION__, "attach to DB", __FILE__, __LINE__, e);
		connectMethod_ = NoConnect;
		LOG4CXX_INFO_EXIT(_l4n, "");
		throw exception(stMsg.c_str());
	}
	LOG4CXX_INFO_EXIT(_l4n, "");
}

void  OtlConnection::session_end() 
{
	LOG4CXX_INFO_ENTRY(_l4n, 
		"\n\tdb.connected = " << db.connected <<
		"\n\tconnectMethod = " << connectMethod_ <<
		""
		);
	try
	{
		if (db.connected)
		{
			switch (connectMethod_) 
			{
				case OracleAttachDetach: 
					// Do not rollback here - it has ugly side effects when calling method rolls back on its own:
					// rollback();
					db.session_end();
					LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nSESSION IS ENDED");
					break;
				case OracleProC: 
					db.logoff();
					LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nLOGOFF");
					break;
				default: 
					// error
					break;
			}
		}
	}
	catch (otl_exception &e) 
	{
		ReportException(__FUNCTION__, "end session ", __FILE__, __LINE__, e);
		connectMethod_ = NoConnect;
		// We do not re-throw exception here - user is done with his work anyway
		// and next call to session_reopen should recover connection if possible
	}

	LOG4CXX_INFO_EXIT(_l4n, "");
}

static const char * szOtlConnectNotInitialized = ": DB server is not attached or session is not open";

//Needed for this_thread::sleep_for()
//#include <chrono>
//#include <thread>

void OtlConnection::session_reopen()
{
    LOG4CXX_INFO_ENTRY(_l4n, "\n\tdb.connected = " << db.connected << ", connectMethod_ = " << connectMethod_);
	bool try_server_attach = false;
    auto maxAttemptCount = 1;

	try
	{
		if (!connected())
		{
			db.session_reopen(0);
		}
		else
		{
			// This is unbalanced call to the method. To ensure that we are still connected try to end the session and then reopen it again:
			LOG4CXX_WARN(_l4n,  "\n" << __FUNCTION__ << "\nUnbalanced call to session_reopen");
			db.session_end();
			db.session_reopen(0);
		}
		db.auto_commit_off();
		LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nSESSION IS REOPENED");
	}
	catch (otl_exception &e) 
	{
		string msg = ReportException(__FUNCTION__, "reopen DB session", __FILE__, __LINE__, e);
		connectMethod_ = NoConnect;
			
		if (_userName.empty() || _password.empty() || _tnsName.empty())
		{
			LOG4CXX_INFO_EXIT(_l4n, "");
			throw exception(msg.c_str());
		}
			
        if
			(
				   e.code == 0xFFFFFFFFCCCCCCCC // server_detach was executed
				|| e.code == 0x1D779570 // server_detach was executed, newer Oracle client (?)
				|| e.code == 24313 // ORA-24313: user already authenticated. It means that session_end could not do its job and user is still "active"
            )
		{
			LOG4CXX_INFO(_l4n,  
                __FUNCTION__ ":\nServer is detached or user alredy authenticated (session has been open). TRYING TO RECOVER");
        }
        else if
            (
                //SIV. In case of errors below Oracle recommends try the operation once more:
                e.code == 03113	// ORA-03113: end-of-file on communication channel
                || e.code == 02361
                || e.code == 03142
                || e.code == 03143
                || e.code == 03144
                || e.code == 12505
                || e.code == 12509
                || e.code == 12510
                || e.code == 12511
                || e.code == 12514
                || e.code == 12521
                || e.code == 12526
                || e.code == 12527
                || e.code == 12528
                )
        {
            LOG4CXX_ERROR(_l4n, "\n" << __FUNCTION__ << 
                ":\nOracle error ORA-" << e.code << ". TRYING TO RECOVER");
            maxAttemptCount = 10;
        }
        else
		{
			LOG4CXX_INFO_EXIT(_l4n, "");
			throw exception(msg.c_str());
		}
        try_server_attach = true;
    }

	if (try_server_attach)
	{
		try
		{
			db.session_end();
			db.server_detach();
		}
		catch (otl_exception &e) 
		{
			ReportException(__FUNCTION__, "detach DB server after unsuccessful call to session_reopen", __FILE__, __LINE__, e);
			// Ignore this exception and try to re-attach to the server anyway
		}
		
        try
        {
            //SIV. this_thread::sleep_for() does not compile in Clr configuration (OTL.NET).
            //We use Windows API Sleep() instead.
            //See https://stackoverflow.com/questions/49071285/sleep-vs-sleep-for
            //std::this_thread::sleep_for(2s);
            if (maxAttemptCount > 1)
                Sleep(2000);
            server_attach_session_init(_userName, _password, _tnsName, maxAttemptCount);
            db.session_reopen(0);
            db.auto_commit_off();
            LOG4CXX_INFO(_l4n, "\n" << __FUNCTION__ << ":\nSESSION IS REOPENED AFTER CONNECTION RECOVERY");
        }
        catch (otl_exception &e)
        {
            auto msg = ReportException(__FUNCTION__, "reopen DB session after connection recovery", __FILE__, __LINE__, e);
            connectMethod_ = NoConnect;
            throw exception(msg.c_str());
        }
	}

	LOG4CXX_INFO_EXIT(_l4n, "");
}

bool OtlConnection::connected()
{
	LOG4CXX_DEBUG_ENTRY(_l4n,
		"\n\tdb.connected = " << db.connected <<
		"\n\tconnectMethod = " << connectMethod_ <<
		""
		);
	
	bool bConnected = (db.connected != 0);

	LOG4CXX_DEBUG_EXIT(_l4n, 
		"\n\tbConnected = " << (int)bConnected <<
		"");

	return bConnected;
}

// Following methods do not rely on _transLevel (no nested transaction are supposed to be allowed)
void OtlConnection::commit()
{
	if (db.connected != 0)
	{
		try
		{
			db.commit();
			LOG4CXX_DEBUG(_l4n, "\n" << __FUNCTION__ << ":\nTRANSACTION IS COMMITED");
		}
		catch (otl_exception &e) 
		{
			string stMsg = ReportException(__FUNCTION__, "commit transaction", __FILE__, __LINE__, e);
			connectMethod_ = NoConnect;
			throw exception(stMsg.c_str());
		}
	}
	else
	{
		string stMsg = __FUNCTION__;
		stMsg += szOtlConnectNotInitialized;
		LOG4CXX_FATAL(_l4n, "\n" << stMsg);
		throw exception(stMsg.c_str()); 
	}
}

void OtlConnection::rollback()
{
	if (db.connected != 0)
	{
		try
		{
			db.rollback();
			LOG4CXX_DEBUG(_l4n, "\n" << __FUNCTION__ << ":\nTRANSACTION IS ROLLED BACK");
		}
		catch (otl_exception &e) 
		{
			string stMsg = ReportException(__FUNCTION__, "rollback transaction", __FILE__, __LINE__, e);
			connectMethod_ = NoConnect;
		}
	}
	else
	{
		string stMsg = __FUNCTION__;
		stMsg += szOtlConnectNotInitialized;
		LOG4CXX_ERROR(_l4n, "\n" << stMsg);
		// Do not throw exception here. 
		// Call to rollback() in production mode is usually in catch block, and this exception would invalidate exception to be thrown from there.
		// In test mode it is sufficient to have log message:
		// throw exception(stMsg.c_str()); 
	}
}

string OtlConnection::ReportException(const char *function, const char * attemptedAction, const char *file, int line, const otl_exception &e) 
{
	string stMsg = FormatException(function, attemptedAction, file, line, e);
#ifndef EXCLUDE_ActivityLog
    if (pal != nullptr)
		pal->LogMessage(2, stMsg);
#endif
	LOG4CXX_ERROR(_l4n, "\n" << stMsg.c_str());
	return stMsg;
}

string OtlConnection::FormatException(const char *function, const char * attemptedAction, const char *file, int line, const otl_exception &e) 
{
    stringstream ss;
    ss
        << function << ". Attempt to " << attemptedAction << " resulted in exception:\n\te.msg = "
        << e.msg << "\n\tcode = " << e.code << "\n\tvar_info = " << e.var_info
        << "\n\tFile: " << file << ", line: " << line
        << "\n\tstm_text:\n" << e.stm_text;

    return  ss.str();
}
