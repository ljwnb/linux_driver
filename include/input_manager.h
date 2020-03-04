#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <pthread.h>

#include "touch_screen.h"

//触摸屏
#define INPUT_TYPE_TOUCHSCREEN    1

typedef struct InputEvent {
    int type;   //输入事件类型
    int x;      //x坐标
    int y;      //y坐标
    int ges;    //手势操作
}T_InputEvent, *PT_InputEvent;

typedef struct InputOpr {
    char *name;             //输入模块的名字
    pthread_t tid;          //子线程的ID
    int (*DeviceInit)(void);//设备初始化函数    
    int (*DeviceExit)(void);//设备退出函数
    int (*GetInputEvent)(PT_InputEvent inputevent);//获取输入事件函数
    struct InputOpr *next;  //链表
}T_InputOpr, *PT_InputOpr;


int RegisterInputOpr(PT_InputOpr ptInputopr);

int GetInputEvent(PT_InputEvent ptInputevent);
int AllInputDevicesInit();
int InputInit();



#endif /* __INPUT_MANAGER_H__ */
