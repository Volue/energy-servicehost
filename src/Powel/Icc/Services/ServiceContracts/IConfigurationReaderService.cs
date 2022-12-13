using System.Collections.Generic;
using System.ServiceModel;

namespace Powel.Icc.Services.ServiceContracts
{
    /// <summary>
    /// A reader of configuration information
    /// </summary>
    [ServiceContract]
    public interface IConfigurationReaderService
    {
        /// <summary>
        /// Returns a List with full parameter-info. The configuration values are not 
        /// expanded, i.e. if one entry is "A=Alpha" and another is "B=%A%Beta"
        /// both entries are returned as is and not expanded 
        /// as by <see cref="GetExpandedConfiguration"/>.
        /// </summary>
        /// <param name="username">The user name for which to get the configuration</param>
        /// <param name="computerName">The computer name for which to get the confguration</param>
        /// <param name="programName">The program name for which to get the configuration. May be null.</param>
        /// <returns>A list of strings on the form "param=value". Returns null on errors, e.g. configuration errors.</returns>
        [OperationContract]
        List<string> GetConfiguration(string username, string computerName, string programName);

        /// <summary>
        /// Returns a List with names of encrypted parameters. 
        /// </summary>
        /// <returns>A list of strings. Returns null on errors, e.g. configuration errors.</returns>
        [OperationContract]
        List<string> GetEncryptedParameters();

        /// <summary>
        /// Returns a Dictionary with parameter name as key and ConfigurationValue-instances as value
        /// where parameters referring to other parameters are resolved,
        /// i.e. if one entry is "A=Alpha" and another is "B=%A%Beta" the
        /// value of B is "AlphaBeta".
        /// <p>The following entries are inserted into configResult:<ul>
        /// <li>USERNAME set to the parameter username</li>
        /// <li>COMPUTERNAME set to the parameter computername</li>
        /// </ul>
        /// </p>
        /// </summary>
        /// <param name="username">The user name for which to get the configuration</param>
        /// <param name="computerName">The computer name for which to get the confguration</param>
        /// <param name="programName">The program name for which to get the configuration. May be null.</param>
        /// <returns>A dictionary mapping keys to their values. Returns null on errors, e.g. configuration errors.</returns>
        [OperationContract]
        Dictionary<string, string> GetExpandedConfiguration(string username, string computerName, string programName);

        /// <summary>
        /// Gets the available roles for a user.
        /// </summary>
        /// <param name="username">The name of the user.</param>
        /// <returns>A dictionary mapping the role name to a username to send as input
        /// to e.g. <see cref="GetExpandedConfiguration"/> in order to get the configuration 
        /// for the role. Returns null on errors, e.g. configuration errors.
        /// </returns>
        [OperationContract]
        Dictionary<string, string> GetRolesForUser(string username);


        /// <summary>
        /// Gets the available roles for a user based on his ActiveDirectory roles.
        /// </summary>
        /// <param name="username">The name of the user.</param>
        /// <returns>A dictionary mapping the role name to a username to send as input
        /// to e.g. <see cref="GetExpandedConfiguration"/> in order to get the configuration 
        /// for the role. Returns null on errors, e.g. configuration errors.
        /// </returns>
        [OperationContract]
        Dictionary<string, string> GetRolesForUserAdGroups(string username);

        void Abort();
    }
}
