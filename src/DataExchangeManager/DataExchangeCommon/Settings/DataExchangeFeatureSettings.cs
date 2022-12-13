using System;
using System.Configuration;
using System.Linq.Expressions;

namespace Powel.Icc.Messaging.DataExchangeCommon.Settings
{
    public abstract class DataExchangeModuleSettings
    {
        protected string GetStringFromConfig(Expression<Func<object>> func)
        {
            var key = ExpressionHelper.GetPropertyName(func);
            return ConfigurationManager.AppSettings[$"{ModuleName}.{key}"];
        }
        protected double GetDoubleFromConfig(Expression<Func<object>> func,double def)
        {
            var str = GetStringFromConfig(func);
            return string.IsNullOrEmpty(str) ? def : Convert.ToDouble(str);
        }

        protected int GetIntFromConfig(Expression<Func<object>> func,int def)
        {
            var str = GetStringFromConfig(func);
            return string.IsNullOrEmpty(str) ? def : Convert.ToInt16(str);
        }

        protected bool GetBoolFromConfig(Expression<Func<object>> func, bool def)
        {
            var str = GetStringFromConfig(func);
            return string.IsNullOrEmpty(str) ? def : Convert.ToBoolean(str);
        }

        public abstract string ModuleName { get; }
    }
}
