using System;
using System.Data;
using System.Collections;
using Powel.Icc.Data;
using System.Diagnostics;
using System.Reflection;
using Oracle.ManagedDataAccess.Types;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for CompAttributesCollection.
	/// </summary>
	public class CompAttributesCollection : CollectionBase
	{
		public CompAttributes FindCompAttributes(int oprtKey) 
		{
            foreach (CompAttributes attr in List)
				if (attr.oprt_key == oprtKey) 
					return attr;
			return null;
		}

		public DataTable GetCompAttributes(SimCompType t, int context)
		{
			DataTable dt = new DataTable();
			dt.Columns.Add("OPRT_KEY",typeof(int));
			dt.Columns.Add("CODE",typeof(string));
			dt.Columns.Add("NAME",typeof(string));
			foreach (CompAttributes a in this)
			{
				if ( a.ValidForCompType(t) ) 
				{
					if (context == 0 && (a.oprt_key == (int)CompAttribute.CAvail))
						continue;
					else if (context == 2 && ((a.oprt_key <= (int)CompAttribute.Avail ||
						a.oprt_key > (int)CompAttribute.VVar && a.oprt_key < (int)CompAttribute.PBlock) || a.oprt_key == (int)CompAttribute.CAvail))
						continue;
					else if (context == 3 && a.oprt_key <= (int)CompAttribute.Subscr)
						continue;
					else if (context == 4 && (a.oprt_key <= (int)CompAttribute.Avail || (a.oprt_key > (int)CompAttribute.VVar
							&& a.oprt_key < (int)CompAttribute.GQMaxMean)))
						continue;
					DataRow dr = dt.NewRow();
					dr[0] = a.oprt_key ; dr[1] = a.code; dr[2] = a.name;
					dt.Rows.Add(dr);
				}
			}
			return dt;
		}

        public CompAttributesCollection()
		{
            //if (RestrictionDataRO.UseCompAttributesFromDbProc)
            //{
            //    RestrictionDataRO.FetchCompAttributes(out var attrTuples);
            //    Profiler.Log(7000 + 10, "Adding CompAttributes, tuples");
            //    foreach (var tuple in attrTuples)
            //    {
            //        tuple.Deconstruct(out var oprtKey, out var code, out var name, out var descript);
            //        Add(new CompAttributes(oprtKey, code, name, descript));
            //    }
            //}
            //else
            //{
                var tbl = RestrictionDataRO.FetchCompAttributes();
                Profiler.Log(7000 + 20, "Adding CompAttributes, DataRow");
                foreach (DataRow r in tbl.Rows)
                {
                    Add(new CompAttributes(r));
                }
            //}
            Profiler.Log(7000 + 30, "Adding CompAttributes. Exit");
        }

        public CompAttributesCollection(OracleRefCursor rcCompAttributes) 
        {
            Profiler.Log(7000 + 10, "CompAttributesCollection(rcCompAttributes)");
            //rcCompAttributes.FetchSize = 200;
            var rdCompAttributes = rcCompAttributes.GetDataReader();
            while (rdCompAttributes.Read())
            {
                var oprtKey = rdCompAttributes["OPRT_KEY"] != DBNull.Value ? Convert.ToInt32(rdCompAttributes["OPRT_KEY"]) : -1;
                var code = rdCompAttributes["CODE"] != DBNull.Value ? Convert.ToString(rdCompAttributes["CODE"]) : "";
                var name = rdCompAttributes["NAME"] != DBNull.Value ? Convert.ToString(rdCompAttributes["NAME"]) : "";
                var descript = rdCompAttributes["DESCRIPT"] != DBNull.Value ? Convert.ToString(rdCompAttributes["DESCRIPT"]) : "";
                Add(new CompAttributes(oprtKey, code, name, descript));
                //    Log.Warn($"\n{MethodBase.GetCurrentMethod().Name}" +
                //        $"\n\t{compAttrCount}. {oprtKey}, {code}, {name}, {descript}");
            }
            Profiler.Log(7000 + 20, "CompAttributesCollection(rcCompAttributes). Exit");
        }

        public CompAttributes this[ int index ]  
		{
			get { return( (CompAttributes) List[index] ); }
			set { List[index] = value; }
		}
		public int Add(CompAttributes value) 
		{
			return( List.Add( value ) );
		}

		public int IndexOf(CompAttributes value) 
		{
			return( List.IndexOf( value ) );
		}

		public void Insert(int index, CompAttributes value) 
		{
			List.Insert(index, value);
		}

		public void Remove(CompAttributes value) 
		{
			List.Remove( value );
		}

		public bool Contains(CompAttributes value) 
		{
			return( List.Contains( value ) );
		}
	}
}
