using System;
using System.Collections.Generic;
using Microsoft.Win32;
using Powel.ConfigurationSystem.Cli;
using log4net;

namespace Powel.ConfigurationSystem.Service
{
    public class ConfigurationManagerService
    {

        private static readonly ILog Log = LogManager.GetLogger(typeof(ConfigurationReaderService));
        private ConfigurationSystemManagerCLIWrapper _manager;
        private const string RegistryPath = @"Software\Powel";

        private string _configUser;
        private string _configPwd;
        private string _configServer;

        private void InitManager()
        {
            _manager = new ConfigurationSystemManagerCLIWrapper(_configUser, _configPwd, _configServer);
        }

        private void GetConfigAuthFromRegistry()
        {
            RegistryKey localMachine = Registry.LocalMachine;
            RegistryKey credentialKey = localMachine.OpenSubKey(RegistryPath, false);
            if(credentialKey == null)
                throw new KeyNotFoundException("Unable to open registry path " +  RegistryPath + ". Can't log on to Configuration Manager.");

            _configUser = Convert.ToString(credentialKey.GetValue("ICC_CFGUSER"),
                System.Threading.Thread.CurrentThread.CurrentCulture);
            _configPwd = Convert.ToString(credentialKey.GetValue("ICC_CFGPASSWD"),
                System.Threading.Thread.CurrentThread.CurrentCulture);
            _configServer = Convert.ToString(credentialKey.GetValue("ICC_CFGSERVER"),
                System.Threading.Thread.CurrentThread.CurrentCulture);
        }

        /// <summary>
        /// Use this constructor if you want to use log into the configureation managet with info from registry
        /// </summary>
        public ConfigurationManagerService()
        {
            GetConfigAuthFromRegistry();
            InitManager();
        }

        public ConfigurationManagerService(string user, string password, string server)
        {
            _configUser = user;
            _configPwd = password;
            _configServer = server;

            InitManager();
        }

        public Dictionary<String, Cli.ConfigurationParameter> GetParametersForGroup(string group)
        {
            Log.DebugFormat("Enter, group='{0}'", group);
            Dictionary<string, Cli.ConfigurationParameter> configResult;
            var isOk = _manager.GetParametersForGroup(group, out configResult);
            if (!isOk)
            {
                configResult = null;
            }
            Log.DebugFormat("Exit. ok: {0}, number of entries: {1}", isOk, isOk ? configResult.Count : 0);
            return configResult;
        }
    }
}
