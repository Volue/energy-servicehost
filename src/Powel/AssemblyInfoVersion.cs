//
// All three version-related assembly attributes
// (AssemblyVersion, AssemblyFileVersion and AssemblyInformationalVersion).
//
// Add this file as link to each project, and remove the included attributes from the
// projects' own AssemblyInfo.cs file.
//
// Numeric version attributes consists of the following four values:
//    - Major Version
//    - Minor Version
//    - Build Number
//    - Revision
// in format "major.minor.build.revision" (e.g. "7.2.4.0").
// You can specify all the values, or you can default the Revision and Build Numbers 
// by using the special symbol '*' (e.g. "1.0.*").
//
// There are 3 different assembly attributes for version information:
// - AssemblyVersion
//    - This is an assembly identity attribute.
//    - The internal version number.
//    - This is the one version number used by CLR, during build and at
//      runtime to locate, link and load the assemblies.
//      When you add reference to any assembly in your project, it is
//      this version number which gets embedded.
//    - Does not correspond to any native Win32 attributes, and is not shown
//      in file properties in Windows.
//    - It must be a numeric version string as described above.
// - AssemblyFileVersion
//    - This is an informational attribute.
//    - The official version number.
//    - This version number will be used for the Win32 file version resource.
//    - Shown as "File version" in file properties in Windows.
//    - Not used by .NET framework or runtime for referencing, but it is used in
//      some situations like for deciding whether to replace an existing assembly
//      in GAC when attempting to install one that already exists (same identity,
//      including same assembly version).
//    - Can be set to a generic string value, not necessarily as a numeric version
//      string as described above - but it is recommended to use the numeric format
//      (an compiler warning AL1053 will be thrown if not).
//      The compiler will set the given string as the "FileVersion" string value
//      in the language-dependent section (StringFileInfo block) of the Win32 file
//      version resource. But it will also attempt to parse it into four individual
//      version number items according to the numeric format described above, and
//      write this as the binary version number for the file in the language-independent
//      section (fixed-info) of the Win32 version resource. This parsing will be aborted
//      when an unknown symbol (e.g. alphabetic character) is discovered, setting any
//      remaining binary version numbers to 0. In Windows Vista, Server 2008 and
//      newer it is the language-independent file version that is shown in the file
//      properties, so for this information to be correct the version string must
//      be according to the numeric version string format described above. Alternatively
//      a proper numeric version string can be used at the beginning of the string
//      but followed by additional textual information like "RC", "Debug" etc, in this
//      case the numeric version will be set to the language-independent section and the
//      entire string to the language-dependent section.
// - AssemblyInformationalVersion
//    - Also an informational attribute.
//    - Shown as "Product version" in file properties in Windows, usually used
//      to denote the version of the entire product that the assembly is part of.
//    - Not used in CLR at all.
//    - Is also a generic string value, and as with AssemblyFileVersion the string
//      will be stored unchanged in the language-dependent section but parsed into
//      numeric format and set in the language-independent section of the Win32
//      file version resource. But in contrast to AssemblyFileVersion, in
//      Windows Vista, Server 2008 and newer it is the language-dependent product
//      version that is shown in the file properties, so therefore it is more
//      acceptable to use a human readable string notation for this value.
//
// The checked-in files should remain unchanged with fixed version numbers
// "99.0.0.0", to avoid any assembly reference problems due to version mismatch in
// development builds. Release builds changes these according to release policy,
// using the SetVersion.msbuild script.
//
using System.Reflection;
[assembly: AssemblyVersion("99.0.0.0")]
[assembly: AssemblyFileVersion("99.0.0.0")]
[assembly: AssemblyInformationalVersion("99.0.0 (Debug)")]
