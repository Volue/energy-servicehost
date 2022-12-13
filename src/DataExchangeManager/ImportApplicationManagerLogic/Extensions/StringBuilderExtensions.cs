using System.Text;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Extensions
{
    public static class StringBuilderExtensions
    {
        public static void AppendIfNotNull(this StringBuilder arguments, char argumentName, string argumentValue)
        {
            if (!string.IsNullOrEmpty(argumentValue))
            {
                if (arguments.Length > 0)
                {
                    arguments.AppendFormat(" -{0} {1}", argumentName, argumentValue);
                }
                else
                {
                    arguments.AppendFormat("-{0} {1}", argumentName, argumentValue);
                }
            }
        }
    }
}
