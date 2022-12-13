using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Web;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Xml;

namespace Powel.Icc.TimeSeries.CustomNaming
{
	public class NameGenerator
	{
		List<NamePart> namePartList = new List<NamePart>();
		Dictionary<IndexKey,string> indexElementList = new Dictionary<IndexKey,string>();
		string nameRuleDelimiter = "";
		string nameRuleName = "";
		
		/// <summary>
		/// Generates a name from the custom naming rules defined.
		/// </summary>
		/// <param name="cn">CustomName Class</param>
		/// <returns>name</returns>
		public string getName(CustomName cn)
		{
			string fileName = "CustomNaming.xml";
			string filePath = IccConfiguration.IccHome + "\\templates\\" + fileName;

			string name = "";

			//validate xml
			Validation xmlValidator = new Validation();
			TextReader reader = new StreamReader(filePath);
			
			xmlValidator.ReadSchemas(IccConfiguration.IccHome + "\\XML-Schema\\CustomNaming\\");
						
			xmlValidator.ValidateNoSoap(reader);

			reader.Close();
			
			//todo: set namerulename.. check if namerule is already parsed
			if(cn.RootName != nameRuleName)
				parseNameRule(cn, filePath);
						
			foreach(NamePart np in namePartList)
			{
				if (np.ElementType == ElementType.Property)
				{
					if (name != string.Empty)
						name += np.Delimiter;
					name += cn.getValue(np.Name);
				}
				else if (np.ElementType == ElementType.Index)
				{ 
					IndexKey inKey = new IndexKey(np.ElementValue, cn.getValue(np.Name));
					if (indexElementList.ContainsKey(inKey))
					{
						if (name != string.Empty)
							name += np.Delimiter; 
						name += indexElementList[inKey].ToString();
					}
				}
				else if (np.ElementType == ElementType.Text)
				{ 
					if (name != string.Empty)
						name += np.Delimiter;
					name += np.ElementValue;
				}
						
			}             

            if(name == string.Empty)
                throw new Exception(string.Format("Custom naming generated an empty string from name rule '{0}'. Please correct this in '{1}'",cn.RootName,filePath));

			return name;
		}

