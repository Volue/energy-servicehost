using System;
using Microsoft.WindowsAzure.Storage.Blob;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    public class CloudBlobClaimHandler : IClaimHandler
    {
        public static string HandlerId => "CloudBlobClaimHandler";
        public string Name { get { return HandlerId; } }

        public CloudBlobClaimHandler() {}
        public ClaimTicket checkIn(string Payload)
        {
            string blobUriWithSas = IccConfiguration.ImportExport.IccCloudBlobUrlAndSas;
            var container = new CloudBlobContainer(new Uri(blobUriWithSas));
            var ret = new CloudBlobClaimTicket(Name);
            Guid id = Guid.NewGuid();
            ret.Blobname = id.ToString();
            var blob = container.GetBlockBlobReference(ret.Blobname);
            blob.UploadText(Payload);
            return ret;
        }

        public string checkOut(ClaimTicket Ticket, bool Delete)
        {
            string ret = string.Empty;
            var ticket = Ticket as CloudBlobClaimTicket;
            if (ticket == null)
                return ret;

            string blobUriWithSas = IccConfiguration.ImportExport.IccCloudBlobUrlAndSas;
            var container = new CloudBlobContainer(new Uri(blobUriWithSas));
            var blob = container.GetBlockBlobReference(ticket.Blobname);
            if (!blob.Exists()) 
                throw new ClaimHandlerException(Ticket);
            var payload = blob.DownloadText();
            return payload;
        }

        public void clear(ClaimTicket Ticket)
        {
            var ticket = Ticket as CloudBlobClaimTicket;
            if (ticket == null)
                return;

            string blobUriWithSas = IccConfiguration.ImportExport.IccCloudBlobUrlAndSas;
            var container = new CloudBlobContainer(new Uri(blobUriWithSas));
            var blob = container.GetBlockBlobReference(ticket.Blobname);
            blob.Delete();
        }
    }
}
