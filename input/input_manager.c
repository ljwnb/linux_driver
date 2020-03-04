#include <stdio.h>

#include "debug.h"

#include "input_manager.h"

/*
    输入操作管理有一个全局链表头 用于存储所有的输入模块(按键 触摸屏...)
    上层只需要调用本层提供的接口来获取输入就可以了 其他不用操心
*/
static PT_InputOpr g_InputOprHead = NULL;//全局链表头

static T_InputEvent g_inputEvent;//用于所有输入线程 有输入时就会设置好


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//互斥锁
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;//条件变量

/*
    注册输入模块
    将模块自身添加到全局链表中
*/
int RegisterInputOpr(PT_InputOpr ptInputopr)
{
    PT_InputOpr temp;
    
    if (g_InputOprHead == NULL)//为空
    {
        DBG_PRINTF("%s :%d\n", __func__, __LINE__);
        g_InputOprHead   = ptInputopr;
        ptInputopr->next = NULL;
        DBG_PRINTF("%s :%d\n", __func__, __LINE__);
    }
    else
    {
        temp = g_InputOprHead;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next       = ptInputopr;
        ptInputopr->next = NULL;
    }
    DBG_PRINTF("%s :%d\n", __func__, __LINE__);
    return 0;
}


/*
    主线程获取输入
    获取输入事件函数 当需要获取输入时 只需要调用该接口
    没有输入时处于休眠状态 当有输入时就被唤醒
*/
int GetInputEvent(PT_InputEvent ptInputevent)
{
    //休眠
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    
    //当有模块有输入事件时就被唤醒 直接获取全局已设置好的数据
    *ptInputevent = g_inputEvent;
    pthread_mutex_unlock(&mutex);
    
    return 0;
}

/*
    线程处理函数 每个输入模块都创建一个线程来不断获取输入事件
    当有模块成功获取到输入事件后 就唤醒主线程 来接收输入
*/
static void *InputEventThreadFunc(void *arg)
{
    T_InputEvent tInputEvent;
    
    int (*GetEvent)(PT_InputEvent);//定义一个函数指针
    GetEvent = (int (*)(PT_InputEvent))arg;

    while (1)
    {
        //子线程不断获取输入 (阻塞)
        if (GetEvent(&tInputEvent) == 0)
        {
            //唤醒主线程  把tInputEvent赋值给全局变量 线程间通信
            pthread_mutex_lock(&mutex);
            
            g_inputEvent = tInputEvent;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

/*
    所有的输入设备初始化 
    调用所有设备的初始化函数 并创建子线程来获取输入
*/
int AllInputDevicesInit()
{
    PT_InputOpr ptTmp = g_InputOprHead;

    while (ptTmp != NULL)
    {
        if (ptTmp->DeviceInit() == 0)//调用设备初始化函数
        {
            //对于每个输入设备都创建一个线程来获取输入
            pthread_create(&ptTmp->tid, NULL, InputEventThreadFunc, (void *)ptTmp->GetInputEvent);
        }
        ptTmp = ptTmp->next;
    }
}

/*
    调用各个模块的初始化函数，即注册各个模块
*/
int InputInit()
{
    DBG_PRINTF("%s :%d\n", __func__, __LINE__);
    int ret = TouchScreenInit();
    return ret;
    
}

