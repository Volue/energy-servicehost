using Oracle.ManagedDataAccess.Client;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Powel.Icc.Data
{
    public class DemMessagePayload 
    {
        public static void InsertPayload(string Id, string Payload)
        {
            const string stmt = @"insert into dem_message_payload(id,payload,created) values(:id,:payload,:created)";
            using (var cmd = new OracleCommand(stmt))
            {
                cmd.Parameters.Add(null, Id);
                cmd.Parameters.Add(null, Payload);
                cmd.Parameters.Add(null, DateTime.UtcNow);
                Util.ExecuteCommand(cmd);
            }
        }

        public static string SelectPayload(string Id)
        {
            const string stmt = @"select payload from dem_message_payload where id = :id";
            using (var cmd = new OracleCommand(stmt))
            {
                cmd.Parameters.Add(null, Id);
                var pl = Util.CommandToScalar(cmd);
                if (pl == null)
                    throw new KeyNotFoundException(Id); // Someone has a ticket to some info which we don't find.
                return pl.ToString();
            }
        }

        public static void DeletePayload(string Id)
        {
            const string stmt = @"delete from dem_message_payload where id = :id";
            using (var cmd = new OracleCommand(stmt))
            {
                cmd.Parameters.Add(null, Id);
                Util.ExecuteCommand(cmd);
            }
        }
    }
}
