#pragma once

/***************************************************************************************
* .NET SettingsProvider for the Powel Configuration System.
*
* See http://msdn.microsoft.com/en-us/library/system.configuration.settingsprovider.aspx
*
* TODO: "SetPropertyValues" not implemented
* 
*
****************************************************************************************/

using namespace System::Configuration;
namespace ConfigurationSystem{
	public ref class ConfigurationSystemProvider : public SettingsProvider
	{
	private:
		ConfigurationSystemReaderCLIWrapper^ _wrapper;
		System::String^ _username;
		System::String^ _clientHostname;

		void GetUserAndHost();
		bool IsDomainPartOfUsernameInConfigurationSystem();
		bool IsDomainPartOfHostnameInConfigurationSystem();
		void ConfigurationSystemProvider::GetWcfClientHostName();


	public:
		
		virtual void Initialize (System::String^ name, 
									System::Collections::Specialized::NameValueCollection^ config) override;

		
		virtual SettingsPropertyValueCollection^ GetPropertyValues(SettingsContext^ context, 
															SettingsPropertyCollection^ collection) override;
		
		virtual void SetPropertyValues(SettingsContext^ context,
										SettingsPropertyValueCollection^ collection) override;

		virtual property System::String^ ApplicationName 
		{
			System::String^ get() override 
			{
				// TODO: Return what what and why?
				return System::Reflection::Assembly::GetExecutingAssembly()->GetName()->Name;
			}
			void set (System::String^ value) override
			{
				// Do nothing
				return;
			}
		}

	};

}
