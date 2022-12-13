using System;
using System.Data;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for CompAttributes.
	/// </summary>
	public class CompAttributes
	{
		public int oprt_key;
		public string code;
		public string name;
		public string descript;
		public Attr attr;
        private static XmlSerializer _serializer = new XmlSerializer(typeof(Attr));

        public CompAttributes(DataRow r)
        {
            oprt_key = (int)r["OPRT_KEY"];
            if (r["CODE"] != DBNull.Value)
                code = (string)r["CODE"];
            if (r["NAME"] != DBNull.Value)
                name = (string)r["NAME"];
            if (r["DESCRIPT"] != DBNull.Value)
            {
                descript = (string)r["DESCRIPT"];
                StringBuilder sb = new StringBuilder(descript);
                sb.Insert(0, @"<?xml version=""1.0""?>");
                attr = DeSerializeFromString(sb.ToString());
            }
        }

        public CompAttributes(int oprtKey, string code, string name, string descript)
        {
            oprt_key = oprtKey;
            this.code = code;
            this.name = name;
            this.descript = descript;
            if (!string.IsNullOrEmpty(descript))
            {
                var sb = new StringBuilder(descript);
                sb.Insert(0, @"<?xml version=""1.0""?>");
                attr = DeSerializeFromString(sb.ToString());
            }
        }

        public bool ValidForCompType(SimCompType t)
		{
			string code = t.ToString();
			int ix;
			if ( attr.v != null && (ix=attr.v.IndexOf(code)) >= 0 ) 
			{
				ix += code.Length;
				if (ix < attr.v.Length && attr.v[ix]!=',') return false;
				return true;
			}
			else return false;
		}

		[Serializable]
		public class Attr
		{
			public string m;	// Methods
			public string c;	// Short code
			public string v;	// Valid for comp types, comma seperated list
			public string Method
			{
				get { return m; }
			}
			public string ShortCode
			{
				get { return c; }
			}
			public string ValidForComponents
			{
				get { return v; }
			}
		}

        public static Attr DeSerializeFromString(string xml)
        {
            Attr attr;
            using (var textReader = new StringReader(xml))
            {
                var reader = new XmlTextReader(textReader);
                attr = (Attr)_serializer.Deserialize(reader);
                //Console.WriteLine($"\t\tattr.Method = {attr.Method}, ShortCode = {attr.ShortCode}");
            }
            return attr;
        }
    }
}
