// CfgReader.cpp : Implementation of CfgReader

#include "stdafx.h"
#include <assert.h>
#include "CfgReader.h"
#include "StringExpander.h"
#include "PDLog4Cxx.h"
#ifndef EXCLUDE_ActivityLog
#include "ActivityLog.h"
#endif

log4cxx::LoggerPtr _l4n(log4cxx::Logger::getLogger("Powel.CfgReader"));

using namespace std;

// Helper macros for buffer sizes in OTL SQL strings.
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
// Size of buffers for null-terminated strings.
// These must be large enough to fit the longest possible strings, and since OTL strings are
// null terminated it must have room for the nullptr terminator also. So if a column is 255 characters
// long, declared as "VARCHAR2(255)", then we need a buffer size 256 (except if the preprocessor
// directive OTL_ADD_NULL_TERMINATOR_TO_STRING_SIZE is enabled).
#define CFG_STRING_SIZE_NAME 256 // Column NAME in CFGGROUPMEMBER, CFGGROUP and CFGPARAM is VARCHAR2(255)
#define CFG_STRING_SIZE_DESC 1001 // Column DESCRIPTION in CFGGROUP and CFGPARAM is VARCHAR2(1000)
#define CFG_STRING_SIZE_CVAL 1001 // Column CVAL in CFGGROUPPARAMS
#define CFG_STRING_SIZE_PARAM_EQ_VALUE_OTL 1257 // For method that returns parameters as "name=value" strings (size of CFGPARAM.NAME + 1 for '=' + size of CFGGROUPPARAMS.CVAL + nullptr = 1000 + 1 + 255 + 1 = 1257)
// Size of array return values from PL/SQL procedures.
// NB: If size is too small an OTL exception will occur. This effectively sets a hard limit on the
// number of parameters and roles per user that is supported by this library!
#define CFG_MAX_PARAMETERS_PER_USER 500 // For returning configuration parameters for a single user
#define CFG_MAX_ROLES_PER_USER 100 // For returning configuration roles for a single user
// Number of rows to read in each bulk, for queries returning cursors.
// Note that, in contrast to array return sizes, if stream size is too small it just results in additional
// round-trips to the database, not an exception, so this is just a memory vs. performance related tuning.
#define CFG_FETCH_SIZE_ROLES 500 // For returning all configuration roles (as of 12.02.2018 we have 443 roles in config@source, all customers are likely to have a lot less)
#define CFG_FETCH_SIZE_USERS 1000 // For returning all configuration users (as of 12.02.2018 we have 951 users in config@source, some customers might have a lot less than this while others might have more. Remember that this includes not just personal users, but also machines and roleusers)

//-------------------------------
// ProtectedSection - a helper class to ensure 
// all access is serialized
//
class ProtectedSection 
{ /*
	a helper class to ensure
	all access is serialized.
	Ct - takes a lock
	Dt - takes a unlock(when  ps var goes out of scope)
	*/
	CComAutoCriticalSection& mx_;
public:
	ProtectedSection(CComAutoCriticalSection &mx):mx_(mx) { mx.Lock();}
	~ProtectedSection() { mx_.Unlock();}
};

//--- class impl goes here
CComAutoCriticalSection & CfgReader::mx() 
{
	if (!mx_) 
    {
		auto newMx = new CComAutoCriticalSection();
#if _MSC_VER < 1300
		if (InterlockedCompareExchange((PVOID *)&mx_,newMx,0) != 0)
#else
		if (InterlockedCompareExchangePointer((PVOID volatile *)&mx_, newMx, nullptr) != nullptr)

#endif
		{
			delete newMx;// if we get back nullptr, then we won, otherwise another thread
		}
	}
	return *mx_;
}

CComAutoCriticalSection* CfgReader::mx_ = nullptr; // used to sequence all external access 

static std::string GetRegistryValue(HKEY key, const char *value)
{
    char buffer[1024];
    DWORD size = sizeof(buffer);
    DWORD type = 0;

    const auto rc = RegQueryValueExA(key, value, nullptr, &type, (unsigned char *)buffer, &size);
    if (rc == ERROR_SUCCESS && type == REG_SZ && size > 0 && size < 1024)
    {
        return std::string(buffer, size);
    }
    else
    {
        return std::string();
    }
}

