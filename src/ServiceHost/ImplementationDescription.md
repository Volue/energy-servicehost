# Permissions and Security

The ServiceHost needs to run as a user with elevated permissions in order to get configuration settings from Powel configuration system.
- Define the user running Powel Service Host in the group Distributed COM Users
- Give this group DCOM permissions for Local Activation:
- From cmd as Administrator:

```
dcomcnfg
  + Component Services
    + Computers
      - My computer
        Propterties
          Com Security
            Access Permissions, Edit Default
              Add Group: Distributed COM Users
              + Local Access
            Launch and Activation Permissions: Edit Default
              Add Group: Distributed COM Users
              + Local Launch
              + Local Activation
```

# Implementation description for PowelServiceHost.

The ServiceHost implements a host for several services. The current version implements the following services:
- DataExchangeManagerService
- ImportApplicationManagerService
- AzureBusDataExchangeManagerService
- EcpAmqpDataExchangeManagerService
- [IccpDataExchangeManagerService][1]

The service hosts many services which are loaded when they are defined in the App.config file's serviceIterationClasses section:

```
  <serviceIterationClasses>
    <add value="Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService.ImportApplicationManagerService, ImportApplicationManagerService" />
    <add value="Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.DataExchangeManagerService, DataExchangeManagerService" />
    <add value="Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.AzureBusDataExchangeManagerService, AzureBusDataExchangeManagerService" />
    <add value="Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.EcpAmqpDataExchangeManagerService, EcpAmqpDataExchangeManagerService" />
    <add value="Powel.Icc.Messaging.IccpDataExchangeManager.IccpDataExchangeManagerService.IccpDataExchangeManagerService, IccpDataExchangeManagerService" />
  </serviceIterationClasses>
```

These elements are loaded in the method `ServiceHost.LoadIterationClasses()`. This method also registers needed classes with DependencyInjection 
where different modules has their own separate child container for its dependent classes.

The dependencies for each module is defined in each module. This is called from `ServiceDependencies.RegisterContainer()`:

    // IccpDataExchangeManagerService
    if (Convert.ToBoolean(ConfigurationManager.AppSettings["IccpDataExchangeManager.Enabled"] ?? "false"))
    {
        _childContainers.Add(typeof(IccpDataExchangeManagerService).Name, RegisterIccpDataExchangeManagerServiceContainer(_container.CreateChildContainer()));
    }

Later in the LoadIterationClasses function the service's Service class, e.g. IccpDataExchangeManagerService is Resolved. The constructor for the service class 
takes a factory parameter. This factory creates the modules (import, export, status etc) which the service manages. The modules subclasses IDataExchangeModule 
and the factory instantiate all modules in the UnityContainer which subclasses this interface.

For some of the implemented modules they define their module registering class as `internal`. Then you must define extended access in `AssemblyInfoProject.cs`.

> [assembly: InternalsVisibleTo("ServiceHost")]

For detailed description of the modules, see their `ImplementationDescription.md`:

## General description of how the services work.

The services uses MSMQ as transport of the messages. Each service has a triplet of MSMQ queues for prioritization of messages. They are named 
ICC_\<service\>_\<priority\> where \<priority\> is one of HIGH, NORMAL and LOW.

MSMQ has a maximum message length of 4 Mb. To handle messages larger than this, an intermediate storage for the payload is used. The `DataExchangeMessageBase` therefore has implemented 
a claim handler which stores the payload on an intermediate storage and replaces the payload with a claim ticket. The intermediate storage can be one of:
- File storage
- Database storage
- Azure storagebus.

### Import modules

The import modules implements different interfaces, receives data and wraps the data into a `DataExchangeImportMessage` with metadata to assist the 
`ImportApplicationManagerLogic` to handle the message correctly. The `DataExchangeImportMessage` is stored on MSMQ queues with the name ICC_IMPORT_\<priority\> 
which is handled by the `ImportApplicationManagerService`. 

Some import modules accesses the `ImportApplicationManagerLogic` directly by the `IImportApplication` interface and starts the import application directly from the 
import module. This overrides the prioritization included in sending the message to the `ImportApplicationManagerService` through MSMQ. But it secures the 
message handling process to execute without any possible message loss in the intermediate storing of the message on the ICC_IMPORT_* queues. This requires that 
the source of the message has a persistence so the message data is not lost there until the message download is committed.

### Import Application Manager Service

This service reads `DataExchangeImportMessage` messages from the ICC_IMPORT_<priority> queues in the prioritized order, HIGH, NORMAL and LOW. Then the 
`ImportApplicationManagerLogic` is used to handle the message and its payload with the correct import application.

#### `ImportApplicationManagerLogic`

This logic interfaces a set of runners which imports different messages. The input is a `DataExchangeImportMessage`. Some of the metadata elements are used to route
the payload to the correct runner. Currently this set of runners are implemented:
- Ancitra import through NServiceBus
- Import EDI
- Import Trade
- Import GS2
- Import of prices and trade
- Import PRODAT
- ServiceBus for TSS service
- Import UTILTS
- XML EL Import

### Export modules

The export modules reads `DataExchangeExportMessage` from the MSMQ with the name ICC_EXPORT_\<service\>_\<priority\> in the prioritized order, HIGH, NORMAL and LOW. 
Transforms the payload into a message type handled by the interface implemented by this service and sends it.

The utility program `ExpressExportConnector.exe` can be used for writing some payload to the correct ICC_EXPORT_\<service\>_\<priority\> queue by piping the payload
and supplying parameters defining which service and what priority shall be used.

[1]: ../../../Messaging/DataExchangeManager/IccpDataExchangeManagerService/ImplementationDescription.md
