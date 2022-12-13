#include "StdAfx.h"
#include "Configuration.h"
#include "Exceptions.h"
//TOSA #include "Profiler.h"
#include "PDLog4Cxx.h"
#include "Enums.h"

extern "C" { 
int icc_cleanup_env();
char ** icc_env_ptr();
char **icc_env_setup_internal(char *key, int *count) ;
int icc_putenv(char *);
};

#using <mscorlib.dll>
using namespace System;
using namespace System::Collections;
using namespace System::Runtime::InteropServices;
using namespace Powel;
using namespace Powel::Icc::Interop;

const auto szAppName = "ConfigurationReader";

IccConfigurationReader::IccConfigurationReader(void)
{
    //throw std::exception("TEST");
    try
    {
        PDLog4Cxx::Configure(szAppName);
        //TOSA Profiler::StartProfiling(szAppName, Profiler::LEVEL_TRACE);
    }
    catch (std::exception ex)
    {
        // Just for debugging:
        auto dummy = ex.what();
    }
    int count;
	char** env = icc_env_setup_internal(NULL, &count);

	if (count > 0)
	{
		IccConfigurationReader::hashtable = gcnew Hashtable();

		for (int i=0; i<count; i++)
		{
			String^ s = gcnew String(env[i]);
			String^ delimiter = "=";
			array<String^> ^ attributePair = s->Split(delimiter->ToCharArray(),2);
			attributePair[0] = attributePair[0]->ToUpper();

			if (!hashtable->ContainsKey(attributePair[0]))
				hashtable->Add(attributePair[0], attributePair[1]);
		}
	}

	// If we cleanup, things will only work for the first constructed object.
	// We are not getting serious memory leakage even without the cleanup anyway.
	//
	// icc_cleanup_env();
}

IccConfigurationReader::~IccConfigurationReader(void)
{
    //TOSA Profiler::StopProfiling(szAppName, true);
}

String^ IccConfigurationReader::ReadString(String^ key)
{
	if (hashtable == nullptr)
		throw gcnew InvalidOperationException("IccConfigurationReader has not been initialized.");

	key = key->ToUpper();

	if (hashtable->ContainsKey(key))
		return safe_cast<String^>(hashtable[key]);
	else
		throw gcnew ConfigurationException(String::Format("The configuration parameter '{0}' is unknown.", key));
}

int IccConfigurationReader::ReadInt32(String^ key)
{
	return Convert::ToInt32(ReadString(key));
}

bool IccConfigurationReader::ReadBoolean(String^ key)
{
	String^ s = ReadString(key);
	return s && s->Length > 0 && !s->ToLower()->Equals("false") && !s->Equals("0");
}

String^ IccConfigurationReader::ReadOptionalString(String^ key, String^ defaultValue)
{
	try
	{
		return ReadString(key);
	}
	catch (const ConfigurationException^ Ex)
	{
		return defaultValue;
	}
#ifdef _NDEBUG
	catch (...) {
		return defaultValue;
	}
#endif
}

int IccConfigurationReader::ReadOptionalInt32(String^ key, int defaultValue)
{
	try
	{
		return ReadInt32(key);
	}
	catch (const ConfigurationException^ Ex)
	{
		return defaultValue;
	}
#ifdef _NDEBUG
	catch (...)
	{
		return defaultValue;
	}
#endif
}

bool IccConfigurationReader::ReadOptionalBoolean(String^ key, bool defaultValue)
{
	try
	{
		return ReadBoolean(key);
	}
	catch (const ConfigurationException^ Ex)
	{
		return defaultValue;
	}
#ifdef _NDEBUG
	catch (...)
	{
		return defaultValue;
	}
#endif // _NDEBUG
}

array<String^> ^ IccConfigurationReader::Keys::get()
{
	array<String^> ^ keys = gcnew array<String^>(hashtable->Count);
	hashtable->Keys->CopyTo(keys, 0);
	return keys;
}
