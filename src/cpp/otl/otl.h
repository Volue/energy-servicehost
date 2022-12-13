#pragma once
///
// DOCUMENTATION FOR OTL: please visit http://otl.sourceforge.net/
//

// Trace is currently disabled in favor of log4cxx
#ifndef NO_OTL_TRACE
#define NO_OTL_TRACE
#endif
///
// Trace support is default ON
// for release code that needs high speed
// and no trace, define the NO_OTL_TRACE
// when compiling
///
#ifndef NO_OTL_TRACE
#include <fstream>
extern std::ofstream OtlTrace;
extern unsigned int OtlTraceLevel;
#define OTL_TRACE_LEVEL OtlTraceLevel
#define OTL_TRACE_STREAM OtlTrace
#define OTL_TRACE_LINE_PREFIX ""
#endif

#define OTL_STL
///
// Database support: Currently we use only up to 11g release 2 level of Oracle OCI (Oracle 11.2)
#define OTL_ORA11G_R2
// Enabling VC++ 64-bit integers.
// Starting with release 11.2, OCI supports the ability to bind and define integer values greater
// than 32-bit size (more than nine digits of precision) from and into a NUMBER column using a
// 64-bit native host variable (in both 32-bit and 64-bit OCI 11.2).
// OTL can take advantage of this when OTL_ORA11G_R2 is defined, but we need to define OTL_BIGINT
// and OTL_UBIGINT to the native types to be used, in VC++ the natural type is __int64. Then bind
// variable data types "bigint" and "ubigint" can be used.
#define OTL_BIGINT __int64
#define OTL_UBIGINT unsigned __int64
// Extended information during exceptions, very useful!
#define OTL_EXCEPTION_ENABLE_ERROR_OFFSET
// Use most updated oracle OCI initialize routines
#define OTL_ORA_OCI_ENV_CREATE
#define OTL_ORA_OCI_ENV_CREATE_MODE OCI_THREADED // oas
// Oracle timestaps (OTL_ORA_TIMESTAMP)
// - Option to force OTL to use OCIDateTime, allowing timestamp values,
//   which can be down to microseconds and include time zone offset,
//   to be written and read.
// - SIH 10.09.2009:
//   - OTL_ORA_TIMESTAMP gives micro/millisec possiblity and timestamp support, 
//     but: performance in large operations is bad on client side at least
//     and vector support for timestamp pl/sql array from client to host
//     is not available.
//   - Turned off in R90_CORE and later!
#define NIX_OTL_ORA_TIMESTAMP
// OTL stream pooling (OTL_STREAM_POOLING_ON)
// - Performance feature of OTL, where instances of otl_stream variables get saved
//   in a stream pool, when they get closed, for further reuse in the program.
// - SIH 10.09.2009 (SmG release 9):
//   - Streampooling works nice, gives better db-server and client performance
//     but not strictly neccessary in R9*, we try in R10.
//   - Added define, but commented out, in R90_CORE. Stream pooling is _not_ in use.
//   - Unknown, 2011 (SmG release 10.0):
//     - Activated stream pooling (define of OTL_STREAM_POOLING_ON is uncommented).
// - SIV 13.03.2013 (10.3_Historikk, first released in SmG release 10.5?):
//   - Deactivated stream pooling again.
// - AON 28.10.2016 (10.2.1 Fortum hotfix):
//   - Stream pooling seems to have been active in SmG release 10.0, 10.1, 10.2, 10.3 and 10.4.
//   - Deactivating stream pooling in Fortum-specific 10.2.1 release of ScadaEx (branch 10.2.1_Fortum).
//     - Deactivating stream pooling seems to have fixed a crash issue in ScadaEx version 10.2.1
//       (see Defect:D-09435/PowelID:81313/SM-IT01199382). The crash occurred during automatically
//       reconnecting after network disconnection, and call stack from crash dump indicated access violation
//       in otl_stream_pool::init called from otl_connect::logoff, as a result of PDTsFactory::connectToDb
//       performing re-connect.
//   - NB: The issue discovered with ScadaEx is something to consider before activating stream pooling again.
//     It was special in the sense that it occurred on reconnection after network disconnect, with few of our
//     applications are doing, and also there was a bit of randomness to the problem. The OTL version in this
//     release is quite old (4.0.213), so the problem might have been fixed later, but still it is something
//     to be aware of!
//#define OTL_STREAM_POOLING_ON
// Let OTL take advantage of the C++11 new techniques and features since we are building
// with Visual Studio 2013 now.
//#define OTL_CPP_11_ON
// Let OTL take advantage of the C++14 (and C++11) new techniques and features since we are
// building with Visual Studio 2015 now.
#define OTL_CPP_14_ON
// Let OTL take advantage of the C++17 (as well as C++14 and C++11) new techniques and features?
//#define OTL_CPP_17_ON

