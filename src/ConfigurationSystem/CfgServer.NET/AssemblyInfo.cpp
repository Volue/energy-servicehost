#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("ConfigurationSystemCLI")];
[assembly:AssemblyDescriptionAttribute(".NET wrapper for Powel Configuration Server")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Powel AS")];
[assembly:AssemblyProductAttribute("Powel SmG/MDMS")];
[assembly:AssemblyCopyrightAttribute("Copyright © 2018 Powel AS")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly: AssemblyFileVersion("99.0.0.0")]
[assembly: AssemblyVersion("99.0.0.0")]

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

// Obsolete:
//[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];

// AON 18.06.2012:
// Moved the signing from project attribute to the AssemblyInfo file during
// conversion to VS2010 due to bug in VS2010 SP1 for signing of Managed C++ projects.
// Typical build error:
//     error MSB4044: The "WriteLinesToFile" task was not given a value for
//     the required parameter "File".
[assembly:AssemblyKeyFileAttribute("..\\..\\keyPair.snk")];
