/*
$Header$
*/
using System;
using System.Data;


namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for PDKeyTab.
	/// </summary>
	public sealed class PDKeyTab : PDKeyTabBase
	{
        public PDKeyTab(int noEntries, IDbConnection con) : base("keytab", noEntries, con) {;}
	}
}
