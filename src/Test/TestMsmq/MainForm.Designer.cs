namespace TestMsmq
{
    partial class MainForm
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
            this.bgwProducer = new System.ComponentModel.BackgroundWorker();
            this.bgwConsumer = new System.ComponentModel.BackgroundWorker();
            this.bgwQueueStatus = new System.ComponentModel.BackgroundWorker();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.listBoxLabOutput = new System.Windows.Forms.ListBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.btnGetPublicQueues = new System.Windows.Forms.Button();
            this.btnLabPurgeQueue = new System.Windows.Forms.Button();
            this.txtLabLabMsgPeekByCorrelationId = new System.Windows.Forms.TextBox();
            this.txtLabLabMsgPeekById = new System.Windows.Forms.TextBox();
            this.btnLabMsgPeekByCorrelationId = new System.Windows.Forms.Button();
            this.btnLabMsgPeekById = new System.Windows.Forms.Button();
            this.btnLabMsgPeek = new System.Windows.Forms.Button();
            this.chkLabReadTimeout = new System.Windows.Forms.CheckBox();
            this.btnLabMsgReceive = new System.Windows.Forms.Button();
            this.txtLabReadTimeoutMillis = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label24 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsCorrelationId = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsSourceMachine = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsSenderId = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsLookupId = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsPriority = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsBody = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsLabel = new System.Windows.Forms.Label();
            this.lblLabMsgDetailsId = new System.Windows.Forms.Label();
            this.btnLabClearMsgDetails = new System.Windows.Forms.Button();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.comboLabQueueName = new System.Windows.Forms.ComboBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lblTimeToReachQueue = new System.Windows.Forms.Label();
            this.labelTimeToBeReceived = new System.Windows.Forms.Label();
            this.txtTimeToReachQueue = new System.Windows.Forms.TextBox();
            this.txtTimeToBeReceived = new System.Windows.Forms.TextBox();
            this.radLabMsgSendPriority8 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority7 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority6 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority5 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority4 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority3 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority2 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority1 = new System.Windows.Forms.RadioButton();
            this.radLabMsgSendPriority0 = new System.Windows.Forms.RadioButton();
            this.chkLabMsgAutoLabel = new System.Windows.Forms.CheckBox();
            this.chkAttachSenderId = new System.Windows.Forms.CheckBox();
            this.txtLabMsgBody = new System.Windows.Forms.TextBox();
            this.txtLabMsgLabel = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.btnLabMsgSend = new System.Windows.Forms.Button();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBoxOutput = new System.Windows.Forms.GroupBox();
            this.listBoxOutput = new System.Windows.Forms.ListBox();
            this.groupBoxQueue = new System.Windows.Forms.GroupBox();
            this.label10 = new System.Windows.Forms.Label();
            this.txtAutoRefrehInterval = new System.Windows.Forms.TextBox();
            this.chkAutoRefrehInterval = new System.Windows.Forms.CheckBox();
            this.lblQueueSize = new System.Windows.Forms.Label();
            this.btnClearQueue = new System.Windows.Forms.Button();
            this.btnRefreshQueueInfo = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBoxConsumer = new System.Windows.Forms.GroupBox();
            this.lblConsumerLastMessageElapsedTime = new System.Windows.Forms.Label();
            this.lblReceivedData = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.lblLatestMessage = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.lblReceivedMessages = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.btnStopConsumer = new System.Windows.Forms.Button();
            this.btnStartConsumer = new System.Windows.Forms.Button();
            this.groupBoxProducer = new System.Windows.Forms.GroupBox();
            this.lblProducerStartTimeStamp = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.lblProducerSentData = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.lblElapsedTimeProducer = new System.Windows.Forms.Label();
            this.progressBarProducer = new System.Windows.Forms.ProgressBar();
            this.lblSentMessageLabel = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnCancelProducer = new System.Windows.Forms.Button();
            this.btnStartProducer = new System.Windows.Forms.Button();
            this.txtDelayInMillis = new System.Windows.Forms.TextBox();
            this.txtNumberOfMessages = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtMessageSizeKB = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.bgwLabReadQueue = new System.ComponentModel.BackgroundWorker();
            this.cbDeadLetterJournal = new System.Windows.Forms.CheckBox();
            this.cbUseJournalQueue = new System.Windows.Forms.CheckBox();
            this.cbTransactionalMessage = new System.Windows.Forms.CheckBox();
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupBoxOutput.SuspendLayout();
            this.groupBoxQueue.SuspendLayout();
            this.groupBoxConsumer.SuspendLayout();
            this.groupBoxProducer.SuspendLayout();
            this.SuspendLayout();
            // 
            // bgwProducer
            // 
            this.bgwProducer.WorkerReportsProgress = true;
            this.bgwProducer.WorkerSupportsCancellation = true;
            this.bgwProducer.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgwProducer_DoWork);
            this.bgwProducer.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgwProducer_ProgressChanged);
            this.bgwProducer.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgwProducer_RunWorkerCompleted);
            // 
            // bgwConsumer
            // 
            this.bgwConsumer.WorkerReportsProgress = true;
            this.bgwConsumer.WorkerSupportsCancellation = true;
            this.bgwConsumer.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgwConsumer_DoWork);
            this.bgwConsumer.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgwConsumer_ProgressChanged);
            this.bgwConsumer.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgwConsumer_RunWorkerCompleted);
            // 
            // bgwQueueStatus
            // 
            this.bgwQueueStatus.WorkerReportsProgress = true;
            this.bgwQueueStatus.WorkerSupportsCancellation = true;
            this.bgwQueueStatus.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgwQueueStatus_DoWork);
            this.bgwQueueStatus.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgwQueueStatus_ProgressChanged);
            this.bgwQueueStatus.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgwQueueStatus_RunWorkerCompleted);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(870, 546);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox5);
            this.tabPage2.Controls.Add(this.groupBox4);
            this.tabPage2.Controls.Add(this.groupBox3);
            this.tabPage2.Controls.Add(this.groupBox2);
            this.tabPage2.Controls.Add(this.groupBox1);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(862, 520);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Lab";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.listBoxLabOutput);
            this.groupBox5.Location = new System.Drawing.Point(6, 279);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(412, 235);
            this.groupBox5.TabIndex = 8;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Output";
            // 
            // listBoxLabOutput
            // 
            this.listBoxLabOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBoxLabOutput.FormattingEnabled = true;
            this.listBoxLabOutput.Location = new System.Drawing.Point(3, 16);
            this.listBoxLabOutput.Name = "listBoxLabOutput";
            this.listBoxLabOutput.Size = new System.Drawing.Size(406, 216);
            this.listBoxLabOutput.TabIndex = 0;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.btnGetPublicQueues);
            this.groupBox4.Controls.Add(this.btnLabPurgeQueue);
            this.groupBox4.Controls.Add(this.txtLabLabMsgPeekByCorrelationId);
            this.groupBox4.Controls.Add(this.txtLabLabMsgPeekById);
            this.groupBox4.Controls.Add(this.btnLabMsgPeekByCorrelationId);
            this.groupBox4.Controls.Add(this.btnLabMsgPeekById);
            this.groupBox4.Controls.Add(this.btnLabMsgPeek);
            this.groupBox4.Controls.Add(this.chkLabReadTimeout);
            this.groupBox4.Controls.Add(this.btnLabMsgReceive);
            this.groupBox4.Controls.Add(this.txtLabReadTimeoutMillis);
            this.groupBox4.Location = new System.Drawing.Point(424, 53);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(426, 221);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Read queue";
            // 
            // btnGetPublicQueues
            // 
            this.btnGetPublicQueues.Location = new System.Drawing.Point(6, 168);
            this.btnGetPublicQueues.Name = "btnGetPublicQueues";
            this.btnGetPublicQueues.Size = new System.Drawing.Size(147, 23);
            this.btnGetPublicQueues.TabIndex = 14;
            this.btnGetPublicQueues.Text = "GetPublicQueues";
            this.btnGetPublicQueues.UseVisualStyleBackColor = true;
            this.btnGetPublicQueues.Click += new System.EventHandler(this.btnGetPublicQueues_Click);
            // 
            // btnLabPurgeQueue
            // 
            this.btnLabPurgeQueue.Location = new System.Drawing.Point(273, 19);
            this.btnLabPurgeQueue.Name = "btnLabPurgeQueue";
            this.btnLabPurgeQueue.Size = new System.Drawing.Size(147, 23);
            this.btnLabPurgeQueue.TabIndex = 13;
            this.btnLabPurgeQueue.Text = "Purge queue";
            this.btnLabPurgeQueue.UseVisualStyleBackColor = true;
            this.btnLabPurgeQueue.Click += new System.EventHandler(this.btnLabPurgeQueue_Click);
            // 
            // txtLabLabMsgPeekByCorrelationId
            // 
            this.txtLabLabMsgPeekByCorrelationId.Location = new System.Drawing.Point(159, 106);
            this.txtLabLabMsgPeekByCorrelationId.Name = "txtLabLabMsgPeekByCorrelationId";
            this.txtLabLabMsgPeekByCorrelationId.Size = new System.Drawing.Size(100, 20);
            this.txtLabLabMsgPeekByCorrelationId.TabIndex = 12;
            // 
            // txtLabLabMsgPeekById
            // 
            this.txtLabLabMsgPeekById.Location = new System.Drawing.Point(159, 79);
            this.txtLabLabMsgPeekById.Name = "txtLabLabMsgPeekById";
            this.txtLabLabMsgPeekById.Size = new System.Drawing.Size(100, 20);
            this.txtLabLabMsgPeekById.TabIndex = 11;
            // 
            // btnLabMsgPeekByCorrelationId
            // 
            this.btnLabMsgPeekByCorrelationId.Location = new System.Drawing.Point(6, 106);
            this.btnLabMsgPeekByCorrelationId.Name = "btnLabMsgPeekByCorrelationId";
            this.btnLabMsgPeekByCorrelationId.Size = new System.Drawing.Size(147, 23);
            this.btnLabMsgPeekByCorrelationId.TabIndex = 10;
            this.btnLabMsgPeekByCorrelationId.Text = "PeekByCorrelationId";
            this.btnLabMsgPeekByCorrelationId.UseVisualStyleBackColor = true;
            this.btnLabMsgPeekByCorrelationId.Click += new System.EventHandler(this.btnLabMsgPeekByCorrelationId_Click);
            // 
            // btnLabMsgPeekById
            // 
            this.btnLabMsgPeekById.Location = new System.Drawing.Point(6, 77);
            this.btnLabMsgPeekById.Name = "btnLabMsgPeekById";
            this.btnLabMsgPeekById.Size = new System.Drawing.Size(147, 23);
            this.btnLabMsgPeekById.TabIndex = 9;
            this.btnLabMsgPeekById.Text = "PeekById";
            this.btnLabMsgPeekById.UseVisualStyleBackColor = true;
            this.btnLabMsgPeekById.Click += new System.EventHandler(this.btnLabMsgPeekById_Click);
            // 
            // btnLabMsgPeek
            // 
            this.btnLabMsgPeek.Location = new System.Drawing.Point(6, 48);
            this.btnLabMsgPeek.Name = "btnLabMsgPeek";
            this.btnLabMsgPeek.Size = new System.Drawing.Size(147, 23);
            this.btnLabMsgPeek.TabIndex = 7;
            this.btnLabMsgPeek.Text = "Peek";
            this.btnLabMsgPeek.UseVisualStyleBackColor = true;
            this.btnLabMsgPeek.Click += new System.EventHandler(this.btnLabMsgPeek_Click);
            // 
            // chkLabReadTimeout
            // 
            this.chkLabReadTimeout.AutoSize = true;
            this.chkLabReadTimeout.Checked = true;
            this.chkLabReadTimeout.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkLabReadTimeout.Location = new System.Drawing.Point(5, 139);
            this.chkLabReadTimeout.Name = "chkLabReadTimeout";
            this.chkLabReadTimeout.Size = new System.Drawing.Size(94, 17);
            this.chkLabReadTimeout.TabIndex = 8;
            this.chkLabReadTimeout.Text = "Timeout (millis)";
            this.chkLabReadTimeout.UseVisualStyleBackColor = true;
            this.chkLabReadTimeout.CheckedChanged += new System.EventHandler(this.chkLabReadTimeout_CheckedChanged);
            // 
            // btnLabMsgReceive
            // 
            this.btnLabMsgReceive.Location = new System.Drawing.Point(6, 19);
            this.btnLabMsgReceive.Name = "btnLabMsgReceive";
            this.btnLabMsgReceive.Size = new System.Drawing.Size(147, 23);
            this.btnLabMsgReceive.TabIndex = 0;
            this.btnLabMsgReceive.Text = "Receive";
            this.btnLabMsgReceive.UseVisualStyleBackColor = true;
            this.btnLabMsgReceive.Click += new System.EventHandler(this.btnLabMsgReceive_Click);
            // 
            // txtLabReadTimeoutMillis
            // 
            this.txtLabReadTimeoutMillis.Location = new System.Drawing.Point(98, 137);
            this.txtLabReadTimeoutMillis.Name = "txtLabReadTimeoutMillis";
            this.txtLabReadTimeoutMillis.Size = new System.Drawing.Size(55, 20);
            this.txtLabReadTimeoutMillis.TabIndex = 7;
            this.txtLabReadTimeoutMillis.Text = "1000";
            this.txtLabReadTimeoutMillis.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label24);
            this.groupBox3.Controls.Add(this.label18);
            this.groupBox3.Controls.Add(this.label17);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsCorrelationId);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsSourceMachine);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsSenderId);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsLookupId);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsPriority);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsBody);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsLabel);
            this.groupBox3.Controls.Add(this.lblLabMsgDetailsId);
            this.groupBox3.Controls.Add(this.btnLabClearMsgDetails);
            this.groupBox3.Controls.Add(this.label19);
            this.groupBox3.Controls.Add(this.label20);
            this.groupBox3.Controls.Add(this.label21);
            this.groupBox3.Controls.Add(this.label22);
            this.groupBox3.Controls.Add(this.label23);
            this.groupBox3.Location = new System.Drawing.Point(424, 279);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(426, 235);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Message details";
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(10, 138);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(50, 13);
            this.label24.TabIndex = 17;
            this.label24.Text = "SenderId";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(10, 160);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(82, 13);
            this.label18.TabIndex = 16;
            this.label18.Text = "SourceMachine";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(10, 183);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(66, 13);
            this.label17.TabIndex = 15;
            this.label17.Text = "CorrelationId";
            // 
            // lblLabMsgDetailsCorrelationId
            // 
            this.lblLabMsgDetailsCorrelationId.AutoSize = true;
            this.lblLabMsgDetailsCorrelationId.Location = new System.Drawing.Point(95, 183);
            this.lblLabMsgDetailsCorrelationId.Name = "lblLabMsgDetailsCorrelationId";
            this.lblLabMsgDetailsCorrelationId.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsCorrelationId.TabIndex = 14;
            this.lblLabMsgDetailsCorrelationId.Text = "label17";
            // 
            // lblLabMsgDetailsSourceMachine
            // 
            this.lblLabMsgDetailsSourceMachine.AutoSize = true;
            this.lblLabMsgDetailsSourceMachine.Location = new System.Drawing.Point(95, 158);
            this.lblLabMsgDetailsSourceMachine.Name = "lblLabMsgDetailsSourceMachine";
            this.lblLabMsgDetailsSourceMachine.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsSourceMachine.TabIndex = 13;
            this.lblLabMsgDetailsSourceMachine.Text = "label17";
            // 
            // lblLabMsgDetailsSenderId
            // 
            this.lblLabMsgDetailsSenderId.AutoSize = true;
            this.lblLabMsgDetailsSenderId.Location = new System.Drawing.Point(95, 132);
            this.lblLabMsgDetailsSenderId.Name = "lblLabMsgDetailsSenderId";
            this.lblLabMsgDetailsSenderId.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsSenderId.TabIndex = 12;
            this.lblLabMsgDetailsSenderId.Text = "label17";
            // 
            // lblLabMsgDetailsLookupId
            // 
            this.lblLabMsgDetailsLookupId.AutoSize = true;
            this.lblLabMsgDetailsLookupId.Location = new System.Drawing.Point(95, 108);
            this.lblLabMsgDetailsLookupId.Name = "lblLabMsgDetailsLookupId";
            this.lblLabMsgDetailsLookupId.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsLookupId.TabIndex = 11;
            this.lblLabMsgDetailsLookupId.Text = "label17";
            // 
            // lblLabMsgDetailsPriority
            // 
            this.lblLabMsgDetailsPriority.AutoSize = true;
            this.lblLabMsgDetailsPriority.Location = new System.Drawing.Point(95, 83);
            this.lblLabMsgDetailsPriority.Name = "lblLabMsgDetailsPriority";
            this.lblLabMsgDetailsPriority.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsPriority.TabIndex = 10;
            this.lblLabMsgDetailsPriority.Text = "label17";
            // 
            // lblLabMsgDetailsBody
            // 
            this.lblLabMsgDetailsBody.AutoSize = true;
            this.lblLabMsgDetailsBody.Location = new System.Drawing.Point(95, 59);
            this.lblLabMsgDetailsBody.Name = "lblLabMsgDetailsBody";
            this.lblLabMsgDetailsBody.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsBody.TabIndex = 9;
            this.lblLabMsgDetailsBody.Text = "label17";
            // 
            // lblLabMsgDetailsLabel
            // 
            this.lblLabMsgDetailsLabel.AutoSize = true;
            this.lblLabMsgDetailsLabel.Location = new System.Drawing.Point(95, 34);
            this.lblLabMsgDetailsLabel.Name = "lblLabMsgDetailsLabel";
            this.lblLabMsgDetailsLabel.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsLabel.TabIndex = 8;
            this.lblLabMsgDetailsLabel.Text = "label17";
            // 
            // lblLabMsgDetailsId
            // 
            this.lblLabMsgDetailsId.AutoSize = true;
            this.lblLabMsgDetailsId.Location = new System.Drawing.Point(95, 11);
            this.lblLabMsgDetailsId.Name = "lblLabMsgDetailsId";
            this.lblLabMsgDetailsId.Size = new System.Drawing.Size(41, 13);
            this.lblLabMsgDetailsId.TabIndex = 7;
            this.lblLabMsgDetailsId.Text = "label17";
            // 
            // btnLabClearMsgDetails
            // 
            this.btnLabClearMsgDetails.Location = new System.Drawing.Point(13, 199);
            this.btnLabClearMsgDetails.Name = "btnLabClearMsgDetails";
            this.btnLabClearMsgDetails.Size = new System.Drawing.Size(75, 23);
            this.btnLabClearMsgDetails.TabIndex = 6;
            this.btnLabClearMsgDetails.Text = "Clear";
            this.btnLabClearMsgDetails.UseVisualStyleBackColor = true;
            this.btnLabClearMsgDetails.Click += new System.EventHandler(this.btnLabClearMsgDetails_Click);
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(10, 113);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(52, 13);
            this.label19.TabIndex = 5;
            this.label19.Text = "LookupId";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(10, 16);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(16, 13);
            this.label20.TabIndex = 4;
            this.label20.Text = "Id";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(10, 88);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(38, 13);
            this.label21.TabIndex = 3;
            this.label21.Text = "Priority";
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(10, 64);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(31, 13);
            this.label22.TabIndex = 2;
            this.label22.Text = "Body";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(10, 39);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(33, 13);
            this.label23.TabIndex = 1;
            this.label23.Text = "Label";
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.comboLabQueueName);
            this.groupBox2.Location = new System.Drawing.Point(6, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(844, 44);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Queue";
            // 
            // comboLabQueueName
            // 
            this.comboLabQueueName.FormattingEnabled = true;
            this.comboLabQueueName.Items.AddRange(new object[] {
            @"FormatName:Direct=OS:smg-test-sv2\Private$\ICC_IMPORT_STANDARD"});
            this.comboLabQueueName.Location = new System.Drawing.Point(6, 15);
            this.comboLabQueueName.Name = "comboLabQueueName";
            this.comboLabQueueName.Size = new System.Drawing.Size(306, 21);
            this.comboLabQueueName.TabIndex = 0;
            this.comboLabQueueName.Text = @"FormatName:Direct=OS:smg-test-sv2\Private$\ICC_IMPORT_STANDARD";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cbTransactionalMessage);
            this.groupBox1.Controls.Add(this.cbUseJournalQueue);
            this.groupBox1.Controls.Add(this.cbDeadLetterJournal);
            this.groupBox1.Controls.Add(this.lblTimeToReachQueue);
            this.groupBox1.Controls.Add(this.labelTimeToBeReceived);
            this.groupBox1.Controls.Add(this.txtTimeToReachQueue);
            this.groupBox1.Controls.Add(this.txtTimeToBeReceived);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority8);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority7);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority6);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority5);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority4);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority3);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority2);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority1);
            this.groupBox1.Controls.Add(this.radLabMsgSendPriority0);
            this.groupBox1.Controls.Add(this.chkLabMsgAutoLabel);
            this.groupBox1.Controls.Add(this.chkAttachSenderId);
            this.groupBox1.Controls.Add(this.txtLabMsgBody);
            this.groupBox1.Controls.Add(this.txtLabMsgLabel);
            this.groupBox1.Controls.Add(this.label15);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.btnLabMsgSend);
            this.groupBox1.Location = new System.Drawing.Point(6, 53);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(412, 221);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Send message";
            // 
            // lblTimeToReachQueue
            // 
            this.lblTimeToReachQueue.AutoSize = true;
            this.lblTimeToReachQueue.Location = new System.Drawing.Point(9, 116);
            this.lblTimeToReachQueue.Name = "lblTimeToReachQueue";
            this.lblTimeToReachQueue.Size = new System.Drawing.Size(86, 13);
            this.lblTimeToReachQueue.TabIndex = 22;
            this.lblTimeToReachQueue.Text = "TTRQ (seconds)";
            // 
            // labelTimeToBeReceived
            // 
            this.labelTimeToBeReceived.AutoSize = true;
            this.labelTimeToBeReceived.Location = new System.Drawing.Point(9, 94);
            this.labelTimeToBeReceived.Name = "labelTimeToBeReceived";
            this.labelTimeToBeReceived.Size = new System.Drawing.Size(85, 13);
            this.labelTimeToBeReceived.TabIndex = 20;
            this.labelTimeToBeReceived.Text = "TTBR (seconds)";
            // 
            // txtTimeToReachQueue
            // 
            this.txtTimeToReachQueue.Location = new System.Drawing.Point(100, 116);
            this.txtTimeToReachQueue.Name = "txtTimeToReachQueue";
            this.txtTimeToReachQueue.Size = new System.Drawing.Size(100, 20);
            this.txtTimeToReachQueue.TabIndex = 19;
            // 
            // txtTimeToBeReceived
            // 
            this.txtTimeToBeReceived.Location = new System.Drawing.Point(100, 91);
            this.txtTimeToBeReceived.Name = "txtTimeToBeReceived";
            this.txtTimeToBeReceived.Size = new System.Drawing.Size(100, 20);
            this.txtTimeToBeReceived.TabIndex = 18;
            // 
            // radLabMsgSendPriority8
            // 
            this.radLabMsgSendPriority8.AutoSize = true;
            this.radLabMsgSendPriority8.Location = new System.Drawing.Point(276, 139);
            this.radLabMsgSendPriority8.Name = "radLabMsgSendPriority8";
            this.radLabMsgSendPriority8.Size = new System.Drawing.Size(59, 17);
            this.radLabMsgSendPriority8.TabIndex = 17;
            this.radLabMsgSendPriority8.Text = "Lowest";
            this.radLabMsgSendPriority8.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority7
            // 
            this.radLabMsgSendPriority7.AutoSize = true;
            this.radLabMsgSendPriority7.Location = new System.Drawing.Point(276, 124);
            this.radLabMsgSendPriority7.Name = "radLabMsgSendPriority7";
            this.radLabMsgSendPriority7.Size = new System.Drawing.Size(65, 17);
            this.radLabMsgSendPriority7.TabIndex = 16;
            this.radLabMsgSendPriority7.Text = "Very low";
            this.radLabMsgSendPriority7.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority6
            // 
            this.radLabMsgSendPriority6.AutoSize = true;
            this.radLabMsgSendPriority6.Location = new System.Drawing.Point(276, 109);
            this.radLabMsgSendPriority6.Name = "radLabMsgSendPriority6";
            this.radLabMsgSendPriority6.Size = new System.Drawing.Size(45, 17);
            this.radLabMsgSendPriority6.TabIndex = 15;
            this.radLabMsgSendPriority6.Text = "Low";
            this.radLabMsgSendPriority6.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority5
            // 
            this.radLabMsgSendPriority5.AutoSize = true;
            this.radLabMsgSendPriority5.Location = new System.Drawing.Point(276, 94);
            this.radLabMsgSendPriority5.Name = "radLabMsgSendPriority5";
            this.radLabMsgSendPriority5.Size = new System.Drawing.Size(58, 17);
            this.radLabMsgSendPriority5.TabIndex = 14;
            this.radLabMsgSendPriority5.Text = "Normal";
            this.radLabMsgSendPriority5.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority4
            // 
            this.radLabMsgSendPriority4.AutoSize = true;
            this.radLabMsgSendPriority4.Location = new System.Drawing.Point(276, 79);
            this.radLabMsgSendPriority4.Name = "radLabMsgSendPriority4";
            this.radLabMsgSendPriority4.Size = new System.Drawing.Size(90, 17);
            this.radLabMsgSendPriority4.TabIndex = 13;
            this.radLabMsgSendPriority4.Text = "Above normal";
            this.radLabMsgSendPriority4.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority3
            // 
            this.radLabMsgSendPriority3.AutoSize = true;
            this.radLabMsgSendPriority3.Location = new System.Drawing.Point(276, 64);
            this.radLabMsgSendPriority3.Name = "radLabMsgSendPriority3";
            this.radLabMsgSendPriority3.Size = new System.Drawing.Size(47, 17);
            this.radLabMsgSendPriority3.TabIndex = 12;
            this.radLabMsgSendPriority3.Text = "High";
            this.radLabMsgSendPriority3.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority2
            // 
            this.radLabMsgSendPriority2.AutoSize = true;
            this.radLabMsgSendPriority2.Location = new System.Drawing.Point(276, 49);
            this.radLabMsgSendPriority2.Name = "radLabMsgSendPriority2";
            this.radLabMsgSendPriority2.Size = new System.Drawing.Size(69, 17);
            this.radLabMsgSendPriority2.TabIndex = 11;
            this.radLabMsgSendPriority2.Text = "Very high";
            this.radLabMsgSendPriority2.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority1
            // 
            this.radLabMsgSendPriority1.AutoSize = true;
            this.radLabMsgSendPriority1.Location = new System.Drawing.Point(276, 34);
            this.radLabMsgSendPriority1.Name = "radLabMsgSendPriority1";
            this.radLabMsgSendPriority1.Size = new System.Drawing.Size(61, 17);
            this.radLabMsgSendPriority1.TabIndex = 10;
            this.radLabMsgSendPriority1.Text = "Highest";
            this.radLabMsgSendPriority1.UseVisualStyleBackColor = true;
            // 
            // radLabMsgSendPriority0
            // 
            this.radLabMsgSendPriority0.AutoSize = true;
            this.radLabMsgSendPriority0.Checked = true;
            this.radLabMsgSendPriority0.Location = new System.Drawing.Point(276, 19);
            this.radLabMsgSendPriority0.Name = "radLabMsgSendPriority0";
            this.radLabMsgSendPriority0.Size = new System.Drawing.Size(127, 17);
            this.radLabMsgSendPriority0.TabIndex = 9;
            this.radLabMsgSendPriority0.TabStop = true;
            this.radLabMsgSendPriority0.Text = "<no priority specified>";
            this.radLabMsgSendPriority0.UseVisualStyleBackColor = true;
            // 
            // chkLabMsgAutoLabel
            // 
            this.chkLabMsgAutoLabel.AutoSize = true;
            this.chkLabMsgAutoLabel.Checked = true;
            this.chkLabMsgAutoLabel.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkLabMsgAutoLabel.Location = new System.Drawing.Point(175, 25);
            this.chkLabMsgAutoLabel.Name = "chkLabMsgAutoLabel";
            this.chkLabMsgAutoLabel.Size = new System.Drawing.Size(73, 17);
            this.chkLabMsgAutoLabel.TabIndex = 8;
            this.chkLabMsgAutoLabel.Text = "Auto-label";
            this.chkLabMsgAutoLabel.UseVisualStyleBackColor = true;
            this.chkLabMsgAutoLabel.CheckedChanged += new System.EventHandler(this.chkLabMsgAutoLabel_CheckedChanged);
            // 
            // chkAttachSenderId
            // 
            this.chkAttachSenderId.AutoSize = true;
            this.chkAttachSenderId.Location = new System.Drawing.Point(12, 74);
            this.chkAttachSenderId.Name = "chkAttachSenderId";
            this.chkAttachSenderId.Size = new System.Drawing.Size(106, 17);
            this.chkAttachSenderId.TabIndex = 7;
            this.chkAttachSenderId.Text = "Attach sender ID";
            this.chkAttachSenderId.UseVisualStyleBackColor = true;
            // 
            // txtLabMsgBody
            // 
            this.txtLabMsgBody.Location = new System.Drawing.Point(48, 48);
            this.txtLabMsgBody.Name = "txtLabMsgBody";
            this.txtLabMsgBody.Size = new System.Drawing.Size(200, 20);
            this.txtLabMsgBody.TabIndex = 6;
            this.txtLabMsgBody.Text = "body1";
            // 
            // txtLabMsgLabel
            // 
            this.txtLabMsgLabel.Enabled = false;
            this.txtLabMsgLabel.Location = new System.Drawing.Point(48, 22);
            this.txtLabMsgLabel.Name = "txtLabMsgLabel";
            this.txtLabMsgLabel.Size = new System.Drawing.Size(121, 20);
            this.txtLabMsgLabel.TabIndex = 5;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(9, 53);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(31, 13);
            this.label15.TabIndex = 2;
            this.label15.Text = "Body";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(9, 25);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(33, 13);
            this.label13.TabIndex = 1;
            this.label13.Text = "Label";
            // 
            // btnLabMsgSend
            // 
            this.btnLabMsgSend.Location = new System.Drawing.Point(155, 186);
            this.btnLabMsgSend.Name = "btnLabMsgSend";
            this.btnLabMsgSend.Size = new System.Drawing.Size(75, 23);
            this.btnLabMsgSend.TabIndex = 0;
            this.btnLabMsgSend.Text = "Send";
            this.btnLabMsgSend.UseVisualStyleBackColor = true;
            this.btnLabMsgSend.Click += new System.EventHandler(this.btnLabMsgSend_Click);
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBoxOutput);
            this.tabPage1.Controls.Add(this.groupBoxQueue);
            this.tabPage1.Controls.Add(this.groupBoxConsumer);
            this.tabPage1.Controls.Add(this.groupBoxProducer);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(862, 520);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Performance";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBoxOutput
            // 
            this.groupBoxOutput.Controls.Add(this.listBoxOutput);
            this.groupBoxOutput.Location = new System.Drawing.Point(6, 279);
            this.groupBoxOutput.Name = "groupBoxOutput";
            this.groupBoxOutput.Size = new System.Drawing.Size(717, 235);
            this.groupBoxOutput.TabIndex = 7;
            this.groupBoxOutput.TabStop = false;
            this.groupBoxOutput.Text = "Output";
            // 
            // listBoxOutput
            // 
            this.listBoxOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBoxOutput.FormattingEnabled = true;
            this.listBoxOutput.Location = new System.Drawing.Point(3, 16);
            this.listBoxOutput.Name = "listBoxOutput";
            this.listBoxOutput.Size = new System.Drawing.Size(711, 216);
            this.listBoxOutput.TabIndex = 0;
            // 
            // groupBoxQueue
            // 
            this.groupBoxQueue.Controls.Add(this.label10);
            this.groupBoxQueue.Controls.Add(this.txtAutoRefrehInterval);
            this.groupBoxQueue.Controls.Add(this.chkAutoRefrehInterval);
            this.groupBoxQueue.Controls.Add(this.lblQueueSize);
            this.groupBoxQueue.Controls.Add(this.btnClearQueue);
            this.groupBoxQueue.Controls.Add(this.btnRefreshQueueInfo);
            this.groupBoxQueue.Controls.Add(this.label5);
            this.groupBoxQueue.Location = new System.Drawing.Point(247, 6);
            this.groupBoxQueue.Name = "groupBoxQueue";
            this.groupBoxQueue.Size = new System.Drawing.Size(235, 267);
            this.groupBoxQueue.TabIndex = 6;
            this.groupBoxQueue.TabStop = false;
            this.groupBoxQueue.Text = "Queue";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(146, 198);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(63, 13);
            this.label10.TabIndex = 12;
            this.label10.Text = "milliseconds";
            // 
            // txtAutoRefrehInterval
            // 
            this.txtAutoRefrehInterval.Location = new System.Drawing.Point(99, 195);
            this.txtAutoRefrehInterval.Name = "txtAutoRefrehInterval";
            this.txtAutoRefrehInterval.Size = new System.Drawing.Size(41, 20);
            this.txtAutoRefrehInterval.TabIndex = 11;
            this.txtAutoRefrehInterval.Text = "250";
            // 
            // chkAutoRefrehInterval
            // 
            this.chkAutoRefrehInterval.AutoSize = true;
            this.chkAutoRefrehInterval.Location = new System.Drawing.Point(6, 197);
            this.chkAutoRefrehInterval.Name = "chkAutoRefrehInterval";
            this.chkAutoRefrehInterval.Size = new System.Drawing.Size(87, 17);
            this.chkAutoRefrehInterval.TabIndex = 10;
            this.chkAutoRefrehInterval.Text = "refresh every";
            this.chkAutoRefrehInterval.UseVisualStyleBackColor = true;
            this.chkAutoRefrehInterval.Click += new System.EventHandler(this.chkAutoRefrehInterval_CheckedChanged);
            // 
            // lblQueueSize
            // 
            this.lblQueueSize.AutoSize = true;
            this.lblQueueSize.Location = new System.Drawing.Point(130, 26);
            this.lblQueueSize.Name = "lblQueueSize";
            this.lblQueueSize.Size = new System.Drawing.Size(10, 13);
            this.lblQueueSize.TabIndex = 9;
            this.lblQueueSize.Text = "-";
            // 
            // btnClearQueue
            // 
            this.btnClearQueue.Location = new System.Drawing.Point(133, 108);
            this.btnClearQueue.Name = "btnClearQueue";
            this.btnClearQueue.Size = new System.Drawing.Size(96, 23);
            this.btnClearQueue.TabIndex = 8;
            this.btnClearQueue.Text = "Empty queue";
            this.btnClearQueue.UseVisualStyleBackColor = true;
            this.btnClearQueue.Click += new System.EventHandler(this.btnClearQueue_Click);
            // 
            // btnRefreshQueueInfo
            // 
            this.btnRefreshQueueInfo.Location = new System.Drawing.Point(133, 137);
            this.btnRefreshQueueInfo.Name = "btnRefreshQueueInfo";
            this.btnRefreshQueueInfo.Size = new System.Drawing.Size(96, 23);
            this.btnRefreshQueueInfo.TabIndex = 7;
            this.btnRefreshQueueInfo.Text = "Refresh";
            this.btnRefreshQueueInfo.UseVisualStyleBackColor = true;
            this.btnRefreshQueueInfo.Click += new System.EventHandler(this.btnRefreshQueueInfo_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 26);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(60, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "Queue size";
            // 
            // groupBoxConsumer
            // 
            this.groupBoxConsumer.Controls.Add(this.lblConsumerLastMessageElapsedTime);
            this.groupBoxConsumer.Controls.Add(this.lblReceivedData);
            this.groupBoxConsumer.Controls.Add(this.label14);
            this.groupBoxConsumer.Controls.Add(this.label9);
            this.groupBoxConsumer.Controls.Add(this.lblLatestMessage);
            this.groupBoxConsumer.Controls.Add(this.label8);
            this.groupBoxConsumer.Controls.Add(this.lblReceivedMessages);
            this.groupBoxConsumer.Controls.Add(this.label6);
            this.groupBoxConsumer.Controls.Add(this.btnStopConsumer);
            this.groupBoxConsumer.Controls.Add(this.btnStartConsumer);
            this.groupBoxConsumer.Location = new System.Drawing.Point(488, 6);
            this.groupBoxConsumer.Name = "groupBoxConsumer";
            this.groupBoxConsumer.Size = new System.Drawing.Size(235, 267);
            this.groupBoxConsumer.TabIndex = 5;
            this.groupBoxConsumer.TabStop = false;
            this.groupBoxConsumer.Text = "Consumer";
            // 
            // lblConsumerLastMessageElapsedTime
            // 
            this.lblConsumerLastMessageElapsedTime.AutoSize = true;
            this.lblConsumerLastMessageElapsedTime.Location = new System.Drawing.Point(135, 241);
            this.lblConsumerLastMessageElapsedTime.Name = "lblConsumerLastMessageElapsedTime";
            this.lblConsumerLastMessageElapsedTime.Size = new System.Drawing.Size(94, 13);
            this.lblConsumerLastMessageElapsedTime.TabIndex = 18;
            this.lblConsumerLastMessageElapsedTime.Text = "00:00:00.0000000";
            // 
            // lblReceivedData
            // 
            this.lblReceivedData.AutoSize = true;
            this.lblReceivedData.Location = new System.Drawing.Point(131, 154);
            this.lblReceivedData.Name = "lblReceivedData";
            this.lblReceivedData.Size = new System.Drawing.Size(13, 13);
            this.lblReceivedData.TabIndex = 14;
            this.lblReceivedData.Text = "0";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(9, 241);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(67, 13);
            this.label14.TabIndex = 17;
            this.label14.Text = "Elapsed time";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(9, 154);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(77, 13);
            this.label9.TabIndex = 13;
            this.label9.Text = "Received data";
            // 
            // lblLatestMessage
            // 
            this.lblLatestMessage.AutoSize = true;
            this.lblLatestMessage.Location = new System.Drawing.Point(131, 108);
            this.lblLatestMessage.Name = "lblLatestMessage";
            this.lblLatestMessage.Size = new System.Drawing.Size(0, 13);
            this.lblLatestMessage.TabIndex = 12;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(9, 108);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(116, 13);
            this.label8.TabIndex = 11;
            this.label8.Text = "Last received message";
            // 
            // lblReceivedMessages
            // 
            this.lblReceivedMessages.AutoSize = true;
            this.lblReceivedMessages.Location = new System.Drawing.Point(131, 131);
            this.lblReceivedMessages.Name = "lblReceivedMessages";
            this.lblReceivedMessages.Size = new System.Drawing.Size(13, 13);
            this.lblReceivedMessages.TabIndex = 10;
            this.lblReceivedMessages.Text = "0";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(9, 131);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(103, 13);
            this.label6.TabIndex = 9;
            this.label6.Text = "Received messages";
            // 
            // btnStopConsumer
            // 
            this.btnStopConsumer.Location = new System.Drawing.Point(131, 48);
            this.btnStopConsumer.Name = "btnStopConsumer";
            this.btnStopConsumer.Size = new System.Drawing.Size(97, 23);
            this.btnStopConsumer.TabIndex = 8;
            this.btnStopConsumer.Text = "Stop";
            this.btnStopConsumer.UseVisualStyleBackColor = true;
            this.btnStopConsumer.Click += new System.EventHandler(this.btnStopConsumer_Click);
            // 
            // btnStartConsumer
            // 
            this.btnStartConsumer.Location = new System.Drawing.Point(131, 19);
            this.btnStartConsumer.Name = "btnStartConsumer";
            this.btnStartConsumer.Size = new System.Drawing.Size(97, 23);
            this.btnStartConsumer.TabIndex = 7;
            this.btnStartConsumer.Text = "Start";
            this.btnStartConsumer.UseVisualStyleBackColor = true;
            this.btnStartConsumer.Click += new System.EventHandler(this.btnStartConsumer_Click);
            // 
            // groupBoxProducer
            // 
            this.groupBoxProducer.Controls.Add(this.lblProducerStartTimeStamp);
            this.groupBoxProducer.Controls.Add(this.label12);
            this.groupBoxProducer.Controls.Add(this.lblProducerSentData);
            this.groupBoxProducer.Controls.Add(this.label11);
            this.groupBoxProducer.Controls.Add(this.lblElapsedTimeProducer);
            this.groupBoxProducer.Controls.Add(this.progressBarProducer);
            this.groupBoxProducer.Controls.Add(this.lblSentMessageLabel);
            this.groupBoxProducer.Controls.Add(this.label7);
            this.groupBoxProducer.Controls.Add(this.label4);
            this.groupBoxProducer.Controls.Add(this.btnCancelProducer);
            this.groupBoxProducer.Controls.Add(this.btnStartProducer);
            this.groupBoxProducer.Controls.Add(this.txtDelayInMillis);
            this.groupBoxProducer.Controls.Add(this.txtNumberOfMessages);
            this.groupBoxProducer.Controls.Add(this.label3);
            this.groupBoxProducer.Controls.Add(this.label2);
            this.groupBoxProducer.Controls.Add(this.txtMessageSizeKB);
            this.groupBoxProducer.Controls.Add(this.label1);
            this.groupBoxProducer.Location = new System.Drawing.Point(6, 6);
            this.groupBoxProducer.Name = "groupBoxProducer";
            this.groupBoxProducer.Size = new System.Drawing.Size(235, 267);
            this.groupBoxProducer.TabIndex = 4;
            this.groupBoxProducer.TabStop = false;
            this.groupBoxProducer.Text = "Producer";
            // 
            // lblProducerStartTimeStamp
            // 
            this.lblProducerStartTimeStamp.AutoSize = true;
            this.lblProducerStartTimeStamp.Location = new System.Drawing.Point(135, 241);
            this.lblProducerStartTimeStamp.Name = "lblProducerStartTimeStamp";
            this.lblProducerStartTimeStamp.Size = new System.Drawing.Size(94, 13);
            this.lblProducerStartTimeStamp.TabIndex = 16;
            this.lblProducerStartTimeStamp.Text = "00:00:00.0000000";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(9, 241);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(51, 13);
            this.label12.TabIndex = 15;
            this.label12.Text = "Start time";
            // 
            // lblProducerSentData
            // 
            this.lblProducerSentData.AutoSize = true;
            this.lblProducerSentData.Location = new System.Drawing.Point(134, 198);
            this.lblProducerSentData.Name = "lblProducerSentData";
            this.lblProducerSentData.Size = new System.Drawing.Size(10, 13);
            this.lblProducerSentData.TabIndex = 14;
            this.lblProducerSentData.Text = "-";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(9, 198);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(53, 13);
            this.label11.TabIndex = 13;
            this.label11.Text = "Sent data";
            // 
            // lblElapsedTimeProducer
            // 
            this.lblElapsedTimeProducer.AutoSize = true;
            this.lblElapsedTimeProducer.Location = new System.Drawing.Point(134, 217);
            this.lblElapsedTimeProducer.Name = "lblElapsedTimeProducer";
            this.lblElapsedTimeProducer.Size = new System.Drawing.Size(94, 13);
            this.lblElapsedTimeProducer.TabIndex = 12;
            this.lblElapsedTimeProducer.Text = "00:00:00.0000000";
            // 
            // progressBarProducer
            // 
            this.progressBarProducer.Location = new System.Drawing.Point(9, 153);
            this.progressBarProducer.Name = "progressBarProducer";
            this.progressBarProducer.Size = new System.Drawing.Size(218, 10);
            this.progressBarProducer.TabIndex = 11;
            // 
            // lblSentMessageLabel
            // 
            this.lblSentMessageLabel.AutoSize = true;
            this.lblSentMessageLabel.Location = new System.Drawing.Point(134, 179);
            this.lblSentMessageLabel.Name = "lblSentMessageLabel";
            this.lblSentMessageLabel.Size = new System.Drawing.Size(10, 13);
            this.lblSentMessageLabel.TabIndex = 10;
            this.lblSentMessageLabel.Text = "-";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(9, 217);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(67, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "Elapsed time";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(9, 179);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(95, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Last sent message";
            // 
            // btnCancelProducer
            // 
            this.btnCancelProducer.Enabled = false;
            this.btnCancelProducer.Location = new System.Drawing.Point(131, 125);
            this.btnCancelProducer.Name = "btnCancelProducer";
            this.btnCancelProducer.Size = new System.Drawing.Size(97, 23);
            this.btnCancelProducer.TabIndex = 7;
            this.btnCancelProducer.Text = "Cancel";
            this.btnCancelProducer.UseVisualStyleBackColor = true;
            this.btnCancelProducer.Click += new System.EventHandler(this.btnCancelProducer_Click);
            // 
            // btnStartProducer
            // 
            this.btnStartProducer.Location = new System.Drawing.Point(131, 97);
            this.btnStartProducer.Name = "btnStartProducer";
            this.btnStartProducer.Size = new System.Drawing.Size(97, 23);
            this.btnStartProducer.TabIndex = 6;
            this.btnStartProducer.Text = "Start";
            this.btnStartProducer.UseVisualStyleBackColor = true;
            this.btnStartProducer.Click += new System.EventHandler(this.btnStartProducer_Click);
            // 
            // txtDelayInMillis
            // 
            this.txtDelayInMillis.Location = new System.Drawing.Point(131, 71);
            this.txtDelayInMillis.MaxLength = 8;
            this.txtDelayInMillis.Name = "txtDelayInMillis";
            this.txtDelayInMillis.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.txtDelayInMillis.Size = new System.Drawing.Size(96, 20);
            this.txtDelayInMillis.TabIndex = 5;
            this.txtDelayInMillis.Text = "0";
            // 
            // txtNumberOfMessages
            // 
            this.txtNumberOfMessages.Location = new System.Drawing.Point(131, 42);
            this.txtNumberOfMessages.MaxLength = 5;
            this.txtNumberOfMessages.Name = "txtNumberOfMessages";
            this.txtNumberOfMessages.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.txtNumberOfMessages.Size = new System.Drawing.Size(96, 20);
            this.txtNumberOfMessages.TabIndex = 4;
            this.txtNumberOfMessages.Text = "300";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 74);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(99, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Delay (milliseconds)";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(106, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Number of messages";
            // 
            // txtMessageSizeKB
            // 
            this.txtMessageSizeKB.Location = new System.Drawing.Point(131, 13);
            this.txtMessageSizeKB.MaxLength = 5;
            this.txtMessageSizeKB.Name = "txtMessageSizeKB";
            this.txtMessageSizeKB.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.txtMessageSizeKB.Size = new System.Drawing.Size(96, 20);
            this.txtMessageSizeKB.TabIndex = 1;
            this.txtMessageSizeKB.Text = "1024";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(94, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Message size (KB)";
            // 
            // bgwLabReadQueue
            // 
            this.bgwLabReadQueue.WorkerReportsProgress = true;
            this.bgwLabReadQueue.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgwLabReadQueue_DoWork);
            this.bgwLabReadQueue.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgwLabReadQueue_ProgressChanged);
            this.bgwLabReadQueue.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgwLabReadQueue_RunWorkerCompleted);
            // 
            // cbDeadLetterJournal
            // 
            this.cbDeadLetterJournal.AutoSize = true;
            this.cbDeadLetterJournal.Location = new System.Drawing.Point(14, 140);
            this.cbDeadLetterJournal.Name = "cbDeadLetterJournal";
            this.cbDeadLetterJournal.Size = new System.Drawing.Size(131, 17);
            this.cbDeadLetterJournal.TabIndex = 24;
            this.cbDeadLetterJournal.Text = "Use dead letter queue";
            this.cbDeadLetterJournal.UseVisualStyleBackColor = true;
            // 
            // cbUseJournalQueue
            // 
            this.cbUseJournalQueue.AutoSize = true;
            this.cbUseJournalQueue.Location = new System.Drawing.Point(14, 163);
            this.cbUseJournalQueue.Name = "cbUseJournalQueue";
            this.cbUseJournalQueue.Size = new System.Drawing.Size(112, 17);
            this.cbUseJournalQueue.TabIndex = 25;
            this.cbUseJournalQueue.Text = "Use journal queue";
            this.cbUseJournalQueue.UseVisualStyleBackColor = true;
            // 
            // cbTransactionalMessage
            // 
            this.cbTransactionalMessage.AutoSize = true;
            this.cbTransactionalMessage.Location = new System.Drawing.Point(14, 186);
            this.cbTransactionalMessage.Name = "cbTransactionalMessage";
            this.cbTransactionalMessage.Size = new System.Drawing.Size(135, 17);
            this.cbTransactionalMessage.TabIndex = 26;
            this.cbTransactionalMessage.Text = "Transactional message";
            this.cbTransactionalMessage.UseVisualStyleBackColor = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(870, 546);
            this.Controls.Add(this.tabControl1);
            this.Name = "MainForm";
            this.Text = "MSMQ testing";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage1.ResumeLayout(false);
            this.groupBoxOutput.ResumeLayout(false);
            this.groupBoxQueue.ResumeLayout(false);
            this.groupBoxQueue.PerformLayout();
            this.groupBoxConsumer.ResumeLayout(false);
            this.groupBoxConsumer.PerformLayout();
            this.groupBoxProducer.ResumeLayout(false);
            this.groupBoxProducer.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.ComponentModel.BackgroundWorker bgwProducer;
        private System.ComponentModel.BackgroundWorker bgwConsumer;
        private System.ComponentModel.BackgroundWorker bgwQueueStatus;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox groupBoxOutput;
        private System.Windows.Forms.ListBox listBoxOutput;
        private System.Windows.Forms.GroupBox groupBoxQueue;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtAutoRefrehInterval;
        private System.Windows.Forms.CheckBox chkAutoRefrehInterval;
        private System.Windows.Forms.Label lblQueueSize;
        private System.Windows.Forms.Button btnClearQueue;
        private System.Windows.Forms.Button btnRefreshQueueInfo;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBoxConsumer;
        private System.Windows.Forms.Label lblConsumerLastMessageElapsedTime;
        private System.Windows.Forms.Label lblReceivedData;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label lblLatestMessage;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label lblReceivedMessages;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnStopConsumer;
        private System.Windows.Forms.Button btnStartConsumer;
        private System.Windows.Forms.GroupBox groupBoxProducer;
        private System.Windows.Forms.Label lblProducerStartTimeStamp;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label lblProducerSentData;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label lblElapsedTimeProducer;
        private System.Windows.Forms.ProgressBar progressBarProducer;
        private System.Windows.Forms.Label lblSentMessageLabel;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnCancelProducer;
        private System.Windows.Forms.Button btnStartProducer;
        private System.Windows.Forms.TextBox txtDelayInMillis;
        private System.Windows.Forms.TextBox txtNumberOfMessages;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtMessageSizeKB;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Button btnLabMsgSend;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ComboBox comboLabQueueName;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Button btnLabMsgReceive;
        private System.Windows.Forms.TextBox txtLabMsgBody;
        private System.Windows.Forms.TextBox txtLabMsgLabel;
        private System.Windows.Forms.CheckBox chkLabReadTimeout;
        private System.Windows.Forms.TextBox txtLabReadTimeoutMillis;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button btnLabMsgPeek;
        private System.Windows.Forms.Button btnLabMsgPeekById;
        private System.Windows.Forms.Button btnLabMsgPeekByCorrelationId;
        private System.Windows.Forms.TextBox txtLabLabMsgPeekByCorrelationId;
        private System.Windows.Forms.TextBox txtLabLabMsgPeekById;
        private System.Windows.Forms.Button btnLabClearMsgDetails;
        private System.Windows.Forms.Label lblLabMsgDetailsLookupId;
        private System.Windows.Forms.Label lblLabMsgDetailsPriority;
        private System.Windows.Forms.Label lblLabMsgDetailsBody;
        private System.Windows.Forms.Label lblLabMsgDetailsLabel;
        private System.Windows.Forms.Label lblLabMsgDetailsId;
        private System.Windows.Forms.CheckBox chkAttachSenderId;
        private System.Windows.Forms.Label lblLabMsgDetailsSourceMachine;
        private System.Windows.Forms.Label lblLabMsgDetailsSenderId;
        private System.Windows.Forms.Label lblLabMsgDetailsCorrelationId;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.ComponentModel.BackgroundWorker bgwLabReadQueue;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.ListBox listBoxLabOutput;
        private System.Windows.Forms.CheckBox chkLabMsgAutoLabel;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority8;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority7;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority6;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority5;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority4;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority3;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority2;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority1;
        private System.Windows.Forms.RadioButton radLabMsgSendPriority0;
        private System.Windows.Forms.Button btnLabPurgeQueue;
        private System.Windows.Forms.Button btnGetPublicQueues;
        private System.Windows.Forms.Label lblTimeToReachQueue;
        private System.Windows.Forms.Label labelTimeToBeReceived;
        private System.Windows.Forms.TextBox txtTimeToReachQueue;
        private System.Windows.Forms.TextBox txtTimeToBeReceived;
        private System.Windows.Forms.CheckBox cbDeadLetterJournal;
        private System.Windows.Forms.CheckBox cbUseJournalQueue;
        private System.Windows.Forms.CheckBox cbTransactionalMessage;
    }
}

