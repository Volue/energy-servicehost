using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters
{
    public struct MessageExporterSettings
    {
        public TimeSpan HowOftenWeShouldLogFailureForTheSameMessage { get; private set; }

        public static MessageExporterSettings Default
        {
            get
            {
                return new MessageExporterSettings
                {
                    HowOftenWeShouldLogFailureForTheSameMessage = TimeSpan.FromSeconds(600)
                };
            }
        }

        public static MessageExporterSettings Custom(int howOftenWeShouldLogFailureForTheSameMessage)
        {
            return new MessageExporterSettings
            {
                HowOftenWeShouldLogFailureForTheSameMessage = TimeSpan.FromSeconds(howOftenWeShouldLogFailureForTheSameMessage)
            };
        }
    }
}