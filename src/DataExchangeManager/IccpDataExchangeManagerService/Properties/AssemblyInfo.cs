using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("IccpDataExchangeManagerService")]
[assembly: AssemblyDescription("Separate data exchange manager for ICCP/TASE.2")]

// Setting ComVisible to false makes the types in this assembly not visible
// to COM components.  If you need to access a type in this assembly from
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("6f1035cd-77a0-4d7e-8120-52d6f7a56a3d")]

[assembly: log4net.Config.XmlConfigurator(Watch = true)]

[assembly: InternalsVisibleTo("ServiceHost")]
[assembly: InternalsVisibleTo("IccpDataExchangeManagerServiceTest")]
