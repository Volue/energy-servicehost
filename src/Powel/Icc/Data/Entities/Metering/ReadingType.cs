using System.Runtime.Serialization;

namespace Powel.Icc.Data.Entities.Metering
{
    /// <summary>
    /// Object class for a specific reading type
    /// </summary>
    [DataContract]
    public class ReadingType
    {
        #region Fields

        private readonly int _timeAttribute;
        private readonly int _dataQualifier;
        private readonly int _accumulationBehaviour;
        private readonly int _flowDirection;
        private readonly int _uomCategory;
        private readonly int _measurementCategory;
        private readonly int _phase;
        private readonly int _multiplier;
        private readonly int _unitOfMeasurement;
        private readonly string _iecTimeAttributeType;
        private readonly string _iecDataQualifierType;
        private readonly string _iecAccumulationBehaviourType;
        private readonly string _iecDirectionOfFlowType;
        private readonly string _iecUnitOfMeasurementEnumerationType;
        private readonly string _iecMeasurementCategoryType;
        private readonly string _iecPhaseEnumerationType;
        private readonly string _iecMetricMultiplierType;
        private readonly string _iecUnitOfMeasurementType;

        private string _tsExt;
        private int _tsinKey;
        private int _tserKey;
        private int _unmeKey;
        private int _vltpKey;
        private int _paraFlag;
        private int _convertusingLocaltime;
        private int _useRegistrationHandling;
        private int _auditLevel;
        private int _valKey;
        private int _corKey;
        private bool _initialized = false;

        #endregion Fields

        #region Constructors

        public ReadingType( int timeAttribute,
                            int dataQualifier,
                            int accumulationBehaviour,
                            int flowDirection,
                            int uomCategory,
                            int measurementCategory,
                            int phase,
                            int multiplier,
                            int unitOfMeasurement,
                            string iecTimeAttributeType,
                            string iecDataQualifierType,
                            string iecAccumulationBehaviourType,
                            string iecDirectionOfFlowType,
                            string iecUnitOfMeasurementEnumerationType,
                            string iecMeasurementCategoryType,
                            string iecPhaseEnumerationType,
                            string iecMetricMultiplierType,
                            string iecUnitOfMeasurementType)
        {
            _timeAttribute = timeAttribute;
            _dataQualifier = dataQualifier;
            _accumulationBehaviour = accumulationBehaviour;
            _flowDirection = flowDirection;
            _uomCategory = uomCategory;
            _measurementCategory = measurementCategory;
            _phase = phase;
            _multiplier = multiplier;
            _unitOfMeasurement = unitOfMeasurement;
            _iecTimeAttributeType = iecTimeAttributeType;
            _iecDataQualifierType = iecDataQualifierType;
            _iecAccumulationBehaviourType = iecAccumulationBehaviourType;
            _iecDirectionOfFlowType = iecDirectionOfFlowType;
            _iecUnitOfMeasurementEnumerationType = iecUnitOfMeasurementEnumerationType;
            _iecMeasurementCategoryType = iecMeasurementCategoryType;
            _iecPhaseEnumerationType = iecPhaseEnumerationType;
            _iecMetricMultiplierType = iecMetricMultiplierType;
            _iecUnitOfMeasurementType = iecUnitOfMeasurementType;
        }

        public ReadingType(ReadingType srcRt)
        {
            _timeAttribute = srcRt._timeAttribute;
            _dataQualifier = srcRt._dataQualifier;
            _accumulationBehaviour = srcRt._accumulationBehaviour;
            _flowDirection = srcRt._flowDirection;
            _uomCategory = srcRt._uomCategory;
            _measurementCategory = srcRt._measurementCategory;
            _phase = srcRt._phase;
            _multiplier = srcRt._multiplier;
            _unitOfMeasurement = srcRt._unitOfMeasurement;
            _iecTimeAttributeType = srcRt._iecTimeAttributeType;
            _iecDataQualifierType = srcRt._iecDataQualifierType;
            _iecAccumulationBehaviourType = srcRt._iecAccumulationBehaviourType;
            _iecDirectionOfFlowType = srcRt._iecDirectionOfFlowType;
            _iecUnitOfMeasurementEnumerationType = srcRt._iecUnitOfMeasurementEnumerationType;
            _iecMeasurementCategoryType = srcRt._iecMeasurementCategoryType;
            _iecPhaseEnumerationType = srcRt._iecPhaseEnumerationType;
            _iecMetricMultiplierType = srcRt._iecMetricMultiplierType;
            _iecUnitOfMeasurementType = srcRt.IECUnitOfMeasurementType;

            _tsExt = srcRt._tsExt;
            _tsinKey = srcRt._tsinKey;
            _tserKey = srcRt._tserKey;
            _unmeKey = srcRt._unmeKey;
            _vltpKey = srcRt._vltpKey;
            _paraFlag = srcRt._paraFlag;
            _convertusingLocaltime = srcRt._convertusingLocaltime;
            _useRegistrationHandling = srcRt._useRegistrationHandling;
            _auditLevel = srcRt._auditLevel;
            _valKey = srcRt._valKey;
            _corKey = srcRt._corKey;
            _initialized = srcRt._initialized;
        }
        //public ReadingType(int timeAttribute,
        //                    int dataQualifier,
        //                    int accumulationBehaviour,
        //                    int flowDirection,
        //                    int uomCategory,
        //                    int measurementCategory,
        //                    int phase,
        //                    int multiplier,
        //                    int unitOfMeasurement)
        //{
        //    _timeAttribute = timeAttribute;
        //    _dataQualifier = dataQualifier;
        //    _accumulationBehaviour = accumulationBehaviour;
        //    _flowDirection = flowDirection;
        //    _uomCategory = uomCategory;
        //    _measurementCategory = measurementCategory;
        //    _phase = phase;
        //    _multiplier = multiplier;
        //    _unitOfMeasurement = unitOfMeasurement;
        //}
        #endregion Constructors

