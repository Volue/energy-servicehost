using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Web;

//TOSA namespace Powel.Reflection
namespace Powel.Icc.Common
{
	/// <summary>
	/// Summary description for AssemblyInfo.
	/// </summary>
	public class AssemblyInfo
	{
		Assembly assembly;

		public AssemblyInfo(Assembly assembly)
		{
			this.assembly = assembly;
		}

		public string FileName
		{
			get 
			{ 
				Uri uri = new Uri(assembly.GetName().CodeBase);
				return HttpUtility.UrlDecode(uri.AbsolutePath);
			}
		}

		public string Directory
		{
			get 
			{ 
				return Path.GetDirectoryName(FileName);
			}
		}

		public Version AssemblyVersion
		{
			get { return assembly.GetName().Version; }
		}

		public string AssemblyFileVersion
		{
			get { return FileVersionInfo.GetVersionInfo(FileName).FileVersion; }
		}

		public DateTime LastWriteTime
		{
			get { return new FileInfo(FileName).LastWriteTime; }
		}
	}
}
