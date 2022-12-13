using System;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeCommon;

namespace AzureDataExchangeManagerService.UnitTests
{
    [TestFixture, Category("Unit tests")]
    public class PropertyHelperTests
    {
        private string TestProperty {
            get { return "Dummy stuff"; }
        }
        private int TestIntProperty
        {
            get;set;
        }
        private DateTime TestDateProperty
        {
            get; set;
        }
        [Test]
        public void PropertyNamesWithOwnerObjectNameAreCorrectlyComputed()
        {
            var propName = ExpressionHelper.GetPropertyName(this, tc => tc.TestProperty);
            Assert.AreEqual($"{typeof(PropertyHelperTests).Name}.TestProperty", propName);
        }

        [Test]
        public void PropertyNamesAreCorrectlyComputed()
        {
            var propName = ExpressionHelper.GetPropertyName(() => TestProperty);
            Assert.AreEqual("TestProperty", propName);
        }
        [Test]
        public void PropertyNamesForIntsAreCorrectlyComputed()
        {
            var propName = ExpressionHelper.GetPropertyName(() => TestIntProperty);
            Assert.AreEqual("TestIntProperty", propName);
        }

        [Test]
        public void PropertyNamesForDateTimesAreCorrectlyComputed()
        {
            var propName = ExpressionHelper.GetPropertyName(() => TestDateProperty);
            Assert.AreEqual("TestDateProperty", propName);
        }
    }
}
