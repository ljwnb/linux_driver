#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "music.h"

#include "debug.h"

static pthread_t tid_music;

//播放音乐线程函数
void* auto_playmusic_threadfunc(void *arg)
{
    PT_List list_music = (PT_List)arg;
    
    PT_Node r = list_music->first;

    while (r != NULL)
    {
        pid_t pid = fork();//创建一个子进程去执行madplay播放音乐
        if (pid < 0)
        {
            perror("fork error");
            return;
        }
        else if (pid > 0)//父进程
        {
            //父进程管理
            int x;//父进程负责管理歌曲
            pid_t w_pid = wait(&x);//等待因为播放完毕
            //printf("i am father1 wait success!\n");
            
            r = r->next;//自动循环下一首
        }
        else
        {
            //子进程负责播放
            DBG_PRINTF("I am son start music\n");
            int ret = execlp("madplay", "madplay", "-Q", r->pathname, NULL);
            if (ret < 0)
            {
                perror("execlp error");
                exit(-1);
            }
            else
            {
                printf("success!\n");//成功不会打印!!
            }
        }
    }
    
    return NULL;
}

    
void play_music(PT_List music)
{
    //创建一个线程去自动播放
    pthread_create(&tid_music, NULL, auto_playmusic_threadfunc, (void *)music);
    pthread_detach(tid_music);//线程分离

}


