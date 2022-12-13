using System;

namespace DataExchange.Administration.ImportModule
{
    public class ErrorViewModel
    {
        public ErrorViewModel(Exception exception)
        {
            Title = exception.GetType().ToString();
            ExceptionMessage = exception.Message;
            FullException = exception.ToString();
        }

        public string Title { get; private set; }

        public string ExceptionMessage { get; private set; }

        public string FullException { get; private set; }
    }
}
