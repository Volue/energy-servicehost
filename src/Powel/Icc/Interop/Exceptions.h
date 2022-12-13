#pragma once

#using <mscorlib.dll>
using namespace System;

class UnmanagedIccAccessException
{
};

namespace Powel
{
	namespace Icc
	{
		namespace Interop
		{
			public ref class ConfigurationException : public ApplicationException
			{
			public:
				ConfigurationException(void) : ApplicationException() { }
				ConfigurationException(String^ message) : ApplicationException(message) { }
			};

			public ref class DatabaseNotReadyException : public ApplicationException
			{
			public:
				DatabaseNotReadyException(void) : ApplicationException() { }
				DatabaseNotReadyException(String^ message) : ApplicationException(message) { }
				DatabaseNotReadyException(String^ message, String^ connectionString) : ApplicationException(message)
				{ 
					this->ConnectionString = connectionString;
				}
				String^ ConnectionString;
			};

			public ref class LogonCancelledException : public ApplicationException
			{
			public:
				LogonCancelledException(void) : ApplicationException() { }
			};
		}
	}
}