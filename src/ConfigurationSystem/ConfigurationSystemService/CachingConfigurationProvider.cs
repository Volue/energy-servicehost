//SIV. The file is removed from 
//C:\Projects\ICCMDMS\icc\ConfigurationSystem\ConfigurationSystemService\Powel.ConfigurationSystemService.csproj
//because it is never used in production code.
//The test, CachingConfigurationProviderTest.cs, must be refactored to test logically the same class in
//c:\Projects\ICCMDMS\Powel.NET\CompositeWpf\WorkFlowInfrastructure\Configuration\CachingConfigurationProvider.cs
//See NimbusClient.sln/Powel.ConfigurationSystemServiceTest


//using System;
//using System.Collections.Generic;
//using System.Diagnostics;
//using System.ServiceModel;
//using log4net;
//using Powel.Icc.Services.ServiceContracts;
//using Powel.Icc.Services.Utilities;

//namespace Powel.ConfigurationSystem.Service
//{
//    /// <summary>
//    /// A caching configuration provider that reads configuration information and caches it.
//    /// The template parameter decides what implementation of reader will be used. It can
//    /// e.g. be a local reader or a service client that reads from a remote service.
//    /// </summary>
//    /// <typeparam name="T">An implementation of <see cref="IConfigurationReaderService"/></typeparam>
//    public class CachingConfigurationProvider<T> : IConfigurationProvider 
//        where T : IConfigurationReaderService, IDisposable, new()
//    {
//        private static readonly ILog m_Log = LogManager.GetLogger(typeof(CachingConfigurationProvider<T>));

//        protected readonly string m_Username;
//        protected readonly string m_Computername;
//        protected readonly string m_Programname;
//        protected Dictionary<string, string> m_StringDictionary;
//        protected Dictionary<string, bool> m_BoolDictionary;
//        protected Dictionary<string, int> m_IntDictionary;
//        protected Dictionary<string, double> m_DoubleDictionary;
//        protected Dictionary<string, string> m_AvailableRoles;

//        /// <summary>
//        /// <para>Creates a configuration provider given the configuration keys and the reader 
//        /// which does the actual reading from a source unknown to this class.
//        /// </para>
//        /// </summary>
//        /// <param name="username"></param>
//        /// <param name="computername"></param>
//        /// <param name="programname"></param>
//        public CachingConfigurationProvider(string username,
//            string computername,
//            string programname)
//        {
//            m_Log.DebugFormat("Enter, username: {0}, computername: {1}, programname: {2}", username, computername, programname);
//            m_Username = username;
//            m_Computername = computername;
//            m_Programname = programname;
//            m_Log.Debug("Exit");
//        }

//        public bool HasParameter(string parameter)
//        {
//            EnsureConfigurationLoaded();
//            return m_StringDictionary.ContainsKey(parameter);
//        }

//        public bool GetBool(string parameter)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}", parameter);
//            EnsureConfigurationLoaded();
//            bool value;
//            if (m_BoolDictionary.ContainsKey(parameter))
//            {
//                value = m_BoolDictionary[parameter];
//            }
//            else
//            {
//                if (m_StringDictionary.ContainsKey(parameter))
//                {
//                    string stringValue = m_StringDictionary[parameter];
//                    ValidateBool(parameter, stringValue);
//                    value = stringValue.Equals("true", StringComparison.InvariantCultureIgnoreCase);
//                    m_BoolDictionary.Add(parameter, value);
//                }
//                else
//                {
//                    throw new ArgumentException("Parameter not present in configuration: " + parameter);
//                }
//            }
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public bool GetBool(string parameter, bool defaltValue)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}, defaultValue: {1}", parameter, defaltValue);
//            var value = HasParameter(parameter) ? GetBool(parameter) : defaltValue;
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public double GetDouble(string parameter)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}", parameter);
//            EnsureConfigurationLoaded();
//            double value;
//            if (m_DoubleDictionary.ContainsKey(parameter))
//            {
//                value = m_DoubleDictionary[parameter];
//            }
//            else
//            {
//                if (m_StringDictionary.ContainsKey(parameter))
//                {
//                    string stringValue = m_StringDictionary[parameter];
//                    bool ok = double.TryParse(stringValue, out value);
//                    if (ok)
//                    {
//                        m_DoubleDictionary.Add(parameter, value);
//                    }
//                    else
//                    {
//                        throw new Exception(string.Format(
//                            "The parameter '{0}' has an illegal value '{1}'. Expected a floating point number.",
//                            parameter,
//                            value));
//                    }
//                }
//                else
//                {
//                    throw new ArgumentException("Parameter not present in configuration: " + parameter);
//                }
//            }
//            return value;
//        }