		private void parseNameRule(CustomName cn, string fileName)
		{
			string name = "";
			string delimiter = "";
			ElementType elType = ElementType.Unknown;
			string value = "";
			bool inNameRule = false;
			bool inNameElement = false;
			bool inIndexElement = false;
			bool inIndexElementType = false;
			string indexElementName = "";
			string indexElementTypeName = "";
			string indexSubElementName = "";
			NamePart np = new NamePart();
			XmlTextReader xReader = new XmlTextReader(fileName);
			
			namePartList.Clear();
			nameRuleName = cn.RootName;

			while (xReader.Read())
			{
				if (xReader.Name.ToUpper() == "NAMERULE")
				{
					if (xReader.NodeType == XmlNodeType.Element)
					{
						if (xReader.GetAttribute("name") == cn.RootName)
						{ 
							inNameRule = true;
							nameRuleDelimiter = xReader.GetAttribute("delimiter");
						}
							
					}
					if (xReader.NodeType == XmlNodeType.EndElement)
						inNameRule = false;
				}
				// In Name Rule
				if (inNameRule)
				{
					// In Name Element
					if ((xReader.NodeType == XmlNodeType.Element) && (xReader.Name.ToUpper() == "NAMEELEMENT"))
					{
						inNameElement = true;
						name = xReader.GetAttribute("name");
						
						switch(xReader.GetAttribute("elementType"))
						{
							case "Index":
								elType = ElementType.Index;
								break;
							case "Property":
								elType = ElementType.Property;
								break;
							case "Text":
								elType = ElementType.Text;
								break;
							default:
								elType = ElementType.Unknown;
								break;
						}

						delimiter	= xReader.GetAttribute("delimiter");
						if (delimiter == null)
							delimiter = nameRuleDelimiter;
						if(elType == ElementType.Property)
							namePartList.Add(new NamePart(namePartList.Count-1, name, elType, delimiter, value));

					}
					
					if (inNameElement && xReader.NodeType == XmlNodeType.Text)
					{
						value = xReader.Value;
					}
					
					if ((xReader.NodeType == XmlNodeType.EndElement) && (xReader.Name.ToUpper() == "NAMEELEMENT"))
					{
						if ((elType == ElementType.Index)||(elType == ElementType.Text))
							namePartList.Add(new NamePart(namePartList.Count - 1, name, elType, delimiter, value));

						inNameElement = false;
					}
				}
				
				//Index Element
				if (inIndexElement)
				{

					if (xReader.NodeType == XmlNodeType.Element && inIndexElementType)
						indexSubElementName = xReader.Name.Split(Convert.ToChar(":"))[1];

					if (xReader.NodeType == XmlNodeType.Text && inIndexElementType)
						addIndexItem(indexElementName, indexSubElementName, xReader.Value);

					if (xReader.NodeType == XmlNodeType.Element && !inIndexElementType)
					{ 
						inIndexElementType = true;
						indexElementTypeName = xReader.Name.Split(Convert.ToChar(":"))[1];											
					}

					if (xReader.NodeType == XmlNodeType.EndElement && inIndexElementType && xReader.Name.Split(Convert.ToChar(":"))[1] == indexElementTypeName)
						inIndexElementType = false;

				}
				
				if((xReader.NodeType == XmlNodeType.Element) && (xReader.Name.ToUpper() == "INDEXELEMENT"))
				{
					np = getItemByValue(xReader.GetAttribute("name"));
					if(np.ElementValue != null)
					{
						inIndexElement = true;
						indexElementName = xReader.GetAttribute("name");
					}
				}
				if ((xReader.NodeType == XmlNodeType.EndElement) && (xReader.Name.ToUpper() == "INDEXELEMENT"))
					inIndexElement = false;

			}

			xReader.Close();
		}

		private NamePart getItemByValue(string val)
		{
			NamePart namePart = new NamePart();
			foreach(NamePart np in namePartList)
			{
				if (np.ElementValue == val)
					namePart = np;
			}
			return namePart;
		}

		private void addIndexItem(string indexElementName, string indexSubElementName, string indexSubElementValue)
		{
			IndexKey ik = new IndexKey(indexElementName,indexSubElementName);
			if(!indexElementList.ContainsKey(ik))
				indexElementList.Add(new IndexKey(indexElementName, indexSubElementName), indexSubElementValue);
		}

		private struct NamePart
		{
			private int index;
			private string name;
			private ElementType elementType;
			private string delimiter;
			private string elementValue;

			public NamePart(int _index, string _name, ElementType _elementType, string _delimiter, string _elementValue)
			{
				index				= _index;
				name				= _name;
				elementType			= _elementType;
				delimiter			= _delimiter;
				elementValue		= _elementValue;
			}

			public int Index
			{
				get
				{
					return index;
				}
			}
			public string Name
			{
				get
				{
					return name;
				}
			}
			public ElementType ElementType
			{
				get
				{
					return elementType;
				}
			}
			public string Delimiter
			{
				get
				{
					return delimiter;
				}
			}
			public string ElementValue
			{
				get
				{
					return elementValue;
				}
			}
		}

		private struct IndexKey
		{
			private string indexElementName;
			private string indexSubElementName;

			public IndexKey(string _indexElementName, string _indexSubElementName)
			{
				indexElementName		= _indexElementName;
				indexSubElementName		= _indexSubElementName;
			}
			public string IndexElementName
			{
				get
				{
					return indexElementName;
				}
			}
			public string IndexSubElementName
			{
				get
				{
					return indexSubElementName;
				}
			}


		}

		public enum ElementType
		{
			Property,
			Index,
			Text,
			Unknown
		}
	}
}
