#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "disp_manager.h"


#define AUTOMODE_PIC        "auto_mode.bmp"
#define MANUALMODE_PIC      "browse_mode.bmp"


int show_bmp(char *bmp_path)
{
    int i, j;
    int fb_xres, fb_yres;
    PT_DispOpr pt_DispDev = GetDefaultDispDev();
    if (pt_DispDev == NULL)
    {
        printf("未选择显示设备！\n");
        return -1;
    }

    fb_xres = pt_DispDev->Xres;
    fb_yres = pt_DispDev->Yres;
    
    int fd_bmp = open(bmp_path, O_RDONLY);
    if (fd_bmp < 0)
    {
        perror("open bmp fail");
        return -1;
    }
    char head[54];//获取bmp文件头部
    int read_len = read(fd_bmp, head, sizeof(head));
    if (read_len < 0)
    {
        perror("read error");
        return -1;
    }
    if (head[0] != 0x42 || head[1] != 0x4d)
    {
        printf("文件格式不为bmp！\n");
        close(fd_bmp);
        return -1;
    }
    int file_size = (head[0x02] << 0) | (head[0x03] << (1*8)) |(head[0x04] << (2*8)) | (head[0x05] << (3*8));
    int width     = (head[0x12] << 0) | (head[0x13] << (1*8)) |(head[0x14] << (2*8)) | (head[0x15] << (3*8));
    int high      = (head[0x16] << 0) | (head[0x17] << (1*8)) |(head[0x18] << (2*8)) | (head[0x19] << (3*8));
    short bpp     = (head[0x1C] << 0) | (head[0x1D] << (1*8));
    
    if (width > fb_xres || high > fb_yres)
    {
        printf("该文件大小不支持！\n");
        close(fd_bmp);
        return -1;
    }
    if (width < fb_xres || high < fb_yres)//如果图片没有铺满屏幕 清屏
    {
        //除图标外
        if (!(strcmp(bmp_path, AUTOMODE_PIC) == 0 || strcmp(bmp_path, MANUALMODE_PIC) == 0))
        {
            pt_DispDev->CleanScreen(COLOR_BACKGROUND);
        }
    }
    
    char *p = (char *)malloc(width * high * bpp / 8);//分配一块空间 存储图片的内存
    if (p == NULL)
    {
        perror("malloc fail");
        close(fd_bmp);
        return -1;
    }
    int ret = read(fd_bmp, p, width * high * bpp / 8);//读取图片数据
    if (ret < 0)
    {
        perror("read fd_bmp fail");
        free(p);
        close(fd_bmp);
        return -1;
    }
    
    char a, r, g, b;
    int color;
    char *q = p;
    int x, y;
    y = (fb_yres - high) / 2;
    x = (fb_xres - width) / 2;
    
    if (bpp == 24)
    {
        for (i = y; i < high+y; i++)
        {
            for (j = x; j < width+x; j++)
            {
                a = 0x0;
                b = *q++;
                g = *q++;
                r = *q++;
                color = (a<<24) | (r << 16) | (g << 8) | b;
                pt_DispDev->ShowPixel(fb_yres-1-i, j, color);
                //draw_point(fb_yres-1-i, j, color);
            }
        }
    }
    else if (bpp == 32)
    {
        for (i = y; i < high+y; i++)
        {
            for (j = x; j < width+x; j++)
            {
                b = *q++;
                g = *q++;
                r = *q++;
                a = *q++;
                color = (a<<24) | (r << 16) | (g << 8) | b;
                pt_DispDev->ShowPixel(fb_yres-1-i, j, color);
                //draw_point(fb_yres-1-i, j, color);
            }
        }
    }
    free(p);
    p = NULL;
    
    return 0;
}

int show_bmp_pos(int x, int y, char *bmp_path)
{
    int i, j;
    int fb_xres, fb_yres;
    PT_DispOpr pt_DispDev = GetDefaultDispDev();
    if (pt_DispDev == NULL)
    {
        printf("未选择显示设备！\n");
        return -1;
    }

    fb_xres = pt_DispDev->Xres;
    fb_yres = pt_DispDev->Yres;
    
    int fd_bmp = open(bmp_path, O_RDONLY);
    if (fd_bmp < 0)
    {
        perror("open bmp fail");
        return -1;
    }
    char head[54];//获取bmp文件头部
    int read_len = read(fd_bmp, head, sizeof(head));
    if (read_len < 0)
    {
        perror("read error");
        return -1;
    }
    if (head[0] != 0x42 || head[1] != 0x4d)
    {
        printf("文件格式不为bmp！\n");
        close(fd_bmp);
        return -1;
    }
    int file_size = (head[0x02] << 0) | (head[0x03] << (1*8)) |(head[0x04] << (2*8)) | (head[0x05] << (3*8));
    int width     = (head[0x12] << 0) | (head[0x13] << (1*8)) |(head[0x14] << (2*8)) | (head[0x15] << (3*8));
    int high      = (head[0x16] << 0) | (head[0x17] << (1*8)) |(head[0x18] << (2*8)) | (head[0x19] << (3*8));
    short bpp     = (head[0x1C] << 0) | (head[0x1D] << (1*8));
    
    if (width > fb_xres || high > fb_yres)
    {
        printf("该文件大小不支持！\n");
        close(fd_bmp);
        return -1;
    }
    if (width < fb_xres || high < fb_yres)//如果图片没有铺满屏幕 清屏
    {
        if (!(strcmp(bmp_path, AUTOMODE_PIC) == 0 || strcmp(bmp_path, MANUALMODE_PIC) == 0))
        {
            pt_DispDev->CleanScreen(COLOR_BACKGROUND);
        }
    }
    
    char *p = (char *)malloc(width * high * bpp / 8);//分配一块空间 存储图片的内存
    if (p == NULL)
    {
        perror("malloc fail");
        close(fd_bmp);
        return -1;
    }
    int ret = read(fd_bmp, p, width * high * bpp / 8);//读取图片数据
    if (ret < 0)
    {
        perror("read fd_bmp fail");
        free(p);
        close(fd_bmp);
        return -1;
    }
    
    char a, r, g, b;
    int color;
    char *q = p;
    //int x, y;
    
    //y = (fb_yres - high) / 2;  //行
    //x = (fb_xres - width) / 2; //列
    
    if (bpp == 24)
    {
        for (i = x; i < high+x; i++)
        {
            for (j = y; j < width+y; j++)
            {
                a = 0x0;
                b = *q++;
                g = *q++;
                r = *q++;
                color = (a<<24) | (r << 16) | (g << 8) | b;
                pt_DispDev->ShowPixel(fb_yres-1-i, j, color);
                //draw_point(fb_yres-1-i, j, color);
            }
        }
    }
    else if (bpp == 32)
    {
        for (i = x; i < high+x; i++)
        {
            for (j = y; j < width+y; j++)
            {
                b = *q++;
                g = *q++;
                r = *q++;
                a = *q++;
                color = (a<<24) | (r << 16) | (g << 8) | b;
                pt_DispDev->ShowPixel(fb_yres-1-i, j, color);
                //draw_point(fb_yres-1-i, j, color);
            }
        }
    }
    free(p);
    p = NULL;
    
    return 0;
}






