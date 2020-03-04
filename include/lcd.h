#ifndef __LCD_H__
#define __LCD_H__

extern int xres;
extern int yres;
extern int lcd_bpp;
extern int fd_lcd;
extern int *plcd;


int lcd_init(void);
int lcd_uninit(void);

void draw_point(int x, int y, int color);
void draw_block(int x0, int y0, int width, int high, int color);
void clear_lcd(void);


#endif /* __LCD_H__ */
