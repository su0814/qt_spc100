#ifndef PARAM_CONFIG_H
#define PARAM_CONFIG_H

/* view and scene */
#define MAX_SCENE_SIDE_LENGTH  (5000)                                            //视角边长
#define SCENE_POS_ORIGIN       (-100)                                            //起始原点坐标
#define SCENE_MARGIN_THRESHOLD (50)                                              //视角安全区域宽度
#define SCENE_MARGIN_MIN       (SCENE_POS_ORIGIN + SCENE_MARGIN_THRESHOLD)       //安全区域的左边和上边
#define SCENE_MARGIN_MAX       (MAX_SCENE_SIDE_LENGTH - SCENE_MARGIN_THRESHOLD)  //安全区域的右边和下边
#define VIEW_MIN_SCALE         (0.5)
#define VIEW_MAX_SCALE         (2.0)

/* block config */
#define LOGIC_BLOCK_WIDTH      50   //逻辑块的宽度
#define LOGIC_BLOCK_HEIGHT     100  //逻辑块的高度
#define CONDITION_BLOCK_WIDTH  80   //条件块的宽度
#define CONDITION_BLOCK_HEIGHT 20   //条件块的高度
#define CONNECT_POINT_WIDTH    10   //块上连接点的大小
#define CONNECT_POINT_HEIGHT   10
#define BLOCK_SPCING           (CONNECT_POINT_WIDTH * 4)  //块之间的最小距离  防止块的碰撞与重叠

#define MAX_SF_NUM      (19)
#define SF_USER_CODE    (0X20)
#define SS_NUM_START    (0X20)
#define MAX_SS_NUM      (11)  //用户可设置10个  默认1个
#define DEFAULT_SS_CODE (0XFF)

#define BLOCK_DATA_REFRESH_TIME (200)

#define COROUTINE_CODE_MAX_LENGTH (1024)

#define PROJECT_ENCRY_XOR_KEY (0x75)
#define PROJECT_ENCRY_AES_KEY ("shrhdzkj88888888shrhdzkj88888888")
#define PROJECT_ENCRY_AES_IV  ("shanghaiRUOHUIDZ")

#define MAX_COROUTINE_NUM (5)

#define INPUT_DI_RESOURCE_START   (0)
#define INPUT_DI_RESOURCE_NUM     (12)
#define INPUT_AI_RESOURCE_START   (INPUT_DI_RESOURCE_START + INPUT_DI_RESOURCE_NUM)
#define INPUT_AI_RESOURCE_NUM     (2)
#define INPUT_PI_RESOURCE_START   (INPUT_AI_RESOURCE_START + INPUT_AI_RESOURCE_NUM)
#define INPUT_PI_RESOURCE_NUM     (2)
#define INPUT_QEP_RESOURCE_START  (INPUT_PI_RESOURCE_START + INPUT_PI_RESOURCE_NUM)
#define INPUT_QEP_RESOURCE_NUM    (4)
#define INPUT_BOOL_RESOURCE_START (INPUT_QEP_RESOURCE_START + INPUT_QEP_RESOURCE_NUM)
#define INPUT_BOOL_RESOURCE_NUM   (2)
#define INPUT_RESOURCE_NUM \
    (INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + INPUT_PI_RESOURCE_NUM + INPUT_QEP_RESOURCE_NUM)

#define LED_RED    ("rgb(255,0,0)")
#define LED_GREEN  ("rgb(0,255,0)")
#define LED_YELLOW ("rgb(255,255,0)")
#define LED_GREY   ("rgb(190,190,190)")
/* 减速曲线设置 */
#define MAX_DECELERATE_NUM      (5)  //最大减速曲线数量
#define DECELERATE_SUBLEVEL_NUM (3)  //减速检测段数
#endif                               // PARAM_CONFIG_H
