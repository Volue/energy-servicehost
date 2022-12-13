namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeTestServer
{
    partial class DataExchangeTestServerForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.cbWriteToFile = new System.Windows.Forms.CheckBox();
            this.txtFilePath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.gbRequest = new System.Windows.Forms.GroupBox();
            this.txtRequest = new System.Windows.Forms.TextBox();
            this.btnClear = new System.Windows.Forms.Button();
            this.gbDataFile = new System.Windows.Forms.GroupBox();
            this.cbRawData = new System.Windows.Forms.CheckBox();
            this.gbWCF = new System.Windows.Forms.GroupBox();
            this.txtEndPoints = new System.Windows.Forms.TextBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.gbRequest.SuspendLayout();
            this.gbDataFile.SuspendLayout();
            this.gbWCF.SuspendLayout();
            this.SuspendLayout();
            // 
            // cbWriteToFile
            // 
            this.cbWriteToFile.AutoSize = true;
            this.cbWriteToFile.Location = new System.Drawing.Point(6, 19);
            this.cbWriteToFile.Name = "cbWriteToFile";
            this.cbWriteToFile.Size = new System.Drawing.Size(79, 17);
            this.cbWriteToFile.TabIndex = 0;
            this.cbWriteToFile.Text = "Write to file";
            this.cbWriteToFile.UseVisualStyleBackColor = true;
            this.cbWriteToFile.CheckedChanged += new System.EventHandler(this.cbWriteToFile_CheckedChanged);
            // 
            // txtFilePath
            // 
            this.txtFilePath.Location = new System.Drawing.Point(222, 16);
            this.txtFilePath.Name = "txtFilePath";
            this.txtFilePath.Size = new System.Drawing.Size(358, 20);
            this.txtFilePath.TabIndex = 1;
            this.txtFilePath.TextChanged += new System.EventHandler(this.txtFilePath_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(169, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "File path";
            // 
            // gbRequest
            // 
            this.gbRequest.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gbRequest.Controls.Add(this.txtRequest);
            this.gbRequest.Controls.Add(this.btnClear);
            this.gbRequest.Location = new System.Drawing.Point(6, 138);
            this.gbRequest.Name = "gbRequest";
            this.gbRequest.Size = new System.Drawing.Size(586, 323);
            this.gbRequest.TabIndex = 3;
            this.gbRequest.TabStop = false;
            this.gbRequest.Text = "Request";
            // 
            // txtRequest
            // 
            this.txtRequest.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtRequest.Location = new System.Drawing.Point(9, 20);
            this.txtRequest.Multiline = true;
            this.txtRequest.Name = "txtRequest";
            this.txtRequest.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtRequest.Size = new System.Drawing.Size(571, 264);
            this.txtRequest.TabIndex = 1;
            // 
            // btnClear
            // 
            this.btnClear.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnClear.Location = new System.Drawing.Point(6, 294);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(75, 23);
            this.btnClear.TabIndex = 0;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // gbDataFile
            // 
            this.gbDataFile.Controls.Add(this.cbRawData);
            this.gbDataFile.Controls.Add(this.cbWriteToFile);
            this.gbDataFile.Controls.Add(this.txtFilePath);
            this.gbDataFile.Controls.Add(this.label1);
            this.gbDataFile.Location = new System.Drawing.Point(6, 2);
            this.gbDataFile.Name = "gbDataFile";
            this.gbDataFile.Size = new System.Drawing.Size(586, 46);
            this.gbDataFile.TabIndex = 4;
            this.gbDataFile.TabStop = false;
            this.gbDataFile.Text = "Data File";
            // 
            // cbRawData
            // 
            this.cbRawData.AutoSize = true;
            this.cbRawData.Enabled = false;
            this.cbRawData.Location = new System.Drawing.Point(91, 19);
            this.cbRawData.Name = "cbRawData";
            this.cbRawData.Size = new System.Drawing.Size(72, 17);
            this.cbRawData.TabIndex = 3;
            this.cbRawData.Text = "Raw data";
            this.cbRawData.UseVisualStyleBackColor = true;
            this.cbRawData.CheckedChanged += new System.EventHandler(this.cbRawData_CheckedChanged);
            // 
            // gbWCF
            // 
            this.gbWCF.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gbWCF.Controls.Add(this.txtEndPoints);
            this.gbWCF.Location = new System.Drawing.Point(6, 54);
            this.gbWCF.Name = "gbWCF";
            this.gbWCF.Size = new System.Drawing.Size(586, 86);
            this.gbWCF.TabIndex = 5;
            this.gbWCF.TabStop = false;
            this.gbWCF.Text = "WCF Service Endpoints";
            // 
            // txtEndPoints
            // 
            this.txtEndPoints.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtEndPoints.Location = new System.Drawing.Point(9, 17);
            this.txtEndPoints.Multiline = true;
            this.txtEndPoints.Name = "txtEndPoints";
            this.txtEndPoints.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtEndPoints.Size = new System.Drawing.Size(571, 60);
            this.txtEndPoints.TabIndex = 0;
            // 
            // timer1
            // 
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // DataExchangeTestServerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(610, 473);
            this.Controls.Add(this.gbWCF);
            this.Controls.Add(this.gbDataFile);
            this.Controls.Add(this.gbRequest);
            this.Name = "DataExchangeTestServerForm";
            this.Text = "DataExchangeTestServerForm";
            this.gbRequest.ResumeLayout(false);
            this.gbRequest.PerformLayout();
            this.gbDataFile.ResumeLayout(false);
            this.gbDataFile.PerformLayout();
            this.gbWCF.ResumeLayout(false);
            this.gbWCF.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.CheckBox cbWriteToFile;
        private System.Windows.Forms.TextBox txtFilePath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox gbRequest;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.GroupBox gbDataFile;
        private System.Windows.Forms.GroupBox gbWCF;
        private System.Windows.Forms.TextBox txtRequest;
        private System.Windows.Forms.TextBox txtEndPoints;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.CheckBox cbRawData;
    }
}