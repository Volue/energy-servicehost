using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure.PropertyRuleSet
{
    public class Rule
    {
        public string Name { get; set; }
        public SelectEntity Select { get; set; }
        private IDictionary<string,object> _props = new Dictionary<string, object>();
        public IDictionary<string,object> Properties { get { return _props; } }
    }
}
