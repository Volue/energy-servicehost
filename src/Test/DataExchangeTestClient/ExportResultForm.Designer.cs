namespace DataExchangeTestClient
{
    partial class ExportResultForm
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
            this.btnClose = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.lblResponseId = new System.Windows.Forms.Label();
            this.btnAck = new System.Windows.Forms.Button();
            this.txtMetadata = new System.Windows.Forms.TextBox();
            this.txtExportData = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.btnSaveToFile = new System.Windows.Forms.Button();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.SuspendLayout();
            // 
            // btnClose
            // 
            this.btnClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClose.Location = new System.Drawing.Point(385, 223);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(87, 23);
            this.btnClose.TabIndex = 0;
            this.btnClose.Text = "Close";
            this.btnClose.UseVisualStyleBackColor = true;
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Reference:";
            // 
            // lblResponseId
            // 
            this.lblResponseId.AutoSize = true;
            this.lblResponseId.Location = new System.Drawing.Point(81, 9);
            this.lblResponseId.Name = "lblResponseId";
            this.lblResponseId.Size = new System.Drawing.Size(13, 13);
            this.lblResponseId.TabIndex = 4;
            this.lblResponseId.Text = "0";
            // 
            // btnAck
            // 
            this.btnAck.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAck.Enabled = false;
            this.btnAck.Location = new System.Drawing.Point(292, 223);
            this.btnAck.Name = "btnAck";
            this.btnAck.Size = new System.Drawing.Size(87, 23);
            this.btnAck.TabIndex = 8;
            this.btnAck.Text = "Send ACK";
            this.btnAck.UseVisualStyleBackColor = true;
            this.btnAck.Click += new System.EventHandler(this.btnAck_Click);
            // 
            // txtMetadata
            // 
            this.txtMetadata.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.txtMetadata.Location = new System.Drawing.Point(12, 56);
            this.txtMetadata.Multiline = true;
            this.txtMetadata.Name = "txtMetadata";
            this.txtMetadata.ReadOnly = true;
            this.txtMetadata.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtMetadata.Size = new System.Drawing.Size(143, 142);
            this.txtMetadata.TabIndex = 9;
            // 
            // txtExportData
            // 
            this.txtExportData.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtExportData.Location = new System.Drawing.Point(161, 56);
            this.txtExportData.Multiline = true;
            this.txtExportData.Name = "txtExportData";
            this.txtExportData.ReadOnly = true;
            this.txtExportData.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtExportData.Size = new System.Drawing.Size(311, 142);
            this.txtExportData.TabIndex = 10;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(16, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 11;
            this.label2.Text = "Metadata:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(161, 40);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(64, 13);
            this.label3.TabIndex = 12;
            this.label3.Text = "Export data:";
            // 
            // btnSaveToFile
            // 
            this.btnSaveToFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSaveToFile.Location = new System.Drawing.Point(199, 223);
            this.btnSaveToFile.Name = "btnSaveToFile";
            this.btnSaveToFile.Size = new System.Drawing.Size(87, 23);
            this.btnSaveToFile.TabIndex = 13;
            this.btnSaveToFile.Text = "Save to file";
            this.btnSaveToFile.UseVisualStyleBackColor = true;
            this.btnSaveToFile.Click += new System.EventHandler(this.btnSaveToFile_Click);
            // 
            // ExportResultForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(480, 249);
            this.Controls.Add(this.btnSaveToFile);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtExportData);
            this.Controls.Add(this.txtMetadata);
            this.Controls.Add(this.btnAck);
            this.Controls.Add(this.lblResponseId);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnClose);
            this.Name = "ExportResultForm";
            this.Text = "ExportResultForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnClose;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblResponseId;
        private System.Windows.Forms.Button btnAck;
        private System.Windows.Forms.TextBox txtMetadata;
        private System.Windows.Forms.TextBox txtExportData;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnSaveToFile;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
    }
}