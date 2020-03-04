#include <stdio.h>
#include <pthread.h>

#include "list.h"

#include "show_pic.h"
#include "show_jpeg.h"
#include "show_bmp.h"

#include "debug.h"

#define AUTOMODE_PIC        "auto_mode.bmp"
#define MANUALMODE_PIC      "browse_mode.bmp"




static pthread_t tid_auto;//连播线程id

/*
    显示图片
*/
void show_pic(PT_Node r)
{
    if (r->type == TYPE_BMP)
    {
        DBG_PRINTF("%s:%d\n", __func__, __LINE__);
        show_bmp(r->pathname);
        DBG_PRINTF("%s:%d\n", __func__, __LINE__);
    }
    else if (r->type == TYPE_JPEG)
    {
        DBG_PRINTF("%s:%d\n", __func__, __LINE__);
        show_jpeg(r->pathname);
        DBG_PRINTF("%s:%d\n", __func__, __LINE__);
    }
}

//显示功能键
void show_menu(void)
{
    show_bmp_pos(56, 72, AUTOMODE_PIC);//自动显示图片
    show_bmp_pos(56, 472, MANUALMODE_PIC);//手动显示图片
}

//隐藏功能键
void hide_menu(PT_List list)
{
    DBG_PRINTF("%s:%d\n", __func__, __LINE__);
    show_pic(list->cur_show);
}

/*
    连续播放模式 //线程来做
*/
void *auto_showpic_threadfun(void *arg)
{
    PT_List list = (PT_List)arg;
    //线程设置为可被取消
    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0)
    {
        perror("pthread_setcancelstate error");
        return (void*)-1;
    }
    if (list->cur_show == NULL)
    {
        list->cur_show = list->first;//如果现在还没有显示图片 就从第一张图片开始显示
        if (list->cur_show == NULL)
        {
            printf("当前没有图片可以显示！\n");
            return;
        }
    }
    while (1)
    {
        show_pic(list->cur_show);
        sleep(5);
        list->cur_show = list->cur_show->next;
    }
    return NULL;
}


//进入自动连播模式
void EnterAutoShowPicMode(PT_List list)
{
    pthread_create(&tid_auto, NULL, auto_showpic_threadfun, (void *)list);//创建一个线程来连播
    pthread_detach(tid_auto);
}

//退出自动连播模式
void ExitAutoShowPicMode()
{
    if (pthread_cancel(tid_auto) != 0)
    {
        perror("pthread_cancel error");
        return;
    }
}



