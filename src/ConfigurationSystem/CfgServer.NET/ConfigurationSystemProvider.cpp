#include "stdafx.h"

#include "ConfigurationSystemReaderCLIWrapper.h"
#include "ConfigurationSystemProvider.h"

using namespace System::Configuration;
using namespace System::ServiceModel;
using namespace System::Security::Principal;

/***************************************************************************************
* .NET SettingsProvider for the Powel Configuration System.
*
* See http://msdn.microsoft.com/en-us/library/system.configuration.settingsprovider.aspx
*
* TODO: "SetPropertyValues" not implemented
* 
*
****************************************************************************************/
namespace ConfigurationSystem{
	

	void ConfigurationSystemProvider::Initialize (System::String^ name, 
									System::Collections::Specialized::NameValueCollection^ config)
	{
		_wrapper = gcnew ConfigurationSystemReaderCLIWrapper();
		
		if(name == nullptr || name->Length == 0)
			name = "ConfigurationSystemProvider";
	
		GetUserAndHost();

		__super::Initialize(System::Reflection::Assembly::GetExecutingAssembly()->GetName()->Name, config);		
	

	}

	bool ConfigurationSystemProvider::IsDomainPartOfHostnameInConfigurationSystem()
	{
		System::String^ partof = ConfigurationManager::AppSettings->Get("DomainPartOfHostnameInConfigurationSystem");
        if (partof != nullptr && partof->Trim()->ToLower() == "true")
            return true;

        return false;
	}

    bool ConfigurationSystemProvider::IsDomainPartOfUsernameInConfigurationSystem()
    {
		System::String^ partof = ConfigurationManager::AppSettings->Get("DomainPartOfUsernameInConfigurationSystem");
        if (partof != nullptr && partof->Trim()->ToLower() == "true")
            return true;

        return false;
    }

	// Attempts to retrieve current user and hostname.
	// If a WCF Security Context is available, this is used.
	// TODO: Implementet retrieval of current user if no security context is available
	void ConfigurationSystemProvider::GetUserAndHost()
	{
		_username = nullptr;
		_clientHostname = nullptr;

		// If we have a wcf-context, we should use this for username, host
		ServiceSecurityContext^ context = ServiceSecurityContext::Current;
		if(context != nullptr)
		{
			IIdentity^ identity = context->PrimaryIdentity;
            
            // If windows authentication is ok, open with info from ConfigSystem
			WindowsIdentity^ wident = dynamic_cast<WindowsIdentity^>(identity);
			if(wident)
            {
                _username = wident->Name;
                if (!IsDomainPartOfUsernameInConfigurationSystem())
                {
					int index = _username->IndexOf("\\");
                    if(index > 0)
						_username = _username->Substring(index+1);
                }

                GetWcfClientHostName();

                return;
            }
			// TODO: What if Identity isn't WindowsIntentity (Windows Authentication disabled)
		}
		// TODO: Otherwise, attempt to get localhost and current identity
	}

	void ConfigurationSystemProvider::GetWcfClientHostName()
	{
			using namespace System::ServiceModel::Channels;

			// WCF context contains IP-address, we need hostname. Dns?
            try {
                OperationContext^ opcontext = OperationContext::Current;
			    MessageProperties^ messageProperties 
				    = opcontext->IncomingMessageProperties;
			    RemoteEndpointMessageProperty^ endpointProperty =
				    (RemoteEndpointMessageProperty^)messageProperties[RemoteEndpointMessageProperty::Name];
			    _clientHostname = System::Net::Dns::GetHostEntry(endpointProperty->Address)->HostName;
                
			    if (!IsDomainPartOfHostnameInConfigurationSystem())
                {
				    int index = _clientHostname->IndexOf(".");
                    if(index > 0)
					    _clientHostname = _clientHostname->Substring(0,index);
                }
            }
            catch(System::Exception^)
            {
                _clientHostname = System::String::Empty;
            }

	}
		
	SettingsPropertyValueCollection^ ConfigurationSystemProvider::GetPropertyValues(SettingsContext^ context, 
															SettingsPropertyCollection^ collection)
	{
		if(!_wrapper || !collection)
			return nullptr;

		List<System::String^>^ filter = gcnew List<System::String^>(collection->Count);
		for each(SettingsProperty^ prop in collection)
		{
			filter->Add(prop->Name->ToUpper());
		}
		
		Dictionary<System::String^, ConfigurationValue>^ configResult;
		
		System::String^ username=_username;
		System::String^ host;		
		System::String^ progname; // Uh. How do we pass this in? Do we need it?

		// Get values from configserver
		if(!_wrapper->GetConfigurationWithGroup(username, host, progname, filter, configResult))
			return nullptr;
		
		// Create properties to return
		SettingsPropertyValueCollection^ retval = gcnew SettingsPropertyValueCollection();	
		for each(SettingsProperty^ prop in collection)
		{
			SettingsPropertyValue^ val = gcnew SettingsPropertyValue(prop);
			val->IsDirty = false;
			val->SerializedValue = configResult[prop->Name].Value;
			retval->Add(val);
		}
		
		return retval;
	}
		
	void ConfigurationSystemProvider::SetPropertyValues(SettingsContext^ context,
										SettingsPropertyValueCollection^ collection)
	{
		throw gcnew System::NotImplementedException("Writing to configuration system not supported");
	}

}
