using System;
using System.Collections.Generic;
using System.ServiceModel;
using Powel.ConfigurationSystem.Cli;

namespace Powel.ConfigurationSystem.Service
{
    [ServiceContract]
    public interface IConfigurationManagerService
    {
        [OperationContract]
        Dictionary<String, Cli.ConfigurationParameter> GetParametersForGroup(string group);
    }
}
