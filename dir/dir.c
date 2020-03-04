#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "debug.h"
#include "dir.h"

/*
    判断是否为 . 或者 ..目录
    返回值：
        是    返回1
        不是 返回0
*/
int is_point_dir(const char *path)
{
    return (strcmp(".", path) == 0 || strcmp("..", path) == 0);
}

/*
    返回文件的类型
    参数：
        file_name 需要检测后缀的文件名
    返回值：
       返回文件类型宏
         TYPE_BMP/TYPE_JPEG
         TYPE_NONE
*/
int get_file_type(char *file_name)
{
    char *p = file_name + strlen(file_name) - 1;//指向字符串末尾

    while (*p != '.' && p > file_name)
    {
        p--;
    }
    if (p != file_name)
    {
        p++;//定位到文件后缀
        if (strcmp(p, "bmp") == 0)
        {
            return TYPE_BMP;
        }
        else if (strcmp(p, "JPEG") == 0 || strcmp(p, "jpeg") == 0 || strcmp(p, "jpg") == 0)
        {
            return TYPE_JPEG;
        }
        else if (strcmp(p, "mp3") == 0)
        {
            return TYPE_MP3;
        }
        else
        {
            return TYPE_NONE;
        }
    }
    return TYPE_NONE;
}


/* 
    递归读取当前目录下的所有普通文件
    如果是bmp 或者JPEG文件 就存入链表
    参数：
        list: 存放图片的链表
        music:音乐链表
        pathname:需要读取文件的路径名
    返回值：
        成功返回0
        失败返回-1
*/
int read_dir(PT_List list, PT_List music, const char *pathname)
{
    //打开目录
    int ret;
    DIR *d;
    
    d = opendir(pathname);
    if (d == NULL)
    {
        perror("opendir fail");
        return -1;
    }
    ret = chdir(pathname);//改变当前路径
    if (ret < 0)
    {
        perror("chdir fail");
        closedir(d);
        return -1;
    }
    struct dirent *p = NULL;
    //循环读取当前目录下的文件
    while (1)
    {
        p = readdir(d);
        if (p != NULL)//正常读取
        {
            //获取当前路径路径和目录文件拼接到一起的完整路径
            char cur_path[128] = {0};
            char path_bak[128] = {0};//当前路径备份
            
            getcwd(cur_path, sizeof(cur_path));//获取当前路径路径
            getcwd(path_bak, sizeof(path_bak));
            sprintf(cur_path, "%s/%s", cur_path, p->d_name);//获取到完整的路径名
            
            //puts(cur_path);
            
            //如果是目录文件   则判断若是 . .. 则跳过
            if (p->d_type == DT_DIR)//类型是个目录
            {
                if (is_point_dir(p->d_name))//判断若是 . .. 则跳过
                {
                    continue;
                }
                else
                {
                    read_dir(list, music, cur_path);
                    
                    ret = chdir(path_bak);//恢复原来路径
                    if (ret < 0)
                    {
                        perror("chdir fail");
                        closedir(d);
                        return -1;
                    }
                }
            }
            else if (p->d_type == DT_REG)
            {
                //如果是一个普通文件 则打印 并判断该文件的后缀 是否是BMP JPEG 是则加入链表
                DBG_PRINTF("%s\n", p->d_name);
                
                //判断该文件的后缀是否为BMP JPEG
                int type = get_file_type(p->d_name);
                if (type == TYPE_BMP || type == TYPE_JPEG)
                {
                    ret = add_node_to_list(list, cur_path, type);
                    if (ret < 0)
                    {
                        printf("add_node_to_list fail!\n");
                        closedir(d);
                        return -1;
                    }
                }
                else if (type == TYPE_MP3)
                {
                    ret = add_node_to_list(music, cur_path, type);
                    if (ret < 0)
                    {
                        printf("add_node_to_ music list fail!\n");
                        closedir(d);
                        return -1;
                    }
                }
            }
        }
        else if (p == NULL && errno == 0)//读取完毕
        {
            break;
        }
        else//读取异常
        {
            closedir(d);
            return -1;
        }
    }

    //关闭目录
    ret = closedir(d);
    if (ret < 0)
    {
        perror("closedir fail");
        return -1;
    }
    return 0;
}



