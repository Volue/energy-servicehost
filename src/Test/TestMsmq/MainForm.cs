using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Messaging;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Message = System.Messaging.Message;

namespace TestMsmq
{
    public partial class MainForm : Form
    {
        private const int QUEUE_ID = 1;
        private DateTime producerStartTimeStamp;

        public MainForm()
        {
            InitializeComponent();
        }

        private void Output(string text)
        {
            listBoxOutput.Items.Add(text);
        }

        private void Output(Exception e)
        {
            listBoxOutput.Items.Add("EXCEPTION: " + e.Message);
            listBoxOutput.Items.Add(e.StackTrace);
        }

        private void LabOutput(string text)
        {
            listBoxLabOutput.Items.Add(text);
        }

        private void LabOutput(Exception e)
        {
            listBoxLabOutput.Items.Add("EXCEPTION: " + e.Message);
            listBoxLabOutput.Items.Add(e.StackTrace);
        }

        private void ShowErrorMessage(string errorMessage)
        {
            MessageBox.Show(errorMessage);
        }

        private void ShowErrorMessage(Exception e)
        {
            MessageBox.Show(e.Message + "\n" + e.StackTrace, "Error");
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            btnCancelProducer.Enabled = false;
            btnStopConsumer.Enabled = false;
            ClearLabFormMessageDetails();
        }

