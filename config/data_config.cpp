#include "data_config.h"
/* lus user func */
const QStringList               lua_di_func      = { "sdi1()", "sdi2()", "sdi3()", "sdi4()",  "sdi5()",  "sdi6()",
                                  "sdi7()", "sdi8()", "sdi9()", "sdi10()", "sdi11()", "sdi12()" };
const QStringList               lua_ai_func      = { "sai1()", "sai2()" };
const QStringList               lua_pi_func      = { "spi1_cnt()", "spi2_cnt()" };
const QStringList               lua_qep_func     = { "sqep1_cnt()", "sqep2_cnt()", "pi_sqep1_cnt()", "pi_sqep2_cnt()" };
const QStringList               lua_qep_dir_func = { "sqep1_dir()", "sqep2_dir()", "pi_sqep1_dir()", "pi_sqep2_dir()" };
const QStringList               a_di_function    = { "di1_a()", "di2_a()", "di3_a()", "di4_a()",  "di5_a()",  "di6_a()",
                                    "di7_a()", "di8_a()", "di9_a()", "di10_a()", "di11_a()", "di12_a()" };
const QStringList               b_di_function    = { "di1_b()", "di2_b()", "di3_b()", "di4_b()",  "di5_b()",  "di6_b()",
                                    "di7_b()", "di8_b()", "di9_b()", "di10_b()", "di11_b()", "di12_b()" };
const QStringList               a_ai_function    = { "ai1_a()", "ai2_a()" };
const QStringList               b_ai_function    = { "ai1_b()", "ai2_b()" };
const QStringList               a_pi_function    = { "pi1_cnt_a()", "pi2_cnt_a()" };
const QStringList               b_pi_function    = { "pi1_cnt_b()", "pi2_cnt_b()" };
const QStringList               a_qep_function = { "sqep1_cnt()", "sqep2_cnt()", "pi_qep1_cnt_a()", "pi_qep2_cnt_a()" };
const QStringList               b_qep_function = { "sqep1_cnt()", "sqep2_cnt()", "pi_qep1_cnt_b()", "pi_qep2_cnt_b()" };
const QList<QStringList>        cat2_a_function      = { a_ai_function, a_di_function, a_pi_function, a_qep_function };
const QList<QStringList>        cat2_b_function      = { b_ai_function, b_di_function, b_pi_function, b_qep_function };
const QList<QStringList>        cat3_source_function = { lua_ai_func, lua_di_func, lua_pi_func, lua_qep_func };
const QList<QList<QStringList>> cat2_source_function = { cat2_a_function, cat2_b_function };
const QStringList        cat3_qep_dir_function = { "sqep1_dir()", "sqep2_dir()", "pi_sqep1_dir()", "pi_sqep2_dir()" };
const QList<QStringList> cat2_qep_dir_function = {
    { "sqep1_dir()", "sqep2_dir()", "pi_qep1_dir_a()", "pi_qep2_dir_a()" },
    { "sqep1_dir()", "sqep2_dir()", "pi_qep1_dir_b()", "pi_qep2_dir_b()" }
};
const QStringList        a_relay_function   = { "rly1_a", "rly2_a", "smos1", "smos2", "smos3", "smos4" };
const QStringList        b_relay_function   = { "rly1_b", "rly2_b", "smos1", "smos2", "smos3", "smos4" };
const QStringList        relay_mos_function = { "srly1", "srly2", "smos1", "smos2", "smos3", "smos4" };
const QStringList        mos_function       = { "srly1", "srly2", "smos1", "smos2", "smos3", "smos4" };
const QList<QStringList> cat2_a_output      = { a_relay_function };
const QList<QStringList> cat2_b_output      = { a_relay_function };
const QList<QStringList> cat2_output        = { a_relay_function, b_relay_function, mos_function };
const QList<QStringList> cat3_output        = { relay_mos_function };
/* lua code keyword */
const QStringList lua_logic_keyword = { " and ", " or ", " not " };

/* device resource */
const QStringList di_resource = {
    "Di1", "Di2", "Di3", "Di4", "Di5", "Di6", "Di7", "Di8", "Di9", "Di10", "Di11", "Di12"
};
const QStringList        ai_resource        = { "Ai1", "Ai2" };
const QStringList        pi_resource        = { "Pi1", "Pi2" };
const QStringList        qep_resource       = { "QEP1", "QEP2", "PI_QEP1", "PI_QEP2" };
const QStringList        bool_resource      = { "True", "False" };
const QList<QStringList> input_resource     = { di_resource, ai_resource, pi_resource, qep_resource, bool_resource };
const QStringList        relay_mos_resource = { "RELAY1", "RELAY2", "MOS1", "MOS2", "MOS3", "MOS4" };
const QStringList        mos_resource       = { "MOS1", "MOS2", "MOS3", "MOS4" };
const QList<QStringList> output_model       = { relay_mos_resource };
const QList<QStringList> input_model        = { ai_resource, di_resource, pi_resource, qep_resource };
const QStringList base_logic_resource = { "与", "或", "非", "同或", "异或", "RS触发器", "n位编码器", "n位译码器" };
const QStringList application_logic_resource = { "复位", "外部设备监控", "频率检测", "边沿检测", "斜坡向下" };
const QStringList        delay_counter_logic_resource = { "接通延时",     "断开延时",   "可调接通延时",
                                                   "可调断开延时", "事件计数器", "日志记录" };
const QStringList        speed_logic_resource = { "速度互检", "减速监控", "静止检测", "数值比较" };
const QList<QStringList> logic_model = { base_logic_resource, application_logic_resource, delay_counter_logic_resource,
                                         speed_logic_resource };
const QList<QList<QStringList>> model_name = { input_model, output_model, logic_model };
/* project info json name */
const QString project_name         = "project_name";
const QString project_company_name = "companyname";
const QString project_author_ver   = "authorname";
const QString project_path         = "save_path";
/* logic propgram view project json name */
const QString project_object_logic_programe = "logic_programe";
const QString project_programe_logic        = "loigc";
const QString project_programe_condition    = "condition";
const QString project_programe_line         = "line";
/* param */
const QString project_safety_param = "safe_param";
const QString project_config_photo = "config_photo";

/* COLOR */
const QColor LED_COLOR_RED(255, 50, 50);
const QColor LED_COLOR_GREEN(100, 255, 0);
const QColor LED_COLOR_DARKRED(128, 25, 25);
const QColor LED_COLOR_DARKGREEN(50, 128, 0);
