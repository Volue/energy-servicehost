#include "stdafx.h"

#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <atlconv.h>
#include <PDVariantUtility.h>
#include <string>

//TODO. SIV. 
//Cannot use Profiler in this project because linking to LibUtil leads to problem in solutions other than NimbusServer.
//Solution is to include Profiler.cpp into the project, but hard coded path to Mutex.h in the file prevents it.
//Change  Profiler.cpp
//#include <Profiler.h>

extern std::string GetStringFromSafeArray(VARIANT theSafeArray,int i);

#include "ConfigurationSystemReaderCLIWrapper.h"

using namespace System::Threading;
using namespace System::DirectoryServices::AccountManagement;


using namespace log4net;

/***********************************************************************************************
 *	CLI Wrapper for Configuration System COM-object
 *	Ignores environment variables
 *
 */

using namespace System::Collections::Generic;
namespace Powel  { namespace ConfigurationSystem { namespace Cli {

    const auto szComErrorTag = "_com_error";
    ConfigurationSystemReaderCLIWrapper::ConfigurationSystemReaderCLIWrapper()
	{
        _log = LogManager::GetLogger(ConfigurationSystemReaderCLIWrapper::typeid);
        _log->Warn("ConfigurationSystemReaderCLIWrapper()\n\tCalling _configSystemComPtr.CreateInstance(..)");

		_configSystemComPtr.CreateInstance(__uuidof(CfgReaderFactory));

		// If the call above fails it will throw a managed exception containing the native HRESULT error code.
		// HRESULTs corresponding to a standard .NET Framework exceptions will be thrown as such, for example
		// E_ACCESSDENIED will be thrown as System::UnauthorizedAccessException, while others will be be thrown
		// as System::Runtime::InteropServices::COMException (but both inherits from System::Exception).
		// Possibilities:
		//   - COMException representing REGDB_E_CLASSNOTREG if CfgReader type library has not been registered.
		//     Run "PowelCfgServer.exe /RegServer" to fix it.
		//   - ArgumentException representing E_INVALIDARG if CfgReader failed to connect to database.
		//     Could be because of wrong value for configuration database server ICC_CFGSERVER in registry, or server is not available.
		//     Note that this HRESULT value is explicit set by CfgReader constructor when database server attach fails,
		//     so the same value can potentially also be returned by the COM framework with other causes.
		//   - UnauthorizedAccessException representing E_ACCESSDENIED if not permissions to the CfgReader component.

		//_log->Warn("Exit");
	}

	ConfigurationSystemReaderCLIWrapper::~ConfigurationSystemReaderCLIWrapper() 
    {
		this->!ConfigurationSystemReaderCLIWrapper();
	}

	ConfigurationSystemReaderCLIWrapper::!ConfigurationSystemReaderCLIWrapper() 
    {
		// Do nothing. 
		// The _configSystemComPtr is a com::ptr which uses reference counting, destructing when necessary.
	}

	// Read information from ConfigSystem
	bool ConfigurationSystemReaderCLIWrapper::GetConfigurationWithGroup(
			System::String^ username, 
			System::String^ computername, 
			System::String^ progname, 
			[Out] Dictionary<System::String^, ConfigurationValue>^% configResult)
	{
		return GetConfigurationWithGroup(username, computername, progname, nullptr, configResult);
	}


