#include <stdio.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include "libbitmap.h"

int main()
{
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    int cols = 0, rows= 0;
    char data [1024*600*3] = {0,};

    if(fb_init(&screen_width,&screen_height,&bits_per_pixel,&line_length) <0 )
    {
        printf("FrameBuffer Init Failed\r\n");
        return 0;
    }

    fb_clear();

    draw_background(data);
    drawgraph(data);
    fb_close();
}