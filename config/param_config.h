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

#define MAX_SF_NUM      (20)
#define SF_USER_CODE    (0X20)
#define MAX_SS_NUM      (10)
#define DEFAULT_SS_CODE (0XFF)

#define LOGIC_VIEW_DATA_REFRESH_TIME (200)

#define COROUTINE_CODE_MAX_LENGTH (1024)
#endif  // PARAM_CONFIG_H
