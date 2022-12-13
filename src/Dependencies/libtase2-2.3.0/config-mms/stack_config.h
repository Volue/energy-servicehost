/*
 * config.h
 *
 * Contains global defines to configure the stack. You need to recompile the stack to make
 * changes effective (make clean; make)
 *
 */

#ifndef STACK_CONFIG_H_
#define STACK_CONFIG_H_

/* include asserts if set to 1 */
#define DEBUG 0

/* print debugging information with printf if set to 1 */
#define DEBUG_SOCKET 0
#define DEBUG_COTP 0

/* log to user application when set to 1 */
#define DEBUG_ISO_SERVER 1
#define DEBUG_ISO_CLIENT 1
#define DEBUG_MMS_CLIENT 1
#define DEBUG_MMS_SERVER 1
#define DEBUG_MMS_ENDPOINT 1

/* Maximum MMS PDU SIZE - default is 65000 */
#define CONFIG_MMS_MAXIMUM_PDU_SIZE 120000

/*
 * Enable single threaded mode
 *
 * 1 ==> server runs in single threaded mode (a single thread for the server and all client connections)
 * 0 ==> server runs in multi-threaded mode (one thread for each connection and
 * one server background thread - DO NOT USE with libtase2)
 */
#define CONFIG_MMS_SINGLE_THREADED 1

#define CONFIG_MMS_SUPPORT_TLS 1

/*
 * Optimize stack for threadless operation - don't use semaphores
 *
 * WARNING: If set to 1 normal single- and multi-threaded server are no longer working!
 */
#define CONFIG_MMS_THREADLESS_STACK 0

/* number of concurrent MMS client connections the server accepts, -1 for no limit */
#define CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS 256

/* activate TCP keep alive mechanism. 1 -> activate */
#define CONFIG_ACTIVATE_TCP_KEEPALIVE 1

/* time (in s) between last message and first keepalive message */
#define CONFIG_TCP_KEEPALIVE_IDLE 5

/* time between subsequent keepalive messages if no ack received */
#define CONFIG_TCP_KEEPALIVE_INTERVAL 2

/* number of not missing keepalive responses until socket is considered dead */
#define CONFIG_TCP_KEEPALIVE_CNT 2

/* maximum COTP (ISO 8073) TPDU size - valid range is 1024 - 8192 */
#define CONFIG_COTP_MAX_TPDU_SIZE 8192

/* timeout while reading from TCP stream in ms */
#define CONFIG_TCP_READ_TIMEOUT_MS 1000

/* MMS virtual file store base path - where MMS file services are looking for files */
#define CONFIG_VIRTUAL_FILESTORE_BASEPATH "./vmd-filestore/"

/* Maximum number of open file per MMS connection (for MMS file read service) */
#define CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION 5

/* Maximum number of the domain specific data sets - this also includes the static (pre-configured) and dynamic data sets */
#define CONFIG_MMS_MAX_NUMBER_OF_DOMAIN_SPECIFIC_DATA_SETS 256

/* Maximum number of association specific data sets */
#define CONFIG_MMS_MAX_NUMBER_OF_ASSOCIATION_SPECIFIC_DATA_SETS 10

/* Maximum number of VMD specific data sets */
#define CONFIG_MMS_MAX_NUMBER_OF_VMD_SPECIFIC_DATA_SETS 256

/* Maximum number of the members in a data set (named variable list) */
#define CONFIG_MMS_MAX_NUMBER_OF_DATA_SET_MEMBERS 500

/* maximum number of contemporary file upload tasks (obtainFile) per server instance */
#define CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS 5

/* Definition of supported services */
#define MMS_DEFAULT_PROFILE 1

#if (MMS_DEFAULT_PROFILE == 1)
#define MMS_READ_SERVICE 1
#define MMS_WRITE_SERVICE 1
#define MMS_GET_NAME_LIST 1
#define MMS_JOURNAL_SERVICE 0
#define MMS_GET_VARIABLE_ACCESS_ATTRIBUTES 1
#define MMS_DATA_SET_SERVICE 1
#define MMS_DYNAMIC_DATA_SETS 1
#define MMS_GET_DATA_SET_ATTRIBUTES 1
#define MMS_STATUS_SERVICE 1
#define MMS_IDENTIFY_SERVICE 1
#define MMS_FILE_SERVICE 1
#define MMS_OBTAIN_FILE_SERVICE 1
#endif /* MMS_DEFAULT_PROFILE */

/* support flat named variable name space required by IEC 61850-8-1 MMS mapping */
#define CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE 0

/* VMD scope named variables are not used by IEC 61850 (one application is ICCP) */
#define CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES 1

/* allow VMD scope named variable lists (data sets) */
#define CONFIG_MMS_ALLOW_DEFINITION_OF_VMD_SPECIFIC_DATA_SETS 0

/* Sort getNameList response according to the MMS specified collation order - this is required by the standard
 * Set to 0 only for performance reasons and when no certification is required! */
#define CONFIG_MMS_SORT_NAME_LIST 1

#define CONFIG_INCLUDE_PLATFORM_SPECIFIC_HEADERS 0

/* Support user access to raw messages */
#define CONFIG_MMS_RAW_MESSAGE_LOGGING 1

/* Support collection of statistic information (messages sent/received, errors, ...) */
#define CONFIG_MMS_COLLECT_STATISTICS 1

/* Allow to set the virtual filestore basepath for MMS file services at runtime with the
 * MmsServer_setFilestoreBasepath function
 */
#define CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME 1

#define CONFIG_MMS_SEPARATOR_CHAR '+'

#endif /* STACK_CONFIG_H_ */
