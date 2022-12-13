using System;
using System.Data;
using Powel.Icc.Interop;

namespace Powel.Icc.Diagnostics
{
    public interface IEventLogModuleItem : IDisposable
    {
        IDbConnection Connection { get; }
        int AppKey { get; }
        //void Dispose();
        void Resurrect();
        void Open(IccModule moduleKey );
        void Open(IccModule moduleKey, int appKeyOwner, bool useLogKey, string name, string appendix);
        void LogMessage(IccModule moduleKey, int messageKey, params string[] args);
        string GetLogMessage(IccModule moduleKey, int messageKey, params string[] args);
        void LogMessage(int messageKey);
        void LogMessage(int messageKey, params string[] args);
        void LogMessage(IccModule moduleKey, int messageKey);
        string GetLogMessage(int messageKey);
        string GetLogMessage(int messageKey, params string[] args);
        string GetLogMessage(IccModule moduleKey, int messageKey);
        void LogDebugMessage(string message);
        void Recycle(int recycleInterval);
        void Close();
    }
}