///
// Function level access to OTL trace and init/exit functions
extern void SetOtlTraceLevel(unsigned int level);
extern unsigned int GetOtlTraceLevel();
extern void OtlModuleInit();// called pr. thread priori using otl library
extern void OtlModuleExit();// called pr. thread after using otl library

/**
 * Use reference to OtlConnection as parameter to initialize work horse classes.
 * OtlConnection is supposed to be owned by the high level controller object (application or library, e.g. World.cpp in unit tests suite).
 */
#define AGGREGATE_OTL_CONNECT_BY_REF

#include <windows.h>

// Utility class for using the OTL  library
// takes care of OTL module init/exit
// ensures that each module init is matched
// with corresponding exit
class OtlModule {
public:
	OtlModule() {
		OtlModuleInit();
	}
	OtlModule(const OtlModule&) {
		OtlModuleInit();
	}
	~OtlModule() {
		OtlModuleExit();
	}
	void setTraceLevel(int level) {
		SetOtlTraceLevel(level);
	}
	unsigned int getTraceLevel() {
		return GetOtlTraceLevel();
	}
};


// Switch off definitions from old ctype.h used by some Powel applications (ScadaEx.vsproj)
// that interfere with usage of respective functions in otlv4.h:
#undef isalpha
#undef isupper
#undef islower
#undef isalnum
#undef isdigit
#undef isgraph
#undef tolower
#undef _tolower
#undef toupper
#undef _toupper
#undef iscntrl
#undef isspace
#undef ispunct

#include "otlv4.h"

// We have enabled native support for 64-bit integers by specifying OTL_BIGINT above,
// and for convenience we here define a vector of bigints, since it is not defined in otlv4.h.
typedef otl_T_vec<OTL_BIGINT, otl_var_bigint, sizeof(OTL_BIGINT)> otl_bigint_vec;

/**
 * Powel utility class that wraps otl_connect object
 * and provides connection via existing OracleProC (libso_classes)
 * or usr/pwd/tnsName.
 * supports simple transaction nesting.
 */
class OtlConnection
{

	/**
	 * The otl_connect object used by the OracleInterface
	 * in order to communicate with the ICC database. 
	 */
protected:
	otl_connect db;
	
private:
	enum OracleConnectionMethod {
		NoConnect,
		OracleProC,
		OracleAttachDetach
	};
	/**
	 * Can be any of OracleProC, or OracleAttachDetatch.
	 */
	OracleConnectionMethod connectMethod_;
	std::string _userName;
	std::string _password;
	std::string _tnsName;
	
	/**
	 * Pro*C environment and context pointers in case of ProC connection mode.
	 */
	OCIEnv * envhp_;
	OCISvcCtx * svchp_;
	
	/**
	 * Simple counter to keep transaction level.
	 */
	int _transLevel; 
public:
	OtlConnection();
    virtual ~OtlConnection(); // noexcept(false);

	// otl_connect interface wrappers:

	/**
	* Connect to db and then begin and end Oracle session
	* described by username, password, and TNS name (Oracle connect descriptor [alias]).
	* The session must be then re-opened by call to session_reopen
	*/
	virtual void server_attach_session_init(
        const std::string userName, const std::string password, const std::string tnsName
        , const int maxAttemptCount = 10);

	/**
	* Connect to db and then begin and end Oracle session
	* described by stored _userName, _password, and _tnsName or by respective environment variables if stored values are empty.
	* The session must be then re-opened by call to session_reopen
	*/
	virtual void server_attach_session_init();
	
	/**
	* Called before using the otl db connection
	*  variable, do a quick logon if 
	* connection-less mode. 
	*/
	virtual void session_begin();
	inline virtual void db_session_begin() { session_begin(); }

	/**
	 * Called after using the otl db connection,
	 * releases connection to db if we run in
	 * connection-less mode. 
	 */
	virtual void session_end();
	inline virtual void db_session_end() {session_end();}

	inline virtual otl_connect& get_otl_connect() { return db; }
	inline virtual otl_connect& getDbConnection() { return db; }
	
	/**
	 * Connect to db via otl re-using existing connection done by Pro*C.
	 */
	virtual bool setOCIEnvAndService(OCIEnv* envhpA, OCISvcCtx* svchpA);
	inline virtual bool setDbConnection(OCIEnv* envhpA, OCISvcCtx* svchpA) { return setOCIEnvAndService(envhpA, svchpA); }

