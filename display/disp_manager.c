#include <stdio.h>
#include <string.h>

#include "disp_manager.h"
#include "debug.h"

static PT_DispOpr g_ptDispOprHead;  //显示设备链表头
static PT_DispOpr g_ptDefaultDispOpr; //默认的显示设备


/*
    注册输入模块
    将模块自身添加到全局链表中
*/
int RegisterDispOpr(PT_DispOpr pt_Disppr)
{
    PT_DispOpr temp;
    
    if (g_ptDispOprHead == NULL)//为空
    {
        g_ptDispOprHead = pt_Disppr;
        pt_Disppr->next = NULL;
    }
    else
    {
        temp = g_ptDispOprHead;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next      = pt_Disppr;
        pt_Disppr->next = NULL;
    }
    return 0;
}

//根据名字获得显示显示模块结构体
PT_DispOpr GetDispOpr(char *name)
{
    PT_DispOpr ptTemp = g_ptDispOprHead;
    while (ptTemp != NULL)
    {
        if (strcmp(ptTemp->name, name) == 0)
        {
            return ptTemp;
        }
        ptTemp = ptTemp->next;
    }
    return NULL;
}

/*
    根据名字选择默认的显示模块，并调用它的初始化函数 并清屏
*/
void SelectAndInitDispDev(char *name)
{
    g_ptDefaultDispOpr = GetDispOpr(name);
    DBG_PRINTF("%s :%d\n", __func__, __LINE__);
    if (g_ptDefaultDispOpr != NULL)
    {
        g_ptDefaultDispOpr->DeviceInit();
        DBG_PRINTF("%s :%d\n", __func__, __LINE__);
        g_ptDefaultDispOpr->CleanScreen(0);
        DBG_PRINTF("%s :%d\n", __func__, __LINE__);
    }
}

/*
    获取的当前使用的显示设备
*/
PT_DispOpr GetDefaultDispDev(void)
{
    return g_ptDefaultDispOpr;
}

/*
    注册所有显示设备
*/
int DisplayInit(void)
{
    int ret;
    ret = FBInit();
    
    return ret;
}

int DisplayExit(void)
{
    return g_ptDefaultDispOpr->DeviceExit();
}


