#include "data_config.h"

const QStringList calc_str = { ">", "<", ">=", "<=", "==" };

/* lus user func */
const QStringList lua_di_func      = { " sdi1() ", " sdi2() ", " sdi3() ", " sdi4() ",  " sdi5() ",  " sdi6() ",
                                  " sdi7() ", " sdi8() ", " sdi9() ", " sdi10() ", " sdi11() ", " sdi12() " };
const QStringList lua_ai_func      = { " sai1() ", " sai2() " };
const QStringList lua_pi_func      = { " spi1_cnt() ", " spi2_cnt() " };
const QStringList lua_qep_func     = { " sqep1_cnt() ", " sqep2_cnt() ", " pi_sqep1_cnt() ", " pi_sqep2_cnt() " };
const QStringList lua_qep_dir_func = { " sqep1_dir() ", " sqep2_dir() ", " pi_sqep1_dir() ", " pi_sqep2_dir() " };

const QStringList sf_type_str = { "Estop",
                                  "Speed_monitor",
                                  "Relay_monitor",
                                  "User_defined",
                                  "Deceleration_detection1",
                                  "Deceleration_detection2",
                                  "Deceleration_detection3",
                                  "Deceleration_detection4",
                                  "Deceleration_detection5" };
/* lua code keyword */
const QStringList lua_logic_keyword = { " and ", " or ", " not " };

/* device resource */
const QStringList di_resource = {
    "Di1", "Di2", "Di3", "Di4", "Di5", "Di6", "Di7", "Di8", "Di9", "Di10", "Di11", "Di12"
};
const QStringList        ai_resource    = { "Ai1", "Ai2" };
const QStringList        pi_resource    = { "Pi1", "Pi2" };
const QStringList        qep_resource   = { "QEP1", "QEP2", "PI_QEP1", "PI_QEP2" };
const QStringList        bool_resource  = { "True", "False" };
const QList<QStringList> input_resource = { di_resource, ai_resource, pi_resource, qep_resource, bool_resource };

/* hardfault code */
const QStringList a_power_error_code     = { "0001", "0002", "0003", "0004", "0005", "0006",
                                         "0007", "0008", "0009", "000D", "000E" };
const QStringList b_power_error_code     = { "0001", "0002", "0003", "0004", "0005", "0006",
                                         "000A", "000B", "000C", "000D", "000E" };
const QStringList singlecheck_error_code = { "1101", "1102", "1103", "1104" };
const QStringList cyclecheck_error_code  = { "2001", "2002", "2003", "2004", "2005", "2006", "2007", "2008" };
const QStringList a_output_error_code    = { "4001", "4002", "4004", "4005", "400B", "400C" };
const QStringList b_output_error_code    = { "4001", "4002", "4004", "4005", "400D", "400E" };
const QStringList input_error_code       = { "5001", "5002", "5003", "5004", "5005", "5006", "5007", "5008", "5009",
                                       "500A", "500B", "500C", "5101", "5102", "5105", "5106", "5201", "5202",
                                       "5203", "5301", "5302", "5303", "5304", "5305", "5306" };
/* project info json name */
const QString project_name             = "project_name";
const QString project_company_name     = "companyname";
const QString project_author_ver       = "authorname";
const QString project_version          = "version";
const QString project_path             = "save_path";
const QString project_advanced_program = "advanced_program";
/* device source project json name */
const QString project_object_device      = "device_resource";
const QString project_device_icheckstate = "checkstate";
const QString project_device_iothername  = "othername";
const QString project_device_ossinfo     = "ss_info";
/* logic propgram view project json name */
const QString project_object_logic_programe = "logic_programe";
const QString project_programe_logic        = "loigc";
const QString project_programe_condition    = "condition";
const QString project_programe_line         = "line";
/* lua coroutine */
const QString project_object_coroutine = "coroutine";
/* param */
const QString project_safety_param = "ss_param";
