#include "StdAfx.h"
#include "ConfigurationReaderService.h"

#pragma managed

#import "../CfgServer/_CfgServer.tlb"

namespace ConfigurationSystem {

    ConfigurationReaderService::ConfigurationReaderService()
    {
        _log->Debug("Enter");
        _reader = gcnew ConfigurationSystemReaderCLIWrapper();
        _log->Debug("Exit");
    }

    ConfigurationReaderService::~ConfigurationReaderService()
    {
    }

    bool ConfigurationReaderService::GetConfiguration(System::String^ username, System::String^ computername, System::String^ progName, 
                                                 [Out] List<System::String^>^% configResult) {

        return _reader->GetConfiguration(username, computername, progName, configResult);
    }

    bool ConfigurationReaderService::GetRolesForUser(System::String^ username, 
        [Out] Dictionary<System::String^, 
        System::String^>^% roleToRoleCfgName)
    {
        return _reader->GetRolesForUser(username, roleToRoleCfgName);
    }

}