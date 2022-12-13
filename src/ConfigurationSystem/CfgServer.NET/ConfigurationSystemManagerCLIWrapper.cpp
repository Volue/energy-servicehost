#include "stdafx.h"

#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <atlconv.h>
#include <PDVariantUtility.h>
#include <string>

extern std::string GetStringFromSafeArray(VARIANT theSafeArray,int i);

#include "ConfigurationSystemManagerCLIWrapper.h"

/***********************************************************************************************
 *	CLI Wrapper for Configuration System Manager COM-object
 *	Ignores environment variables
 *
 */

using namespace System::Collections::Generic;
namespace Powel  { namespace ConfigurationSystem { namespace Cli {
	
	ConfigurationSystemManagerCLIWrapper::ConfigurationSystemManagerCLIWrapper(System::String^ user, System::String^ password, System::String^ server)
	{
		_log = LogManager::GetLogger(ConfigurationSystemManagerCLIWrapper::typeid); 
		_log->Debug("Enter");

		// Throws exception containing HR
		_configSystemComPtr.CreateInstance(__uuidof(CfgManagerFactory));
		_log->Debug("Exit");

		if(!Logon(user, password, server))
		{
			throw gcnew System::Exception(System::String::Format("Unable to logon the configuration system manager with user {0}, server {0}", user, server));
		}

	}

    ConfigurationSystemManagerCLIWrapper::~ConfigurationSystemManagerCLIWrapper() {
        Logoff();
		this->!ConfigurationSystemManagerCLIWrapper();
    }

    ConfigurationSystemManagerCLIWrapper::!ConfigurationSystemManagerCLIWrapper() {
        // Do nothing. 
        // The _configSystemComPtr is a com::ptr which uses reference counting, destructing when necessary.
    }

	bool ConfigurationSystemManagerCLIWrapper::Logon(System::String^ user, System::String^ password, System::String^ server) 
	{
		// Marshal String^ to _bstr_r
		_bstr_t userStr = marshal_as<_bstr_t>(user);
		_bstr_t pwdStr = marshal_as<_bstr_t>(password);
		_bstr_t serverStr = marshal_as<_bstr_t>(server);
	
		try {
			// Returns 1 for logon_failed, 2 for no access
			_variant_t resVariant = _configSystemComPtr->logon(	userStr,
																pwdStr, 
																serverStr);	
			if(resVariant.vt == 1 || resVariant.vt == 2) {

				_log->ErrorFormat("Unable to logon the configuration manager. The returned error code was {0}", 
                 resVariant.vt);

				return false;
			}				 

			if(resVariant.vt == 3) {
				_log->Info("Already logged on the configuration manager.");
			}

		}catch(_com_error e)
		{
			ReportComError(e, "ICfgManager->logon()");
			return false;
		}
		
		return true;
	}

	void ConfigurationSystemManagerCLIWrapper::Logoff() 
	{
		try {
			_variant_t resVariant = _configSystemComPtr->logoff();
		} catch(_com_error e) {
			ReportComError(e, "ICfgManager->logoff()");
		}
	}

