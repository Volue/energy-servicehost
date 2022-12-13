#pragma once
#include <string>
#using <mscorlib.dll>

using namespace System;
using namespace System::Collections;
using namespace System::Runtime::InteropServices;

namespace Powel
{
	namespace Icc
	{
		namespace Interop
		{
			public interface class IConfigurationReader
			{
				String^ ReadString(String^ key);
				int ReadInt32(String^ key);
				bool ReadBoolean(String^ key);
				String^ ReadOptionalString(String^ key, String^ defaultValue);
				int ReadOptionalInt32(String^ key, int defaultValue);
				bool ReadOptionalBoolean(String^ key, bool defaultValue);
			};

			public ref class IccConfigurationReader : public IConfigurationReader
			{
			public:
				IccConfigurationReader(void);
				virtual String^ ReadString(String^ key);
				virtual int ReadInt32(String^ key);
				virtual bool ReadBoolean(String^ key);
				virtual String^ ReadOptionalString(String^ key, String^ defaultValue);
				virtual int ReadOptionalInt32(String^ key, int defaultValue);
				virtual bool ReadOptionalBoolean(String^ key, bool defaultValue);
				property array<String^> ^ Keys { array<String^> ^ get(); }
			private:
				~IccConfigurationReader(void);

				static Hashtable^ hashtable;
			};
		}
	}
}
