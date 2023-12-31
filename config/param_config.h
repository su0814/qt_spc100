#ifndef PARAM_CONFIG_H
#define PARAM_CONFIG_H

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
#define MAX_SS_NUM      (10)
#define DEFAULT_SS_CODE (0XFF)

#define BLOCK_DATA_REFRESH_TIME (200)

#define COROUTINE_CODE_MAX_LENGTH (1024)

#define PROJECT_ENCRY_XOR_KEY (0x75)
#define PROJECT_ENCRY_AES_KEY ("shrhdzkj88888888shrhdzkj88888888")
#define PROJECT_ENCRY_AES_IV  ("shanghaiRUOHUIDZ")

#define MAX_COROUTINE_NUM (5)

#define INPUT_DI_RESOURCE_NUM  (12)
#define INPUT_AI_RESOURCE_NUM  (2)
#define INPUT_PI_RESOURCE_NUM  (2)
#define INPUT_QEP_RESOURCE_NUM (4)
#define INPUT_RESOURCE_NUM \
    (INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + INPUT_PI_RESOURCE_NUM + INPUT_QEP_RESOURCE_NUM)
#endif  // PARAM_CONFIG_H
