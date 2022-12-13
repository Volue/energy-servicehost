//
// Common company-related assembly attributes.
//
// Add this file as link to each project, and remove the included attributes from the
// projects' own AssemblyInfo.cs file.
//
// Most of the attributes here are informational only.
//
using System.Reflection;
//using System.Runtime.CompilerServices;

//
// Informational attributes
//
[assembly: AssemblyCompany("Volue ASA")]
[assembly: AssemblyProduct("Energy Smart Generation")]
[assembly: AssemblyCopyright("Copyright © 2022 Volue ASA")]
[assembly: AssemblyTrademark("")]

//
// Assembly manifest attributes
// (AssemblyTitle and AssemblyDescription should be set by the projects' own AssemblyInfo.cs files). 
//
#if DEBUG
[assembly: AssemblyConfiguration("Debug")]
#else
[assembly: AssemblyConfiguration("Release")]
#endif

//
// Assembly identity attributes
//
[assembly: AssemblyCulture("")]
