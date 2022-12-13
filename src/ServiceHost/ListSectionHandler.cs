using System;
using System.Collections;
using System.Configuration;
using System.Xml;

namespace ServiceHost
{
	/// <summary>
	/// Summary description for ListSectionHandler.
	/// </summary>
	public class ListSectionHandler : IConfigurationSectionHandler
	{
		public ListSectionHandler()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public object Create(object parent, object configContext, System.Xml.XmlNode section)
		{
			ArrayList list = new ArrayList();

			foreach (XmlNode node in section)
				if (node.NodeType == XmlNodeType.Element)
					list.Add(node.Attributes["value"].Value);

			return list.ToArray(typeof(string));
		}
	}
}
