namespace Powel
{
	namespace Icc
	{
		namespace Interop
		{
			public enum class IccProgram
			{
				// PROGRAM
				P_TEST = 0, //TODO ok?
				P_SIM = 15,
				P_IMPCOMMON = 30,
				P_TSIMPUIS = 32,
				P_ALARM = 79,
				P_AVAILABILITY = 80,
				P_WORK_ORDER_ADM = 81,
				P_WORK_ORDER_CLIENT = 82,
				P_MDMS_REPORT = 83,
				P_REPLICATION = 84,
				P_MESSAGE_CONTROL = 85,
				P_CORRECTION_SETTLEMENT = 86,
				P_SNOWSTORAGE = 87,
				P_CAPACITY_MATRIX = 88,
				P_SERVICE_HOST = 89,
				P_VF_OUTPUT_RETRIEVER = 90,
				P_MDMS_FILE_REPORT = 93,
				P_POWEL_ICC_WS_WORKORDER = 94,
				P_PROFILE_RECONCILIATION = 95,
				P_POWEL_ICC_WS_INTEGRATION = 96,
				P_ALARMADM = 99,
				P_POWEL_ICC_WS_METERING = 100,
				P_ACTIVATION_REQUEST = 104,
			};

			public enum class IccModule
			{
				// TP_ICC_APPLICATION
				M_SIM = 45,
				M_ALARM = 108,
				M_ALARM_ADM = 109,
				M_AVAILABILITY = 111,
				M_ALARM_SERVICE = 112,
				M_XML_WEB_SERVICES = 117,
				M_WORK_ORDER_ADM = 118,
				M_WORK_ORDER_CLIENT = 119,
				M_MDMS_REPORT = 120,
				M_AVAILABILITY_IMPORT = 121,
				M_REPLICATION_SERVICE = 122,
				M_INSTALLATION_SERVICE = 124,
				M_MESSAGE_CONTROL = 125,
				M_SNOWSTORAGE = 126,
				M_CORRECTION_SETTLEMENT = 132,
				M_CAPACITY_MATRIX = 133,
				M_SERVICE_HOST = 134,
				M_VF_OUTPUT_RETRIEVER = 135,
				M_INTEGRATION_HUB = 141,
				M_MDMS_FILE_REPORT = 138,
				M_PROFILE_RECONCILIATION = 146,
				M_POWEL_ICC_WS_WORKORDER = 147,
				M_POWEL_ICC_WS_INTEGRATION = 148,
				M_POWEL_ICC_TIME_SERIES_IMPORT_SERVICE = 149,
				M_POWEL_ICC_TIME_SERIES_IMPORT_RESPONSE_SERVICE = 150,
				M_POWEL_ICC_TIME_SERIES_EXPORT_SERVICE = 151,
				M_POWEL_ICC_METERING_ORDER_EXPORT_SERVICE = 152,
				M_POWEL_ICC_METERING_ONDEMAND_ORDER_EXPORT_SERVICE = 153,
				M_POWEL_ICC_GS2_EXPORT_SERVICE = 154,
				M_MESSAGELOG = 155,
				M_POWEL_ICC_METERING_SERVICE_QUEUE_HANDLER_SERVICE = 156,
				M_POWEL_ICC_METERING_SERVICE_RESPONSE_SERVICE = 157,
				M_SAP_UPDATE = 158,
				M_GRIB_UTILITY = 163,
				M_IMPORT_APPLICATION_MANAGER = 170,
				M_DATA_EXCHANGE_MANAGER = 171,
				M_EXPRESS_EXPORT_CONNECTOR = 172,
				M_AZUREBUS_DATA_EXCHANGE_MANAGER = 173,
				M_ECP_AMQP_DATA_EXCHANGE_MANAGER = 174,
				M_ICCP_DATA_EXCHANGE_MANAGER = 178,
				M_NP_AUCTION_DATA_EXCHANGE_MANAGER = 179,
				M_ACTIVATION_REQUEST = 180,
				M_NP_REQUESTER = 182,
				M_NP_AUCTION_SERVICE_HOST = 183,
				M_EXPRESS_IMPORT_CONNECTOR = 191,
				M_SERVICEBUS_FOR_TSS_SERVICE = 193,
				M_MASSTRANSIT_FILEWATCHER_DATA_EXCHANGE_MANAGER
			};

			public enum class LogFormat
			{
				Printf,
				Positional
			};

			public enum class ReportType
			{
				Graph,
				Table,
				Time_Series,
				Time_Series_Report
			};

			public enum class ObjectTypes
			{
				Test = 0,
				TimeSeries = 1,
                WorkOrder = 18,
                Component = 19,
				ImportDefinition = 900
			};
		}
	}
}

