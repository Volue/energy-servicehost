using System.Reflection;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("ImportApplicationManagerLogic")]
[assembly: AssemblyDescription("Common module which implements the import application logic. Used by ImportApplicationManagerService, but also from various DataExchangeManagers if set up.")]

// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("d4d96a1e-9feb-4f64-9723-be4ee9a6069d")]

[assembly: log4net.Config.XmlConfigurator(Watch = true)]