//        public double GetDouble(string parameter, double defaltValue)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}, defaultValue: {1}", parameter, defaltValue);
//            var value = HasParameter(parameter) ? GetDouble(parameter) : defaltValue;
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public int GetInt(string parameter)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}", parameter);
//            EnsureConfigurationLoaded();
//            int value;
//            if (m_IntDictionary.ContainsKey(parameter))
//            {
//                value = m_IntDictionary[parameter];
//            }
//            else
//            {
//                if (m_StringDictionary.ContainsKey(parameter))
//                {
//                    string stringValue = m_StringDictionary[parameter];
//                    bool ok = int.TryParse(stringValue, out value);
//                    if (ok)
//                    {
//                        m_IntDictionary.Add(parameter, value);
//                    }
//                    else
//                    {
//                        throw new Exception(string.Format(
//                            "The parameter '{0}' has an illegal value '{1}'. Expected an integer.",
//                            parameter,
//                            value));
//                    }
//                }
//                else
//                {
//                    throw new ArgumentException("Parameter not present in configuration: " + parameter);
//                }
//            }
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public int GetInt(string parameter, int defaltValue)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}, defaultValue: {1}", parameter, defaltValue);
//            var value = HasParameter(parameter) ? GetInt(parameter) : defaltValue;
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public string GetPath(string parameter)
//        {
//            return GetString(parameter);
//        }

//        public string GetPath(string parameter, string defaltValue)
//        {
//            m_Log.Debug("Enter");
//            var value = HasParameter(parameter) ? GetPath(parameter) : defaltValue;
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public string GetString(string parameter)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}", parameter);
//            EnsureConfigurationLoaded();
//            string value;
//            if (m_StringDictionary.ContainsKey(parameter))
//            {
//                value = m_StringDictionary[parameter];
//            }
//            else
//            {
//                throw new ArgumentException("Parameter not present in configuration: " + parameter);
//            }

//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public bool TryGetString(string parameterName, out string parameterValue)
//        {
//            EnsureConfigurationLoaded();

//            return m_StringDictionary.TryGetValue(parameterName, out parameterValue);
//        }

//        public string GetString(string parameter, string defaltValue)
//        {
//            m_Log.DebugFormat("Enter, parameter: {0}, defaultValue: {1}", parameter, defaltValue);
//            var value = HasParameter(parameter) ? GetString(parameter) : defaltValue;
//            m_Log.DebugFormat("Exit. Returning {0}", value);
//            return value;
//        }

//        public IDictionary<string, string> GetAvailableRoles()
//        {
//            if (m_AvailableRoles == null)
//            {
//                lock (this)
//                {
//                    if (m_AvailableRoles == null)
//                    {
//                        using (var reader = new T())
//                        {
//                            try
//                            {
//                                m_Log.Debug("Getting available roles from server");
//                                m_AvailableRoles = reader.GetRolesForUser(m_Username);
//                            }
//                            catch (Exception ex)
//                            {
//                                reader.Abort();
//                                m_Log.Error("The configuration reader service returned an error: " + ex.Message);
//                                throw;
//                            }
//                        }
//                    }
//                }
//            }

//            //
//            // Return a new dictionary so that a client can't modify this object's data.
//            //
//            return m_AvailableRoles == null ?
//                null : new Dictionary<string, string>(m_AvailableRoles);
//        }

//        private void EnsureConfigurationLoaded()
//        {
//            if (m_StringDictionary == null)
//            {
//                lock (this)
//                {
//                    if (m_StringDictionary == null)
//                    {
//                        try
//                        {
//                            var reader = new T();

//                            m_Log.Debug("Getting configuration from server");

//                            var expandedConfig = reader.GetExpandedConfiguration(m_Username, m_Computername, m_Programname);

//                            if (expandedConfig == null)
//                            {
//                                m_Log.Error("Configuration reader returned null. Are credentials configured? Is DB client and server correctly configured?");
//                            }

//                            m_StringDictionary = expandedConfig;
//                            m_BoolDictionary = new Dictionary<string, bool>();
//                            m_IntDictionary = new Dictionary<string, int>();
//                            m_DoubleDictionary = new Dictionary<string, double>();


//                            reader.Dispose();
//                        }
//                        catch (CommunicationException e)
//                        {
//                            m_Log.Fatal(e);
//                            throw new ApplicationException("Failed to connect to the configuration service, is the service started?", e);
//                        }
//                        catch (TimeoutException e)
//                        {
//                            m_Log.Fatal(e);
//                            throw new ApplicationException("Connection to configuration service timed out", e);
//                        }
//                        catch (Exception e)
//                        {
//                            m_Log.Fatal(e);
//                            throw new ApplicationException("An unexpected error occured while trying to connect to the configuration service", e);
//                        }
//                        finally
//                        {
//                            //reader.Abort();
//                        }
//                    }
//                }
//            }
//        }

//        [Conditional("DEBUG")]
//        private static void ValidateBool(string parameter, string value)
//        {
//            if (!value.Equals("true", StringComparison.InvariantCultureIgnoreCase)
//                && !value.Equals("false", StringComparison.InvariantCultureIgnoreCase))
//            {
//                throw new Exception(string.Format(
//                    "The parameter '{0}' has an illegal value '{1}'. Expected 'true' or 'false'.",
//                    parameter,
//                    value));
//            }
//        }

//    }

//}
