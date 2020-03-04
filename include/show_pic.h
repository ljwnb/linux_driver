#ifndef __SHOW_PIC_H__
#define __SHOW_PIC_H__


void show_pic(PT_Node r);
void show_menu(void);
void hide_menu(PT_List list);
void *auto_showpic_threadfun(void *arg);
void EnterAutoShowPicMode(PT_List list);
void ExitAutoShowPicMode();




#endif /* __SHOW_PIC_H__ */
