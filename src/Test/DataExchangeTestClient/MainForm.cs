using System;
using System.Collections.Generic;
using System.Globalization;
using System.Messaging;
using System.Text;
using System.Transactions;
using System.Windows.Forms;
using System.IO;
using DataExchangeModuleStandardWsClient;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;
using Message = System.Messaging.Message;

namespace DataExchangeTestClient
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private string Newline { get { return "\r\n"; } }

        private void OutputException(Exception e)
        {
            OutputText(e.GetType() + ": " + e.Message);
            OutputText(e.StackTrace);
        }

        private void OutputText(string text)
        {
            txtOutput.Text += Newline + text;
        }

        private void AutoClearOutput()
        {
            if (cbAutoClear.Checked)
            {
                ClearOutput();
            }
        }

        private void ClearOutput()
        {
            txtOutput.Clear();
        }

        private void StandardWsOutput(SetMessageStatusResponse response)
        {
            StandardWsOutputAcknowledge(response.RequestOk);
            StandardWsOutputNotAcknowledgedReasonText(response.RequestNotOkReasonText);
        }

        private void StandardWsOutput(SubmitEventResponse response)
        {
            StandardWsOutputAcknowledge(response.RequestOk);
            StandardWsOutputNotAcknowledgedReasonText(response.RequestNotOkReasonText);
        }

        private void StandardWsOutput(AcknowledgeExportResponse response)
        {
            OutputText("");
            if (response != null)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("ACK ").AppendLine(response.RequestOk ? "accepted" : "rejected");
                if (response.RequestNotOkReasonText != null)
                {
                    sb.Append("Message:").Append(response.RequestNotOkReasonText);
                }
                OutputText(sb.ToString());
            }
            else
            {
                OutputText("<no export acknowledgement operation>");
            }
        }

        private void StandardWsOutput(SubmitImportResponse response)
        {
            StandardWsOutputAcknowledge(response.RequestOk);
            StandardWsOutputNotAcknowledgedReasonText(response.RequestNotOkReasonText);
        }

        private void StandardWsOutputNotAcknowledgedReasonText(string notAcknowledgedReasonText)
        {
            if (notAcknowledgedReasonText != null)
            {
                OutputText("NAK reason:" + notAcknowledgedReasonText);
            }
        }

        private void StandardWsOutputAcknowledge(bool acknowledge)
        {
            OutputText("Acknowledge:" + acknowledge);
        }

        private StandardWsClient GetClient()
        {
            return StandardWsClient.GetClient(comboStandardWsAddress.Text);
        }

        private void btnStandardWsSubmitImport_Click(object sender, EventArgs e)
        {
            bool abort = false;
            AutoClearOutput();
            OutputText("[submit import]");

            if (rbStandardWsImportFile.Checked && txtStandardWsImportFile.Text.Trim().Length == 0)
            {
                OutputText("Failed: Please specify a filename");
                abort = true;
            }

            Dictionary<string, string> metadata = new Dictionary<string, string>();
            if (!abort)
            {
                foreach (string s in txtStandardWsImportMetadata.Lines)
                {
                    if (!string.IsNullOrEmpty(s) && s.Contains(";"))
                    {
                        string[] bits = s.Split(';');
                        if (bits.Length == 2)
                        {
                            metadata.Add(bits[0], bits[1]);
                        }
                        else
                        {
                            OutputText("ignoring invalid import metadata '" + s + "'");
                        }
                    }
                }
            }

            if (!abort)
            {
                string importData;
                if (rbStandardWsImportText.Checked)
                {
                    importData = txtStandardWsImportData.Text;
                }
                else
                {
                    EncodingListBoxItem encodingItem = (EncodingListBoxItem)listStandardWsFileEncoding.SelectedItem;

                    importData = File.ReadAllText(txtStandardWsImportFile.Text, encodingItem.GetEncoding());
                }

                StandardWsClient client = GetClient();
                SubmitImportResponse response;

                if (cbStandardWsImportGenerateTransactionId.Checked)
                {
                    txtStandardWsImportTransactionId.Text = Guid.NewGuid().ToString();
                }
                try
                {
                    response = client.SubmitImport(metadata, importData, txtStandardWsImportTransactionId.Text);
                    StandardWsOutput(response);
                }
                catch (Exception ex)
                {
                    OutputException(ex);
                }
            }
        }

        private void btnStandardWsGetExport_Click(object sender, EventArgs e)
        {
            AutoClearOutput();
            OutputText("[get export]");

            StandardWsClient client = GetClient();
            int timeoutInSeconds = int.Parse(txtStandardWsExportReadTimeoutInSeconds.Text);
            try
            {
                GetExportResponse exportData = client.GetExport(timeoutInSeconds);
                if (exportData.RequestOk)
                {
                    if (exportData.ExportMessage == null)
                    {
                        OutputText("Request OK - no message available");
                    }
                    else
                    {
                        OutputText("Request OK - reference: " + exportData.ExportMessage.Reference);
                        ExportResultForm form = new ExportResultForm();
                        form.SetClient(client);
                        form.SetExportData(exportData.ExportMessage);
                        form.ShowDialog();

                        StandardWsOutput(form.Response);
                    }
                }
                else
                {
                    OutputText("Request failed: " + exportData.RequestNotOkReasonText);
                }

            }
            catch (Exception ex)
            {
                OutputException(ex);
            }
        }

        private void btnStandardWsSetMessageStatus_Click(object sender, EventArgs e)
        {
            AutoClearOutput();
            OutputText("[set status]");

            StandardWsClient client = GetClient();
            MessageStatusComboboxItem item = (MessageStatusComboboxItem)comboStandardWsMessageStatus.SelectedItem;
            try
            {
                string statusText;

                if (chkNoStatusMessage.Checked)
                {
                    statusText = null;
                }
                else
                {
                    statusText = txtStandardWsSetStatusText.Text;
                }

                SetMessageStatusResponse response = client.SetMessageStatus(txtStandardWsSetStatusTransactionId.Text, item.Status, statusText);
                StandardWsOutput(response);
            }
            catch (Exception ex)
            {
                OutputException(ex);
            }
        }

        private void btnStandardWsSubmitEvent_Click(object sender, EventArgs e)
        {
            AutoClearOutput();
            OutputText("[submit event]");

            StandardWsClient client = GetClient();
            EventLevelComboboxItem item = (EventLevelComboboxItem)comboStandardWsEventLevel.SelectedItem;
            try
            {
                SubmitEventResponse response = client.SubmitEvent(item.EventLevel, txtStandardWsEventText.Text);
                StandardWsOutput(response);
            }
            catch (Exception ex)
            {
                OutputException(ex);
            }
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            comboStandardWsAddress.SelectedIndex = 0;

            comboStandardWsMessageStatus.Items.Add(new MessageStatusComboboxItem(MessageStatusTypeEnum.ExportTransferredProcessingStarted));
            comboStandardWsMessageStatus.Items.Add(new MessageStatusComboboxItem(MessageStatusTypeEnum.ExportTransferredProcessingOk));
            comboStandardWsMessageStatus.Items.Add(new MessageStatusComboboxItem(MessageStatusTypeEnum.ExportTransferredError));
            comboStandardWsMessageStatus.Items.Add(new MessageStatusComboboxItem(MessageStatusTypeEnum.ExportSentReceivedOk));
            comboStandardWsMessageStatus.Items.Add(new MessageStatusComboboxItem(MessageStatusTypeEnum.ExportSentReceivedError));

            comboStandardWsEventLevel.Items.Add(new EventLevelComboboxItem(EventLevelTypeEnum.Info));
            comboStandardWsEventLevel.Items.Add(new EventLevelComboboxItem(EventLevelTypeEnum.Warning));
            comboStandardWsEventLevel.Items.Add(new EventLevelComboboxItem(EventLevelTypeEnum.Error));
            comboStandardWsEventLevel.Items.Add(new EventLevelComboboxItem(EventLevelTypeEnum.Fatal));
            comboStandardWsEventLevel.SelectedIndex = 0;

            EncodingListBoxItem item = new EncodingListBoxItem(Encoding.GetEncoding(1252));
            listStandardWsFileEncoding.Items.Clear();
            listStandardWsFileEncoding.Items.Add(item);
            listStandardWsFileEncoding.SelectedItem = item;
            listStandardMsmqFileEncoding.Items.Clear();
            listStandardMsmqFileEncoding.Items.Add(item);
            listStandardMsmqFileEncoding.SelectedItem = item;

            SetMsmqImportText();
            SetStandardMsmqExportMode(false);
        }

        private void SetMsmqImportText()
        {

            StringBuilder text = new StringBuilder();
            text.AppendLine("NP## UNOC 3 7080005051392                       14   TIMER          70800009XXXX                        14   TIMER          130617 1343 PD-1-4248145                                          ");
            text.AppendLine("NP00             RAVREGN                                                     ");
            text.AppendLine("NP01 7   9   PD-ediexp-1-4248145                   201306171343 201203010000 201203012400   1");
            text.AppendLine("NP03 7080000923168                           9                                       NO                                     ");
            text.AppendLine("NP05 70800009XXXX                            9                                       NO                                     ");
            text.AppendLine("NP07                                                                                                                                                                                                                                              ");
            text.AppendLine("NP10 P_AURL-1                  SM                                       ");
            text.AppendLine("NP11 7080005051392             9                                                                                                                                                                              ");
            text.AppendLine("NP12 1411                                SM  MWH        ");
            text.AppendLine("NP13 201203010000201203010100            Z13         145.900 136 324");
            text.AppendLine("NP13 201203010100201203010200            Z13         132.200 136 324");
            text.AppendLine("NP13 201203010200201203010300            Z13         130.900 136 324");
            text.AppendLine("NP13 201203010300201203010400            Z13         137.900 136 324");
            text.AppendLine("NP13 201203010400201203010500            Z13         185.000 136 324");
            text.AppendLine("NP13 201203010500201203010600            Z13         285.100 136 324");
            text.AppendLine("NP13 201203010600201203010700            Z13         398.700 136 324");
            text.AppendLine("NP13 201203010700201203010800            Z13         470.100 136 324");
            text.AppendLine("NP13 201203010800201203010900            Z13         498.600 136 324");
            text.AppendLine("NP13 201203010900201203011000            Z13         450.400 136 324");
            text.AppendLine("NP13 201203011000201203011100            Z13         489.700 136 324");
            text.AppendLine("NP13 201203011100201203011200            Z13         459.900 136 324");
            text.AppendLine("NP13 201203011200201203011300            Z13         457.900 136 324");
            text.AppendLine("NP13 201203011300201203011400            Z13         415.800 136 324");
            text.AppendLine("NP13 201203011400201203011500            Z13         447.300 136 324");
            text.AppendLine("NP13 201203011500201203011600            Z13         427.300 136 324");
            text.AppendLine("NP13 201203011600201203011700            Z13         419.200 136 324");
            text.AppendLine("NP13 201203011700201203011800            Z13         475.000 136 324");
            text.AppendLine("NP13 201203011800201203011900            Z13         462.700 136 324");
            text.AppendLine("NP13 201203011900201203012000            Z13         468.500 136 324");
            text.AppendLine("NP13 201203012000201203012100            Z13         387.300 136 324");
            text.AppendLine("NP13 201203012100201203012200            Z13         284.200 136 324");
            text.AppendLine("NP13 201203012200201203012300            Z13         164.800 136 324");
            text.AppendLine("NP13 201203012300201203020000            Z13         121.000 136 324");
            text.AppendLine("NP90           8315.400                   ");
            text.AppendLine("NP99");

            txtStandardMsmqImportText.Text = text.ToString();
        }

        private void cbStandardWsImportGenerateTransactionId_CheckedChanged(object sender, EventArgs e)
        {
            txtStandardWsImportTransactionId.Enabled = !cbStandardWsImportGenerateTransactionId.Checked;
        }

        private void btnStandardWsMetadataProtocol_Click(object sender, EventArgs e)
        {
            menuStandardWsMetadataProtocol.Show(btnStandardWsMetadataProtocol, 10, 10);
        }

        private void RemoveMetadataValue(string key)
        {
            string oldRowPrefix = key + ";";
            string[] lines = txtStandardWsImportMetadata.Text.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None);
            StringBuilder newText = new StringBuilder();
            foreach (string line in lines)
            {
                if (!line.StartsWith(oldRowPrefix) && line.Length > 0)
                {
                    newText.AppendLine(line);
                }
            }
            txtStandardWsImportMetadata.Text = newText.ToString();
        }

        private void AddOrReplaceMetadataValue(string key, string value)
        {
            string oldRowPrefix = key + ";";
            string newRow = key + ";" + value;
            string[] lines = txtStandardWsImportMetadata.Text.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None);
            StringBuilder newText = new StringBuilder();
            bool added = false;
            foreach (string line in lines)
            {
                if (line.StartsWith(oldRowPrefix))
                {
                    newText.AppendLine(newRow);
                    added = true;
                }
                else if (line.Length > 0)
                {
                    newText.AppendLine(line);
                }
            }
            if (!added)
            {
                newText.AppendLine(newRow);
            }
            txtStandardWsImportMetadata.Text = newText.ToString();
        }

        private void menuItemProtocol_Click(object sender, EventArgs e)
        {
            var menuItem = sender as ToolStripMenuItem;

            if (menuItem != null)
            {
                AddOrReplaceMetadataValue("protocol", menuItem.Text);
            }
        }

        private void menuItemStandardWsMetadataProtocolRemove_Click(object sender, EventArgs e)
        {
            RemoveMetadataValue("protocol");
        }

        private void menuItemPriority_Click(object sender, EventArgs e)
        {
            var menuItem = sender as ToolStripMenuItem;

            if (menuItem != null)
            {
                AddOrReplaceMetadataValue("priority", menuItem.Text);
            }
        }

        private void menuItemStandardWsMetadataPriorityRemove_Click(object sender, EventArgs e)
        {
            RemoveMetadataValue("priority");
        }

        private void removeExternaltextToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RemoveMetadataValue("externaltext");
        }

        private void someMessageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AddOrReplaceMetadataValue("externaltext", "some text");
        }

        private void menuItemCountry_Click(object sender, EventArgs e)
        {
            var menuItem = sender as ToolStripMenuItem;

            if (menuItem != null)
            {
                AddOrReplaceMetadataValue("country", menuItem.Text);
            }
        }

        private void menuItemStandardWsMetadataCountryRemove_Click(object sender, EventArgs e)
        {
            RemoveMetadataValue("country");
        }

        private void chkNoStatusMessage_CheckedChanged(object sender, EventArgs e)
        {
            var checkbox = sender as CheckBox;

            if (checkbox != null)
            {
                if (checkbox.Checked)
                {
                    txtStandardWsSetStatusText.Enabled = false;
                }
                else
                {
                    txtStandardWsSetStatusText.Enabled = true;
                }
            }
        }

        private void btnStandardMsmqQueueSize_Click(object sender, EventArgs e)
        {
            try
            {
                MessageQueue messageQueue = new MessageQueue(cbStandardMsmqQueuePath.Text);
                txtStandardMsmqQueueSize.Text = CountQueueMessages(messageQueue).ToString(CultureInfo.InvariantCulture);
            }
            catch (Exception ex)
            {
                OutputException(ex);
                OutputText("Hint: is queue path correct?");
            }

        }

        private static int CountQueueMessages(MessageQueue messageQueue)
        {
            int count;

            messageQueue.MessageReadPropertyFilter = new MessagePropertyFilter
                {
                    AdministrationQueue = false,
                    ArrivedTime = false,
                    CorrelationId = false,
                    Priority = false,
                    ResponseQueue = false,
                    SentTime = false,
                    Body = false,
                    Label = false,
                    Id = false
                };

            count = messageQueue.GetAllMessages().Length;

            return count;
        }

        private void btnStandardMsmqGetImport_Click(object sender, EventArgs e)
        {
            bool abort = false;
            OutputText("[submit import]");

            if (rbStandardMsmqImportFile.Checked && txtStandardMsmqImportFile.Text.Trim().Length == 0)
            {
                OutputText("Failed: Please specify a filename");
                abort = true;
            }

            if (!abort)
            {
                string importData;
                if (rbStandardMsmqImportText.Checked)
                {
                    importData = txtStandardMsmqImportText.Text;
                }
                else
                {
                    EncodingListBoxItem encodingItem = (EncodingListBoxItem)listStandardMsmqFileEncoding.SelectedItem;

                    importData = File.ReadAllText(txtStandardMsmqImportFile.Text, encodingItem.GetEncoding());
                }

                try
                {
                    DataExchangeImportMessage msg = new DataExchangeImportMessage();
                    msg.Country = cbStandardMsmqImportCountry.Text;
                    msg.Protocol = cbStandardMsmqImportProtocol.Text;
                    msg.SubAddress = cbStandardMsmqImportSubAddress.Text;
                    msg.SenderName = cbStandardMsmqImportSenderName.Text;
                    msg.ProductCode = cbStandardMsmqImportProductCode.Text;
                    msg.SetMessageData(importData,null);
                    msg.Priority = cbStandardMsmqImportPriority.Text;

                    MessageQueue messageQueue = new MessageQueue(cbStandardMsmqQueuePath.Text);

                    var m = new Message
                        {
                            Formatter = new XmlMessageFormatter(new[] {typeof (DataExchangeImportMessage)}),
                            Body = msg
                        };

                    using (var transactionScope = new TransactionScope(TransactionScopeOption.Required))
                    {
                        messageQueue.Send(m, MessageQueueTransactionType.Automatic);
                        transactionScope.Complete();
                    }

                }
                catch (Exception ex)
                {
                    OutputException(ex);
                }
            }
        }

        private void btnStandardMsmqGetExport_Click(object sender, EventArgs e)
        {
            OutputText("[get export]");

            try
            {
                MessageQueue messageQueue = new MessageQueue(cbStandardMsmqQueuePath.Text);

                Message m;

                using (var transactionScope = new TransactionScope(TransactionScopeOption.Required))
                {
                    m = messageQueue.Receive(TimeSpan.Zero, MessageQueueTransactionType.Automatic);
                    transactionScope.Complete();
                }

                if (m != null)
                {
                    m.Formatter = new XmlMessageFormatter(new[] {typeof (DataExchangeImportMessage)});

                    DataExchangeImportMessage msg = (DataExchangeImportMessage)m.Body;

                    cbStandardMsmqImportCountry.Text = msg.Country;
                    cbStandardMsmqImportProtocol.Text = msg.Protocol;
                    cbStandardMsmqImportSubAddress.Text = msg.SubAddress;
                    cbStandardMsmqImportSenderName.Text = msg.SenderName;
                    cbStandardMsmqImportProductCode.Text = msg.ProductCode;
                    txtStandardMsmqImportText.Text = msg.GetMessageData();
                    cbStandardMsmqImportPriority.Text = msg.Priority;
                }
            }
            catch (MessageQueueException exception)
            {
                if (exception.MessageQueueErrorCode != MessageQueueErrorCode.IOTimeout)
                {
                    OutputText("No message available");
                }
            }
            catch (Exception ex)
            {
                OutputException(ex);
            }
        }

        private void SetStandardWsImportData(bool specifyFile)
        {
            txtStandardWsImportFile.Enabled = specifyFile;
            txtStandardWsImportData.Enabled = !specifyFile;
            btnStandardWsSelectFile.Enabled = specifyFile;
            listStandardWsFileEncoding.Enabled = specifyFile;
        }

        private void rbStandardWsImportText_CheckedChanged(object sender, EventArgs e)
        {
            SetStandardWsImportData(!rbStandardWsImportText.Checked);
        }

        private void rbStandardWsImportFile_CheckedChanged(object sender, EventArgs e)
        {
            SetStandardWsImportData(rbStandardWsImportFile.Checked);
        }

        private void btnStandardWsSelectFile_Click(object sender, EventArgs e)
        {
            DialogResult res = openFileDialog1.ShowDialog();
            if (res == DialogResult.OK)
            {
                string filename = openFileDialog1.FileName;

                if (File.Exists(filename))
                {

                    txtStandardWsImportFile.Text = filename;
                }
                else
                {
                    MessageBox.Show("ERROR: The selected file does not exist");
                    txtStandardWsImportFile.Text = "";
                }
            }
        }

        private void rbStandardMsmqImportText_CheckedChanged(object sender, EventArgs e)
        {
            SetStandardMsmqImportData();
        }

        private void rbStandardMsmqImportFile_CheckedChanged(object sender, EventArgs e)
        {
            SetStandardMsmqImportData();
        }

        private void btnStandardMsmqSelectFile_Click(object sender, EventArgs e)
        {
            DialogResult res = openFileDialog1.ShowDialog();
            if (res == DialogResult.OK)
            {
                string filename = openFileDialog1.FileName;

                if (File.Exists(filename))
                {

                    txtStandardMsmqImportFile.Text = filename;
                }
                else
                {
                    MessageBox.Show("ERROR: The selected file does not exist");
                    txtStandardMsmqImportFile.Text = "";
                }
            }
        }

        private void btnStandardMsmqImport_Click(object sender, EventArgs e)
        {
            SetStandardMsmqExportMode(false);
        }

        private void btnStandardMsmqExport_Click(object sender, EventArgs e)
        {
            SetStandardMsmqExportMode(true);
        }

        private void SetStandardMsmqExportMode(bool exportMode)
        {
            if(exportMode)
            {
                lblStandardMsmqImportExportMode.Text = "Mode: EXPORT";
            }
            else
            {
                lblStandardMsmqImportExportMode.Text = "Mode: IMPORT";
            }

            btnStandardMsmqGetImport.Enabled = !exportMode;
            btnStandardMsmqGetExport.Enabled = exportMode;


            btnStandardMsmqImport.Enabled = exportMode;
            btnStandardMsmqExport.Enabled = !exportMode;

            cbStandardMsmqImportCountry.Enabled = !exportMode;
            cbStandardMsmqImportPriority.Enabled = !exportMode;
            cbStandardMsmqImportProductCode.Enabled = !exportMode;
            cbStandardMsmqImportProtocol.Enabled = !exportMode;
            cbStandardMsmqImportSenderName.Enabled = !exportMode;
            cbStandardMsmqImportSubAddress.Enabled = !exportMode;
            txtStandardMsmqImportText.Enabled = !exportMode;

            SetStandardMsmqImportDataForm(!exportMode);

            if (!exportMode)
            {
                SetStandardMsmqImportData();
            }
        }

        private void SetStandardMsmqImportDataForm(bool enabled)
        {
            rbStandardMsmqImportFile.Enabled = enabled;
            rbStandardMsmqImportText.Enabled = enabled;
            btnStandardMsmqSelectFile.Enabled = enabled;
            txtStandardMsmqImportFile.Enabled = enabled;
            listStandardMsmqFileEncoding.Enabled = enabled;
        }

        private void SetStandardMsmqImportData()
        {
            bool specifyFile = rbStandardMsmqImportFile.Checked;
        
            rbStandardMsmqImportFile.Enabled = true;
            rbStandardMsmqImportText.Enabled = true;

            txtStandardMsmqImportFile.Enabled = specifyFile;
            txtStandardMsmqImportText.Enabled = !specifyFile;
            btnStandardMsmqSelectFile.Enabled = specifyFile;
            listStandardMsmqFileEncoding.Enabled = specifyFile;
        }

        private void btnClearOutput_Click(object sender, EventArgs e)
        {
            ClearOutput();
        }

        private void menuItemMessageName_Click(object sender, EventArgs e)
        {
            var menuItem = sender as ToolStripMenuItem;

            if (menuItem != null)
            {
                AddOrReplaceMetadataValue("messagename", menuItem.Text);
            }   
        }

        private void menuItemStandardWsMetadataMessageNameRemove_Click(object sender, EventArgs e)
        {
            RemoveMetadataValue("messagename");
        }
    }
}
