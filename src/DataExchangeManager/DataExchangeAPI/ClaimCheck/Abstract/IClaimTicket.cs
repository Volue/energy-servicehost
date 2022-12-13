using System;
using System.Runtime.Serialization;
using System.Xml;
using System.Xml.Serialization;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract
{
    public interface IClaimTicket
    {
        string HandlerName { get; }
    }

    [DataContract]
    [XmlInclude(typeof(LocalFileshareClaimTicket))]
    [XmlInclude(typeof(CloudBlobClaimTicket))]
    [XmlInclude(typeof(DatabaseClaimTicket))]
    [KnownType(typeof(LocalFileshareClaimTicket))]
    [KnownType(typeof(CloudBlobClaimTicket))]
    [KnownType(typeof(DatabaseClaimTicket))]
    public class ClaimTicket : IClaimTicket
    {
        private string _handlerName;
        public string HandlerName { get { return _handlerName; } set { _handlerName = value; } }
    }
}
