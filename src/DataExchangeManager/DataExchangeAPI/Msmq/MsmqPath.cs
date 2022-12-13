namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public class MsmqPath
    {
        public MsmqPath()
        {
        }

        public MsmqPath(MsmqPath msmqPath)
        {
            FullPath = msmqPath.FullPath;
        }

        public string FullPath { get; set; }

        public MachineNameType MachineNameType { get; set; }
    }
}
