using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.Threading.Tasks;
using Powel.ConfigurationSystem.Cli;
using log4net;
using Powel.Icc.Services.DataContracts.HealthCheck;
using Powel.Icc.Services.ServiceContracts;
using System.Diagnostics;

namespace Powel.ConfigurationSystem.Service
{
    public class ConfigurationReaderService : IConfigurationReaderService, IDisposable, IHealthCheckService
    {
        private static readonly ILog Log = LogManager.GetLogger(typeof(ConfigurationReaderService));
        private readonly ConfigurationSystemReaderCLIWrapper _reader;

        public ConfigurationReaderService()
        {
            Log.Warn("ConfigurationReaderService(). Enter\n\t");
            _reader = new ConfigurationSystemReaderCLIWrapper();
            Log.Warn("ConfigurationReaderService(). Exit");
        }

        public List<string> GetConfiguration(string username, string computerName, string progName)
        {
            Log.Warn(
                $"ConfigurationReaderService.GetConfiguration. Enter\n\tusername = '{username}'" +
                $", computerName = '{computerName}', progName = '{progName}'");
            var isOk = _reader.GetConfiguration(username, computerName, progName, out var configResult);
            //If isOk == false, GetConfiguration returns COM error description in configResult.First().Value:
            Log.Warn($"ConfigurationReaderService.GetConfiguration. Exit\n\tisOk = {isOk}" +
                     $", configResult.Count = {configResult.Count}, First = {configResult.First()}");
            return configResult;
        }

        public List<string> GetEncryptedParameters()
        {
            Log.Warn(
                $"ConfigurationReaderService.GetEncryptedParameters.");
            var isOk = _reader.GetEncryptedParameters(out var configResult);
            //If isOk == false, GetEncryptedParameters returns COM error description in configResult.First().Value:
            Log.Warn($"ConfigurationReaderService.GetEncryptedParameters. Exit\n\tisOk = {isOk}" +
                     $", configResult.Count = {configResult.Count}, First = {configResult.First()}");
            return configResult;
        }

        public Dictionary<string, string> GetExpandedConfiguration(string username, string computerName, string progName)
        {
            Log.Warn(
                $"ConfigurationReaderService.GetExpandedConfiguration. Enter\n\tusername = '{username}'" +
                $", computerName = '{computerName}', progName = '{progName}'");
            var isOk = _reader.GetExpandedConfiguration(username, computerName, progName, out var configResult);

            //If isOk == false, GetExpandedConfiguration returns COM error description in configResult.First().Value:
            Log.Warn($"ConfigurationReaderService.GetExpandedConfiguration. Exit\n\tisOk = {isOk}" +
                     $", configResult.Count = {configResult.Count}, First = {configResult.First()}");

            return configResult;
        }

        public Dictionary<string, string> GetExpandedConfigurationByRole(int roleKey, string computername, string progName)
        {
            Log.DebugFormat("Enter, roleKey='{0}' computername='{1}', progName='{2}'", roleKey, computername, progName);
            Dictionary<string, string> configResult;
            var isOk = _reader.GetExpandedConfigurationByRole(roleKey, computername, progName, out configResult);
            if (!isOk)
            {
                configResult = null;
            }
            Log.DebugFormat("Exit. ok: {0}, nEntries: {1}", isOk, isOk ? configResult.Count : 0);
            return configResult;
        }

        public Dictionary<string, string> GetRolesForUser(string username)
        {
            Log.Warn($"Enter\n\tusername = '{username}'");
            var isOk = _reader.GetRolesForUser(username, out var configResult);
            //If isOk == false, GetConfiguration returns COM error description in configResult.First().Value:
            Log.Warn($"Exit\n\tisOk = {isOk}, configResult.Count = {configResult.Count}, First = { configResult.FirstOrDefault()}");

            return configResult;
        }

        public Dictionary<string, string> GetRolesForUserAdGroups(string username)
        {

            Log.Warn($"Enter\n\tusername = '{username}'");
            var isOk = _reader.GetRolesForUserActiveDirectoryGroups(username, out var configResult);
            //If isOk == false, GetConfiguration returns COM error description in configResult.First().Value:
            Log.Warn($"Exit\n\tisOk = {isOk}, configResult.Count = {configResult.Count}, First = {configResult.FirstOrDefault()}");

            return configResult;
        }

        public void Abort()
        {
            // This class doesn't need to implement Abort. Only ConfigurationReaderServiceClient needs to.
        }

        public Dictionary<int, string> GetRoles()
        {
            Dictionary<int,Role> roles = null;
            var isOk = _reader.GetRoles(out roles);
            if (!isOk)
            {
                roles = null;
            }

            if (Log.IsDebugEnabled)
            {
                Log.DebugFormat("Exit. ok: {0}, nEntries={1}, first={2},{3}",
                                isOk,
                                isOk ? roles.Count : 0,
                                isOk && roles.Count > 0
                                    ? roles.First().Key.ToString()
                                    : string.Empty, 
                                isOk && roles.Count > 0
                                    ? roles.First().Value.Name.ToString()
                                    : string.Empty);
            }

            return roles.ToDictionary(kp => kp.Key, kp => kp.Value.Name);
        }

        public Dictionary<int, string> GetRolesFromCache()
        {
            Dictionary<int, string> roles = null;
            var isOk = _reader.GetRoleKeysFromCache(ref roles);
            if (!isOk)
            {
                roles = null;
            }

            if (Log.IsDebugEnabled)
            {
                Log.DebugFormat("Exit. ok: {0}, nEntries={1}, first={2},{3}",
                                isOk,
                                isOk ? roles.Count : 0,
                                (isOk && roles.Count > 0)
                                    ? roles.First().Key.ToString()
                                    : string.Empty,
                                (isOk && roles.Count > 0)
                                    ? roles.First().Value.ToString()
                                    : string.Empty);
            }

            return roles;
        }

        #region IDisposable
        
        private bool m_IsDisposed;

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            Log.Warn($"Dispose. Enter. isDisposing = {isDisposing}, m_IsDisposed = {m_IsDisposed}");
            if (!m_IsDisposed)
            {
                if (isDisposing)
                {
                    // Cleanup managed objects by calling their Dispose():
                    _reader.Dispose();
                }

                // Cleanup unmanaged objects
            }

            m_IsDisposed = true;
            Log.Warn("Dispose. Exit");
        }

        ~ConfigurationReaderService()
        {
            Dispose(false);
        }

        public Task<HealthCheckResult> CheckHealthAsync()
        {
            return Task.FromResult(HealthCheckResult.Healthy(OperationContext.Current.IncomingMessageHeaders.To));
        }

        #endregion IDisposable 

    }
}
