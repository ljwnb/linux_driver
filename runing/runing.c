#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "disp_manager.h"
#include "input_manager.h"

#include "show_jpeg.h"
#include "show_bmp.h"

#include "debug.h"

#define MODE_AUTO   1
#define MODE_MANUAL 2

static int Cur_Mode;//当前模式

void runing(PT_List list, PT_List music)
{
    T_InputEvent t_Input;
    
    //程序开始自动进入连播模式 并在后台播放音乐
    EnterAutoShowPicMode(list);
    //开始播放音乐 创建一个线程 自动循环播放链表中的音乐
    play_music(music);
    
    Cur_Mode  = MODE_AUTO;
    
    while (1)
    {
        //获取输入
        GetInputEvent(&t_Input);
        if (Cur_Mode == MODE_AUTO)//连播模式下所有操作 都改为进入功能选择
        {
            /*
                如果当前为连播模式 所有操作为功能选择 关闭连播模式 显示菜单 继续判断输入
                如果为浏览模式 把状态切换为浏览模式
                其他所有操作 都为退出菜单 进入连播
            */
            ExitAutoShowPicMode();//暂时退出连播模式
            //显示菜单
            show_menu();
            //获取输入
            GetInputEvent(&t_Input);
            if (t_Input.ges == Ges_Manual)//进入浏览模式
            {
                DBG_PRINTF("%s:%d\n", __func__, __LINE__);
                hide_menu(list);
                Cur_Mode = MODE_MANUAL;
            }
            else
            {
                DBG_PRINTF("%s:%d\n", __func__, __LINE__);
                hide_menu(list);
                EnterAutoShowPicMode(list);
            }
            
        }
        else//浏览模式
        {
            switch (t_Input.ges)
            {
               case Ges_Left:
               case Ges_Up://上一张
               {
                    DBG_PRINTF("up pic\n");
                    list->cur_show = list->cur_show->next;
                    show_pic(list->cur_show);
                    break;
               }
               case Ges_Right:
               case Ges_Down://下一张
               {
                    DBG_PRINTF("down pic\n");
                    list->cur_show = list->cur_show->prev;
                    show_pic(list->cur_show);
                    break;
               }
               case Ges_Othre://出现菜单
               {
                    show_menu();
                    //获取输入
                    GetInputEvent(&t_Input);
                    if (t_Input.ges == Ges_AUTO)//进入连播模式
                    {
                        DBG_PRINTF("%s:%d\n", __func__, __LINE__);
                        Cur_Mode = MODE_AUTO;
                        hide_menu(list);
                        EnterAutoShowPicMode(list);//进入连播
                    }
                    else
                    {
                        DBG_PRINTF("%s:%d\n", __func__, __LINE__);
                        hide_menu(list);
                    }
                    break;
               }
               default:
               {
                    break;
               }
            }
        }
    

    }
}



