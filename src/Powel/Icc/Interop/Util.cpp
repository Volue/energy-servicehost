#include "StdAfx.h"
#include ".\util.h"

using namespace std;

#using <mscorlib.dll>
using namespace System;
using namespace System::Runtime::InteropServices;

string Util::MarshalString(String^ s)
{
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	string os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
	return os;
}

pdtime Util::DateTimeTopdtime(DateTime dt)
{
	return (pdtime)(dt - DateTime(1970, 1, 1)).TotalSeconds;
}

DateTime Util::pdtimeToDateTime(pdtime pdt)
{
	return DateTime(1970, 1, 1).AddTicks(pdt * 10000000);
}
