#ifndef __DISP_MANAGER_H__
#define __DISP_MANAGER_H__


#define COLOR_BACKGROUND   0xE7DBB5  /* 泛黄的纸 */

typedef struct DispOpr {
    char *name;     //显示模块的名字
    int Xres;       //X分辨率     
    int Yres;       //Y分辨率
    int Bpp;        //一个像素用多少位来表示
    int (*DeviceInit)(void);    //设备初始化
    int (*DeviceExit)(void);    //设备退出
    int (*CleanScreen)(unsigned int color);//清屏
    int (*ShowPixel)(int x, int y, int color);//指定某坐标为某颜色
    
    struct DispOpr *next;   //链表
}T_DispOpr, *PT_DispOpr;


int RegisterDispOpr(PT_DispOpr pt_Disppr);
PT_DispOpr GetDispOpr(char *name);
void SelectAndInitDispDev(char *name);
PT_DispOpr GetDefaultDispDev(void);
int DisplayInit(void);


#endif /* __DISP_MANAGER_H__ */