        #region Properties

        [DataMember]
        public int TimeAttribute
        {
            get { return _timeAttribute; }
        }

        [DataMember]
        public int DataQualifier
        {
            get { return _dataQualifier; }
        }

        [DataMember]
        public int AccumulationBehaviour
        {
            get { return _accumulationBehaviour; }
        }

        [DataMember]
        public int FlowDirection
        {
            get { return _flowDirection; }
        }

        [DataMember]
        public int UomCategory
        {
            get { return _uomCategory; }
        }

        [DataMember]
        public int MeasurementCategory
        {
            get { return _measurementCategory; }
        }

        [DataMember]
        public int Phase
        {
            get { return _phase; }
        }

        [DataMember]
        public int Multiplier
        {
            get { return _multiplier; }
        }

        [DataMember]
        public int UnitOfMeasurement
        {
            get { return _unitOfMeasurement; }
        }

        [DataMember]
        public string IECTimeAttributeType
        {
            get { return _iecTimeAttributeType; }
        }

        [DataMember]
        public string IECDataQualifierType
        {
            get { return _iecDataQualifierType; }
        }

        [DataMember]
        public string IECAccumulationBehaviourType
        {
            get { return _iecAccumulationBehaviourType; }
        }

        [DataMember]
        public string IECDirectionOfFlowType
        {
            get { return _iecDirectionOfFlowType; }
        }

        [DataMember]
        public string IECUnitOfMeasurementEnumerationType
        {
            get { return _iecUnitOfMeasurementEnumerationType; }
        }

        [DataMember]
        public string IECMeasurementCategoryType
        {
            get { return _iecMeasurementCategoryType; }
        }

        [DataMember]
        public string IECPhaseEnumerationType
        {
            get { return _iecPhaseEnumerationType; }
        }

        [DataMember]
        public string IECMetricMultiplierType
        {
            get { return _iecMetricMultiplierType; }
        }

        [DataMember]
        public string IECUnitOfMeasurementType
        {
            get { return _iecUnitOfMeasurementType; }
        }

        [DataMember]
        public string TsExt
        {
            get { return _tsExt; }
            set { _tsExt = value; }
        }

        [DataMember]
        public int TsinKey
        {
            get { return _tsinKey; }
            set { _tsinKey = value; }
        }

        [DataMember]
        public int TserKey
        {
            get { return _tserKey; }
            set { _tserKey = value; }
        }

        [DataMember]
        public int UnmeKey
        {
            get { return _unmeKey; }
            set { _unmeKey = value; }
        }

        [DataMember]
        public int VltpKey
        {
            get { return _vltpKey; }
            set { _vltpKey = value; }
        }

        [DataMember]
        public int Paraflag
        {
            get { return _paraFlag; }
            set { _paraFlag = value; }
        }

        [DataMember]
        public int ConvertusingLocaltime
        {
            get { return _convertusingLocaltime; }
            set { _convertusingLocaltime = value; }
        }

        [DataMember]
        public int UseRegistrationHandling
        {
            get { return _useRegistrationHandling; }
            set { _useRegistrationHandling = value; }
        }

        [DataMember]
        public int AuditLevel
        {
            get { return _auditLevel; }
            set { _auditLevel = value; }
        }

        [DataMember]
        public int ValKey
        {
            get { return _valKey; }
            set { _valKey = value; }
        }

        [DataMember]
        public int CorKey
        {
            get { return _corKey; }
            set { _corKey = value; }
        }

        [DataMember]
        public bool Initialized
        {
            get { return _initialized; }
            set { _initialized = value; }
        }
        #endregion Properties
    }
}
