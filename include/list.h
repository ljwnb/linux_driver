#ifndef _LIST_H__
#define _LIST_H__

#define TYPE_NONE   0
#define TYPE_BMP    1
#define TYPE_JPEG   2
#define TYPE_MP3    3

typedef struct node {
    char pathname[128]; //图片路径名
    int type;           //图片类型标记
    
    struct node *next;
    struct node *prev;
}T_Node, *PT_Node;

typedef struct list {
    PT_Node first;
    PT_Node last;

    PT_Node cur_show;   //当前显示的图片
    int num;
    
}T_List, *PT_List;


//void print_list(PT_List list);

int add_node_to_list(PT_List list, char *path_name, int type);
PT_List creat_list(void);



#endif /* _LIST_H__ */
