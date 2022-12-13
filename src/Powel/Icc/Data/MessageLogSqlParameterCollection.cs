using System.Collections.Generic;
using System.Data.Common;
using Oracle.ManagedDataAccess.Client;

namespace Powel.Icc.Data
{
    /// <summary>
    /// Manage SQL query parameters
    /// </summary>
    public class SqlParameterCollection
    {
        List<DbParameter> parameters = new List<DbParameter>();

        public void Clear()
        {
            parameters.Clear();
        }

        public DbParameter CreateParameter(object value)
        { 
            DbParameter param = new OracleParameter();
            param.ParameterName = string.Format(":{0:00}", parameters.Count + 1);
            param.Value = value;
            parameters.Add(param);
            return param;        
        }

        public DbParameter[] DbParameters
        {
            get { return parameters.ToArray(); }
        }

    }
}
