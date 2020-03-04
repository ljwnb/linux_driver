#ifndef __TOUCH_SCREEN_H__
#define __TOUCH_SCREEN_H__


//触摸屏设备文件
#define TOUCHSCREEN_DEVICE  "/dev/input/event0"

//手势方向
#define Ges_Othre   0//其他
#define Ges_Left    1
#define Ges_Right   2
#define Ges_Up      3
#define Ges_Down    4
#define Ges_AUTO    5//自动播放按钮
#define Ges_Manual  6//手动播放按钮


int TouchScreenInit(void);


#endif /* __TOUCH_SCREEN_H__ */
