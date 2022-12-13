//#include "stdafx.h"

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
[assembly:AssemblyTitleAttribute("OTL.NET")];
[assembly:AssemblyDescriptionAttribute("OTL for .NET wrapper for OTL by Sergei Kuchin, 1996,2008")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Powel AS")];
[assembly:AssemblyProductAttribute("Powel SmG/MDMS")];
[assembly:AssemblyCopyrightAttribute("Copyright © 2018 Powel AS")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

// Strong name and no delayed signing
//[assembly: AssemblyDelaySign(true)];
[assembly: AssemblyVersion("99.0.0.0")]
[assembly: AssemblyFileVersion("99.0.0.0")]
[assembly:ComVisible(false)];
[assembly:CLSCompliantAttribute(true)];// really needed to avoid warnings.
// Obsolete:
//[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
// SIH: ELIN uses separate key.[assembly:AssemblyKeyName("")];
// AND: we have to use commandline signing to avoid
// mt-tool to tamper with a signed file
//[assembly: AssemblyKeyFile("OTL.NET.snk")];

// AON 18.06.2012:
// Moved the signing from project attribute to the AssemblyInfo file during
// conversion to VS2010 due to bug in VS2010 SP1 for signing of Managed C++ projects.
// Typical build error:
//     error MSB4044: The "WriteLinesToFile" task was not given a value for
//     the required parameter "File".
// TODO: But what about the comment from SIH regarding ELIN above!??
[assembly:AssemblyKeyFileAttribute("..\\Powel.NET\\keyPair.snk")];
