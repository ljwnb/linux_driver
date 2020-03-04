#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "disp_manager.h"
#include "input_manager.h"
#include "touch_screen.h"

#include "debug.h"

static int fd_ts;

//触点坐标
struct point {
    int x;
    int y;
};


//触摸屏设备初始化
static int TouchScreenDeviceInit(void)
{
    fd_ts = open(TOUCHSCREEN_DEVICE, O_RDONLY);
    if (fd_ts < 0)
    {
        perror("open ts fail");
        return -1;
    }
    return 0;
}

//触摸屏退出
static int TouchScreenDeviceExit(void)
{
    close(fd_ts);
    return 0;
}

/*
    获取触摸屏输入
    成功返回0
    失败返回-1
*/
static int TouchScreenGetInputEvent(PT_InputEvent pt_Inputevent)
{
    struct point start, end;//起始坐标和终点坐标
    int temp_x = -1, temp_y = -1;
    start.x = start.y = end.x = end.y = -1;
    struct input_event ts_data;//输入事件结构体
    int xres = GetDefaultDispDev()->Xres;
    int yres = GetDefaultDispDev()->Yres;
    
    while (1)//获取一次触摸事件 从触摸按下到松开才算一次
    {
        int ret = read(fd_ts, &ts_data, sizeof(ts_data));//读取触摸屏数据
        if (ret < 0)
        {
            perror("read touch");
            return -1;
        }
        if (ts_data.type == EV_ABS)//绝对位移类事件
        {
            if (ts_data.code == ABS_X)
            {
                temp_x = ts_data.value;
            }
            else if (ts_data.code == ABS_Y)
            {
                temp_y = ts_data.value;
            }
        }
        else if (ts_data.type == EV_KEY && ts_data.code == 0x14a)//压力事件
        {
            if (ts_data.value > 0)//按下
            {
                if (start.x == -1 && start.y == -1)//只记录第一次触摸的位置
                {
                    start.x = temp_x;
                    start.y = temp_y;
                }
            }
            else if (ts_data.value == 0)//松开
            {
                end.x = temp_x;
                end.y = temp_y;
                if(end.x == -1 || end.y == -1 || start.x == -1 || start.y == -1)
                {
                    continue;
                }
                break;//直到触摸屏松开才结束输入
            }
        }
    }

    pt_Inputevent->type = INPUT_TYPE_TOUCHSCREEN;
    pt_Inputevent->x    = end.x;
    pt_Inputevent->y    = end.y;

    DBG_PRINTF("x = %d, y = %d\n", end.x, end.y);
    
    //开始判断手势方向            滑动大于1/5屏幕才能生效
    if (start.x - end.x > xres/5)//左划
    {
        DBG_PRINTF("left\n");
        pt_Inputevent->ges =  Ges_Left;
    }
    else if (end.x - start.x > xres/5)//右
    {
        DBG_PRINTF("right\n");
        pt_Inputevent->ges =  Ges_Right;
    }
    else if (start.y - end.y > yres/5)//上
    {
        DBG_PRINTF("up\n");
        pt_Inputevent->ges = Ges_Up;
    }
    else if (end.y -start.y > yres/5)//下
    {
        DBG_PRINTF("down\n");
        pt_Inputevent->ges = Ges_Down;
    }
    else
    {
        //不是滑动操作 视为点击
        if (end.y >= 370 && end.y <= 530 && end.x >= 90 && end.x <= 410)//连播按钮
        {
            DBG_PRINTF("auto\n");
            pt_Inputevent->ges = Ges_AUTO;
        }
        else if (end.y >= 370 && end.y <= 530 && end.x >= 590 && end.x <= 910)//手动按钮
        {
            DBG_PRINTF("handle\n");
            pt_Inputevent->ges = Ges_Manual;
        }
        else
        {
            DBG_PRINTF("click\n");
            pt_Inputevent->ges =  Ges_Othre;//其他      
        }
    }
    
    return 0;
}


//分配/设置/注册 T_InputOpr结构体
static T_InputOpr t_touchscreen = {
    .name           = "touchscreen",
    .DeviceInit     = TouchScreenDeviceInit,
    .DeviceExit     = TouchScreenDeviceExit,
    .GetInputEvent  = TouchScreenGetInputEvent,
};

/*
    触摸屏模块初始化 
    将自身注册到输入管理层
*/
int TouchScreenInit(void)
{
    DBG_PRINTF("%s :%d\n", __func__, __LINE__);
    return RegisterInputOpr(&t_touchscreen);
}

