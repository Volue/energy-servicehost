using System.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class ProcessRunner : IProcessRunner
    {
        public int Run(string fileName, string workingDirectory, string arguments)
        {
            var process = new System.Diagnostics.Process
                {
                    StartInfo =
                        {
                            WorkingDirectory = workingDirectory,
                            FileName = fileName,
                            Arguments = arguments,
                            WindowStyle = ProcessWindowStyle.Hidden
                        }
                };

            process.Start();
            process.WaitForExit();

            return process.ExitCode;
        }
        public int Run(string fileName, string workingDirectory, string arguments,string messageData)
        {
            var process = new System.Diagnostics.Process
            {
                StartInfo =
                {
                    WorkingDirectory = workingDirectory,
                    FileName = fileName,
                    Arguments = arguments,
                    WindowStyle = ProcessWindowStyle.Hidden,
                    UseShellExecute = false,
                    RedirectStandardInput = true
                }
            };

            process.Start();
            var streamWriter = process.StandardInput;
            streamWriter.Write(messageData);
            streamWriter.Close();
            process.WaitForExit();

            return process.ExitCode;
        }
    }
}
