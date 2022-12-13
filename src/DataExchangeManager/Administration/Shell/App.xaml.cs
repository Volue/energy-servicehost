using System;
using System.Globalization;
using System.IO;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Windows;
using NDesk.Options;
using Powel.Icc.Common;
using log4net;

namespace DataExchange.Administration.Shell
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private static readonly Mutex Mutex = new Mutex(true, "{8F6F0AC4-B9A1-45fd-A8CF-72F04E6BDE8F}" + WindowsIdentity.GetCurrent().Name.Replace("\\","_"));

        private enum ExitCode
        {
            Success = 0,
            Error = 1
        }

        private string _cultureName;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            if (Mutex.WaitOne(TimeSpan.Zero, true))
            {
                try
                {
                    Log.Debug("This application is not already running, running the bootstrapper to start the application.");

                    ParseCommandLineArguments(e.Args);

				    SetLanguage();

                    var bootstrapper = new Bootstrapper();
                    bootstrapper.Run();
                }
                catch (Exception exception)
                {
                    Log.Error("Failed to run the bootstrapper.", exception);

                    ShowError(exception.ToString());
                    ShutDownApplicationWithFailureReturnCode();
                }
            }
            else
            {
                Log.Debug("This application is already running, showing the already started process, then shutting myself down with success return code.");

                // Sends a message within the application, picked up by ShellView.xaml.cs
                NativeMethods.PostMessage(
                    (IntPtr)NativeMethods.HWND_BROADCAST,
                    NativeMethods.WM_SHOWME,
                    IntPtr.Zero,
                    IntPtr.Zero);

                Current.Shutdown((int)ExitCode.Success);
            }
        }

        private void ShowError(string message)
        {
            MessageBox.Show(message, Shell.Properties.Resources.FailedToStart, MessageBoxButton.OK, MessageBoxImage.Error);
        }

        private void ShutDownApplicationWithFailureReturnCode()
        {
            Log.Debug("Shutting down the application with failure return code.");

            Shutdown((int)ExitCode.Error);
        }

        private void ParseCommandLineArguments(string[] args)
        {
            Log.DebugFormat("Command line arguments: \"{0}\".", string.Join(" ", args));

            var optionSet = new OptionSet
                {
                    {
                        "culture|c=", "The culture name in the format languagecode2-country/regioncode2. languagecode2 is a lowercase two-letter code derived from ISO 639-1. country/regioncode2 is derived from ISO 3166 and usually consists of two uppercase letters.",  cultureName =>
                        {
                            _cultureName = cultureName;
                        }
                    }
                };

            try
            {
                optionSet.Parse(args);
            }
            catch (OptionException exception)
            {
                Log.Error("Invalid command line arguments.", exception);

                // Show the usage dialog
                var usage = GetUsageMessage(optionSet);
                ShowError(usage);
                ShutDownApplicationWithFailureReturnCode();
            }
        }

        private static string GetUsageMessage(OptionSet optionSet)
        {
            var usage = new StringBuilder();
            usage.AppendLine("Usage:");
            usage.AppendLine();
            usage.AppendLine("{0} [OPTIONS]");
            usage.AppendLine();
            usage.AppendLine("Options:");
            usage.AppendLine("{1}");

            string options;
            using (var stream = new MemoryStream())
            {
                var writer = new StreamWriter(stream);
                optionSet.WriteOptionDescriptions(writer);
                writer.Flush();

                stream.Position = 0;
                var reader = new StreamReader(stream);
                options = reader.ReadToEnd();
            }

            return string.Format(usage.ToString(), AppDomain.CurrentDomain.FriendlyName, options);
        }

        private void SetLanguage()
        {
            CultureInfo cultureInfo = null;

            // When possible, use the language specified in the command line
            // argument, using the IccConfiguration class to determine language
            // is very slow, and mat cause the application to load for a while
            // when starting up.

            if(!string.IsNullOrEmpty(_cultureName))
            {
                Log.DebugFormat("The culture, \"{0}\", is specified in the command line arguments.", _cultureName);
                
                try
                {
                    cultureInfo = CultureInfo.CreateSpecificCulture(_cultureName);
                }
                catch(CultureNotFoundException)
                {
                    Log.Error("The specified culture is not valid, the application will shut down.");

                    ShowError(string.Format("Invalid culture, {0}, specified in the command line arguments.", _cultureName));

                    ShutDownApplicationWithFailureReturnCode();
                }
            }

            if(cultureInfo == null)
            {
                Log.Debug("No culture is specified in the command line arguments, fetching the culture from the ICC configuration.");

                try
                {
                    cultureInfo = IccConfiguration.Globalization.LanguageCulture;
                }
                catch(Exception exception)
                {
                    // We can't read language from the configuration system, this is safe to ignore, we can just use the default language instead
                    Log.Error("Failed to read the culture from the ICC configuration.", exception);
                }
            }

            if(cultureInfo != null)
            {
                Thread.CurrentThread.CurrentCulture = cultureInfo;
                Thread.CurrentThread.CurrentUICulture = cultureInfo;
            }
            else
            {
                Log.Warn("No culture has been specified, using the system default culture.");
            }

            Log.DebugFormat("Culture = \"{0}\". Culture value determines the results of culture-dependent functions, such as the date, number, and currency formatting, and so on.", Thread.CurrentThread.CurrentCulture.Name);
            Log.DebugFormat("UICulture = \"{0}\". UICulture value determines what resource file to load.", Thread.CurrentThread.CurrentUICulture.Name);
        }
    }
}
