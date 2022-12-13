using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using DataExchangeModuleStandardWsClient;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;

namespace DataExchangeTestClient
{
    public partial class ExportResultForm : Form
    {
        private StandardWsClient _client;
        private string _externalReference;
        private string _exportData;
        public AcknowledgeExportResponse Response { get; set; }

        public ExportResultForm()
        {
            InitializeComponent();
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        internal void SetClient(StandardWsClient client)
        {
            _client = client;
        }

        internal void SetExportData(ExportMessage exportMessage)
        {
            if (exportMessage != null)
            {
                btnAck.Enabled = true;

                lblResponseId.Text = exportMessage.Reference;
                _exportData = exportMessage.ExportData ?? string.Empty;
                _externalReference = exportMessage.Reference;

                txtExportData.Text = _exportData;

                if (exportMessage.Metadata != null)
                {
                    StringBuilder metadataStringBuilder = new StringBuilder();
                    foreach (var metadataitem in exportMessage.Metadata)
                    {
                        metadataStringBuilder.Append(metadataitem.Key).Append(':').AppendLine(metadataitem.Value);
                    }
                    txtMetadata.Text = metadataStringBuilder.ToString();
                }
            }
            else
            {
                btnAck.Enabled = false;
                lblResponseId.Text = "";
                txtMetadata.Text = "";
                txtExportData.Text = "";
            }
        }

        private void btnAck_Click(object sender, EventArgs e)
        {
            Response = _client.SendExportAck(_externalReference);
            Close();
        }

        private void btnSaveToFile_Click(object sender, EventArgs e)
        {
            saveFileDialog1.OverwritePrompt = true;
            DialogResult result = saveFileDialog1.ShowDialog();
            if (result == DialogResult.OK)
            {
                string fileName = saveFileDialog1.FileName;
                File.WriteAllText(fileName, _exportData);
            }
        }
    }
}

