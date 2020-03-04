#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dir.h"

#include "disp_manager.h"

#include "debug.h"


void print_usage(char *p)
{
    printf("Usage: \n");
    printf("%s <pic_pathname>\n", p);
}

int main(int argc, char *argv[])
{
    int ret;
    if (argc != 2)
    {
        print_usage(argv[0]);
        return -1;
    }
    PT_List list = creat_list();//图片链表
    if (list == NULL)
    {
        printf("creat list fail!\n");
        return -1;
    }
    PT_List music = creat_list();//音乐链表
    if (list == NULL)
    {
        printf("creat music list fail!\n");
        return -1;
    }
    if (DisplayInit() != 0)//显示设备初始化
    {
        printf("DisplayInit error!\n");
        return -1;
    }
    
    SelectAndInitDispDev("fb");//选择fb作为显示设备
    
    if (InputInit() != 0)//输入设备初始化
    {
        printf("Input error!\n");
        return -1;
    }
    AllInputDevicesInit();

    ret = read_dir(list, music, argv[1]);//将目录下的图片和音乐存入链表
    if (ret != 0)
    {
        printf("read_dir error");
        return -1;
    }
    runing(list, music);//开始运行功能
    
    return 0;
}


