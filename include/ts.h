#ifndef __TS_H__
#define __TS_H__


//手势方向
#define Ges_None    0
#define Ges_Left    1
#define Ges_Right   2
#define Ges_Up      3
#define Ges_Down    4


int get_touch(void);
void ts_uninit(void);
int ts_init(void);

#endif /* __TS_H__ */
