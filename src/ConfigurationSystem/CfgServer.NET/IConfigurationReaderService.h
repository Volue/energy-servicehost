#pragma once


#pragma managed

namespace ConfigurationSystem {

    using namespace System;
    using namespace System::Collections::Generic;
    using namespace System::ServiceModel;
    using namespace System::Runtime::InteropServices;

    [ServiceContract]
    public interface class IConfigurationReaderService {

		// Returns a List with full parameter-info
        [OperationContract]
        bool GetConfiguration(System::String^ username, System::String^ computername, System::String^ progName, 
				[Out] List<System::String^>^% configResult);

		//// Returns a Dictionary with parametername as key and ConfigurationValue-instances as value
  //      [OperationContract]
		//bool GetConfigurationWithGroup(System::String^ username, 
		//								System::String^ computername, 
		//								System::String^ progName, 
		//								[Out] Dictionary<System::String^, ConfigurationValue>^% configResult);
		//
		//// Returns a Dictionary with parametername as key and ConfigurationValue-instances as value
  //      [OperationContract]
		//bool GetConfigurationWithGroup(System::String^ username, 
		//								System::String^ computername, 
		//								System::String^ progName,
		//								List<System::String^>^ filter,
		//								[Out] Dictionary<System::String^, ConfigurationValue>^% configResult);

        [OperationContract]
        bool GetRolesForUser(System::String^ username, 
            [Out] Dictionary<System::String^, 
            System::String^>^% roleToRoleCfgName);

    };


}