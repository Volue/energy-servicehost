using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeTestServer
{
    public partial class DataExchangeTestServerForm : Form
    {
        private static object obj = new object();
        private static string _requestReceived = "";

        public static string GetRequestReceived()
        {
            string sVal = ""; 
            lock (obj)
            {
                sVal = _requestReceived;
            }

            return sVal;
        }

        public static string GetAndResetRequestReceived()
        {
            string sVal = "";
            lock (obj)
            {
                sVal = _requestReceived;
                _requestReceived = "";
            }

            return sVal;
        }

        public static void SetRequestReceived(string msg)
        {
            lock (obj)
            {
                _requestReceived += msg;
            }
        }

        public DataExchangeTestServerForm(List<string> endpoints)
        {
            InitializeComponent();
            timer1.Enabled = true;

            if (Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs == null)
            {
                Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs = new CmdArgs();
            }

            txtFilePath.Text = Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.filePath;
            cbWriteToFile.Checked = Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.writeToFile;
            cbRawData.Checked = Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.rawData;
            _requestReceived = "";
            txtEndPoints.Clear();
            txtEndPoints.Lines = endpoints.ToArray();
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            txtRequest.Clear();
        }

        private void txtFilePath_TextChanged(object sender, EventArgs e)
        {
            Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.filePath = txtFilePath.Text;
        }

        private void cbWriteToFile_CheckedChanged(object sender, EventArgs e)
        {
            Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.writeToFile = cbWriteToFile.Checked;
            cbRawData.Enabled = cbWriteToFile.Checked;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            string s = GetAndResetRequestReceived();

            if (!string.IsNullOrEmpty(s))
            {
                txtRequest.AppendText(s);
            }
        }

        private void cbRawData_CheckedChanged(object sender, EventArgs e)
        {
            Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer.CmdArgs.rawData = cbRawData.Checked;
        }
    }
}
