using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest
{
    [TestFixture]
    public class DataExchangeQueuePriorityConverterTest
    {
        [Test]
        public void FromString_StringIsExact_PriorityIsCorrect()
        {
            // Assign

            const string value = "Normal";

            // Act

            var priority = DataExchangeQueuePriorityConverter.FromString(value);

            // Assert

            Assert.AreEqual(DataExchangeQueuePriority.Normal, priority);
        }

        [Test]
        public void FromString_StringIsLowerCase_PriorityIsCorrect()
        {
            // Assign

            const string value = "normal";

            // Act

            var priority = DataExchangeQueuePriorityConverter.FromString(value);

            // Assert

            Assert.AreEqual(DataExchangeQueuePriority.Normal, priority);
        }

        [Test]
        public void FromString_StringIsUpperCase_PriorityIsCorrect()
        {
            // Assign

            const string value = "NORMAL";

            // Act

            var priority = DataExchangeQueuePriorityConverter.FromString(value);

            // Assert

            Assert.AreEqual(DataExchangeQueuePriority.Normal, priority);
        }

        [Test]
        public void FromString_StringIsInvalid_PriorityIsUndefined()
        {
            // Assign

            const string value = "invalid";

            // Act

            var priority = DataExchangeQueuePriorityConverter.FromString(value);

            // Assert

            Assert.AreEqual(DataExchangeQueuePriority.Undefined, priority);
        }

        [Test]
        public void FromString_StringIsNull_PriorityIsUndefined()
        {
            // Assign

            const string value = null;

            // Act

            var priority = DataExchangeQueuePriorityConverter.FromString(value);

            // Assert

            Assert.AreEqual(DataExchangeQueuePriority.Undefined, priority);
        }

        [Test]
        public void FromString_StringIsEmpty_PriorityIsUndefined()
        {
            // Assign

            const string value = "";

            // Act

            var priority = DataExchangeQueuePriorityConverter.FromString(value);

            // Assert

            Assert.AreEqual(DataExchangeQueuePriority.Undefined, priority);
        }
    }
}
