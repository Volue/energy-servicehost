using System;
using System.Globalization;
using System.IO;
using System.Reflection;
//TOSA using Powel.Reflection;

namespace Powel.Icc.Common
{
	/// <summary>
	/// Summary description for Culture.
	/// </summary>
	public sealed class Culture
	{
		public static string CultureToLanguage(CultureInfo cultureInfo)
		{
			try
			{
				switch (cultureInfo.Name.Substring(0, 2))
				{
					case "cs":
						return "czech";
					case "da":
						return "dansk";
					case "no":
						return "norsk";
					case "nn":
						return "norsk";
					case "nb":
						return "norsk";
					case "sv":
						return "svensk";
					default:
						return "english";
				}
			}
			catch
			{
				return "english";
			}
		}

		public static CultureInfo LanguageToCulture(string language)
		{
			switch (language)
			{
				case "english":
                case "":
					return CultureInfo.InvariantCulture;
				case "czech":
					return CultureInfo.GetCultureInfo("cs");
				case "dansk":
					return CultureInfo.GetCultureInfo("da");
				case "norsk":
					return CultureInfo.GetCultureInfo("nb-NO");
				case "svensk":
					return CultureInfo.GetCultureInfo("sv-SE");
				default:
					throw new ArgumentException("Unknown language.");
			}
		}

		public static string GetHelpFilePath(CultureInfo cultureInfo, string fileName)
		{
			AssemblyInfo assemblyInfo = new AssemblyInfo(Assembly.GetEntryAssembly());
			string assemblyDirectory = Path.GetDirectoryName(assemblyInfo.FileName);
			string guiDirectory = Path.Combine(assemblyDirectory, @"..\gui");

			string localName = String.Format(@"{0}\{1}\{2}", 
				guiDirectory, CultureToLanguage(cultureInfo), fileName);
			
			string invariantName = String.Format(@"{0}\{1}\{2}", 
				guiDirectory, CultureToLanguage(CultureInfo.InvariantCulture), fileName);

			if (File.Exists(localName))
				return localName;
			else if (File.Exists(invariantName))
				return invariantName;
			else
				throw new ApplicationException(String.Format("A help file could not be found in '{0}' nor in '{1}'.",
					localName, invariantName));
		}
    }

    public sealed class LanguageSettings
    {
        public enum Language : int
        {
            NOT_DEFINED = 0,
            NORWEGIAN = 1,
            ENGLISH = 2,
            SWEDISH = 4
        }
        private static Language ediCountry = Language.NOT_DEFINED;
        private static Language guiLanguage = Language.NOT_DEFINED;
        public static Language GuiLanguage
        {
            get { return guiLanguage; }
            set { guiLanguage = value; }
        }
        public static void setGuiLanguage(string lang)
        {
            switch (lang)
            {
                case "NORSK": GuiLanguage = Language.NORWEGIAN; break;
                case "SVENSK": GuiLanguage = Language.SWEDISH; break;
                case "ENGLISH": GuiLanguage = Language.ENGLISH; break;
                default: GuiLanguage = Language.ENGLISH; break;
            }
        }

        public static Language EdiCountry
        {
            get { return ediCountry; }
            set { ediCountry = value; }
        }
        public static void setEdiCountry(string lang)
        {
            switch (lang)
            {
                case "NORSK": EdiCountry = Language.NORWEGIAN; break;
                case "SVENSK": EdiCountry = Language.SWEDISH; break;
                case "ENGLISH": EdiCountry = Language.ENGLISH; break;
                default: EdiCountry = GuiLanguage; break;
            }
        }

        static LanguageSettings()
        {
            GuiLanguage = Language.NOT_DEFINED;
            EdiCountry = Language.NOT_DEFINED;
        }
    }	
}
