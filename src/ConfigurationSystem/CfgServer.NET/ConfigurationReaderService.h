#pragma once

#include "IConfigurationReaderService.h"
#include "ConfigurationSystemReaderCLIWrapper.h"

namespace ConfigurationSystem {

    public ref class ConfigurationReaderService : IConfigurationReaderService {
    private:
        static log4net::ILog^ _log = LogManager::GetLogger(ConfigurationReaderService::typeid);
        ConfigurationSystemReaderCLIWrapper^ _reader;

    public:
        ConfigurationReaderService();
        ~ConfigurationReaderService();

        virtual bool GetConfiguration(System::String^ username, System::String^ computername, System::String^ progName, 
            [Out] List<System::String^>^% configResult);

        //bool GetConfigurationWithGroup(System::String^ username, 
        //    System::String^ computername, 
        //    System::String^ progName, 
        //    [Out] Dictionary<System::String^, ConfigurationValue>^% configResult);

        //bool GetConfigurationWithGroup(System::String^ username, 
        //    System::String^ computername, 
        //    System::String^ progName,
        //    List<System::String^>^ filter,
        //    [Out] Dictionary<System::String^, ConfigurationValue>^% configResult);

        virtual bool GetRolesForUser(System::String^ username, 
            [Out] Dictionary<System::String^, 
            System::String^>^% roleToRoleCfgName);

    };
}