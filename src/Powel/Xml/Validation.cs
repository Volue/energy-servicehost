using System;
using System.IO;
using System.Xml;
using System.Xml.Schema;
//using Powel.Icc.Interop;

namespace Powel.Xml
{
	/// <summary>
	/// Summary description for Validation.
	/// </summary>
	public class Validation
	{
		XmlSchemaSet schemas;

		public Validation()
		{
		}

		public void ReadSchemas(string path)
		{
			// XML Schema cache for schema validation
			schemas = new XmlSchemaSet();
			string[] schemaFiles = Directory.GetFiles(path, "*.xsd");

			foreach (string schemaFile in schemaFiles)
			{
				XmlTextReader r = new XmlTextReader(schemaFile);
				XmlSchema schema = XmlSchema.Read(r, null);
				if( schema != null)
					schemas.Add(schema);
			}
		}

        public void AddSchema(string schemaFile, bool clearCache)
        {
            if (schemas == null || clearCache)
                schemas = new XmlSchemaSet();
            XmlTextReader r = new XmlTextReader(schemaFile);
            XmlSchema schema = XmlSchema.Read(r, null);
            if (schema != null)
                schemas.Add(schema);
        }

        public void Validate(string message)
		{
			Validate(new StringReader(message));
		}

		public void Validate(TextReader textReader)
		{
			XmlReaderSettings settings = new XmlReaderSettings();
			settings.Schemas = schemas;
			settings.ValidationType = ValidationType.Schema;
			settings.ValidationEventHandler += new ValidationEventHandler(this.ValidationEvent);

			using (XmlReader tr = XmlReader.Create(textReader))
			{
				using (XmlReader vr = XmlReader.Create(tr, settings))
				{
					while (tr.Read())
					{
						if ((tr.Name == "soap:Body") && (tr.NodeType == XmlNodeType.Element))
						{
							tr.Read();//read past "soap:Body"
							try
							{
								//performing validation
								while (vr.Read()) { }
							}
							catch (System.InvalidOperationException ex)
							{
								//if we run into soap:Body we stop validating
								if ((tr.Name == "soap:Body") && (tr.NodeType == XmlNodeType.EndElement))
									return;
								else
									throw ex;
							}
						}
					}
				}
			}
		}

		public void ValidateNoSoap(TextReader textReader)
		{
			XmlReaderSettings settings = new XmlReaderSettings();
			settings.Schemas = schemas;
			settings.ValidationType = ValidationType.Schema;
			settings.ValidationEventHandler += new ValidationEventHandler(this.ValidationEvent);

			using (XmlReader tr = XmlReader.Create(textReader))
			{
				using (XmlReader vr = XmlReader.Create(tr, settings))
				{
					try
					{
						while (vr.Read())
						{}
					}
					catch(Exception e)
					{
						tr.Close();
						vr.Close();
						throw e;
					}

					tr.Close();
					vr.Close();
				}
			}
		}

		public void ValidationEvent( object sender, ValidationEventArgs args)
		{
			throw args.Exception;
		}
	}
}