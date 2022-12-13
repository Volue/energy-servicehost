using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("NpAuctionDataExchangeManagerService")]
[assembly: AssemblyDescription("Data Exchange Manager for Nord Pool's Auction API")]

// Setting ComVisible to false makes the types in this assembly not visible
// to COM components.  If you need to access a type in this assembly from
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("2720dbba-b843-4961-b6a4-db92a478baac")]

[assembly: log4net.Config.XmlConfigurator(Watch = true)]

[assembly: InternalsVisibleTo("ServiceHost")]
[assembly: InternalsVisibleTo("NpAuctionService")]
//[assembly: InternalsVisibleTo("IccpDataExchangeManagerServiceTest")]
