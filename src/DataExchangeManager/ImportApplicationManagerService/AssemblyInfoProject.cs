using System.Runtime.CompilerServices;

// the test project must be able to see our internals
[assembly: InternalsVisibleTo("ServiceHost")]
[assembly: InternalsVisibleTo("IntegrationTests")]