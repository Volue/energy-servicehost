using System;
using System.IO;
using System.Text;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;
using Powel.Icc.Common;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    public class LocalFileshareClaimHandler : IClaimHandler
    {
        public static string HandlerId => "LocalFileshareClaimHandler";
        public string Name { get { return HandlerId;} }
        public LocalFileshareClaimHandler()
        {
            var path = IccConfiguration.ImportExport.IccLocalFileshareDir;
            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);
        }

        public ClaimTicket checkIn(string Payload)
        {
            var path = IccConfiguration.ImportExport.IccLocalFileshareDir;
            Guid id = Guid.NewGuid();
            var ret = new LocalFileshareClaimTicket(Name);
            ret.Filename = id.ToString();
            var fullPath = Path.Combine(path, ret.Filename);
            using (var fStream = new FileStream(fullPath, FileMode.Create, FileAccess.Write))
            {
                // Write UTF-8 without BOM
                using (var sWriter = new StreamWriter(fStream, new UTF8Encoding(false)))
                {
                    sWriter.Write(Payload);
                }
            }
            return ret;
        }

        public string checkOut(ClaimTicket Ticket, bool Delete = false)
        {
            string ret = string.Empty;
            var ticket = Ticket as LocalFileshareClaimTicket;
            if (ticket == null)
                return ret;

            var path = IccConfiguration.ImportExport.IccLocalFileshareDir;
            var fullPath = Path.Combine(path, ticket.Filename);
            try
            {
                using (var fStream = new FileStream(fullPath, FileMode.Open, FileAccess.Read))
                {
                    // Write UTF-8 without BOM
                    using (var sReader = new StreamReader(fStream, new UTF8Encoding(false)))
                    {
                        ret = sReader.ReadToEnd();
                    }
                }
                if (Delete)
                    File.Delete(fullPath);
            }
            catch (FileNotFoundException e)
            {
                throw new ClaimHandlerException(Ticket,e);
            }
            return ret;
        }

        public void clear(ClaimTicket Ticket)
        {
            var ticket = Ticket as LocalFileshareClaimTicket;
            if (ticket == null)
                return;

            var path = IccConfiguration.ImportExport.IccLocalFileshareDir;
            var fullPath = Path.Combine(path,ticket.Filename);
            File.Delete(fullPath);
        }
    }
}