	// Read information from ConfigSystem
	bool ConfigurationSystemReaderCLIWrapper::GetConfigurationWithGroup(
			System::String^ username, 
			System::String^ computername, 
			System::String^ progname,
			List<System::String^>^ filter,
			[Out] Dictionary<System::String^, ConfigurationValue>^% configResult)
	{
		//Profiler profiler(__FUNCTION__);
		_log->Debug("Enter");
		configResult = nullptr;

		// NOTE: Might not be neccessary
		if(!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t usernameStr = marshal_as<_bstr_t>(username);
		_bstr_t computernameStr = marshal_as<_bstr_t>(computername);
		_bstr_t prognameStr = marshal_as<_bstr_t>(progname);

		// Create and init variants
		VARIANT gnameVariant;
		VARIANT pnameVariant;
		VARIANT pvalueVariant;
		VariantInit(&gnameVariant);
		VariantInit(&pnameVariant);
		VariantInit(&pvalueVariant);

		// Lock on ComPtr object
		// To prevent randomly occurring concurrency problems(especially when the CLI wrapper smart pointer reference is used in concurrent calls).
		Monitor::Enter(lockComPtr);

        configResult = gcnew Dictionary<System::String^, ConfigurationValue>();

        try
        {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetConfigurationWithGroup(
				usernameStr,
				computernameStr,
				prognameStr, 
				&gnameVariant, 
				&pnameVariant, 
				&pvalueVariant);

		}
		catch(_com_error e)
		{
            auto msg = ReportComError(e, "GetConfigurationWithGroup()");
            configResult->Add(marshal_as<System::String^>(szComErrorTag), ConfigurationValue(msg, ""));
            return false;
        }
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> gnameList(gnameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> pnameList(pnameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> pvalueList(pvalueVariant,bnull);

		int dictSize;
		if(filter)
			dictSize = filter->Count;
		else
			dictSize = pnameList.size();

		// TODO: Verify that lists has same length
		for (size_t i = 0; i < pnameList.size(); i++) 
        {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string paramString=GetStringFromSafeArray(pnameVariant,i);
			System::String^ pname = marshal_as<System::String^>(paramString.c_str());
			if(!filter || filter->Contains(pname->ToUpper())) 
			{
				// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
				std::string nameString = GetStringFromSafeArray(gnameVariant,i);
				std::string valueString = GetStringFromSafeArray(pvalueVariant,i);

				auto pvalue = marshal_as<System::String^>(valueString.c_str());
                auto gname  = marshal_as<System::String^>(nameString.c_str());
				configResult->Add(pname, ConfigurationValue(pvalue, gname));
			}
		}
		
		// Clear variants
		VariantClear(&gnameVariant);
		VariantClear(&pnameVariant);
		VariantClear(&pvalueVariant);

		_log->DebugFormat("Exit. Returning {0} parameters.", configResult->Count);

		return true;
	}

	bool ConfigurationSystemReaderCLIWrapper::GetUsers([Out] List<System::String^>^% users)
	{
		//Profiler profiler(__FUNCTION__);
		// NOTE: Might not be necessary
		if(!_configSystemComPtr)
			return false;

		// Create and init variants
		VARIANT nameVariant;
		VariantInit(&nameVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

		try 
        {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetUsers(&nameVariant);
			int nRoles = resVariant.lVal;
			if (nRoles < 0)
			{
				_log->FatalFormat("ConfigurationSystem.GetUsers() failed. Possible reasons include: Invalid credentials for configuration system. Return value = {0} (-1 = COM error, -4 = OTL exception)", nRoles);
				return false;
			}
		}
		catch(_com_error e)
		{
			ReportComError(e, "ICfgReader->GetUsers()");
			return false;
		}
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> nameList(nameVariant,bnull);
		users = gcnew List<System::String^>(nameList.size());

		System::String^ userString;
		for(size_t i=0; i < nameList.size(); i++) 
        {
			std::string userStdString = GetStringFromSafeArray(nameVariant, i);
			userString = marshal_as<System::String^>(userStdString.c_str());
			users->Add(userString);
		}
		
		// Clear variants
		VariantClear(&nameVariant);

		_log->DebugFormat("Exit. Returning {0} parameters.", users->Count);

		return true;
	}

	// Read information from ConfigSystem
	// Configuration values is returns as a list of strings on the format "[ConfigName]=[ConfigValue]"
	// Return true if succeeded, false in case of error
	bool ConfigurationSystemReaderCLIWrapper::GetConfiguration(
		System::String^ username, 
		System::String^ computername, 
		System::String^ progname, 
		[Out] List<System::String^>^% configResult)
	{
		//Profiler profiler(__FUNCTION__);
		_log->Debug("Enter");
		configResult = nullptr;

		// NOTE: Might not be necessary
		if(!_configSystemComPtr)
			return false;
		
		// Marshal String^ to _bstr_r
		_bstr_t usernameStr = marshal_as<_bstr_t>(username);
		_bstr_t computernameStr = marshal_as<_bstr_t>(computername);
		_bstr_t prognameStr = marshal_as<_bstr_t>(progname);

		VARIANT configResultVariant;
		VariantInit(&configResultVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

        configResult = gcnew List<System::String^>();
        
        try
        {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetConfiguration(usernameStr,
				computernameStr,
				prognameStr,
				&configResultVariant);
		} 
		catch(_com_error e)
		{
             auto msg = ReportComError(e, "GetConfiguration()");
             configResult->Add(marshal_as<System::String^>(szComErrorTag) + msg);
             return false;
        }
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		// Copy results into a managed list
		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> cfgList(configResultVariant,bnull);
		for(size_t i=0;i < cfgList.size();i++) 
        {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string elementString = GetStringFromSafeArray(configResultVariant,i);
			configResult->Add(marshal_as<System::String^>(elementString.c_str()));
		}

		// Clear variant
		VariantClear(&configResultVariant);

		_log->DebugFormat("Exit. Returning {0} parameters", configResult->Count);

		return true;
	}

	// Read information from ConfigSystem
	// Encrypted parameter names are returned as a list of strings
	// Return true if succeeded, false in case of error
	bool ConfigurationSystemReaderCLIWrapper::GetEncryptedParameters(
		[Out] List<System::String^>^% configResult)
	{
		//Profiler profiler(__FUNCTION__);
		_log->Debug("Enter");
		configResult = nullptr;

		// NOTE: Might not be necessary
		if (!_configSystemComPtr)
			return false;

		VARIANT configResultVariant;
		VariantInit(&configResultVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

		configResult = gcnew List<System::String^>();

		try
		{
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetEncryptedParameters(&configResultVariant);
		}
		catch (_com_error e)
		{
			auto msg = ReportComError(e, "GetEncryptedParameters()");
			configResult->Add(marshal_as<System::String^>(szComErrorTag) + msg);
			return false;
		}
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		// Copy results into a managed list
		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> cfgList(configResultVariant, bnull);
		for (size_t i = 0; i < cfgList.size(); i++)
		{
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string elementString = GetStringFromSafeArray(configResultVariant, i);
			configResult->Add(marshal_as<System::String^>(elementString.c_str()));
		}

		// Clear variant
		VariantClear(&configResultVariant);

		_log->DebugFormat("Exit. Returning {0} parameters", configResult->Count);

		return true;
	}

	bool ConfigurationSystemReaderCLIWrapper::GetExpandedConfiguration(System::String^ username, 
		System::String^ computername, 
		System::String^ progname, 
		[Out] Dictionary<System::String^, System::String^>^% configResult) 
	{
		//Profiler profiler(__FUNCTION__);
		//_log->Warn("Enter");

		// NOTE: Might not be necessary
		if(!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t usernameStr = marshal_as<_bstr_t>(username);
		_bstr_t computernameStr = marshal_as<_bstr_t>(computername);
		_bstr_t prognameStr = marshal_as<_bstr_t>(progname);

		VARIANT configResultNameVariant;
		VariantInit(&configResultNameVariant);
		VARIANT configResultValueVariant;
		VariantInit(&configResultValueVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

        configResult = gcnew Dictionary<System::String^, System::String^>();
        
        try
        {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetExpandedConfiguration(usernameStr,
				computernameStr,
				prognameStr,
				&configResultNameVariant,
				&configResultValueVariant);
			int result = resVariant.lVal;
			if (result == -1)
			{
                auto msg = marshal_as<System::String^>(
                    "GetExpandedConfiguration() failed. Possible reason is invalid credentials for configuration system");
				_log->Error(msg);
                configResult->Add(marshal_as<System::String^>(szComErrorTag), msg);
                return false;
			}
		}
		catch(_com_error e)
		{
			auto msg = ReportComError(e, "GetExpandedConfiguration()");
            configResult->Add(marshal_as<System::String^>(szComErrorTag), msg);
            return false;
		}
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		// Copy results into a managed list
		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> cfgNameList(configResultNameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> cfgValueList(configResultValueVariant,bnull);
		for(size_t i = 0; i < cfgNameList.size(); i++) 
        {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string nameString = GetStringFromSafeArray(configResultNameVariant, i);
			std::string valueString = GetStringFromSafeArray(configResultValueVariant, i);
			configResult->Add(marshal_as<System::String^>(nameString.c_str()), marshal_as<System::String^>(valueString.c_str()));
		}

		// Clear variant
		VariantClear(&configResultNameVariant);
		VariantClear(&configResultValueVariant);

		//_log->WarnFormat("Exit\n\tconfigResult->Count = {0}", configResult->Count);

		return true;
	}

	bool ConfigurationSystemReaderCLIWrapper::GetExpandedConfigurationByRole(int roleKey, 
		System::String^ computername, 
		System::String^ progname, 
		[Out] Dictionary<System::String^, System::String^>^% configResult) 
	{
		//Profiler profiler(__FUNCTION__);
		//_log->Debug("Enter");
		configResult = nullptr;

		// NOTE: Might not be necessary
		if(!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t computernameStr = marshal_as<_bstr_t>(computername);
		_bstr_t prognameStr = marshal_as<_bstr_t>(progname);

		VARIANT configResultNameVariant;
		VariantInit(&configResultNameVariant);
		VARIANT configResultValueVariant;
		VariantInit(&configResultValueVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

		try {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetExpandedConfigurationByRole(roleKey,
				computernameStr,
				prognameStr,
				&configResultNameVariant,
				&configResultValueVariant);
			int result = resVariant.lVal;
			if (result == -1)
			{
				_log->Fatal("ConfigurationSystem.GetExpandedConfigurationByRole() failed. Possible reasons include: Invalid credentials for configuration system.");
				return false;
			} else if (result == -2) {
				_log->Fatal("ConfigurationSystem.GetExpandedConfigurationByRole() failed. Database connection not in the connected state.");
				return false;
			} else if (result == -3) {
				_log->Fatal("ConfigurationSystem.GetExpandedConfigurationByRole() failed. No associated configuration system user found for the role.");
				return false;
			} else if (result == -4) {
				_log->Fatal("ConfigurationSystem.GetExpandedConfigurationByRole() failed. Error retrieving the associated configuration system user for the role.");
				return false;
			}
		} 
		catch(_com_error e)
		{
			ReportComError(e, "ICfgReader->GetExpandedConfigurationByRole()");
			return false;
		}
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		// Copy results into a managed list
		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> cfgNameList(configResultNameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> cfgValueList(configResultValueVariant,bnull);
		configResult= gcnew Dictionary<System::String^, System::String^>(cfgValueList.size());
		for(size_t i=0;i < cfgNameList.size();i++) {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string nameString = GetStringFromSafeArray(configResultNameVariant, i);
			std::string valueString = GetStringFromSafeArray(configResultValueVariant, i);
			configResult->Add(marshal_as<System::String^>(nameString.c_str()), marshal_as<System::String^>(valueString.c_str()));
		}

		// Clear variant
		VariantClear(&configResultNameVariant);
		VariantClear(&configResultValueVariant);

		//_log->DebugFormat("Exit\n\tReturning {0} parameters", configResult->Count);

		return true;
	}

	bool ConfigurationSystemReaderCLIWrapper::GetUserDomainGroups(System::String^ username, [Out] List<System::String^>^% groups)
    {
		groups = gcnew List<System::String^>();
		PrincipalContext^ ctx;
		try
		{
			ctx = gcnew PrincipalContext(ContextType::Domain);
		}
		catch(PrincipalServerDownException^ ex)
		{
			// no domain controller available, may be normal situation in some environments
			auto msg = marshal_as<System::String^>(
				"Cannot create PrincipalContext for Domain: {0}");
			_log->DebugFormat(msg, ex->Message);
			msg = marshal_as<System::String^>(
				"No domain controller available. Skipping Active directory groups membership verification");
			_log->Info(msg);
			return false;
		}

		auto userPrincipal = UserPrincipal::FindByIdentity(ctx, username);

		if(userPrincipal == nullptr)
		{
			auto msg = marshal_as<System::String^>(
				"Cannot find UserPrincipal {0} within domain context");
			_log->ErrorFormat(msg, username);
			return false;
		}

		auto actualGroups = userPrincipal->GetGroups();
		auto msg = marshal_as<System::String^>(
			"UserPrincipal groups: [");
		for each(auto principal in actualGroups)
		{
			msg += principal->Name;
		    if(dynamic_cast<GroupPrincipal^>(principal) &&
				!System::String::IsNullOrWhiteSpace(principal->Name) &&
				!groups->Contains(principal->Name))
		    {
				msg += " (GroupPrincipal)";
				groups->Add(principal->Name);
		    }
			msg += " ,";
		}
		msg += "]";

		_log->InfoFormat(msg);
		return true;
    }

	bool ConfigurationSystemReaderCLIWrapper::GetRolesForUserActiveDirectoryGroups(System::String^ username, [Out] Dictionary<System::String^, System::String^>^% configResult)
    {
		configResult = gcnew Dictionary<System::String^, System::String^>();

		List<System::String^>^ adRoles;
    	if(!GetUserDomainGroups(username, adRoles))
    	{
    		return false;
    	}
		for each(System::String^ adRole in adRoles )
		{
			Dictionary<System::String^, System::String^>^ newRoles;
			auto ret = GetRolesForUser(adRole, newRoles);
			if(!ret)
			{
				auto msg = marshal_as<System::String^>(
					"GetRolesForUser({0}) failed. Possible reason is invalid credentials for configuration system");
				_log->ErrorFormat(msg, username);
				return ret;
			}
			
			for each(KeyValuePair<System::String^, System::String^> kvp in newRoles)
			{
				configResult->Add(kvp.Key, kvp.Value);
			}
			
		}

		return true;
    }


	bool ConfigurationSystemReaderCLIWrapper::GetRolesForUser(System::String^ username, 
			[Out] Dictionary<System::String^, System::String^>^% configResult)
	{
		//Profiler profiler(__FUNCTION__);
		//_log->WarnFormat("Enter\n\tusername = {0}", username);

		//NOTE: Might not be necessary
		if (!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t usernameStr = marshal_as<_bstr_t>(username);

		// Create and init variants
		VARIANT gnameVariant;
		VARIANT pnameVariant;
		VariantInit(&gnameVariant);
		VariantInit(&pnameVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

        configResult = gcnew Dictionary<System::String^, System::String^>();
        
        try
        {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetRolesForUser(
				usernameStr,
				&gnameVariant, 
				&pnameVariant);
			int nRoles = resVariant.lVal;
			if (nRoles == -1)
			{
                auto msg = marshal_as<System::String^>(
                    "GetRolesForUser({0}) failed. Possible reason is invalid credentials for configuration system");
                _log->ErrorFormat(msg, username);
                configResult->Add(marshal_as<System::String^>(szComErrorTag), msg);
            }
		}
		catch(_com_error e)
		{
            auto msg = ReportComError(e, "GetRolesForUser()");
            configResult->Add(marshal_as<System::String^>(szComErrorTag), msg);
            return false;
		}
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> gnameList(gnameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> pnameList(pnameVariant,bnull);

		// TODO: Verify that lists has same length
		System::String^ role;
		System::String^ roleCfgUser;
		for(size_t i=0; i < gnameList.size(); i++) 
        {
			std::string roleString = GetStringFromSafeArray(gnameVariant,i);
			std::string nameString = GetStringFromSafeArray(pnameVariant,i);
			role = marshal_as<System::String^>(roleString.c_str());
			roleCfgUser = marshal_as<System::String^>(nameString.c_str());
			configResult->Add(role, roleCfgUser);
		}
		
		// Clear variants
		VariantClear(&gnameVariant);
		VariantClear(&pnameVariant);

		//_log->WarnFormat("Exit\n\troleToRoleCfgName->Count = {0}", configResult->Count);

		return true;
    }

	// Read data from the configuration system (via COM object).
	bool ConfigurationSystemReaderCLIWrapper::GetRoles([Out] Dictionary<int,Role>^% roles)
	{
		//Profiler profiler(__FUNCTION__);
		// NOTE: Might not be necessary
		if (!_configSystemComPtr)
			return false;

		// Create and init variants
		VARIANT idVariant;
		VARIANT nameVariant;
		VARIANT userVariant;
		VariantInit(&idVariant);
		VariantInit(&nameVariant);
		VariantInit(&userVariant);

		// Lock on ComPtr object
		Monitor::Enter(lockComPtr);

		try 
        {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->GetRolesWithRoleUser(&idVariant, &nameVariant, &userVariant);
			int nRoles = resVariant.lVal;
			if (nRoles < 0)
			{
				_log->FatalFormat("ConfigurationSystem.GetRoles() failed. Possible reasons include: Invalid credentials for configuration system. Return value = {0} (-1 = COM error, -4 = OTL exception)", nRoles);
				return false;
			}
		}
		catch(_com_error e)
		{
			ReportComError(e, "ICfgReader->GetRoles()");
			return false;
		}
		finally
		{
			// Release lock on ComPtr object
			Monitor::Exit(lockComPtr);
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<int> idList(idVariant,0);
		const PowelVariantUtility::VVAccessor<_bstr_t> nameList(nameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> userList(userVariant, bnull);

		roles = gcnew Dictionary<int,Role>(idList.size());

		// TODO: Verify that lists has same length
		int idInt;
		System::String^ roleNameString;
		System::String^ userNameString;
		for(size_t i=0; i < idList.size(); i++) {
			idInt = idList[i]; 
			std::string nameString = GetStringFromSafeArray(nameVariant,i);
			std::string userString = GetStringFromSafeArray(userVariant, i);
			roleNameString = marshal_as<System::String^>(nameString.c_str());
			userNameString = marshal_as<System::String^>(userString.c_str());
			roles->Add(idInt, Role(idInt,roleNameString,userNameString));
		}
		
		// Clear variants
		VariantClear(&idVariant);
		VariantClear(&nameVariant);
		VariantClear(&userVariant);

		_log->DebugFormat("Exit. Returning {0} parameters.", roles->Count);

		return true;
	}

	//The mutex lock must be administrated in calling method. 
    //E.g. lock retrieved prior and released after calling this method.
	void ConfigurationSystemReaderCLIWrapper::VerifyCache()
	{
		//Profiler profiler(__FUNCTION__);
		System::DateTime^ dtUtcNow = gcnew System::DateTime();
		dtUtcNow = dtUtcNow->UtcNow;

		if (roleCacheLifeTime == nullptr) 
        {
			roleCacheLifeTime = gcnew System::DateTime();
			roleCacheLifeTime = roleCacheLifeTime->MinValue;
		}

		// Reload cache if life time has been exceeded
		if (roleCacheLifeTime->Ticks < dtUtcNow->Ticks) 
        {
			roleCacheArray = nullptr;
		}

		if (roleCacheArray == nullptr) 
        {
			Dictionary<int,Role>^% roles = gcnew Dictionary<int,Role>();

			if (!GetRoles(roles)) 
            {
				_log->Error("\n\tFailed to retrieve role information.");
				//throw gcnew System::Exception("Failed to retrieve roles");
			}

			int size = roles->Count;

			roleCacheArray = gcnew array<Object^,2>(size,3);
						
			// Update cache
			int i=0;
			for each(KeyValuePair<int,Role>^ kvp in roles) {
				roleCacheArray[i,0] = kvp->Key;
				roleCacheArray[i,1] = kvp->Value.Name;
				roleCacheArray[i,2] = kvp->Value.User;
				++i;
			}

			// Update life time for cache
			System::DateTime^ dtUtcNow = gcnew System::DateTime();
			dtUtcNow = dtUtcNow->UtcNow; 
			roleCacheLifeTime = dtUtcNow->AddHours(ROLECACHE_LIFETIME_HOURS);
		}
	}

	// Retrieve role key from role name (using cache), returning -1 if key not found.
	int ConfigurationSystemReaderCLIWrapper::GetRoleKeyFromCache(System::String^ roleName)
	{
		//Profiler profiler(__FUNCTION__);
		int roleKey = -1;

		// Lock on role cache object
		Monitor::Enter(lockObject);

		try {
			VerifyCache();

			int size = roleCacheArray->GetLength(0);
			int i = 0;

			while ((roleKey < 0) && (i<size)) {
				if (roleCacheArray[i,1]->Equals(roleName)) {
					roleKey = (int)roleCacheArray[i,0];
				}
				i++;
			}
		}
		finally
		{
			// Release lock on role object
			Monitor::Exit(lockObject);
		}

		return roleKey;
	}

	// Returns a list with all configuration system roles (groups with group type 'R'), with role id as key (from cache)
	bool ConfigurationSystemReaderCLIWrapper::GetRoleKeysFromCache(Dictionary<int,System::String^>^% roles)
	{
		//Profiler profiler(__FUNCTION__);
		// Lock on role cache object
		Monitor::Enter(lockObject);

		try {
			VerifyCache();

			int size = roleCacheArray->GetLength(0);
			roles = gcnew Dictionary<int, System::String^>(size);

			for (int i=0; i<size; i++) {
				roles->Add((int)roleCacheArray[i,0], (System::String^)roleCacheArray[i,1]);
			}
		}
		finally
		{
			// Release lock on role object
			Monitor::Exit(lockObject);
		}

		return true;
	}

	// Returns a list with all configuration system roles (groups with group type 'R'), with role name as key (from cache)
	bool ConfigurationSystemReaderCLIWrapper::GetRoleNamesFromCache(Dictionary<System::String^, int>^% roles)
	{
		//Profiler profiler(__FUNCTION__);
		// Lock on role cache object
		Monitor::Enter(lockObject);

		try {
			VerifyCache();

			int size = roleCacheArray->GetLength(0);
			roles = gcnew Dictionary<System::String^, int>(size);

			for (int i=0; i<size; i++) {
				roles->Add((System::String^)roleCacheArray[i,1], (int)roleCacheArray[i,0]); 
			}
		}
		finally
		{
			// Release lock on role object
			Monitor::Exit(lockObject);
		}

		return true;
	}

	// Helper class for logging details about _com_error
    System::String^ ConfigurationSystemReaderCLIWrapper::ReportComError(_com_error e, System::String^ calledFunction)
	{
        auto msg = System::String::Format("COM error after {0}: {1} - {2}"
            , calledFunction
            , e.Error()
            , marshal_as<System::String^>(e.ErrorMessage())
            );
        
        _bstr_t bstrSource(e.Source());
        _bstr_t bstrDescription(e.Description());
        auto source = marshal_as<System::String^>((LPCTSTR)bstrSource);
        auto description = marshal_as<System::String^>((LPCTSTR)bstrDescription);
        
        if (!System::String::IsNullOrEmpty(source))
            msg += "\nsource: " + source;
        
        if (!System::String::IsNullOrEmpty(description))
            msg += "\ndescription: " + description;

        if (e.Error() == E_INVALIDARG)
            msg += "\nPossible reasons are invalid configuration Db logon parameters or network problems" +
                "\nSee c:/PowelSmartLogs/CfgServer/log4cxx*.log for details";

		_log->Error(msg);
        return msg;
	}

}}}