namespace {
struct DatabaseInfo {
    bool valid = false;
    std::string tnsName;
    std::string username;
    std::string password;
};
}

static DatabaseInfo GetDatabaseInfoFromRegistry(const std::string& key)
{
    auto hkey = HKEY();
    auto rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &hkey);

    if (rc != ERROR_SUCCESS)
    {
        return DatabaseInfo{ false };
    }

    auto tnsName = GetRegistryValue(hkey, "ICC_CFGSERVER");
    auto username = GetRegistryValue(hkey, "ICC_CFGUSER");
    auto password = GetRegistryValue(hkey, "ICC_CFGPASSWD");

    if (tnsName.empty() || username.empty() || password.empty())
    {
        return DatabaseInfo{ false };
    }
    else
    {
        return DatabaseInfo{
            true,
            std::move(tnsName),
            std::move(username),
            std::move(password)
        };
    }
}

void CfgReader::InitDbInfo()
{
    const auto registryPathGlobal = std::string("SOFTWARE\\Powel");
    auto registryPathUser = std::string();

    char userName[1024];
    DWORD userNameSize = sizeof(userName);

    if (GetUserNameA(userName, &userNameSize))
    {
        registryPathUser = registryPathGlobal + '\\' + userName;
    }

    auto databaseInfo = DatabaseInfo();
    const char * source = "none";

    if (!registryPathUser.empty())
    {
        databaseInfo = GetDatabaseInfoFromRegistry(registryPathUser);
        source = registryPathUser.c_str();
    }

    if (!databaseInfo.valid)
    {
        databaseInfo = GetDatabaseInfoFromRegistry(registryPathGlobal);
        source = registryPathGlobal.c_str();
    }

    if (!databaseInfo.valid)
    {
        const auto tnsName = getenv("ICC_CFGSERVER");
        const auto username = getenv("ICC_CFGUSER");
        const auto password = getenv("ICC_CFGPASSWD");

        databaseInfo = DatabaseInfo {
            true,
            tnsName ? tnsName : "icc",
            username ? username : "icc",
            password ? password : "icc"
        };
        source = tnsName || username || password ? "environment" : "default";
    }

    tnsName_ = std::move(databaseInfo.tnsName);
    dbUser_ = std::move(databaseInfo.username);
    dbPassword_ = std::move(databaseInfo.password);

    LOG4CXX_WARN(_l4n,
        __FUNCTION__ << "\n\t"
        << "tnsName_ = " << tnsName_ << ", dbUser_ = " << dbUser_ << ", source = " << source
    );
}

CfgReader::CfgReader(): attached_(false)
{
	CfgModuleInit();
	mx(); //call mx() to make it work first time
	InitDbInfo();
}

HRESULT CfgReader::FinalConstruct()
{
    return S_OK; //dummy
}

