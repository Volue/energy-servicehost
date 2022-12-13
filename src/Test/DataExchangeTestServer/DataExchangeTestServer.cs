using System;
using System.Collections.ObjectModel;
using System.IO;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeTestServer
{
    public class CmdArgs
    {
        public bool disableGUI { get; set; }
        public bool writeToFile { get; set; }
        public bool rawData { get; set; }
        public string filePath { get; set; }

        public CmdArgs()
        {
            disableGUI = false;
            writeToFile = false;
            rawData = false;
            filePath = @"c:\Temp\DataExchangeTestServer";
        }
    } 

    public class WriteToFile
    {
        public static CmdArgs CmdArgs { get; set; }

        public static void SerializeToFile(string prefix, string sSerializedString)
        {
            FileStream fStream = null;
            StreamWriter sWriter = null;

            if ((DataExchangeTestServer.CmdArgs != null) && (!string.IsNullOrEmpty(DataExchangeTestServer.CmdArgs.filePath)))
            {
                try
                {
                    if (!Directory.Exists(DataExchangeTestServer.CmdArgs.filePath))
                    {
                        Directory.CreateDirectory(DataExchangeTestServer.CmdArgs.filePath);
                    }

                    string sFilename = string.Format("{0}\\{1}_{2}.xml", DataExchangeTestServer.CmdArgs.filePath, prefix, DateTime.Now.ToString("yyyyMMdd_HHmmss"));
                    fStream = new FileStream(sFilename, FileMode.Create, FileAccess.Write);
                    sWriter = new StreamWriter(fStream);

                    sWriter.Write(sSerializedString);
                    sWriter.Flush();
                }
                catch (Exception)
                {
                    // Not much to do.
                }
                finally
                {
                    if (sWriter != null)
                    {
                        sWriter.Close();
                        sWriter.Dispose();
                        sWriter = null;
                    }

                    if (fStream != null)
                    {
                        fStream.Close();
                        fStream.Dispose();
                        fStream = null;
                    }
                }
            }
        }
    }

    /// <summary>
    /// SOAP XML worker method to view incoming and outgoing SOAP messages to the WCF service.
    /// </summary>
    public class ConsoleOutputMessageInspector : IDispatchMessageInspector
    {
        public object AfterReceiveRequest(ref Message request, IClientChannel channel, InstanceContext instanceContext)
        {
            // Make a copy of the SOAP packet for viewing.
            MessageBuffer buffer = request.CreateBufferedCopy(Int32.MaxValue);
            Message msgCopy = buffer.CreateMessage();

            request = buffer.CreateMessage();

            // Get the SOAP XML content.
            string strMessage = buffer.CreateMessage().ToString();

            bool isMessageBody = false;

            // Get the SOAP XML body content.
            try
            {
                System.Xml.XmlDictionaryReader xrdr = msgCopy.GetReaderAtBodyContents();
                string bodyData = xrdr.ReadOuterXml();

                // Replace the body placeholder with the actual SOAP body.
                strMessage = strMessage.Replace("... stream ...", bodyData);

                isMessageBody = true;

                // Display the SOAP XML.
                System.Diagnostics.Debug.WriteLine("Received:\n" + strMessage);

                if ((DataExchangeTestServer.CmdArgs != null) && (DataExchangeTestServer.CmdArgs.writeToFile) && (DataExchangeTestServer.CmdArgs.rawData))
                {
                    WriteToFile.SerializeToFile("Received_RAW", strMessage);    
                }                
            }
            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine(string.Format("Received: message not found. Error : {0}\n", e.Message));
            }

            string msgBody = (isMessageBody) ? string.Format("Message body size {0}", strMessage.Length) : "No message body";
            DataExchangeTestServerForm.SetRequestReceived(string.Format("Request received {0}. {1}{2}", DateTime.Now.ToString("yyyyMMdd_HHmmss"), msgBody, Environment.NewLine));

            return null;
        }

        public void BeforeSendReply(ref Message reply, object correlationState)
        {
            // Make a copy of the SOAP packet for viewing.
            MessageBuffer buffer = reply.CreateBufferedCopy(Int32.MaxValue);
            reply = buffer.CreateMessage();

            // Display the SOAP XML.
            string strMessage = buffer.CreateMessage().ToString();
            System.Diagnostics.Debug.WriteLine("Sending:\n" + strMessage);

            if ((DataExchangeTestServer.CmdArgs != null) && (DataExchangeTestServer.CmdArgs.writeToFile) && (DataExchangeTestServer.CmdArgs.rawData))
            {
                WriteToFile.SerializeToFile("Sending_RAW", strMessage);
            }
        }
    }

    public class MyServiceBehavior : Attribute, IServiceBehavior
    {
        public void ApplyDispatchBehavior(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase)
        {
            foreach (ChannelDispatcher cDispatcher in serviceHostBase.ChannelDispatchers)
            {
                foreach (EndpointDispatcher eDispatcher in cDispatcher.Endpoints)
                {
                    eDispatcher.DispatchRuntime.MessageInspectors.Add(new ConsoleOutputMessageInspector());
                }
            }
        }

        public void AddBindingParameters(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase, Collection<ServiceEndpoint> endpoints, BindingParameterCollection bindingParameters)
        {
        }

        public void Validate(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase)
        {
        }
    }


    public class DataExchangeTestServerArguments
    {
        public CmdArgs CmdArgs { get; set; }
    }

    [MyServiceBehavior]
    public class DataExchangeTestServer : IDataExchangeTestServer
    {
        public DataExchangeTestServer()
        {
            WriteToFile.CmdArgs = new CmdArgs();
        }

        public DataExchangeTestServer(DataExchangeTestServerArguments arguments)
        {
            WriteToFile.CmdArgs = arguments.CmdArgs;
        }

        public static CmdArgs CmdArgs
        {
            get { return WriteToFile.CmdArgs; }
            set { WriteToFile.CmdArgs = value; }
        }

        public string Echo(string value)
        {
            if ((CmdArgs != null) && (CmdArgs.writeToFile) && (!CmdArgs.rawData))
            {
                WriteToFile.SerializeToFile("Echo", value.ToString());
            }

            return value;            
        }

        public bool Consume(string value)
        {
            if ((CmdArgs != null) && (CmdArgs.writeToFile) && (!CmdArgs.rawData))
            {
                WriteToFile.SerializeToFile("Consume", value);
            }

            return true;
        }
    }
}
