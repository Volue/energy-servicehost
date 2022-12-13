namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract
{
    public interface IProcessRunner
    {
        int Run(string fileName, string workingDirectory, string arguments);
        int Run(string fileName, string workingDirectory, string arguments, string messageData);
    }
}
