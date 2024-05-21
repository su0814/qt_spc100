#ifndef PARAM_CONFIG_H
#define PARAM_CONFIG_H

/* view and scene */
#define MAX_SCENE_SIDE_LENGTH  (5000)                                            //视角边长
#define SCENE_POS_ORIGIN       (0)                                               //起始原点坐标
#define SCENE_MARGIN_THRESHOLD (50)                                              //视角安全区域宽度
#define SCENE_MARGIN_MIN       (SCENE_POS_ORIGIN + SCENE_MARGIN_THRESHOLD)       //安全区域的左边和上边
#define SCENE_MARGIN_MAX       (MAX_SCENE_SIDE_LENGTH - SCENE_MARGIN_THRESHOLD)  //安全区域的右边和下边
#define VIEW_MIN_SCALE         (0.5)
#define VIEW_MAX_SCALE         (2.0)

/* block config */
#define LOGIC_BLOCK_WIDTH      60   //逻辑块的宽度
#define LOGIC_BLOCK_HEIGHT     140  //逻辑块的高度
#define CONDITION_BLOCK_WIDTH  140  //条件块的宽度
#define CONDITION_BLOCK_HEIGHT 20   //条件块的高度
#define CONNECT_POINT_WIDTH    10   //块上连接点的大小
#define CONNECT_POINT_HEIGHT   10
#define BLOCK_SPCING           (CONNECT_POINT_WIDTH * 2)  //块之间的最小距离  防止块的碰撞与重叠
#define BLOCK_SPCING_LEFT      (CONNECT_POINT_WIDTH * 4 - 1)
#define BLOCK_SPCING_RIGHT     (CONNECT_POINT_WIDTH * 4 - 1)
#define BLOCK_SPCING_TOP       (CONNECT_POINT_WIDTH * 2 - 1)
#define BLOCK_SPCING_BOTTOM    (CONNECT_POINT_WIDTH * 2 - 1)

#define BLOCK_DATA_REFRESH_TIME (200)

#define PROJECT_ENCRY_XOR_KEY      (0x75)
#define PROJECT_ENCRY_AES_KEY      ("shrhdzkj88888888shrhdzkj88888888")
#define PROJECT_ENCRY_AES_IV       ("shanghaiRUOHUIDZ")
#define PROJECT_LOAD_ENCRY_XOR_KEY (0X7F)
#endif  // PARAM_CONFIG_H
