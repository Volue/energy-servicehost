using System;

namespace TASE2.Library.Client
{
    public class ClientException : Exception
    {
        private ClientError errorCode;

        public ClientException(string message, ClientError error) : base(message)
        {
            this.errorCode = error;
        }

        public ClientError GetError()
        {
            return errorCode;
        }
    }
}

