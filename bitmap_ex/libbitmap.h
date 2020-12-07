#ifndef _LIBBITMAP_H_
#define _LIBBITMAP_H_

#define FULLSIZE 1024*600*3
#define WIDTH 1024
#define HEIGHT 600

int fb_init(int * screen_width, int * screen_height, int * bits_per_pixel, int * line_length);
void fb_clear(void);
void fb_doubleBufSwap(void);
void fb_close(void);
void fb_write(char* picData);
void draw_background(char *data);
void drawgraph(char *data);
#endif