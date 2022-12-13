using System;
using System.Collections.Generic;
using System.Text;

namespace Powel.Icc.TimeSeries.CustomNaming
{
	public abstract class CustomName
	{
		protected Dictionary<string,string> cnElementValues = new Dictionary<string,string>();
		protected string rootName;		
		public string RootName
		{
			get
			{
				return rootName;
			}
		}

		public abstract string getValue(string nameElementName);
	}
}
