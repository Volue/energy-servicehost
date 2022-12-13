#pragma once


#pragma managed

#import <CfgServer.tlb>

#include <msclr\marshal.h>
#include <msclr\marshal_windows.h>
#include <msclr\com\ptr.h>
#include "IConfigurationSystemManager.h"

using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr;
using namespace msclr::interop;

using namespace PowelCfgServer;
using namespace log4net;

namespace Powel { namespace ConfigurationSystem { namespace Cli {

	public ref class ConfigurationSystemManagerCLIWrapper : IConfigurationSystemManager {
	private:
		// Smartpointer to native configurationsystem com-object
		com::ptr<ICfgManager> _configSystemComPtr;

		// Logging-interface
		log4net::ILog^ _log;

		// Helper for logging _com_error's
		void ReportComError(_com_error e, System::String^ calledFunction);

		// Log on the configuration manager
		virtual bool Logon(System::String^ user, System::String^ password, System::String^ server) sealed;

		// Log off the configuration manager
		virtual void Logoff() sealed;

	public:
		ConfigurationSystemManagerCLIWrapper(System::String^ user, System::String^ password, System::String^ server);
		~ConfigurationSystemManagerCLIWrapper();
		!ConfigurationSystemManagerCLIWrapper();

		// Returns name of groups
		virtual bool FindGroups(System::String^ filter, [Out] List<System::String^>^% groups) sealed;

		// Returns details for groups
		virtual bool GetGroups(List<System::String^>^ groups, [Out] Dictionary<System::String^, ConfigurationGroup>^% groupResult) sealed;

		// Returns all parameters set for a group
		virtual bool GetParametersForGroup(System::String^ group, [Out] Dictionary<System::String^, ConfigurationParameter>^% parameterResult) sealed;

		// Returns members of a group
		virtual bool GetMembersForGroup(System::String^ group, [Out] List<System::String^>^% members) sealed;

		// Adds members to a group
		virtual bool AddMembersToGroup(System::String^ group, List<System::String^>^ members) sealed;

		// Removes members from a group
		virtual bool RemoveMembersFromGroup(System::String^ group, List<System::String^>^ members) sealed;

	};

}}}


