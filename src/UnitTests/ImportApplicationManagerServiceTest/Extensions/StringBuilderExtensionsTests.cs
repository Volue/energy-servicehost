using System.Text;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Extensions;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest.Extensions
{
    [TestFixture]
    public class StringBuilderExtensionsTests
    {
        [Test]
        [TestCase(null)]
        [TestCase("")]
        public void AppendIfNotNull_NullOrEmpty_DoesNotAppend(string value)
        {
            StringBuilder sb = new StringBuilder();

            StringBuilderExtensions.AppendIfNotNull(sb, 'a', value);

            Assert.AreEqual(0, sb.Length);
        }

        [Test]
        public  void AppendIfNotNull_SingleNonEmpty_Appends()
        {
            StringBuilder sb = new StringBuilder();

            StringBuilderExtensions.AppendIfNotNull(sb, 'a', "test");

            Assert.AreEqual("-a test", sb.ToString());
        }

        [Test]
        public void AppendIfNotNull_MultipleNonEmpty_Appends()
        {
            StringBuilder sb = new StringBuilder();

            StringBuilderExtensions.AppendIfNotNull(sb, 'a', "test1");
            StringBuilderExtensions.AppendIfNotNull(sb, 'b', "test2");

            Assert.AreEqual("-a test1 -b test2", sb.ToString());
        }
    }
}
