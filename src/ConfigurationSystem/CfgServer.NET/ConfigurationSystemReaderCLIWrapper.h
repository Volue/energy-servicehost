#pragma once

#pragma managed

#import <CfgServer.tlb>

#include <msclr\marshal.h>
#include <msclr\marshal_windows.h>
#include <msclr\com\ptr.h>
#include "IConfigurationSystemReader.h"

using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr;
using namespace msclr::interop;
using namespace PowelCfgServer;
using namespace log4net;

namespace Powel { namespace ConfigurationSystem { namespace Cli {
	
	public ref class ConfigurationSystemReaderCLIWrapper : IConfigurationSystemReader 
    {
	private:
		// Smartpointer to native configurationsystem com-object
		static Object^ lockComPtr = gcnew Object();
		com::ptr<ICfgReader> _configSystemComPtr;

		//SIV. It looks like we cannot have it static:
		log4net::ILog^ _log;

		// Lifetime constant for role cache
		static const double ROLECACHE_LIFETIME_HOURS = 1.0;

		// Cache for role functionality
		static Object^ lockObject = gcnew Object();
		static System::DateTime^ roleCacheLifeTime = nullptr;
		static array<Object^,2>^ roleCacheArray = nullptr;

		// Helper for logging _com_error's
        System::String^ ReportComError(_com_error e, System::String^ calledFunction);
		void VerifyCache();

	public:
		ConfigurationSystemReaderCLIWrapper();
		~ConfigurationSystemReaderCLIWrapper();
		!ConfigurationSystemReaderCLIWrapper();

		// Returns a List with full parameter-info
		virtual bool GetConfiguration(System::String^ username, System::String^ computername, System::String^ progName,
			[Out] List<System::String^>^% configResult) sealed;

		// Returns a List with encrypted parameter names
		virtual bool GetEncryptedParameters([Out] List<System::String^>^% configResult) sealed;
		
		// Returns a Dictionary with parameter name as key and ConfigurationValue-instances as value
		// where parameters referring to other parameters are resolved.
		// The following entries are inserted into configResult:
		// * USERNAME set to the parameter username
		// * COMPUTERNAME set to the parameter computername
		virtual bool GetExpandedConfiguration(System::String^ username, 
			System::String^ computername, 
			System::String^ progName, 
			[Out] Dictionary<System::String^, System::String^>^% configResult) sealed;

		// Returns a Dictionary with parameter name as key and ConfigurationValue-instances as value
		// where parameters referring to other parameters are resolved.
		// The following entries are inserted into configResult:
		// * USERNAME set to the parameter username (role user)
		// * COMPUTERNAME set to the parameter computername
		virtual bool GetExpandedConfigurationByRole(int roleKey, 
			System::String^ computername, 
			System::String^ progName, 
			[Out] Dictionary<System::String^, System::String^>^% configResult) sealed;

		// Returns a Dictionary with parametername as key and ConfigurationValue-instances as value
		virtual bool GetConfigurationWithGroup(System::String^ username, 
										System::String^ computername, 
										System::String^ progName, 
										[Out] Dictionary<System::String^, ConfigurationValue>^% configResult) sealed;
		
		// Returns a Dictionary with parametername as key and ConfigurationValue-instances as value
		virtual bool GetConfigurationWithGroup(System::String^ username, 
										System::String^ computername, 
										System::String^ progName,
										List<System::String^>^ filter,
										[Out] Dictionary<System::String^, ConfigurationValue>^% configResult) sealed;


		// Produces a list with all configuration system users
		virtual bool GetUsers([Out] List<System::String^>^% users) sealed;

		// Produces a Dictionary of role names mapped to role user names for the roles of the supplied user.
		virtual bool GetRolesForUser(System::String^ username, 
			[Out] Dictionary<System::String^, 
			System::String^>^% roleToRoleCfgName) sealed;

		virtual bool GetUserDomainGroups(System::String^ username, [Out] List<System::String^>^% groups);

		// Produces a Dictionary of role names mapped to role user names for the roles of the supplied user's ActiveDirectory groups.
		virtual bool GetRolesForUserActiveDirectoryGroups(System::String^ username, 
                                                          [Out] Dictionary<System::String^, 
                                                                           System::String^>^% roleToRoleCfgName) sealed;

		// Produces a list with all configuration system roles (groups with group type 'R').
		// Information is collected from the database for each call. To use cache, call "GetRoleKeysFromCache" or "GetRoleNamesFromCache". 
		// Returns true on success
		virtual bool GetRoles(Dictionary<int, Role>^% roles) sealed;

		// Implementation of cached functions

		// Look up role key from role name. If not found, role key is -1.
		// Information is retrieved from a cache, which is updated in a timely fashion with a periodic interval.
		// Param: roleToRoleCfgName is assigned a new Dictionary object on success
		// Returns true on success
		virtual int GetRoleKeyFromCache(System::String^ rolename) sealed;

		// Produces a dictionary with all configuration system role keys and their names (groups with group type 'R')
		// Information is retrieved from a cache, which is updated in a timely fashion with a periodic interval.
		// Returns true on success
		virtual bool GetRoleKeysFromCache(Dictionary<int,System::String^>^% roles) sealed;

		// Produces a dictionary with all configuration system role names and their keys (groups with group type 'R')
		// Information is retrieved from a cache, which is updated in a timely fashion with a periodic interval.
		// Returns true on success
		virtual bool GetRoleNamesFromCache(Dictionary<System::String^, int>^% roles) sealed;
	};
}}}


