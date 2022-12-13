#ifndef _icc_messages_h_
#define _icc_messages_h_

#ifdef WIN32
/* 
winsock.h defines s_host, which is a part of
struct defs below 
Then we have to undef it here..
sih 980705
*/
#ifdef s_host
#undef s_host
#endif
#endif
/*******
  Constant Definitions
*******/

#define ADM_REASON_PARENT_CRASH 5
#define ADM_REASON_PARENT_EXIT 4
#define ADM_REASON_QUIT 2
#define ADM_REASON_SHUTDOWN 3
#define ADM_REASON_STOP 0
#define ADM_REASON_TIMEOUT 1
#define M_ADM 6
#define M_APP_INITIATED 4
#define M_BUFFER 7
#define M_CALCSET_CODE_LEN 21
#define M_CALCSET_DESC_LEN 256
#define M_CALCSET_EXPR_LEN 256
#define M_CALCSET_NAME_LEN 41
#define M_CALLBACK 2
#define M_CATCH_NAME 41
#define M_COM_SIZE 512
#define M_END_OF_SEQUENCE 2
#define M_FNAME_SIZE 256
#define M_F_TXY_Y_MAX 1
#define M_F_TXY_Y_MIN 2
#define M_GLOBAL_MENU 8
#define M_GMS_STATE 1
#define M_HBV_FORECAST_TYPE_SIZE 15
#define M_HBV_RUN_TYPE_SIZE 12
#define M_INIT 3
#define M_LOG 5
#define M_MENU_TAG_SIZE 61
#define M_NAME_SIZE 41
#define M_START_OF_SEQUENCE 1
#define M_STRING_SIZE 512
#define M_SUBSCRIBE 9
#define M_TEXT_SIZE 100
#define M_TIME_SIZE 18
#define M_TXY_FORM_SIZE 16



/*******
  Message Number Definitions
*******/

#define MSG_ACK  6
#define MSG_ADM_CALLBACK  11
#define MSG_ADM_RESTORE_WS  13
#define MSG_ADM_SAVE_WS  12
#define MSG_ADM_STATUS  10
#define MSG_ADM_STOP  9
#define MSG_BUFFER_CTRL  2
#define MSG_COMMAND  1
#define MSG_FCCNF_MESS  19
#define MSG_FSCNF_MESS  20
#define MSG_GEN_MESSAGE  17
#define MSG_GMEN_INFO  3
#define MSG_HBV_RESULT_INFO  24
#define MSG_HBV_SETTINGS  22
#define MSG_HBV_SIM_PERIOD  23
#define MSG_HBV_SNOW_UPDATE  26
#define MSG_HBV_SNOW_VALUES  25
#define MSG_HELLO  0
#define MSG_HYVAL_MESS  21
#define MSG_LOG  16
#define MSG_LOG_FILTER  15
#define MSG_LOG_HISTORY  14
#define MSG_MULTI_MSG  5
#define MSG_MVRES_MESS  18
#define MSG_NAK  7
#define MSG_STRINGS  8
#define MSG_SYNC  28
#define MSG_TRADE_CALC_SET  27
#define MSG_TXY_SERIES  4



/*******
  Message Structure Definitions
*******/

typedef struct HELLO_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    sender_id ;
  int    sender_inst ;
  int    gpid ;
  int    pid ;
  char   pname [ M_NAME_SIZE ] ;
  char   version [ M_NAME_SIZE ] ;
  char   host [ M_NAME_SIZE ] ;
  char   mach [ M_NAME_SIZE ] ;
  char   os [ M_NAME_SIZE ] ;
  char   osver [ M_NAME_SIZE ] ;

} HELLO ;


typedef struct COMMAND_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  char   command [ M_COM_SIZE ] ;

} COMMAND ;


typedef struct BUFFER_CTRL_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    action ;
  int    n ;

} BUFFER_CTRL ;


typedef struct GMEN_INFO_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    enable ;
  int    check ;
  int    option ;
  char   menutag [ M_MENU_TAG_SIZE ] ;

} GMEN_INFO ;


