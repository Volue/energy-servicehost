using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure.PropertyRuleSet
{
    public class SelectEntity
    {
        private IDictionary<string, string> _metaD = new Dictionary<string, string>();
        public IDictionary<string,string> MetaData { get { return _metaD; } }
        public string Payload { get; set; }
    }
}
