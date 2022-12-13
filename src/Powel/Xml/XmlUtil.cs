using System;
using System.Globalization;
using System.Xml;
using Powel.Icc.Services.Time;

namespace Powel.Xml
{
    /// <summary>
    /// Summary description for XmlUtil.
    /// </summary>
    public class XmlUtil
    {
        private static readonly string[] XmlDateTimeFormats =
        {
            "yyyy-MM-ddTHH\\:mm\\:ss.fffffffzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.fffffffZ",
            "yyyy-MM-ddTHH\\:mm\\:ss.ffffffzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.ffffffZ",
            "yyyy-MM-ddTHH\\:mm\\:ss.fffffzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.fffffZ",
            "yyyy-MM-ddTHH\\:mm\\:ss.ffffzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.ffffZ",
            "yyyy-MM-ddTHH\\:mm\\:ss.fffzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.fffZ",
            "yyyy-MM-ddTHH\\:mm\\:ss.ffzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.ffZ",
            "yyyy-MM-ddTHH\\:mm\\:ss.fzzz",
            "yyyy-MM-ddTHH\\:mm\\:ss.fZ",
            "yyyy-MM-ddTHH\\:mm\\:sszzz",
            "yyyy-MM-ddTHH\\:mm\\:ssZ"
        };

        /// <summary>
        /// System.XmlConvert.ToDateTime is not good, because it 
        /// automatically converts to the current user's local time.
        /// ParseDate is an alternative method that returns a correct
        /// UtcTime.
        /// </summary>
        public static UtcTime ParseDate(string s)
        {
            RegionalCalendar calendar = RegionalCalendar.UtcCalendar;
            DateTime dt = DateTime.ParseExact(s, XmlUtil.XmlDateTimeFormats, null, DateTimeStyles.AdjustToUniversal);

            if (dt == DateTime.MinValue)
                return UtcTime.MinValue;
            else if (dt == DateTime.MaxValue)
                return UtcTime.MaxValue;
            else
                return calendar.ToUtcTime(dt);
        }

        public static XmlNode SerializeString(XmlDocument document, string namespaceUri, string elementName,
            string value)
        {
            if (value == null)
                return null;

            XmlNode node = document.CreateElement(elementName, namespaceUri);
            node.InnerText = value;

            return node;
        }

        public static string DeserializeString(XmlNode node)
        {
            if (node == null)
                return null;

            return node.InnerText;
        }

        public static XmlNode SerializeBoolean(XmlDocument document, string namespaceUri, string elementName, bool value)
        {
            XmlNode node = document.CreateElement(elementName, namespaceUri);
            node.InnerText = XmlConvert.ToString(value);

            return node;
        }

        public static bool DeserializeBoolean(XmlNode node)
        {
            if (node == null)
                return false;
            else
                return XmlConvert.ToBoolean(node.InnerText);
        }

        public static XmlNode SerializeInteger(XmlDocument document, string namespaceUri, string elementName, long value)
        {
            XmlNode node = document.CreateElement(elementName, namespaceUri);
            node.InnerText = XmlConvert.ToString(value);

            return node;
        }

        public static long DeserializeInt64(XmlNode node)
        {
            return XmlConvert.ToInt64(node.InnerText);
        }

        public static XmlNode SerializeDouble(XmlDocument document, string namespaceUri, string elementName,
            double value)
        {
            XmlNode node = document.CreateElement(elementName, namespaceUri);
            node.InnerText = XmlConvert.ToString(value);

            return node;
        }

        public static double DeserializeDouble(XmlNode node)
        {
            return XmlConvert.ToDouble(node.InnerText);
        }

        public static XmlNode SerializeUtcTime(XmlDocument document, string namespaceUri, string elementName,
            UtcTime value)
        {
            if (value == UtcTime.Null)
                return null;

            XmlNode node = document.CreateElement(elementName, namespaceUri);
            node.InnerText = value.ToXmlDateTime();

            return node;
        }

        public static UtcTime DeserializeUtcTime(XmlNode node)
        {
            if (node == null)
                return UtcTime.Null;

            return XmlUtil.ParseDate(node.InnerText);
        }

        public static XmlNode SerializeDateTime(XmlDocument document, string namespaceUri, string elementName,
            DateTime value)
        {
            XmlNode node = document.CreateElement(elementName, namespaceUri);
            node.InnerText = XmlConvert.ToString(value, XmlDateTimeSerializationMode.Local);

            return node;
        }

        public static DateTime DeserializeDateTime(XmlNode node)
        {
            return DeserializeDateTime(node.InnerText);
        }

        public static DateTime DeserializeDateTime(string s)
        {
            if (s.StartsWith("9999-12-31T23:59:59.9999999"))
                return DateTime.MaxValue;
            else if (s.StartsWith("0001-01-01T00:00:00.0000000"))
                return DateTime.MinValue;
            else
                return XmlConvert.ToDateTime(s, XmlDateTimeSerializationMode.Local);
        }


        /// <summary>
        /// Deserializes the XML string, which is in a XML node inside the XML fragment (as string).
        /// </summary>
        /// <param name="xmlFragmentString">XML fragment as a string. If null, this method returns null.</param>
        /// <param name="nodeName">Name of the node to get string content for. If null, this method returns null.</param>
        /// <returns>the text content of the node, which is in the XML fragment. Returns null if nothing is found.</returns>

        public static string DeserializeString(string xmlFragmentString, string nodeName)
        {
            if (xmlFragmentString == null || nodeName == null)
            {
                return null;
            }
            var xmlDoc = new XmlDocument();
            var rootNode = xmlDoc.AppendChild(xmlDoc.CreateElement("rootNode"));
            rootNode.InnerXml = xmlFragmentString;
            var nodeList = xmlDoc.GetElementsByTagName(nodeName);
            return nodeList.Count <= 0 ? null : nodeList.Item(0).InnerText;
        }


        /// <summary>
        /// Deserializes the XML string, which is in a XML node inside the XML fragment (as string, case insensitive).
        /// </summary>
        /// <param name="xmlFragmentString">XML fragment as a string. If null, this method returns null.</param>
        /// <param name="nodeName">Name of the node to get string content for. If null, this method returns null.</param>
        /// <returns>the text content of the node, which is in the XML fragment. Returns null if nothing is found.</returns>

        public static string DeserializeStringInsensitive(string xmlFragmentString, string nodeName)
        {
            if (string.IsNullOrEmpty(xmlFragmentString) || string.IsNullOrEmpty(nodeName))
                return null;

            var startTag = "<" + nodeName + ">";
            var endTag = "</" + nodeName + ">";
            var posStart = xmlFragmentString.IndexOf(startTag, StringComparison.OrdinalIgnoreCase);
            var posEnd = xmlFragmentString.IndexOf(endTag, StringComparison.OrdinalIgnoreCase);
            if (posStart < 0 || posEnd < 1) return string.Empty;
            var len = posEnd - (posStart + startTag.Length);
            return len < 1 ? null : xmlFragmentString.Substring(posStart + startTag.Length, len).Trim();
        }
    }
}