typedef struct TXY_SERIES_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    kind ;
  char   name [ M_NAME_SIZE ] ;
  char   text [ M_TEXT_SIZE ] ;
  char   start_time [ M_TIME_SIZE ] ;
  char   __padding_6__ [ 1 ] ;
  int    id ;
  int    group ;
  int    x_unit ;
  int    x_step ;
  double ref ;
  int    val_unit ;
  int    data_type ;
  int    number_of_missing_data ;
  int    update ;
  int    period ;
  int    number ;
  int    flags ;
  char   __padding_18__ [ 4 ] ;
  double max_y ;
  double min_y ;
  int    origin ;
  char   x_format [ M_TXY_FORM_SIZE ] ;
  char   y_format [ M_TXY_FORM_SIZE ] ;
  int    vlcode ;
  int    producer ;
  int    num_values 		/* DEFAULT = 1 */;
  double values [ 1 /****> num_values <****/ ] ;

} TXY_SERIES ;


typedef struct MULTI_MSG_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    state ;

} MULTI_MSG ;


typedef struct ACK_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    sender_id ;
  int    sender_inst ;
  int    retval ;

} ACK ;


typedef struct NAK_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    sender_id ;
  int    sender_inst ;
  int    reason ;

} NAK ;


typedef struct STRINGS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    sender_id ;
  int    sender_inst ;
  int    kind ;
  int    s_length ;
  char   delimiter ;
  char   __padding_7__ [ 3 ] ;
  int    num_values 		/* DEFAULT = 1 */;
  char   strings [ 1 /****> num_values <****/ ] ;

} STRINGS ;


typedef struct ADM_STOP_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    sender_id ;
  int    sender_inst ;
  int    reason ;

} ADM_STOP ;


typedef struct ADM_STATUS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    busy ;
  int    user_cpu ;
  int    user_ucpu ;
  int    sys_cpu ;
  int    sys_ucpu ;

} ADM_STATUS ;


typedef struct ADM_CALLBACK_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    s_flags ;
  int    s_category ;
  int    s_parent ;
  int    s_nchildren ;
  int    s_id ;
  int    s_inst ;
  int    s_gpid ;
  int    s_pid ;
  int    s_lpid ;
  int    s_nice ;
  int    s_state ;
  int    s_status ;
  int    s_started ;
  int    s_cpu ;
  int    s_ucpu ;
  char   s_name [ M_FNAME_SIZE ] ;
  char   s_version [ M_NAME_SIZE ] ;
  char   s_host [ M_NAME_SIZE ] ;
  char   s_mach [ M_NAME_SIZE ] ;
  char   s_os [ M_NAME_SIZE ] ;
  char   s_osver [ M_NAME_SIZE ] ;

} ADM_CALLBACK ;


typedef struct ADM_SAVE_WS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  char   filename [ M_FNAME_SIZE ] ;

} ADM_SAVE_WS ;


typedef struct ADM_RESTORE_WS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  char   filename [ M_FNAME_SIZE ] ;

} ADM_RESTORE_WS ;


typedef struct LOG_HISTORY_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    max_messages ;
  char   start_time [ M_TIME_SIZE ] ;

} LOG_HISTORY ;


typedef struct LOG_FILTER_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    app_id ;
  int    app_inst ;
  char   application [ M_NAME_SIZE ] ;
  char   __padding_5__ [ 3 ] ;
  int    severity_level ;
  int    output ;
  int    type ;

} LOG_FILTER ;


typedef struct LOG_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    sender_id ;
  int    sender_inst ;
  int    mess_no ;
  int    severity_level ;
  int    out ;
  char   data_string [ M_STRING_SIZE ] ;

} LOG ;


typedef struct GEN_MESSAGE_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    event ;
  int    addressee ;
  int    fscno ;
  int    fsno ;
  int    sensor ;
  int    m_data ;

} GEN_MESSAGE ;


typedef struct MVRES_MESS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    event ;
  int    addressee ;
  int    fscno ;
  int    fsno ;
  int    sensor ;
  char   time [ M_TIME_SIZE ] ;
  char   __padding_8__ [ 2 ] ;
  int    s_interval ;
  int    num_values 		/* DEFAULT = 1 */;
  char   __padding_10__ [ 4 ] ;
  double values [ 1 /****> num_values <****/ ] ;

} MVRES_MESS ;


typedef struct FCCNF_MESS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    event ;
  int    addressee ;
  int    fscno ;
  int    s_interval ;
  int    num_values 		/* DEFAULT = 1 */;
  int    arr_of_sensors [ 1 /****> num_values <****/ ] ;

} FCCNF_MESS ;


