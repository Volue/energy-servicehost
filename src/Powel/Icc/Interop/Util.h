#pragma once
//#include "PDTime.h"
#include <string>
#using <mscorlib.dll>
using namespace System;

typedef __int64 pdtime;
const pdtime MAXPDTIME = 0x7FFFFFFFFFFFFFFFL;

class Util
{
public:
	static std::string MarshalString(String^ s);
	static pdtime DateTimeTopdtime(DateTime dt);
	static System::DateTime pdtimeToDateTime(pdtime pdt);
};
