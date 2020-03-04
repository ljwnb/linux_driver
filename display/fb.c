#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include "disp_manager.h"
#include "fb.h"

static T_DispOpr t_FBDisOpr;

static int fd_lcd;
static int *plcd;

static int ScreenSize;


/*
    初始化fb
*/
static int FBDeviceInit(void)
{
    fd_lcd = open("/dev/fb0", O_RDWR);
    if (fd_lcd < 0)
    {
        perror("open fail");
        return -1;
    }
    struct fb_var_screeninfo lcd_info;//获取可变参数信息
    
    int ret = ioctl(fd_lcd, FBIOGET_VSCREENINFO, &lcd_info);//获取屏幕信息
    if (ret < 0)
    {
        perror("ioctl fail");
        close(fd_lcd);
        return -1;
    }
    t_FBDisOpr.Xres    = lcd_info.xres;
    t_FBDisOpr.Yres    = lcd_info.yres;
    t_FBDisOpr.Bpp     = lcd_info.bits_per_pixel;
    ScreenSize = t_FBDisOpr.Xres * t_FBDisOpr.Yres * t_FBDisOpr.Bpp / 8;
    plcd = (int *)mmap(NULL, ScreenSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_lcd, 0);//内存映射
    if (plcd == NULL)
    {
        perror("mmap error");
        close(fd_lcd);
        return -1;
    }
    
    return 0;    
}

/*
    退出fb
*/
static int FBDeviceExit(void)
{
    int ret = munmap(plcd, ScreenSize);
    if (ret < 0)
    {
        perror("munmap error");
        return -1;
    }
    close(fd_lcd);
    return 0;    
}

/*
    显示一个像素
    x 行 y列
*/
static int FBShowPixel(int x, int y, int color)
{
    *(plcd + x * t_FBDisOpr.Xres + y) = color;
}


static int FBCleanScreen(unsigned int color)
{
    int i, j;
    for (i = 0; i < t_FBDisOpr.Yres; i++)
    {
        for (j = 0; j < t_FBDisOpr.Xres; j++)
        {
            FBShowPixel(i, j, color);
        }
    }
}

//分配/设置/注册T_DispOpr结构体
static T_DispOpr t_FBDisOpr = {
    .name        = "fb",
    .DeviceInit  = FBDeviceInit,
    .DeviceExit  = FBDeviceExit,
    .ShowPixel   = FBShowPixel,
    .CleanScreen = FBCleanScreen,
};

/*
    将自身注册到显示管理模块
*/
int FBInit()
{
    return RegisterDispOpr(&t_FBDisOpr);
}


