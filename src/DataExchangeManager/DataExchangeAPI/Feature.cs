using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{

    public abstract class TypeSafeEnum
    {
        public string Name { get; set; }
        public string Description { get; set; }

        protected TypeSafeEnum(string name, string description)
        {
            Name = name;
            Description = description;
        }
    }

    public sealed class Feature : TypeSafeEnum
    {
        public bool IsEnabled(Func<string, string> provider)
        {
            return Convert.ToBoolean(provider(this.FeatureKey));
        }
        public string FeatureKey => /*"Features." + */Name + ".Enabled";

        public static Feature AzureDataExchangeManager => new Feature("AzureDataExchangeManager", "Separate data exchange manager for AzureBus");

        public Feature(string name, string description) : base(name, description)
        {
        }
    }
}