        //private void PopulateLabMsgSendPriority()
        //{
        //    comboLabMsgSendPriority.Items.Add("<no priority specified>");
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.Highest);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.VeryHigh);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.High);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.AboveNormal);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.Normal);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.Low);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.VeryLow);
        //    comboLabMsgSendPriority.Items.Add(MessagePriority.Lowest);
        //    comboLabMsgSendPriority.SelectedIndex = 5;
        //}

        private string GetQueuePath(int id)
        {
            return @".\Private$\TestMsmqQ" + id;
        }

        private void ClearQueue(int id)
        {
            try
            {
                MessageQueue mq = GetQueue(id);
                mq.Purge();
            }
            catch (Exception e)
            {
                Output(e);
            }
        }

        private int GetMessageQueueCount(MessageQueue q)
        {
            var filter = new MessagePropertyFilter()
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
            q.MessageReadPropertyFilter = filter;
            return q.GetAllMessages().Length;
        }

        private int GetQueueSize(int id)
        {
            MessageQueue mq = GetQueue(id);
            return GetMessageQueueCount(mq);
        }



        private MessageQueue GetQueue(int id)
        {
            string queuePath = GetQueuePath(id);
            return GetQueue(queuePath);
        }

        private MessageQueue GetQueue(string queuePath)
        {
            MessageQueue mq = null;

            try
            {
                mq = new MessageQueue(queuePath);
/*
                if (MessageQueue.Exists(queuePath))
                {
                    mq = new MessageQueue(queuePath);
                }
                else
                {
                    mq = MessageQueue.Create(queuePath);
                }
 */
            }
            catch (Exception ex)
            {
                // Exception here in case MSMQ is not installed on the machine
                //
                //To install Message Queuing 4.0 on Windows Server 2008 or Windows Server 2008 R2
                //    In Server Manager, click Features.
                //    In the right-hand pane under Features Summary, click Add Features.
                //    In the resulting window, expand Message Queuing.
                //    Expand Message Queuing Services.
                //    Click Directory Services Integration (for computers joined to a Domain), then click HTTP Support.
                //    Click Next,then click Install.
                //
                //To install Message Queuing 4.0 on Windows 7 or Windows Vista
                //    Open Control Panel.
                //    Click Programs and then, under Programs and Features, click Turn Windows Features on and off.
                //    Expand Microsoft Message Queue (MSMQ) Server, expand Microsoft Message Queue (MSMQ) Server Core, and then select the check boxes for the following Message Queuing features to install:
                //        * MSMQ Active Directory Domain Services Integration (for computers joined to a Domain).
                //        * MSMQ HTTP Support.
                //    Click OK.
                //    If you are prompted to restart the computer, click OK to complete the installation.
                //
                ShowErrorMessage(ex);
            }

            return mq;
        }

        private void btnStartProducer_Click(object sender, EventArgs e)
        {
            int numberOfMessages;
            if (!int.TryParse(txtNumberOfMessages.Text, out numberOfMessages))
            {
                ShowErrorMessage("Failed to parse number of messages");
                return;
            }
            if (numberOfMessages < 1)
            {
                ShowErrorMessage("Number of messages must be at least 1");
                return;
            }

            int messageSizeKb;
            if (!int.TryParse(txtMessageSizeKB.Text, out messageSizeKb))
            {
                ShowErrorMessage("Failed to parse messages size");
                return;
            }

            int delayInMillis;
            if (!int.TryParse(txtDelayInMillis.Text, out delayInMillis))
            {
                ShowErrorMessage("Failed to parse delay in milliseconds");
                return;
            }

            SetGuiStateProducerIdle(false);

            producerStartTimeStamp = DateTime.Now;
            lblProducerStartTimeStamp.Text = producerStartTimeStamp.ToString();

            Output("Starting producer");

            ProducerArguments arguments = new ProducerArguments
                                              {
                                                  DelayInMillis = delayInMillis,
                                                  MessageSizeKb = messageSizeKb,
                                                  NumberOfMessages = numberOfMessages,
                                                  QueueId = 1
                                              };
            bgwProducer.RunWorkerAsync(arguments);
        }

        private void btnCancelProducer_Click(object sender, EventArgs e)
        {
            btnCancelProducer.Enabled = false;
            bgwProducer.CancelAsync();
        }

        private void btnStartConsumer_Click(object sender, EventArgs e)
        {
            SetGuiStateConsumerIdle(false);
            ClearConsumerState();
            bgwConsumer.RunWorkerAsync();
        }

        private void btnStopConsumer_Click(object sender, EventArgs e)
        {
            btnStopConsumer.Enabled = false;
            bgwConsumer.CancelAsync();
        }

        private void SetGuiStateProducerIdle(bool idleState)
        {
            txtNumberOfMessages.Enabled = idleState;
            txtMessageSizeKB.Enabled = idleState;
            txtDelayInMillis.Enabled = idleState;
            btnStartProducer.Enabled = idleState;
            btnCancelProducer.Enabled = !idleState;
        }

        private void SetGuiStateConsumerIdle(bool idleState)
        {
            btnStartConsumer.Enabled = idleState;
            btnStopConsumer.Enabled = !idleState;
        }

        private void ClearConsumerState()
        {
            ShowConsumerState(0, 0, "");
        }

        private string GetDataAmountAsString(long dataInBytes)
        {
            string data;
            long kb = dataInBytes / 1024;
            if (kb < (1024 * 10))
            {
                data = kb + " KB";
            }
            else
            {
                long mb = kb / 1024;
                data = mb + " MB";
            }
            return data;
        }

        private void ShowConsumerState(long receivedData, int receivedMessages, string latestMessage)
        {
            lblReceivedData.Text = GetDataAmountAsString(receivedData);
            lblReceivedMessages.Text = receivedMessages.ToString();
            lblLatestMessage.Text = latestMessage;
        }

        private void ShowProducerState(int currentMessage, int totalMessageCount, string sentMessageLabel, TimeSpan elapsedTime, long sentData)
        {
            progressBarProducer.Value = currentMessage;
            progressBarProducer.Minimum = 0;
            progressBarProducer.Maximum = totalMessageCount;
            lblSentMessageLabel.Text = sentMessageLabel;
            lblElapsedTimeProducer.Text = elapsedTime.ToString();
            lblProducerSentData.Text = GetDataAmountAsString(sentData);
        }

        private void ShowQueueState(int queueSize)
        {
            lblQueueSize.Text = queueSize.ToString();
        }

        private string CreateFakeData(int sizeInKb)
        {
            string sixteen = "0123456789ABCDEF";
            StringBuilder oneKb = new StringBuilder();
            for (int i = 0; i < 64; i++)
            {
                oneKb.Append(sixteen);
            }

            StringBuilder fakeData = new StringBuilder();
            for (int i = 0; i < sizeInKb; i++)
            {
                fakeData.Append(oneKb);
            }

            return fakeData.ToString();
        }

        private void bgwProducer_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                // ensure we have the neccessary input date
                if (!(e.Argument is ProducerArguments))
                {
                    return;
                }
                ProducerArguments arguments = (ProducerArguments)e.Argument;

                string dataChunk = CreateFakeData(arguments.MessageSizeKb);
                MessageQueue mq = GetQueue(arguments.QueueId);

                //Stopwatch stopwatch = new Stopwatch();
                //stopwatch.Start();
                DateTime startTime = DateTime.Now;

                long sentData = 0;
                for (int i = 1; i <= arguments.NumberOfMessages; i++)
                {
                    string label = "message_" + i;
                    Message msg = new Message();
                    msg.Body = dataChunk;
                    msg.Label = label;
                    try
                    {
                        mq.Send(msg);
                    }
                    catch (Exception ex)
                    {
                        bgwProducer.ReportProgress(0, new ErrorReport { Exception = ex, Text = "Failed to send message" });
                        throw;
                    }
                    sentData += dataChunk.Length;
                    TimeSpan elapsedTime = DateTime.Now - startTime;
                    ProducerReport report = new ProducerReport
                                                {
                                                    CurrentMessageNo = i,
                                                    TotalNumberOfMessages = arguments.NumberOfMessages,
                                                    SentMessageLabel = label,
                                                    ElapsedTime = elapsedTime,
                                                    SentData = sentData
                                                };
                    bgwProducer.ReportProgress(0, report);

                    const int chunkSize = 500;
                    int chunks = arguments.DelayInMillis / chunkSize;
                    int millis = arguments.DelayInMillis % chunkSize;

                    for (int delay = 0; delay < chunks; delay++)
                    {
                        if (bgwProducer.CancellationPending)
                        {
                            e.Cancel = true;
                            break;
                        }
                        Thread.Sleep(chunkSize);
                    }
                    if (bgwProducer.CancellationPending)
                    {
                        e.Cancel = true;
                        break;
                    }
                    Thread.Sleep(millis);
                }
            }
            catch (Exception ex)
            {
                bgwProducer.ReportProgress(0, new ErrorReport { Exception = ex, Text = "Exception in producer main loop" });
            }
        }

        private void bgwProducer_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            Output("Producer RunWorkerCompleted");
            SetGuiStateProducerIdle(true);
        }

        private void bgwConsumer_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                MessageQueue mq = GetQueue(QUEUE_ID);

                int receivedMessages = 0;
                long receivedData = 0;
                TimeSpan waitTime = new TimeSpan(0, 0, 1);
                while (true)
                {
                    Message m = null;
                    try
                    {
                        m = mq.Receive(waitTime);
                    }
                    catch (MessageQueueException ex)
                    {
                        m = null; // no message
                    }
                    catch (Exception ex)
                    {
                        ShowErrorMessage(ex);
                    }

                    if (m != null)
                    {
                        m.Formatter = new XmlMessageFormatter(new String[] { "System.String,mscorlib" });
                        string data = m.Body.ToString();
                        receivedData += data.Length;
                        receivedMessages++;

                        ConsumerReport report = new ConsumerReport
                                                    {
                                                        LatestMessageLabel = m.Label,
                                                        ReceivedData = receivedData,
                                                        ReceivedMessages = receivedMessages
                                                    };
                        bgwConsumer.ReportProgress(0, report);
                    }

                    if (bgwConsumer.CancellationPending)
                    {
                        e.Cancel = true;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                bgwConsumer.ReportProgress(0, new ErrorReport { Exception = ex, Text = "Exception in consumer main loop" });
            }
        }

        private void bgwConsumer_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            Output("Consumer RunWorkerCompleted");
            SetGuiStateConsumerIdle(true);
        }

        private void bgwConsumer_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            HandleUserState(e.UserState);
            TimeSpan elapsedTime = DateTime.Now - producerStartTimeStamp;
            lblConsumerLastMessageElapsedTime.Text = elapsedTime.ToString();
        }

        private void bgwProducer_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            HandleUserState(e.UserState);
        }

        private void bgwQueueStatus_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                // ensure we have the neccessary input date
                if (!(e.Argument is QueueStateArguments))
                {
                    return;
                }
                QueueStateArguments arguments = (QueueStateArguments)e.Argument;

                const int chunkSize = 500;
                int sleepIntervalSeconds = arguments.IntervalInMillis / chunkSize;
                int sleepIntervalMillis = arguments.IntervalInMillis % chunkSize;

                while (true)
                {
                    QueueStateReport report = new QueueStateReport();
                    report.QueueSize = GetQueueSize(QUEUE_ID);

                    if (bgwQueueStatus.WorkerReportsProgress)
                    {
                        bgwQueueStatus.ReportProgress(0, report);
                    }
                    for (int i = 0; i < sleepIntervalSeconds; i++)
                    {
                        if (bgwQueueStatus.CancellationPending)
                        {
                            e.Cancel = true;
                            break;
                        }
                        Thread.Sleep(chunkSize);
                    }
                    if (bgwQueueStatus.CancellationPending)
                    {
                        e.Cancel = true;
                        break;
                    }
                    Thread.Sleep(sleepIntervalMillis);
                }
            }
            catch (Exception ex)
            {
                bgwQueueStatus.ReportProgress(0, new ErrorReport { Exception = ex, Text = "Exception in queue status main loop" });
            }
        }

        private void bgwQueueStatus_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            HandleUserState(e.UserState);
        }

        private void HandleUserState(object userState)
        {
            if (userState == null)
            {
                Output("UserState is null");
            }
            else if (userState is ProducerReport)
            {
                ProducerReport report = (ProducerReport)userState;
                ShowProducerState(report.CurrentMessageNo, report.TotalNumberOfMessages, report.SentMessageLabel, report.ElapsedTime, report.SentData);
            }
            else if (userState is ConsumerReport)
            {
                ConsumerReport report = (ConsumerReport)userState;
                ShowConsumerState(report.ReceivedData, report.ReceivedMessages, report.LatestMessageLabel);
            }
            else if (userState is QueueStateReport)
            {
                QueueStateReport report = (QueueStateReport)userState;
                ShowQueueState(report.QueueSize);
            }
            else if (userState is ErrorReport)
            {
                ErrorReport report = (ErrorReport)userState;
                if (report.Text != null)
                {
                    Output(report.Text);
                }
                if (report.Exception != null)
                {
                    Output(report.Exception);
                }
            }
            else if (userState is Exception)
            {
                Exception ex = (Exception)userState;
                Output(ex);
            }
            else if (userState is String)
            {
                String text = (String)userState;
                Output(text);
            }
            else
            {
                Output("Unknown UserState");
            }
        }

        private void btnClearQueue_Click(object sender, EventArgs e)
        {
            ClearQueue(QUEUE_ID);

        }

        private void btnRefreshQueueInfo_Click(object sender, EventArgs e)
        {
            int queueSize = GetQueueSize(QUEUE_ID);
            lblQueueSize.Text = queueSize.ToString();
        }

        private void chkAutoRefrehInterval_CheckedChanged(object sender, EventArgs e)
        {
            if (chkAutoRefrehInterval.Checked)
            {
                int refreshIntervalMillis;
                if (int.TryParse(txtAutoRefrehInterval.Text, out refreshIntervalMillis) && refreshIntervalMillis > 99)
                {
                    txtAutoRefrehInterval.Enabled = false;
                    QueueStateArguments arguments = new QueueStateArguments
                                                        {
                                                            IntervalInMillis = refreshIntervalMillis
                                                        };
                    bgwQueueStatus.RunWorkerAsync(arguments);
                    Output("Queue status auto refresh started");
                }
                else
                {
                    ShowErrorMessage("Invalid refresh interval, 100 is minimum");
                }
            }
            else
            {
                bgwQueueStatus.CancelAsync();
                chkAutoRefrehInterval.Enabled = false;
            }
        }

        private void bgwQueueStatus_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            Output("Queue status auto refresh stopped");
            chkAutoRefrehInterval.Enabled = true;
            txtAutoRefrehInterval.Enabled = true;
        }

        private void btnLabMsgSend_Click(object sender, EventArgs e)
        {
            if(chkLabMsgAutoLabel.Checked)
            {
                txtLabMsgLabel.Text = DateTime.Now.Ticks.ToString();
            }
            Message m = GetMessageFromLabForm();
            MessageQueue mq = GetLabQueue();
            
            if (mq != null)
            {
                try
                {
                    if (!cbTransactionalMessage.Checked)
                    {
                        mq.Send(m);
                    }
                    else
                    {
                        MessageQueueTransaction transaction = new MessageQueueTransaction();

                        transaction.Begin();

                        mq.Send(m, transaction);

                        transaction.Commit();
                    }

                    LabOutput("Sent message with label " + m.Label);
                }
                catch (Exception ex)
                {
                    ShowErrorMessage(ex);                        
                }
            }            
        }

        private MessageQueue GetLabQueue()
        {
            return GetQueue(GetLabQueuePath());
        }

        private string GetLabQueuePath()
        {
            return comboLabQueueName.Text;
        }

        private Message GetMessageFromLabForm()
        {
            Message m = new Message();
            m.Label = txtLabMsgLabel.Text;
            m.Body = txtLabMsgBody.Text;

            m.UseDeadLetterQueue = cbDeadLetterJournal.Checked;

            m.UseJournalQueue = cbUseJournalQueue.Checked;

            if (txtTimeToReachQueue.Text != String.Empty)
            {
                m.TimeToReachQueue = new TimeSpan(0, 0, 0, Int32.Parse(txtTimeToReachQueue.Text));
            }
            if (txtTimeToBeReceived.Text != String.Empty)
            {
                m.TimeToBeReceived = new TimeSpan(0, 0, 0, Int32.Parse(txtTimeToBeReceived.Text));
            }

            if (radLabMsgSendPriority0.Checked)
            {
                // don't set priority
            }
            else if (radLabMsgSendPriority1.Checked)
            {
                m.Priority = MessagePriority.Highest;
            }
            else if (radLabMsgSendPriority2.Checked)
            {
                m.Priority = MessagePriority.VeryHigh;
            }
            else if (radLabMsgSendPriority3.Checked)
            {
                m.Priority = MessagePriority.High;
            }
            else if (radLabMsgSendPriority4.Checked)
            {
                m.Priority = MessagePriority.AboveNormal;
            }
            else if (radLabMsgSendPriority5.Checked)
            {
                m.Priority = MessagePriority.Normal;
            }
            else if (radLabMsgSendPriority6.Checked)
            {
                m.Priority = MessagePriority.Low;
            }
            else if (radLabMsgSendPriority7.Checked)
            {
                m.Priority = MessagePriority.VeryLow;
            }
            else if (radLabMsgSendPriority8.Checked)
            {
                m.Priority = MessagePriority.Lowest;
            }

            //m.AttachSenderId = chkAttachSenderId.Checked;
            return m;
        }

        private void PopulateLabFormMessage(Message m)
        {
            if (m != null)
            {
                lblLabMsgDetailsId.Text = m.Id;
                lblLabMsgDetailsLabel.Text = m.Label;
                m.Formatter = new XmlMessageFormatter(new String[] { "System.String,mscorlib" });
                lblLabMsgDetailsBody.Text = m.Body.ToString();
                lblLabMsgDetailsPriority.Text = m.Priority.ToString();
                //lblLabMsgDetailsLookupId.Text = m.LookupId.ToString();
                //m.CorrelationId;
                //m.SourceMachine;
                //m.SenderId;
            }
            else
            {
                ClearLabFormMessageDetails();
            }
        }

        private void ClearLabFormMessageDetails()
        {
            lblLabMsgDetailsId.Text = "";
            lblLabMsgDetailsLabel.Text = "";
            lblLabMsgDetailsBody.Text = "";
            lblLabMsgDetailsPriority.Text = "";
            lblLabMsgDetailsLookupId.Text = "";
        }

        private void btnLabMsgReceive_Click(object sender, EventArgs e)
        {
            ReadQueueInternal(LabReadOperation.Receive, null);
        }

        private void btnLabMsgPeek_Click(object sender, EventArgs e)
        {
            ReadQueueInternal(LabReadOperation.Peek, null);
        }

        private void btnLabMsgPeekById_Click(object sender, EventArgs e)
        {
            ReadQueueInternal(LabReadOperation.PeekById, txtLabLabMsgPeekById.Text);
        }

        private void btnLabMsgPeekByCorrelationId_Click(object sender, EventArgs e)
        {
            ReadQueueInternal(LabReadOperation.PeekByCorrelationId, txtLabLabMsgPeekByCorrelationId.Text);
        }

        private bool GetLabReadTimeout(out TimeSpan timeout)
        {
            bool ok;
            int timeoutInMillis;
            if (int.TryParse(txtLabReadTimeoutMillis.Text, out timeoutInMillis))
            {
                timeout = new TimeSpan(0, 0, 0, 0, timeoutInMillis);
                ok = true;
            }
            else
            {
                ShowErrorMessage("Invalid timeout '" + txtLabReadTimeoutMillis.Text + "'");
                timeout = new TimeSpan(0);
                ok = false;
            }
            return ok;
        }

        private void chkLabReadTimeout_CheckedChanged(object sender, EventArgs e)
        {
            txtLabReadTimeoutMillis.Enabled = chkLabReadTimeout.Checked;
        }

        private void btnLabClearMsgDetails_Click(object sender, EventArgs e)
        {
            ClearLabFormMessageDetails();
        }

        private void LabReadMsgEnabled(bool enabled)
        {
            btnLabMsgReceive.Enabled = enabled;
            btnLabMsgPeek.Enabled = enabled;
            btnLabMsgPeekById.Enabled = enabled;
            btnLabMsgPeekByCorrelationId.Enabled = enabled;
        }

        private void ReadQueueInternal(LabReadOperation readOperation, string readArgument)
        {
            LabReadMsgEnabled(false);

            bool useTimeout = false;
            TimeSpan timeout = new TimeSpan(0);
            if (chkLabReadTimeout.Checked)
            {
                if (GetLabReadTimeout(out timeout))
                {
                    useTimeout = true;
                }
            }

            LabReadQueueArguments arguments = new LabReadQueueArguments
            {
                QueueName = GetLabQueuePath(),
                Operation = readOperation,
                UseTimeout = useTimeout,
                Timeout = timeout,
                ReadArgument = readArgument // e.g. Id, CorrelationId
            };
            bgwLabReadQueue.RunWorkerAsync(arguments);

        }

        private void bgwLabReadQueue_DoWork(object sender, DoWorkEventArgs e)
        {
            LabReadQueueArguments arg = (LabReadQueueArguments)e.Argument;

            try
            {
                Message m;
                MessageQueue q = GetQueue(arg.QueueName);

                if (q != null)
                {
                    q.MessageReadPropertyFilter.Priority = true;
                    if (arg.UseTimeout)
                    {
                        switch (arg.Operation)
                        {
                            case LabReadOperation.Receive:
                                m = q.Receive(arg.Timeout);
                                break;
                            case LabReadOperation.Peek:
                                m = q.Peek(arg.Timeout);
                                break;
                            case LabReadOperation.PeekById:
                                m = q.PeekById(arg.ReadArgument, arg.Timeout);
                                break;
                            case LabReadOperation.PeekByCorrelationId:
                                m = q.PeekByCorrelationId(arg.ReadArgument, arg.Timeout);
                                break;
                            default:
                                m = null; // should not happen
                                break;
                        }
                    }
                    else
                    {
                        switch (arg.Operation)
                        {
                            case LabReadOperation.Receive:
                                m = q.Receive();
                                break;
                            case LabReadOperation.Peek:
                                m = q.Peek();
                                break;
                            case LabReadOperation.PeekById:
                                m = q.PeekById(arg.ReadArgument);
                                break;
                            case LabReadOperation.PeekByCorrelationId:
                                m = q.PeekByCorrelationId(arg.ReadArgument);
                                break;
                            default:
                                m = null; // should not happen
                                break;
                        }
                    }

                    LabReadReport report = new LabReadReport();
                    report.Message = m;
                    bgwLabReadQueue.ReportProgress(0, report);
                }
            }
            catch (MessageQueueException ex)
            {
                LabReadReport report = new LabReadReport();
                report.Message = null;
                bgwLabReadQueue.ReportProgress(0, report);
            }
            catch (Exception ex)
            {
                bgwLabReadQueue.ReportProgress(0, ex);
            }
        }

        private void bgwLabReadQueue_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            if (e.UserState == null)
            {
                LabOutput("UserState is null");
            }
            else if (e.UserState is LabReadReport)
            {
                LabReadReport report = (LabReadReport)e.UserState;
                if (report.Message == null)
                {
                    ClearLabFormMessageDetails();
                    LabOutput("(message is null)");
                }
                else
                {
                    PopulateLabFormMessage(report.Message);
                    LabOutput("Read message with label " + report.Message.Label);
                }
            }
            else if (e.UserState is Exception)
            {
                Exception ex = (Exception)e.UserState;
                LabOutput(ex);
            }
            else if (e.UserState is String)
            {
                String text = (String)e.UserState;
                LabOutput(text);
            }
            else
            {
                LabOutput("Unknown UserState");
            }
        }

        private void bgwLabReadQueue_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            LabReadMsgEnabled(true);
        }

        private void chkLabMsgAutoLabel_CheckedChanged(object sender, EventArgs e)
        {
            txtLabMsgLabel.Enabled = !chkLabMsgAutoLabel.Checked;
        }

        private void btnLabPurgeQueue_Click(object sender, EventArgs e)
        {
            GetLabQueue().Purge();
        }

        private void btnGetPublicQueues_Click(object sender, EventArgs e)
        {
            try
            {
                MessageQueue[] mqs = MessageQueue.GetPublicQueues();

                foreach(MessageQueue mq in mqs)
                {
                    listBoxLabOutput.Items.Add(mq.Path);
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.ToString());
            }
        }
    }

    public class ProducerArguments
    {
        public int NumberOfMessages { get; set; }
        public int MessageSizeKb { get; set; }
        public int DelayInMillis { get; set; }
        public int QueueId { get; set; }
    }

    public class QueueStateArguments
    {
        public int IntervalInMillis { get; set; }
    }

    public class ProducerReport
    {
        public int CurrentMessageNo { get; set; }
        public int TotalNumberOfMessages { get; set; }
        public string SentMessageLabel { get; set; }
        public TimeSpan ElapsedTime { get; set; }
        public long SentData { get; set; }
    }

    public class ConsumerReport
    {
        public string OutputMessage { get; set; }
        public string LatestMessageLabel { get; set; }
        public int ReceivedMessages { get; set; }
        public long ReceivedData { get; set; }
    }

    public class ErrorReport
    {
        public string Text { get; set; }
        public Exception Exception { get; set; }
    }

    public class QueueStateReport
    {
        public int QueueSize { get; set; }
    }

    public enum LabReadOperation
    {
        Receive,
        Peek,
        PeekById,
        PeekByCorrelationId
    }

    public class LabReadReport
    {
        public Message Message { get; set; }
    }

    public class LabReadQueueArguments
    {
        public LabReadOperation Operation { get; set; }
        public bool UseTimeout { get; set; }
        public TimeSpan Timeout { get; set; }


        public string ReadArgument { get; set; }

        public string QueueName { get; set; }
    }


}