typedef struct FSCNF_MESS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    event ;
  int    addressee ;
  int    fscno ;
  int    fsno ;
  int    num_values 		/* DEFAULT = 1 */;
  int    sensdata [ 1 /****> num_values <****/ ] ;

} FSCNF_MESS ;


typedef struct HYVAL_MESS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    event ;
  int    addressee ;
  char   ts_id [ 40 ] ;
  char   starttime [ M_TIME_SIZE ] ;
  char   stoptime [ M_TIME_SIZE ] ;
  int    num_values 		/* DEFAULT = 1 */;
  char   __padding_8__ [ 4 ] ;
  double values [ 1 /****> num_values <****/ ] ;

} HYVAL_MESS ;


typedef struct HBV_SETTINGS_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    settings_type ;
  char   catchment_name [ M_CATCH_NAME ] ;
  char   __padding_4__ [ 7 ] ;
  double catchment_area ;
  char   start_simulation [ M_TIME_SIZE ] ;
  char   end_simulation [ M_TIME_SIZE ] ;
  int    start_version ;
  char   step_length [ 9 ] ;
  char   __padding_9__ [ 3 ] ;
  int    simulation_length ;
  char   run_type [ M_HBV_RUN_TYPE_SIZE ] ;
  char   forecast_type [ M_HBV_FORECAST_TYPE_SIZE ] ;
  char   __padding_12__ [ 1 ] ;
  int    first_year ;
  int    last_year ;
  int    num_of_alternatives ;
  int    breakpoint ;

} HBV_SETTINGS ;


typedef struct HBV_SIM_PERIOD_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  char   default_start [ M_TIME_SIZE ] ;
  char   default_end [ M_TIME_SIZE ] ;
  char   upper_time_limit [ M_TIME_SIZE ] ;
  char   __padding_5__ [ 2 ] ;
  int    num_sim_steps ;
  int    time_unit ;
  int    step_length ;

} HBV_SIM_PERIOD ;


typedef struct HBV_RESULT_INFO_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  char   catchment_name [ M_CATCH_NAME ] ;
  char   __padding_3__ [ 3 ] ;
  int    update_flag ;
  char   update_start [ M_TIME_SIZE ] ;
  char   update_end [ M_TIME_SIZE ] ;
  int    update_time_unit ;
  int    update_time_step ;
  int    calibration_flag ;
  char   calibration_start [ M_TIME_SIZE ] ;
  char   calibration_end [ M_TIME_SIZE ] ;
  int    calibration_time_unit ;
  int    calibration_time_step ;
  int    forecast_flag ;
  char   forecast_start [ M_TIME_SIZE ] ;
  char   forecast_end [ M_TIME_SIZE ] ;
  int    forecast_time_unit ;
  int    forecast_time_step ;
  int    forecast_type ;
  int    num_met_forecasts ;
  int    first_hist_year ;
  int    last_hist_year ;

} HBV_RESULT_INFO ;


typedef struct HBV_SNOW_VALUES_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  char   state_date [ M_TIME_SIZE ] ;
  char   __padding_3__ [ 2 ] ;
  int    state_version ;
  int    no_elevations ;
  int    period ;
  char   text [ M_NAME_SIZE ] ;
  char   __padding_7__ [ 3 ] ;
  int    num_values 		/* DEFAULT = 1 */;
  char   __padding_8__ [ 4 ] ;
  double values [ 1 /****> num_values <****/ ] ;

} HBV_SNOW_VALUES ;


typedef struct HBV_SNOW_UPDATE_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    hight_level_number ;
  int    column_number ;
  char   __padding_4__ [ 4 ] ;
  double new_value ;

} HBV_SNOW_UPDATE ;


typedef struct TRADE_CALC_SET_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    OpunKey ;
  char   Name [ M_CALCSET_NAME_LEN ] ;
  char   __padding_4__ [ 3 ] ;
  int    Seqno ;
  char   Code [ M_CALCSET_CODE_LEN ] ;
  char   StarDate [ M_TIME_SIZE ] ;
  char   EndDate [ M_TIME_SIZE ] ;
  char   Express [ M_CALCSET_EXPR_LEN ] ;
  char   Descript [ M_CALCSET_DESC_LEN ] ;

} TRADE_CALC_SET ;


typedef struct SYNC_struct
{

  long   tag ;
  int    tag_type ;
  long   client_data ;
  int    status ;

} SYNC ;

#endif