	virtual void session_reopen();
	virtual bool connected();

	virtual void commit();
	virtual void rollback();

	// THIS FUNCTION SHOULD BE USED ONLY FOR TESTING:
	void SimulateConnectionLoss();

	static std::string ReportException(const char* file, int line, const otl_exception& e) { return ReportException("", "", file, line, e); }
	static std::string ReportException(const char* function, const char* attemptedAction, const char* file, int line, const otl_exception& e);
	static std::string FormatException(const char* function, const char* attemptedAction, const char* file, int line, const otl_exception& e);

	/**
	 * Start/commit/rollback Transaction are functions
	 * that emulates the nesting mimic of the former ICC so_classes.
	 * Do not use them in new code. Use plain commit/rollback on the highest logical transaction level instead.
	 */
	virtual void startTransaction() { ++_transLevel; }
	virtual void commitTransaction()
	{
		if(_transLevel>0)
			--_transLevel;
		if (_transLevel==0)
			db.commit();
	}
	
	virtual void rollbackTransaction()
	{
		if(_transLevel>0)
			--_transLevel;
		if (_transLevel==0)
			db.rollback();
	}

    const OCIEnv * GetOCIEnvHandle() const { return envhp_; }
    const OCISvcCtx * GetOCIServiceHandle() const { return svchp_; }
};

/**
 * Utility class that keeps reference to real OtlConnection. 
 * Purpose is to provide easy migration from inheritance to aggregation by reference for OtlConnection
 * OtlConnection is supposed to be owned by the application/library controller.
 */
class OtlConnectionUser
{
public:
	OtlConnectionUser(OtlConnection& cA): _otlConnect(cA) {}
	
	virtual OtlConnection& getDbConnection() { return _otlConnect; }
	virtual otl_connect& getDb() { return _otlConnect.getDbConnection(); }
	
	// session_begin() must not be used by external clients. Use session_reopen() instead
	// virtual void session_begin() {_otlConnect.session_begin();}
	// inline virtual void db_session_begin() {session_begin();}

	virtual void session_reopen() { _otlConnect.session_reopen(); }

	virtual void session_end() { _otlConnect.session_end(); }
	inline virtual void db_session_end() { session_end(); }

	virtual void server_attach_session_init(std::string userA, std::string pwdA, std::string serverA)
		{ _otlConnect.server_attach_session_init(userA, pwdA, serverA); }
	virtual bool setOCIEnvAndService(OCIEnv* envhpA, OCISvcCtx* svchpA)
		{ return _otlConnect.setOCIEnvAndService(envhpA, svchpA); }
	inline virtual bool setDbConnection(OCIEnv* envhpA, OCISvcCtx* svchpA)
		{ return setOCIEnvAndService(envhpA, svchpA); }

	virtual void startTransaction() { _otlConnect.startTransaction(); }
	virtual void commitTransaction() { _otlConnect.commitTransaction(); }
	virtual void rollbackTransaction() { _otlConnect.rollbackTransaction(); }
protected:
	OtlConnection& _otlConnect; //no ownership
};

/**
 * dbValue is a utility that return type T, or nullvalue if otlvalue is null.
 */
template <class T,class DBT>
T dbValue(const otl_value<DBT>& l,T nullValue) {
	return l.is_null()?nullValue:T(l.v);
}

/** Interface for reporting fatal and exceptions
* */
class IDBErrorReporter {
public:
	virtual void reportException(otl_exception& exx) = 0;
	virtual void reportException(const char* file, int line, otl_exception& exx) { reportException(exx); }
	virtual void reportFatal(int x) = 0;
	virtual void reportFatal(int x, ...) = 0;
	virtual ~IDBErrorReporter() {}
};
/*
#include <PDTime.h>
extern otl_datetime asOtlDatetime(pdtime Tid,const PDZone& Z);
inline pdtime aspdtime(const otl_value<otl_datetime>& Tid,const PDZone& Z) {
	return Tid.is_null() ? NOPDTIME : 
		Z.aspdtime(Tid.v.year,Tid.v.month,Tid.v.day,Tid.v.hour,Tid.v.minute,Tid.v.second); }
inline pdtime aspdtime(const otl_datetime& Tid,const PDZone& Z) {
	return Z.aspdtime(Tid.year,Tid.month,Tid.day,Tid.hour,Tid.minute,Tid.second); }
*/
