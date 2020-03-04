#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "disp_manager.h"
#include "input_manager.h"

#include "show_jpeg.h"
#include "show_bmp.h"


/*
    采用的是双向循环链表
*/
PT_List creat_list(void)
{
    PT_List list;
    list = (PT_List)malloc(sizeof(T_List));
    if (list == NULL)
    {
        return NULL;
    }
    list->first = list->last = NULL;
    list->cur_show = NULL;
    list->num   = 0;

    return list;
}

int add_node_to_list(PT_List list, char *path_name, int type)
{
    PT_Node node;
    node = (PT_Node)malloc(sizeof(T_Node));
    if (node == NULL)
    {
        return -1;
    }
    strncpy(node->pathname, path_name, sizeof(node->pathname));
    
    node->type = type;
    node->next = node->prev = NULL;

    if (list->first == NULL)
    {
        list->first       = list->last = node;
        list->last->next  = list->first;
        list->first->prev = list->last;
    }
    else
    {
        //尾插
        node->prev = list->last;
        node->next = list->first;

        list->last->next  = node;
        list->first->prev = node;

        list->last = node;
    }
    list->num++;
    
    return 0;
}



