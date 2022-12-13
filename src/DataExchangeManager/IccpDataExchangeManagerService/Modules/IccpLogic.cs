using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;
using log4net;
using Newtonsoft.Json;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.IccpDataExchangeManager.Settings;
using Powel.MessageBus.Contracts.TimeSeries.Contract;
using Powel.MessageBus.Contracts.TimeSeries.Model;
using TASE2.Library.Common;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.Modules
{
    public class IccpLogic : WsLogicBase
    {
        protected static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        internal class PointValueMapping
        {
            public PointValue Value { get; set; }
            public string ExternalReference { get; set; }
            public string ValueUnit { get; set; }   // For possible unit conversion in ts import utility.
        }
        internal class DataPoints : Dictionary<string,PointValueMapping>    // VariableName,PointValueMapping
        {
        }
        internal class DataSets : Dictionary<string,DataPoints> // DataSetName,DataPoints
        {
        }

        private Dictionary<string, DataSets> _domains = new Dictionary<string, DataSets>(); // DomainName,DataSets

        private readonly IImportApplication _importApplication;

        // IccpParameters
        public IccpParameters.DatasetString[] Priorities { get; set; }
        private IccpParameters.TimeserieMap[] _timeseriesMaps;
        public IccpParameters.TimeserieMap[] TimeseriesMaps {
            get { return _timeseriesMaps; }
            set
            {
                ClearExternalReference();
                _timeseriesMaps = value;
                MatchTimeseriesMapping();
            }
        }

        public bool UseImportApplicationManagerService { get; set; }

        public IccpLogic(Func<IDataExchangeApi> dataExchangeApiFactory, IServiceEventLogger serviceEventLogger, IImportApplication importApplication)
            : base(dataExchangeApiFactory, serviceEventLogger)
        {
            _importApplication = importApplication;
        }

        public void AddDataSetPoint(string domainName, string dataSetName, string variableName, PointValue pointValue)
        {
            string domName = domainName;
            if (domName == null)
                domName = String.Empty;

            DataSets dataSets;
            if (!_domains.TryGetValue(domName, out dataSets))
            {
                dataSets = new DataSets();
                _domains.Add(domName,dataSets);
            }

            DataPoints dataPoints;
            if (!dataSets.TryGetValue(dataSetName, out dataPoints))
            {
                dataPoints = new DataPoints();
                dataSets.Add(dataSetName,dataPoints);
            }

            PointValueMapping pointValueMapping;
            if (!dataPoints.TryGetValue(variableName, out pointValueMapping))
            {
                pointValueMapping = new PointValueMapping {Value = pointValue, ExternalReference = String.Empty};
                dataPoints.Add(variableName,pointValueMapping);
            }
            pointValueMapping.Value = pointValue;
            pointValueMapping.ExternalReference = String.Empty;
            pointValueMapping.ValueUnit = String.Empty;
        }

        private void ClearExternalReference()
        {
            foreach (var domain in _domains)
            {
                foreach (var dataSet in domain.Value)
                {
                    foreach (var dataPoint in dataSet.Value)
                    {
                        dataPoint.Value.ExternalReference = string.Empty;
                        dataPoint.Value.ValueUnit = string.Empty;
                    }
                }
            }
        }

        public int MatchTimeseriesMapping()
        {
            int nMatched = 0;
            if (_timeseriesMaps.Length == 0 || _domains.Count == 0)
                return nMatched;

            foreach (var timeseriesMap in _timeseriesMaps)
            {
                DataSets dataSets;
                if (_domains.TryGetValue(timeseriesMap.Variable.Domain, out dataSets))
                {
                    DataPoints dataPoints;
                    if (dataSets.TryGetValue(timeseriesMap.Variable.Dataset, out dataPoints))
                    {
                        PointValueMapping pointValueMapping;
                        if (dataPoints.TryGetValue(timeseriesMap.Variable.VariableCode, out pointValueMapping))
                        {
                            pointValueMapping.ExternalReference = timeseriesMap.ExternalReference;
                            pointValueMapping.ValueUnit = timeseriesMap.ValueUnit;
                            nMatched++;
                        }
                        else Log.Warn($"{timeseriesMap.Variable.Domain}/{timeseriesMap.Variable.Dataset}/{timeseriesMap.Variable.VariableCode} not matched");
                    }
                }
            }

            return nMatched;
        }

        public List<string> GetDataSetsWithExternalReference(string domainName)
        {
            // Returns list of unique dataset names that is not a dynamic dataset and which have variables with an external reference.
            var ret = new List<string>();
            DataSets dataSets;
            if (_domains.TryGetValue(domainName, out dataSets))
            {
                var dynDss = GetDynamicDatasets(domainName);
                foreach (var dataPoints in dataSets)
                {
                    if (!ret.Contains(dataPoints.Key) && !dynDss.Contains(dataPoints.Key))
                    {
                        foreach (var dataPoint in dataPoints.Value)
                        {
                            if (!string.IsNullOrEmpty(dataPoint.Value.ExternalReference) && !ret.Contains(dataPoints.Key))
                                ret.Add(dataPoints.Key);
                        }
                    }
                }
            }

            return ret;
        }

        public List<string> GetDynamicDatasets(string domainName)
        {
            var ret = new List<string>();
            DataSets dataSets;
            if (_domains.TryGetValue(domainName, out dataSets))
            {
                foreach (var timeseriesMap in _timeseriesMaps)
                {
                    if (timeseriesMap.Variable.Domain != domainName)
                        continue;
                    string dataset = timeseriesMap.Variable.Dataset;
                    if (dataSets.ContainsKey(dataset))
                        continue;   // Datasets which already exists on server can not be defined as dynamic dataset.
                    if (!ret.Contains(dataset))
                        ret.Add(dataset);
                }
            }

            return ret;
        }

        public List<string> GetDynamicDatasetVariables(string domainName, string datasetName)
        {
            var ret = new List<string>();
            DataSets dataSets;
            if (_domains.TryGetValue(domainName, out dataSets))
            {
                foreach (var timeserieMap in _timeseriesMaps.Where(m => m.Variable.Domain == domainName && m.Variable.Dataset == datasetName))
                {
                    string domainPrefix = domainName + "/";
                    if (ret.Count == 0)
                    {
                        // The "Transfer_Set_Name" is important. Otherwise the client side cannot relate the report to a specific transfer set 
                        // when report by exception (RBE) is used. The client should add the "Transfer_Set_Name" as the first element of the data set 
                        // when using dynamic data sets.
                        var variables = new string[] { "Transfer_Set_Name", "DSConditions_Detected", "Event_Code_Detected", "Transfer_Set_Time_Stamp" };
                        foreach (var variable in variables)
                        {
                            ret.Add(domainPrefix + variable);
                        }
                    }
                    var domainVariable = domainPrefix + timeserieMap.Variable.VariableCode;
                    if (!ret.Contains(domainVariable))
                        ret.Add(domainVariable);
                    else Log.Warn($"Duplicate variable: {domainName}/{datasetName}/{timeserieMap.Variable.VariableCode}");
                }
            }

            return ret;
        }

        private TimeSeriesContract _message;
        public void CreateNewMessage(string docId, string origin = "")
        {
            _message = new TimeSeriesContract
            {
                TimeSeriesHeader = new TimeSeriesHeader
                {
                    DocumentIdentification = docId,
                    Origin = origin,
                    CreatedDateTime = DateTime.Now,
                    TSType = ""
                },
                TimeSeries = new List<MessageBus.Contracts.TimeSeries.Model.TimeSeries>()
            };
        }

        public bool SendMessage(string datasetName)
        {
            if (_message == null || _message.TimeSeries.Count == 0)
            {
                return false;
            }

            SubmitImport(BuildDataExchangeImportMessage(datasetName), null);
            return true;
        }

        internal DataExchangeImportMessage BuildDataExchangeImportMessage(string datasetName)
        {
            if (_message == null)
                return null;
            var dataExchMsg = new DataExchangeImportMessage
            {
                ExternalReference = _message.TimeSeriesHeader.DocumentIdentification,
                SenderId = _message.TimeSeriesHeader.Origin,
                Format = "JSON",
                Protocol = "JSON.TS",
                Priority = (Priorities.FirstOrDefault(p => p.Dataset == datasetName) ?? Priorities.FirstOrDefault(p => p.Dataset == IccpParameters.Default)).String,
                RoutingAddress = "ICCP:" + _message.TimeSeriesHeader.Origin
            };
            var msgStr = JsonConvert.SerializeObject(_message);
            Log.Debug($"Sending: {msgStr}");
            var msgSize = msgStr.Length;
            const int metaDataSize = 1000;
            dataExchMsg.Version = metaDataSize + msgSize >= 4 * 1024 * 1024 ? 1 : 0;  // MSMQ has a MAX message length of 4Mb
            dataExchMsg.SetMessageData(msgStr);
            return dataExchMsg;
        }

        private PointValueMapping GetPointValueMapping(string domainName, string dataSetName, string variableName)
        {
            DataSets dataSets;
            if (_domains.TryGetValue(domainName, out dataSets))
            {
                DataPoints dataPoints;
                if (dataSets.TryGetValue(dataSetName,out dataPoints))
                {
                    PointValueMapping pointValueMapping;
                    if (dataPoints.TryGetValue(variableName,out pointValueMapping))
                    {
                        return pointValueMapping;
                    }
                }
            }

            return null;
        }

        public void AddMessageValue(string domainName,string dataSetName,string variableName,PointValue pointValue)
        {
            if (_message == null || !pointValue.HasTimeStamp)
                return;
            string externalReference;
            string valueUnit;
            if (string.IsNullOrEmpty(dataSetName))
            {
                // Dynamic dataset without Transfer_Set_Name
                var timeseriesMap = _timeseriesMaps.FirstOrDefault(m => m.Variable.Domain == domainName && m.Variable.VariableCode == variableName);
                if (timeseriesMap == null)
                    return;
                externalReference = timeseriesMap.ExternalReference;
                valueUnit = timeseriesMap.ValueUnit;
            }
            else
            {
                // Server dataset
                var pointValueMapping = GetPointValueMapping(domainName, dataSetName, variableName);
                if (pointValueMapping == null)
                {
                    // Dynamic dataset with Transfer_Set_Name
                    var timeseriesMap = _timeseriesMaps.FirstOrDefault(m => m.Variable.Domain == domainName && m.Variable.Dataset == dataSetName && m.Variable.VariableCode == variableName);
                    if (timeseriesMap == null)
                        return;
                    externalReference = timeseriesMap.ExternalReference;
                    valueUnit = timeseriesMap.ValueUnit;
                }
                else
                {
                    if (string.IsNullOrEmpty(pointValueMapping.ExternalReference))
                        return;
                    externalReference = pointValueMapping.ExternalReference;
                    valueUnit = pointValueMapping.ValueUnit;
                }
            }

            var time = DateTimeOffset.FromUnixTimeMilliseconds((long) pointValue.TimeStamp).DateTime;
            var ts = _message.TimeSeries.FirstOrDefault(t => t.TSReference == externalReference);
            if (ts == null)
            {
                _message.TimeSeries.Add(new MessageBus.Contracts.TimeSeries.Model.TimeSeries
                {
                    TSReference = externalReference,
                    TimeStamp = time,
                    MeasureUnit = valueUnit,
                    Values = new List<TimeSeriesValue> {new TimeSeriesValue {Value = ValueAsString(pointValue)}}
                });
                return;
            }

            if (ts.TimeStamp != time)
            {
                ts.TimeStamp = time;
            }
            ts.Values.First().Value = ValueAsString(pointValue);    // Don't understand why Values is a List all the time there is no way of knowing the TimeStamp for Value[1].
        }

        private string ValueAsString(PointValue pointValue)
        {
            var ret = string.Empty;
            switch (pointValue.Type)
            {
                case PointValueType.DISCRETE:
                    ret = pointValue.ValueDiscrete.ToString();
                    break;
                case PointValueType.REAL:
                    ret = pointValue.ValueReal.ToString(CultureInfo.InvariantCulture);
                    break;
                default:
                    throw new NotImplementedException($"Not implemented type: {pointValue.Type.ToString()}");
            }

            return ret;
        }
        public override void SubmitImport(DataExchangeImportMessage message, Action<bool> onCompletion)
        {
            if (UseImportApplicationManagerService)
                base.SubmitImport(message, onCompletion);
            else
            {
                var ret = _importApplication.HandleMessageWithExceptionHandling(() =>
                    {
                        var lRet = _importApplication.HandleMessage(message);
                        message.DeleteMessageData();
                        return lRet;
                    },
                    onImportRunnerFailed: (exc) =>
                    {
                        message.DeleteMessageData();    // Not put on Error queue. further handling from MessageLogSearch
                    },
                    onTimeout: (exc) =>
                    {
                        message.DeleteMessageData();    // Not put on Error queue. further handling from MessageLogSearch
                    },
                    onErrorCaught: () =>
                    {
                        var api = _dataExchangeApiFactory();
                        using (IDataExchangeQueueTransaction transaction = api.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
                        {
                            transaction.Begin();
                            api.EnqueueErroneousImportMessage(message, transaction); // Move to the error queue.
                            transaction.Commit();
                        }
                    });
                onCompletion?.Invoke(ret);
            }
        }
    }
}