	bool ConfigurationSystemManagerCLIWrapper::FindGroups(System::String^ filter, [Out] List<System::String^>^% groups)
	{
		_log->Debug("Enter");
		groups = nullptr;

		// NOTE: Might not be necessary
		if(!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t filterStr = marshal_as<_bstr_t>(filter);

		// Create and init variants
		VARIANT groupsVariant;
		VariantInit(&groupsVariant);

		try {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->findGroups(filterStr, &groupsVariant);
			int nResult = resVariant.lVal;
			if (nResult < 0) {
				_log->FatalFormat("ConfigurationSystem.findGroups() failed. Possible reasons include: Invalid credentials for configuration system. Return value = {0} (-1 = db not connected, -2 = OTL exception)", nResult);
				return false;
			}
		}
		catch(_com_error e)
		{
			ReportComError(e, "ICfgManager->findGroups()");
			throw;
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> groupsList(groupsVariant,bnull);

		groups = gcnew List<System::String^>(groupsList.size());
		for (size_t i = 0; i < groupsList.size(); ++i) {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string groupString = GetStringFromSafeArray(groupsVariant, i);
			groups->Add(marshal_as<System::String^>(groupString.c_str()));
		}
		
		// Clear variants
		VariantClear(&groupsVariant);

		_log->DebugFormat("Exit. Returning {0} parameters.", groups->Count);

		return true;
	}


	bool ConfigurationSystemManagerCLIWrapper::GetGroups(
			List<System::String^>^ groups,
			[Out] Dictionary<System::String^, ConfigurationGroup>^% groupResult)
	{
		_log->Debug("Enter");
		groupResult = nullptr;

		// NOTE: Might not be neccessary
		if(!_configSystemComPtr)
			return false;

		// Crate and init variant, to contain the list of group names
		VARIANT nameVariant = PowelVariantUtility::createSafeArray<_bstr_t>(groups->Count, VT_BSTR, 0);
		_bstr_t bnull;
		PowelVariantUtility::VVAccessor<_bstr_t> nameList(nameVariant, bnull);
		nameList.resize(groups->Count);
		for (int i = 0; i < groups->Count; ++i) {
			// Marshal String^ to _bstr_r
			nameList[i] = marshal_as<_bstr_t>((*groups)[i]);
		}

		// Create and init variants
		VARIANT typeVariant;
		VARIANT priorityVariant;
		VARIANT descriptionVariant;
		VariantInit(&typeVariant);
		VariantInit(&priorityVariant);
		VariantInit(&descriptionVariant);

		try {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->getGroups(nameVariant, &typeVariant, &priorityVariant, &descriptionVariant);

		} catch(_com_error e) {
			ReportComError(e, "ICfgManager->getGroups()");
			throw;
		} finally {
			// Clear variants
			VariantClear(&nameVariant);
		}

		const PowelVariantUtility::VVAccessor<_bstr_t> typeList(typeVariant,bnull);
		const PowelVariantUtility::VVAccessor<int> priorityList(priorityVariant, 0);
		const PowelVariantUtility::VVAccessor<_bstr_t> descriptionList(descriptionVariant,bnull);

		int listSize = groups->Count;
		groupResult = gcnew Dictionary<System::String^, ConfigurationGroup>(listSize);

		for(int i=0; i < listSize; ++i) {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string typeString = GetStringFromSafeArray(typeVariant,i);
			std::string descriptionString = GetStringFromSafeArray(descriptionVariant,i);

			System::String^ name = groups[i];
			System::String^ type = marshal_as<System::String^>(typeString.c_str());	
			int priority = priorityList[i];
			System::String^ description = marshal_as<System::String^>(descriptionString.c_str());

			groupResult->Add(name, ConfigurationGroup(name, type, priority, description));
		}
		
		// Clear variants
		VariantClear(&typeVariant);
		VariantClear(&priorityVariant);
		VariantClear(&descriptionVariant);

		_log->DebugFormat("Exit. Returning {0} groups.", groupResult->Count);
		return true;
	}
	bool ConfigurationSystemManagerCLIWrapper::GetParametersForGroup(
			System::String^ group, 
			[Out] Dictionary<System::String^, ConfigurationParameter>^% configResult)
	{
		_log->Debug("Enter");
		configResult = nullptr;

		// NOTE: Might not be neccessary
		if(!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t groupStr = marshal_as<_bstr_t>(group);

		// Create and init variants
		VARIANT nameVariant;
		VARIANT typeVariant;
		VARIANT descriptionVariant;
		VARIANT modifiedVariant;
		VARIANT valueVariant;
		VariantInit(&nameVariant);
		VariantInit(&typeVariant);
		VariantInit(&descriptionVariant);
		VariantInit(&modifiedVariant);
		VariantInit(&valueVariant);

		try {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->getParametersForGroupWithDate(groupStr, &nameVariant, &typeVariant, &descriptionVariant, &modifiedVariant, &valueVariant);
		} catch(_com_error e) {
			ReportComError(e, "ICfgManager->getParametersForGroup()");
			throw;
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> nameList(nameVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> typeList(typeVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> descriptionList(descriptionVariant,bnull);
		const PowelVariantUtility::VVAccessor<_bstr_t> valueList(valueVariant,bnull);

		size_t listSize = nameList.size();
		configResult = gcnew Dictionary<System::String^, ConfigurationParameter>(listSize);

		for(size_t i=0; i < listSize; ++i) {

			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string nameString = GetStringFromSafeArray(nameVariant,i);
			std::string valueString = GetStringFromSafeArray(valueVariant,i);
			std::string typeString = GetStringFromSafeArray(typeVariant,i);
			std::string descriptionString = GetStringFromSafeArray(descriptionVariant,i);

			System::String^ name		= marshal_as<System::String^>(nameString.c_str());	
			System::String^ value		= marshal_as<System::String^>(valueString.c_str());
			System::String^ type		= marshal_as<System::String^>(typeString.c_str());	
			System::String^ description = marshal_as<System::String^>(descriptionString.c_str());	

			configResult->Add(name, ConfigurationParameter(name, value, type, description));
		}
		
		// Clear variants
		VariantClear(&nameVariant);
		VariantClear(&valueVariant);
		VariantClear(&typeVariant);
		VariantClear(&descriptionVariant);
		VariantClear(&modifiedVariant);

		_log->DebugFormat("Exit. Returning {0} parameters.", configResult->Count);
		return true;
	}

	bool ConfigurationSystemManagerCLIWrapper::GetMembersForGroup(System::String^ group, [Out] List<System::String^>^% members)
	{
		_log->Debug("Enter");
		members = nullptr;

		// NOTE: Might not be necessary
		if(!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t groupStr = marshal_as<_bstr_t>(group);

		// Create and init variants
		VARIANT membersVariant;
		VariantInit(&membersVariant);

		try {
			// Returns number of results, we can ignore this
			_variant_t resVariant = _configSystemComPtr->getMembersForGroup(groupStr, &membersVariant);
			int nResult = resVariant.lVal;
			if (nResult < 0) {
				_log->FatalFormat("ConfigurationSystem.getMembersForGroup() failed. Possible reasons include: Invalid credentials for configuration system. Return value = {0} (-1 = db not connected, -2 = OTL exception)", nResult);
				return false;
			}
		} catch(_com_error e) {
			ReportComError(e, "ICfgManager->getMembersForGroup()");
			throw;
		}

		_bstr_t bnull;
		const PowelVariantUtility::VVAccessor<_bstr_t> membersList(membersVariant,bnull);

		members = gcnew List<System::String^>(membersList.size());
		for (size_t i = 0; i < membersList.size(); ++i) {
			// Avoiding cast from _bstr_t to const char* because it failed on Windows 7 and 2008 Server in some circumstances.
			std::string memberString = GetStringFromSafeArray(membersVariant, i);
			members->Add(marshal_as<System::String^>(memberString.c_str()));
		}
		
		// Clear variants
		VariantClear(&membersVariant);

		_log->DebugFormat("Exit. Returning {0} members.", members->Count);
		return true;
	}

	bool ConfigurationSystemManagerCLIWrapper::AddMembersToGroup(System::String^ group, List<System::String^>^ members)
	{
		_log->Debug("Enter");

		// NOTE: Might not be necessary
		if (!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t groupStr = marshal_as<_bstr_t>(group);
		
		// Crate and init variant, to contain the list of member names
		VARIANT membersVariant = PowelVariantUtility::createSafeArray<_bstr_t>(members->Count, VT_BSTR, 0);
		_bstr_t bnull;
		PowelVariantUtility::VVAccessor<_bstr_t> membersList(membersVariant, bnull);
		membersList.resize(members->Count);
		for (int i = 0; i < members->Count; ++i) {
			// Marshal String^ to _bstr_r
			
			membersList[i] = marshal_as<_bstr_t>((*members)[i]);
		}

		try {
			// Returns integer, zero if succes and negative error code if not.
			_variant_t resVariant = _configSystemComPtr->addMembersToGroup(groupStr, membersVariant);
			int nResult = resVariant.lVal;
			if (nResult != 0) {
				_log->FatalFormat("ConfigurationSystem.addMembersToGroup() failed. Possible reasons include: Invalid credentials for configuration system. Return value = {0} (-1 = db not connected, -2 = OTL exception)", nResult);
				return false;
			}
		} catch (_com_error e) {
			ReportComError(e, "ICfgManager->addMembersToGroup()");
			throw;
		} finally {
			// Clear variants
			VariantClear(&membersVariant);
		}

		_log->Debug("Exit");
		return true;
	}

	bool ConfigurationSystemManagerCLIWrapper::RemoveMembersFromGroup(System::String^ group, List<System::String^>^ members)
	{
		_log->Debug("Enter");

		// NOTE: Might not be necessary
		if (!_configSystemComPtr)
			return false;

		// Marshal String^ to _bstr_r
		_bstr_t groupStr = marshal_as<_bstr_t>(group);
		
		// Crate and init variant, to contain the list of member names
		VARIANT membersVariant = PowelVariantUtility::createSafeArray<_bstr_t>(members->Count, VT_BSTR, 0);
		_bstr_t bnull;
		PowelVariantUtility::VVAccessor<_bstr_t> membersList(membersVariant, bnull);
		membersList.resize(members->Count);
		for (int i = 0; i < members->Count; ++i) {
			// Marshal String^ to _bstr_r
			membersList[i] = marshal_as<_bstr_t>((*members)[i]);
		}

		try {
			// Returns integer, zero if succes and negative error code if not.
			_variant_t resVariant = _configSystemComPtr->removeMembersFromGroup(groupStr, membersVariant);
			int nResult = resVariant.lVal;
			if (nResult != 0) {
				_log->FatalFormat("ConfigurationSystem.removeMembersFromGroup() failed. Possible reasons include: Invalid credentials for configuration system. Return value = {0} (-1 = db not connected, -2 = OTL exception)", nResult);
				return false;
			}
		} catch (_com_error e) {
			ReportComError(e, "ICfgManager->removeMembersFromGroup()");
			throw;
		} finally {
			// Clear variants
			VariantClear(&membersVariant);
		}

		_log->Debug("Exit");
		return true;
	}

	
	// Helper class for logging details about _com_error
	void ConfigurationSystemManagerCLIWrapper::ReportComError(_com_error e, System::String^ calledFunction)
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

        _log->Error(msg);
    }

}}}