HRESULT CfgReader::server_attach_session_init()
{
    // This method establishes connection to configuration Db, 
    // and the opens and closes the session associated with it
    // This is to minimize overhead in relation with session_begin. 
    // Other db dependent methods re-open the connection by calling session_reopen.
    // The method returns E_INVALIDARG if an error occurs. 
    // By default E_OUTOFMEMORY (OutOfMemoryException) is always returned from CreateInstance
    // when the constructor throws any exception (see CComObject<Base>::CreateInstance in atlcom.h),
    // and this is often wrong and just misleading.

    LOG4CXX_WARN_ENTRY(_l4n, "\n\ttnsName_ = " << tnsName_ << ", dbUser_ = " << dbUser_ << 
        ", attached_ = " << boolalpha << attached_);

    if (attached_)
    {
        //assert(false, __FUNCTION__ ". Unexpected call to ");
        LOG4CXX_WARN_EXIT(_l4n, "\n\tAlready attached. returning S_OK = " << S_OK);
        return S_OK;
    }

    try
    {
        conn_.server_attach_session_init(dbUser_.c_str(), dbPassword_.c_str(), tnsName_.c_str());
        conn_.session_begin(); //To allow consequent calls to session_reopen run smoothly
        conn_.session_end();
        attached_ = true;
        LOG4CXX_WARN_EXIT(_l4n, "\n\tAfter server_attach. attached_ = " << boolalpha << attached_ << 
            ". Returning S_OK = " << S_OK);
        return S_OK;
    }
    catch (exception & ) //server_attach_session_init() throws std::exception 
    { 
        ProtectedSection global(mx());
        LOG4CXX_WARN_EXIT(_l4n, "\n\treturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG; //The default E_OUTOFMEMORY is misleading. Use something different 
    }
}

void CfgReader::FinalRelease() 
{
    //try
    //{
        if (attached_)
        {
            LOG4CXX_WARN(_l4n, __FUNCTION__ "\n\ttnsName_ = " << tnsName_);
        }
    //}
    //catch (otl_exception & ex)
    //{
    //    ProtectedSection global(mx());
    //    auto msg = OtlConnection::ReportException(__FUNCTION__, "detach from Db server", __FILE__, __LINE__, ex);
    //    OutputDebugString(msg.c_str());
    //}
}

CfgReader::~CfgReader() 
{
	CfgModuleExit();
    LOG4CXX_WARN(_l4n, __FUNCTION__ "\n\tthis = " << this <<
        "\n\ttnsName_ = " << tnsName_ << ", dbUser_ = " << dbUser_ <<
        ", attached_ = " << boolalpha << attached_);
    attached_ = false;
}

STDMETHODIMP CfgReader::GetConfiguration(
    BSTR osUser, BSTR computerName, BSTR progName, VARIANT* configResult, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;
	
    try 
    {
        conn_.session_reopen();

        otl_nocommit_stream p(1,
			"begin "
			"icc_cfgsystem.getConfiguration"
			"(:cname<char[" STR(CFG_STRING_SIZE_NAME) "],in>,:uname<char["  STR(CFG_STRING_SIZE_NAME) "],in>,:env_table<char[" STR(CFG_STRING_SIZE_PARAM_EQ_VALUE_OTL) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:res<int,out>);"
			"end;",
			conn_.get_otl_connect()
		);
		otl_string_vec env_table;
		int res;
		USES_CONVERSION;
		char *userName= strdup(W2A(osUser));
		char *cName = strdup(W2A(computerName));
        if (!userName)
            userName = strdup("");
        if (!cName)
            cName = strdup("");

        LOG4CXX_WARN_ENTRY(_l4n,
            "\n\tosUser = " << userName << ", computerName = " << cName);

        p << cName << userName;
		p >> env_table;
		p >> res;
		_bstr_t bnull;
		int configCount=env_table.len();
		try 
        {
			VARIANT cv= createSafeArray<_bstr_t>(configCount,VT_BSTR,0);
			VVAccessor<_bstr_t> cfgV(cv,bnull);
			for(int i=0;i<env_table.len();i++) {
				cfgV[i]= _bstr_t(env_table[i].c_str());
			}
			*configResult=cv;
			*bResult=_variant_t(long(configCount));
        }
        catch (_com_error &e) 
        {
			retVal= e.Error();
		}
		free(userName);
        free(cName);
		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tconfigCount = " << configCount << ", res = " << res <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get configuration", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::GetEncryptedParameters(VARIANT* encryptResult, VARIANT* bResult)
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    try
    {
        conn_.session_reopen();

        otl_nocommit_stream p(1,
            "begin "
            "icc_cfgsystem.getEncryptedParameters"
            "(:env_table<char[" STR(CFG_STRING_SIZE_PARAM_EQ_VALUE_OTL) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:res<int,out>);"
            "end;",
            conn_.get_otl_connect()
        );
        otl_string_vec env_table;
        int res;
        USES_CONVERSION;
        p >> env_table;
        p >> res;
        _bstr_t bnull;
        int configCount = env_table.len();
        try
        {
            VARIANT cv = createSafeArray<_bstr_t>(configCount, VT_BSTR, 0);
            VVAccessor<_bstr_t> cfgV(cv, bnull);
            for (int i = 0; i < env_table.len(); i++) {
                cfgV[i] = _bstr_t(env_table[i].c_str());
            }
            *encryptResult = cv;
            *bResult = _variant_t(long(configCount));
        }
        catch (_com_error& e)
        {
            retVal = e.Error();
        }
        conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tconfigCount = " << configCount << ", res = " << res <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception& ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get encrypted parameters", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception& ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::GetConfigurationWithGroup(
    BSTR osUser, BSTR computerName, BSTR progName, VARIANT* gname, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    try
    {
        conn_.session_reopen();

        otl_nocommit_stream p(1,
            "begin "
            "icc_cfgsystem.getConfigurationWithGroup"
            "(:cname<char[" STR(CFG_STRING_SIZE_NAME) "],in>,:uname<char[" STR(CFG_STRING_SIZE_NAME) "],in>,:gname_table<char[" STR(CFG_STRING_SIZE_NAME) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:pname_table<char[" STR(CFG_STRING_SIZE_NAME) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:pvalue_table<char[" STR(CFG_STRING_SIZE_CVAL) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:res<int,out>);"
            "end;",
            conn_.get_otl_connect()
        );
        otl_string_vec gname_table;
        otl_string_vec pname_table;
        otl_string_vec pvalue_table;
        int res;
        USES_CONVERSION;
        char *userName = strdup(W2A(osUser));
        char *cName = strdup(W2A(computerName));
        if (!userName) 
            userName = strdup("");
        if (!cName) 
            cName = strdup("");
        LOG4CXX_WARN_ENTRY(_l4n, "\n\tosUser = " << userName << ", computerName = " << cName);
        p << cName << userName;
        p >> gname_table >> pname_table >> pvalue_table;
        p >> res;
        _bstr_t bnull;
        int configCount = gname_table.len();
        try 
        {
            VARIANT gn = createSafeArray<_bstr_t>(configCount, VT_BSTR, 0);
            VARIANT pn = createSafeArray<_bstr_t>(configCount, VT_BSTR, 0);
            VARIANT pv = createSafeArray<_bstr_t>(configCount, VT_BSTR, 0);
            VVAccessor<_bstr_t> gName(gn, bnull);
            VVAccessor<_bstr_t> pName(pn, bnull);
            VVAccessor<_bstr_t> pValue(pv, bnull);
            for (int i = 0; i < gname_table.len(); i++) 
            {
                gName[i] = _bstr_t(gname_table[i].c_str());
                pName[i] = _bstr_t(pname_table[i].c_str());
                pValue[i] = _bstr_t(pvalue_table[i].c_str());
            }
            *gname = gn;
            *pname = pn;
            *pvalue = pv;
            *bResult = _variant_t(long(configCount));
        }
        catch (_com_error &e) 
        {
            retVal = e.Error();
        }
        free(userName); 
        free(cName);
        conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tconfigCount = " << configCount << ", res = " << res << 
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get configuration with groups", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }

}

STDMETHODIMP 
CfgReader::GetRolesForUser(BSTR osUser, VARIANT* roles, VARIANT* cfgUsers, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    try
    {
        conn_.session_reopen();

        otl_nocommit_stream p(1,
			"begin "
			"icc_cfgsystem.getRolesForUser"
			"(:uname<char[" STR(CFG_STRING_SIZE_NAME) "],in>,:roleNames<char[" STR(CFG_STRING_SIZE_NAME) "],out[" STR(CFG_MAX_ROLES_PER_USER) "]>,:roleUsers<char[" STR(CFG_STRING_SIZE_NAME) "],out[" STR(CFG_MAX_ROLES_PER_USER) "]>,:res<int,out>);"
			"end;",
            conn_.get_otl_connect()
		);
		otl_string_vec roleName;
		otl_string_vec roleCfgName;
		int res;
		USES_CONVERSION;
		char *userName= strdup(W2A(osUser));
		if (!userName) 
            userName=strdup("");
        LOG4CXX_WARN_ENTRY(_l4n, "\n\tosUser = " << userName);
        p<<userName;
		p>>roleName>>roleCfgName;
		p>>res;
		_bstr_t bnull;
		int configCount=roleName.len();
		try 
        {
			VARIANT rn= createSafeArray<_bstr_t>(configCount,VT_BSTR,0);
			VARIANT cn= createSafeArray<_bstr_t>(configCount,VT_BSTR,0);
			VVAccessor<_bstr_t> rName(rn,bnull);
			VVAccessor<_bstr_t> cName(cn,bnull);
			for(int i=0;i<configCount;i++) {
				rName[i] = _bstr_t(roleName[i].c_str());
				cName[i] = _bstr_t(roleCfgName[i].c_str());
			}
			*roles=rn;
			*cfgUsers=cn;
			*bResult=_variant_t(long(configCount));
        } 
        catch (_com_error &e) 
        {
			retVal= e.Error();
			*bResult=_variant_t(long(-1));
		}
		free(userName);
		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tconfigCount = " << configCount << ", res = " << res <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get roles for user", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::GetRoles(VARIANT* ids, VARIANT* roleNames, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    LOG4CXX_WARN_ENTRY(_l4n, "\n\tdbuser = " << dbUser_);
    try
    {
        conn_.session_reopen();

        otl_nocommit_stream usrStm(
			CFG_FETCH_SIZE_ROLES, // Buffer size in number of rows
			"select g.cfggroup_key, g.name from cfggroup g"
			" where g.grouptype = :groupType<char[2],in>",
            conn_.get_otl_connect()
		);

		int key;
		char sKey[20];
		char name[255];
		USES_CONVERSION;
		string groupType = "R";
		usrStm<<(char*)groupType.c_str();
		vector<int> keys;
		vector<string> names;

		// Store values in vectors
		while (!usrStm.eof()) {
			usrStm>>key>>name;
			itoa(key,sKey,10);
			keys.push_back(key);
			names.push_back(name);
		}

		int keyCount = keys.size();

		try 
        {
			// Create variants for values
			_bstr_t bnull;
			VARIANT idn= createSafeArray<int>(keyCount,VT_I4,0);
			VARIANT cn= createSafeArray<_bstr_t>(keyCount,VT_BSTR,0);
			VVAccessor<int> iKey(idn,0);
			VVAccessor<_bstr_t> cName(cn,bnull);

			for(int i=0; i<keyCount; i++) 
            {
				iKey[i] = _variant_t(keys[i]);
				cName[i] = _bstr_t(names[i].c_str());
			}

			*ids=idn;
			*roleNames=cn;
			*bResult=_variant_t(long(keyCount));
		} 
        catch (_com_error &e) 
        {
			retVal= e.Error();
			*bResult=_variant_t(long(-1));
		}

		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tkeyCount = " << keyCount <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get roles", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::GetRolesWithRoleUser(
    VARIANT* ids, VARIANT* roleNames, VARIANT* roleUsers, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    LOG4CXX_WARN_ENTRY(_l4n, "\n\tdbuser = " << dbUser_);
    try
    {
        conn_.session_reopen();

        otl_nocommit_stream usrStm(
			CFG_FETCH_SIZE_ROLES, // Buffer size in number of rows
			"select g.cfggroup_key as key, g.name, gp.cval as roleuser"
			" from"
			"  (select * from cfggroup g where g.grouptype = :groupType<char[2],in>) g"
			"  left join"
			"  (select gp.cval, gp.cfggroup_key"
			"    from cfggroupparams gp inner join cfgparam p on gp.cfgparam_key = p.cfgparam_key"
			"    where p.name = :paramName<char[" STR(CFG_STRING_SIZE_NAME) "],in>) gp"
			"  on g.cfggroup_key = gp.cfggroup_key",
            conn_.get_otl_connect()
		);

		USES_CONVERSION;
		string groupType = "R";
		string paramName = "ROLE_CFGNAME";
		usrStm<<(char*)groupType.c_str()<<(char*)paramName.c_str();
		vector<int> keys;
		vector<string> names;
		vector<string> users;

		// Store values in vectors
		int key;
		char sKey[20];
		char name[255];
		char user[255];
		while (!usrStm.eof()) {
			usrStm>>key>>name>>user;
			itoa(key,sKey,10);
			keys.push_back(key);
			names.push_back(name);
			users.push_back(user);
		}

		int keyCount = keys.size();

		try 
        {
			// Create variants for values
			_bstr_t bnull;
			VARIANT idn = createSafeArray<int>(keyCount,VT_I4,0);
			VARIANT cn = createSafeArray<_bstr_t>(keyCount,VT_BSTR,0);
			VARIANT un = createSafeArray<_bstr_t>(keyCount, VT_BSTR, 0);
			VVAccessor<int> iKey(idn,0);
			VVAccessor<_bstr_t> cName(cn,bnull);
			VVAccessor<_bstr_t> uName(un, bnull);
			for(int i=0; i<keyCount; i++) 
            {
				iKey[i] = _variant_t(keys[i]);
				cName[i] = _bstr_t(names[i].c_str());
				uName[i] = _bstr_t(users[i].c_str());
			}
			*ids=idn;
			*roleNames=cn;
			*roleUsers=un;
			*bResult=_variant_t(long(keyCount));
		} 
        catch (_com_error &e) 
        {
			retVal= e.Error();
			*bResult=_variant_t(long(-1));
		}

		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tkeyCount = " << keyCount <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get roles with role user", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP
CfgReader::GetUsers(VARIANT* userNames, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    LOG4CXX_WARN_ENTRY(_l4n, "\n\tdbuser = " << dbUser_);
    try
    {
        conn_.session_reopen();

        otl_nocommit_stream p(
			CFG_FETCH_SIZE_USERS, // Buffer size in number of rows
			"select a.name from cfggroupmember a",
            conn_.get_otl_connect()
		);
		USES_CONVERSION;
		vector<string> names;
		char name[255];
		while (!p.eof()) 
        {
			p>>name;
			names.push_back(name);
		}
		size_t n = names.size();
		try 
        {
			// Create variants for values
			_bstr_t bnull;
			VARIANT namesVariant = createSafeArray<_bstr_t>(n,VT_BSTR,0);
			VVAccessor<_bstr_t> aName(namesVariant,bnull);
			for(size_t i = 0; i != n; ++i) 
            {
				aName[i] = _bstr_t(names[i].c_str());
			}
			*userNames = namesVariant;
			*bResult=_variant_t(long(n));
		} 
        catch (_com_error &e) 
        {
			retVal= e.Error();
			*bResult=_variant_t(long(-1));
		}
		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tnames.size() = " << names.size() <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get users", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::GetExpandedConfiguration(
    BSTR osUser, BSTR computerName, BSTR progName, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    LOG4CXX_WARN_ENTRY(_l4n, "\n\tdbuser = " << dbUser_);
    try
    {
        conn_.session_reopen();
        retVal = GetExpandedConfigurationInternal(osUser, computerName, progName, pname, pvalue, bResult);
		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\tretVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get expanded configuration", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::GetExpandedConfigurationByRole(
    int roleKey, BSTR computerName, BSTR progName, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult) 
{
    auto retVal = server_attach_session_init();
    if (retVal != S_OK)
        return retVal;

    LOG4CXX_WARN_ENTRY(_l4n, "\n\troleKey = " << roleKey);
    try
    {
        conn_.session_reopen();

        // Get configuration system user for role (get value with highest priority)
		otl_nocommit_stream usrStm(
			1, // Buffer size in number of rows. Here it is always one (we are just looking up the roleuser for a specific role).
			"select cval as username from"
			" (select gp.cval from cfggroupparams gp"
			" inner join cfgparam p on p.cfgparam_key = gp.cfgparam_key"
			" inner join cfggroup g on g.cfggroup_key = gp.cfggroup_key"
			" where p.name = :paramName<char[" STR(CFG_STRING_SIZE_NAME) "],in> and g.cfggroup_key = :roleKey<int,in> and g.grouptype = :groupType<char[2],in>"
			" order by g.priority desc)"
			" where rownum < 2",
            conn_.get_otl_connect()
		);

		USES_CONVERSION;
		char username[CFG_STRING_SIZE_NAME];
		memset(username, '\0', CFG_STRING_SIZE_NAME);

		string paramName = "ROLE_CFGNAME";
		string grpType = "R";
		usrStm<<(char*)paramName.c_str()<<roleKey<<(char*)grpType.c_str();

		if (!usrStm.eof()) 
        {
			usrStm>>username;
		}

		if (strlen(username) > 0) 
        {
			retVal = GetExpandedConfigurationInternal(_bstr_t(username), computerName, progName, pname, pvalue, bResult);
		} 
        else 
        {
            cerr << "No role user found" << endl;
            *bResult = _variant_t(long(-3));
        }

		conn_.session_end(); // disconnect from Oracle
        LOG4CXX_WARN_EXIT(_l4n, "\n\t(OS) username = " << username <<
            ", retVal = " << INT_AS_HEX(retVal));
        return retVal;
    }
    catch (otl_exception & ex)
    {
        ProtectedSection global(mx());
        auto msg = OtlConnection::ReportException(__FUNCTION__, "get expanded configuration by role", __FILE__, __LINE__, ex);
        return E_INVALIDARG;
    }
    catch (exception & ex) //session_reopen() throws std::exception
    {
        ProtectedSection global(mx());
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tException:\n" << ex.what() << "\nreturning E_INVALIDARG = " << INT_AS_HEX(E_INVALIDARG));
        return E_INVALIDARG;
    }
}

STDMETHODIMP CfgReader::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICfgReader
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

bool CfgReader::GetExpandedConfigurationInternal(
    BSTR osUser, BSTR computerName, BSTR progName, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult) 
{
    if (!conn_.connected())
    {
        LOG4CXX_ERROR_EXIT(_l4n, "\n\tUnintended usage. conn_.connected() = " << boolalpha << conn_.connected());
        return false;
    }

    HRESULT retVal = S_OK;
    auto configCountOut = 0;

    otl_nocommit_stream p(1,
		"BEGIN\n\t"
		    "ICC_CfgSystem.getConfigurationWithGroup\n\t\t"
		    "(:cname<char[" STR(CFG_STRING_SIZE_NAME) "],in>,:uname<char[" STR(CFG_STRING_SIZE_NAME) "],in>,:gname_table<char[" STR(CFG_STRING_SIZE_NAME) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:pname_table<char[" STR(CFG_STRING_SIZE_NAME) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:pvalue_table<char[" STR(CFG_STRING_SIZE_CVAL) "],out[" STR(CFG_MAX_PARAMETERS_PER_USER) "]>,:res<int,out>);"
		"END;",
        conn_.get_otl_connect()
    	);

	otl_string_vec gname_table;
	otl_string_vec pname_table;
	otl_string_vec pvalue_table;
	int res;
	USES_CONVERSION;
	char *userName= strdup(W2A(osUser));
	char *cName=strdup(W2A(computerName));
	if(!userName) 
        userName=strdup("");
	if(!cName) 
        cName=strdup("");
    LOG4CXX_WARN_ENTRY(_l4n, 
        "\n\tosUser = " << userName << ", computerName = " << cName);
    p<<cName<<userName;
	p>>gname_table>>pname_table>>pvalue_table;
	p>>res;
	_bstr_t bnull;
    auto configCountIn = gname_table.len();
    configCountOut = configCountIn;
    try
    {
		CfgMap nameValueMap;

		// Insert USERNAME and COMPUTERNAME so that configuration may resolve these.
		// Check the return value to see if an insertion was made or the parameter existed.
		pair<CfgMap::iterator,bool> insertResult;
		insertResult=nameValueMap.insert(pair<string,string>(string("USERNAME"),string(userName)));
		if (insertResult.second) configCountOut++;
		insertResult=nameValueMap.insert(pair<string,string>(string("COMPUTERNAME"),string(cName)));
		if (insertResult.second) configCountOut++;

		VARIANT pn= createSafeArray<_bstr_t>(configCountOut,VT_BSTR,0);
		VARIANT pv= createSafeArray<_bstr_t>(configCountOut,VT_BSTR,0);
		VVAccessor<_bstr_t> pName(pn,bnull);
		VVAccessor<_bstr_t> pValue(pv,bnull);

		for(int i=0;i<configCountIn;i++) {
			nameValueMap.insert(pair<string,string>(pname_table[i].c_str(), pvalue_table[i].c_str()));
		}

		StringExpander expander;
		expander.expandMap(nameValueMap);

		int j=0;
		for (CfgMap::iterator it=nameValueMap.begin() ; it != nameValueMap.end(); it++) {
			pName[j]  = _bstr_t((*it).first.c_str());
			pValue[j] = _bstr_t((*it).second.c_str());
			j++;
		}

		*pname=pn;
		*pvalue=pv;
		*bResult=_variant_t(long(configCountOut));
	} 
    catch (_com_error &e) 
    {
		retVal= e.Error();
	}
	free(userName);
    free(cName);
    LOG4CXX_WARN_EXIT(_l4n, "\n\tretVal = " << INT_AS_HEX(retVal) << ", configCountOut = " << configCountOut);

    return retVal == S_OK;
}